#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TGraph.h>
#include <iostream>
#include <TRandom.h>

// To be modified
static const Int_t classes       = 2;
static const Int_t prompt        = 0;
static const Int_t guessRereco   = 0;
static const Int_t rereco74x     = 0;
static const Int_t rereco76x     = 0;
static const Int_t scaleUp       = 0;
static const Int_t scaleDown     = 0;
static const Int_t extraSmUp     = 0;
static const Int_t extraSmDown   = 0;
static const Int_t extraSm0TUp   = 0;
static const Int_t extraSm0TDown = 0; 

using namespace std;

void signalModelFormat(const char* filename, TString cicGenIso, TString kpl, TString theMass, TString cat) {

  // initialize the random number
  TRandom myRandom(12345);

  // Options
  cout << "Formatting " << filename << " from Pasquale's to our format for signal model studies" << endl;  
  if (prompt)      cout << "prompt reco smearings applied"    << endl;
  if (guessRereco) cout << "guessed rereco smearings applied" << endl;
  if (rereco74x)   cout << "74x rereco smearings by Giuseppe applied" << endl;
  if (rereco76x)   cout << "76x rereco smearings by Giuseppe applied" << endl;
  if (!prompt && !guessRereco && !rereco74x && !rereco76x) { cout << "no nominal smearing applied" << endl; }
  if (extraSmUp)     cout << "extra smearing from high mass DY applied - 3.8T, UP"   << endl;
  if (extraSmDown)   cout << "extra smearing from high mass DY applied - 3.8T, DOWN" << endl;
  if (extraSm0TUp)   cout << "extra smearing from high mass DY applied - 0T, UP"     << endl;
  if (extraSm0TDown) cout << "extra smearing from high mass DY applied - 0T, DOWN"   << endl;
  if (!extraSmUp && !extraSmDown && !extraSm0TUp && !extraSm0TDown) { cout << "no extra smearing applied" << endl; }
  if (scaleUp)     cout << "scale 1% up"   << endl;
  if (scaleDown)   cout << "scale 1% down" << endl;
  if (!scaleDown && !scaleUp) cout << "nominal scale" << endl;

  // R9 corrector
  TFile* fR9corr = TFile::Open("~gfasanel/public/R9_transformation/transformation.root");
  TGraph* gR9EB = (TGraph*) fR9corr->Get("transformR90");
  TGraph* gR9EE = (TGraph*) fR9corr->Get("transformR91");

  // Pasquale's ntuples
  TFile *fileOrig = 0;
  TTree *treeOrig = 0;
  fileOrig = TFile::Open(filename);
  if( fileOrig ) {
    fileOrig->cd();
    TString theTree = TString::Format("RSGravToGG_kMpl_"+kpl+"_M_"+theMass+"_TuneCUEP8M1_13TeV_pythia8_13TeV_"+cat);
    TString theDir  = TString::Format(cicGenIso+"/trees/"+theTree);
    cout << "Reading " << theDir << endl;
    treeOrig = (TTree*)fileOrig->Get(theDir);
  } else {
    cout << "File " << filename << " not existing !" << endl;
    return;
  }

  fileOrig->cd();
  if (!treeOrig) {
    cout << "Tree not existing !" << endl; 
    return;    
  }

  treeOrig->SetMakeClass(0);
  
  // number of entries saved in the first tree
  int nentriesOrig = treeOrig->GetEntries();   

  // New file
  TString theNewFile = TString::Format("/tmp/crovelli/FormSigMod_"+cicGenIso+"_kpl"+kpl+"_M"+theMass+"_cat"+cat+".root");
  cout << "OutputFile: " << theNewFile << endl;
  TFile *fileNew = TFile::Open(theNewFile,"RECREATE");
  TTree *treeNew = new TTree("DiPhotonTree","reduced tree for signal model studies");
  
  std::vector<TTree*> trees; 
  trees.push_back(treeNew);

  // original tree leaves
  Float_t puweight     = 0.;
  Float_t mass         = 0.;
  Float_t genMass      = 0.;
  Float_t leadR9       = 0.;
  Float_t subleadR9    = 0.;
  Float_t leadScEta    = 0.;
  Float_t subleadScEta = 0.;
  Float_t leadInitialEnergy    = 0.;
  Float_t subLeadInitialEnergy = 0.;
  Float_t leadEnergy    = 0.;
  Float_t subLeadEnergy = 0.;
  
  // List of branches - original tree
  TBranch  *b_puweight;
  TBranch  *b_mass;
  TBranch  *b_genMass;
  TBranch  *b_leadR9;
  TBranch  *b_subleadR9;
  TBranch  *b_leadScEta;
  TBranch  *b_subleadScEta;
  TBranch  *b_leadInitialEnergy;
  TBranch  *b_subLeadInitialEnergy;
  TBranch  *b_leadEnergy;
  TBranch  *b_subLeadEnergy;

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("puweight",     &puweight,     &b_puweight);
  treeOrig->SetBranchAddress("mass",         &mass,         &b_mass);
  treeOrig->SetBranchAddress("genMass",      &genMass,      &b_genMass);
  treeOrig->SetBranchAddress("leadR9",       &leadR9,       &b_leadR9);
  treeOrig->SetBranchAddress("subleadR9",    &subleadR9,    &b_subleadR9);
  treeOrig->SetBranchAddress("leadScEta",    &leadScEta,    &b_leadScEta);
  treeOrig->SetBranchAddress("subleadScEta", &subleadScEta, &b_subleadScEta);
  treeOrig->SetBranchAddress("leadInitialEnergy",    &leadInitialEnergy,    &b_leadInitialEnergy);
  treeOrig->SetBranchAddress("subLeadInitialEnergy", &subLeadInitialEnergy, &b_subLeadInitialEnergy);
  treeOrig->SetBranchAddress("leadEnergy",    &leadEnergy,    &b_leadEnergy);
  treeOrig->SetBranchAddress("subLeadEnergy", &subLeadEnergy, &b_subLeadEnergy);

  // New variables
  float mgg, unsmearedMgg, mggGen;
  int eventClass;
  int highR9;
  float lR9, slR9, lScEta, slScEta;
  float lSmear, slSmear;

  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches
    theTreeNew->Branch("mgg", &mgg, "mgg/F");
    theTreeNew->Branch("unsmearedMgg", &unsmearedMgg, "unsmearedMgg/F");
    theTreeNew->Branch("mggGen", &mggGen, "mggGen/F");
    theTreeNew->Branch("eventClass", &eventClass, "eventClass/I");
    theTreeNew->Branch("highR9", &highR9, "highR9/I");
    theTreeNew->Branch("puweight", &puweight, "puweight/F");
    theTreeNew->Branch("lR9", &lR9, "lR9/F");
    theTreeNew->Branch("slR9", &slR9, "slR9/F");
    theTreeNew->Branch("lScEta", &lScEta, "lScEta/F");
    theTreeNew->Branch("slScEta", &slScEta, "slScEta/F");
    theTreeNew->Branch("lSmear",&lSmear,"lSmear/F");
    theTreeNew->Branch("slSmear",&slSmear,"slSmear/F");
  }

  for(int i=0; i<nentriesOrig; i++) {
    
    treeOrig->GetEntry(i);

    // EE-EE excluded
    if (fabs(leadScEta)>1.5 && fabs(subleadScEta)>1.5) continue;

    // R9 corrections
    float leadR9p = leadR9;
    if(fabs(leadScEta)<1.5){
      leadR9p = gR9EB->Eval(leadR9);
    } else if(fabs(leadScEta)>1.5){
      leadR9p = gR9EE->Eval(leadR9);
    }
    float subleadR9p = subleadR9;
    if(fabs(subleadScEta)<1.5){
      subleadR9p = gR9EB->Eval(subleadR9);
    } else if(fabs(subleadScEta)>1.5){
      subleadR9p = gR9EE->Eval(subleadR9);
    }

    // R9 classes - after corrections
    if (leadR9p>0.94 && subleadR9p>0.94) highR9=1;
    else highR9=0;

    // 2 classes: EBEB, EBEE
    eventClass = -1;
    if (fabs(leadScEta)<1.5 && fabs(subleadScEta)<1.5) eventClass = 0;
    else eventClass = 1;

    // 4 classes: EBHighR9, EBLowR9, EEHighR9, EELowR9
    if (classes==4) {
      int theNewClass = -1;
      if (eventClass==0 && highR9) theNewClass=0;
      else if (eventClass==0 && !highR9) theNewClass=1;
      else if (eventClass==1 && highR9)  theNewClass=2;
      else if (eventClass==1 && !highR9) theNewClass=3;
      eventClass = theNewClass;
    }

    // 3 classes
    if (classes==3) {
      int theNewClass = -1;
      if (eventClass==0 && highR9) theNewClass=0;
      else if (eventClass==0 && !highR9) theNewClass=1;
      else if (eventClass==1) theNewClass=2;
      eventClass = theNewClass;
    }

    // To check ntuples
    lR9     = leadR9p;
    slR9    = subleadR9p;
    lScEta  = leadScEta;
    slScEta = subleadScEta;
    
    // before smearings
    unsmearedMgg = mass;

    // gen level
    mggGen = genMass;


    // In case we want to apply smearings
    if (prompt || guessRereco || rereco74x || rereco76x)  {

      // hardcoded!
      float smearEBlowEtaHighR9  = 0.;
      float smearEBlowEtaLowR9   = 0.;
      float smearEBhighEtaHighR9 = 0.;
      float smearEBhighEtaLowR9  = 0.;
      float smearEElowEtaHighR9  = 0.;
      float smearEElowEtaLowR9   = 0.;
      float smearEEhighEtaHighR9 = 0.;
      float smearEEhighEtaLowR9  = 0.;
      
      // prompt reco smearings
      if (prompt) {
	smearEBlowEtaHighR9  = 0.014142;
	smearEBlowEtaLowR9   = 0.013654;
	smearEBhighEtaHighR9 = 0.017120;
	smearEBhighEtaLowR9  = 0.020859;
	smearEElowEtaHighR9  = 0.027289;
	smearEElowEtaLowR9   = 0.028083;
	smearEEhighEtaHighR9 = 0.030831;
	smearEEhighEtaLowR9  = 0.031793;
      }
      
      // guess for re-reco   
      if (guessRereco) {   
	smearEBlowEtaHighR9  = 0.009;
	smearEBlowEtaLowR9   = 0.009;
	smearEBhighEtaHighR9 = 0.015;
	smearEBhighEtaLowR9  = 0.015;
	smearEElowEtaHighR9  = 0.021;
	smearEElowEtaLowR9   = 0.021;
	smearEEhighEtaHighR9 = 0.021;
	smearEEhighEtaLowR9  = 0.021;
      } 
      
      // preliminary by Giuseppe (dec15), photon-trained regression, 74x rereco      
      if (rereco74x) { 
	smearEBlowEtaHighR9  = 0.0087;
	smearEBlowEtaLowR9   = 0.0105;
	smearEBhighEtaHighR9 = 0.0130;
	smearEBhighEtaLowR9  = 0.0181;
	smearEElowEtaHighR9  = 0.0204;
	smearEElowEtaLowR9   = 0.0223;
	smearEEhighEtaHighR9 = 0.0231;
	smearEEhighEtaLowR9  = 0.0260;
      }
      
      // https://gfasanel.web.cern.ch/gfasanel/RUN2_ECAL_Calibration/February2016_Rereco_76/invMass_SC_pho_regrCorr/step4/table_outFile-step4-invMass_SC_pho_regrCorr-loose-Et_20-noPF-HggRunEtaR9_smear_tex.dat
      if (rereco76x) { 
	smearEBlowEtaHighR9  = 0.0084;
	smearEBlowEtaLowR9   = 0.0097;
	smearEBhighEtaHighR9 = 0.0126;
	smearEBhighEtaLowR9  = 0.0180;
	smearEElowEtaHighR9  = 0.0220;
	smearEElowEtaLowR9   = 0.0244;
	smearEEhighEtaHighR9 = 0.0236;
	smearEEhighEtaLowR9  = 0.0268;
      }
          
      // Gaussian
      float theGaussMean = 1.;
      if (scaleUp) {
	theGaussMean = 1.01;
      } else if (scaleDown) {
	theGaussMean = 0.99;
      }
      
      // Apply smearings
      float theFirstSmear = 0.;   
      if (fabs(leadScEta)<1 && leadR9p>=0.94)      theFirstSmear = smearEBlowEtaHighR9;
      else if (fabs(leadScEta)<1 && leadR9p<0.94)  theFirstSmear = smearEBlowEtaLowR9;
      else if (fabs(leadScEta)>=1 && fabs(leadScEta)<1.5 && leadR9p>=0.94)  theFirstSmear = smearEBhighEtaHighR9;
      else if (fabs(leadScEta)>=1 && fabs(leadScEta)<1.5 && leadR9p<0.94)   theFirstSmear = smearEBhighEtaLowR9;
      else if (fabs(leadScEta)>=1.5 && fabs(leadScEta)<2. && leadR9p>=0.94) theFirstSmear = smearEElowEtaHighR9;
      else if (fabs(leadScEta)>=1.5 && fabs(leadScEta)<2. && leadR9p<0.94)  theFirstSmear = smearEElowEtaLowR9;
      else if (fabs(leadScEta)>=2 && fabs(leadScEta)<2.5 && leadR9p>=0.94)  theFirstSmear = smearEEhighEtaHighR9;
      else if (fabs(leadScEta)>=2 && fabs(leadScEta)<2.5 && leadR9p<0.94)   theFirstSmear = smearEEhighEtaLowR9;
      //
      float theSecondSmear = 0.;   
      if (fabs(subleadScEta)<1 && subleadR9p>=0.94)      theSecondSmear = smearEBlowEtaHighR9;
      else if (fabs(subleadScEta)<1 && subleadR9p<0.94)  theSecondSmear = smearEBlowEtaLowR9;
      else if (fabs(subleadScEta)>=1 && fabs(subleadScEta)<1.5 && subleadR9p>=0.94)  theSecondSmear = smearEBhighEtaHighR9;
      else if (fabs(subleadScEta)>=1 && fabs(subleadScEta)<1.5 && subleadR9p<0.94)   theSecondSmear = smearEBhighEtaLowR9;
      else if (fabs(subleadScEta)>=1.5 && fabs(subleadScEta)<2. && subleadR9p>=0.94) theSecondSmear = smearEElowEtaHighR9;
      else if (fabs(subleadScEta)>=1.5 && fabs(subleadScEta)<2. && subleadR9p<0.94)  theSecondSmear = smearEElowEtaLowR9;
      else if (fabs(subleadScEta)>=2 && fabs(subleadScEta)<2.5 && subleadR9p>=0.94)  theSecondSmear = smearEEhighEtaHighR9;
      else if (fabs(subleadScEta)>=2 && fabs(subleadScEta)<2.5 && subleadR9p<0.94)   theSecondSmear = smearEEhighEtaLowR9;
      //
      float theGaussSigma = 0.5 * sqrt (theFirstSmear*theFirstSmear + theSecondSmear*theSecondSmear);
      float fromGauss = myRandom.Gaus(theGaussMean,theGaussSigma);
      mgg = mass*fromGauss;

      // To check ntuples
      lSmear  = theFirstSmear;
      slSmear = theSecondSmear;
 
    } else if (extraSmUp || extraSm0TUp || extraSmDown || extraSm0TDown ) {   // for systematics

      // sigma of smearings by Giuseppe at Zpeak
      float smearEBlowEtaHighR9  = 0.;
      float smearEBlowEtaLowR9   = 0.;
      float smearEBhighEtaHighR9 = 0.;
      float smearEBhighEtaLowR9  = 0.;
      float smearEElowEtaHighR9  = 0.;
      float smearEElowEtaLowR9   = 0.;
      float smearEEhighEtaHighR9 = 0.;
      float smearEEhighEtaLowR9  = 0.;

      // hardcoded, 3.8T
      if (extraSmUp || extraSmDown) {
	smearEBlowEtaHighR9  = 0.0080;
	smearEBlowEtaLowR9   = 0.0094;
	smearEBhighEtaHighR9 = 0.0115;
	smearEBhighEtaLowR9  = 0.0183;
	smearEElowEtaHighR9  = 0.0221;
	smearEElowEtaLowR9   = 0.0201;
	smearEEhighEtaHighR9 = 0.0230;
	smearEEhighEtaLowR9  = 0.0268;
      } else if ( extraSm0TUp || extraSm0TDown ){ 
	// hardcoded, 0T
	smearEBlowEtaHighR9  = 0.00788265;
	smearEBlowEtaLowR9   = 0.00788265;
	smearEBhighEtaHighR9 = 0.014427;
	smearEBhighEtaLowR9  = 0.014427;
	smearEElowEtaHighR9  = 0.01977;
	smearEElowEtaLowR9   = 0.01977;
	smearEEhighEtaHighR9 = 0.0235932;
	smearEEhighEtaLowR9  = 0.0235932;
      }

      float theFirstSmear = 0.;   
      if (fabs(leadScEta)<1 && leadR9>=0.94)      theFirstSmear = smearEBlowEtaHighR9;
      else if (fabs(leadScEta)<1 && leadR9<0.94)  theFirstSmear = smearEBlowEtaLowR9;
      else if (fabs(leadScEta)>=1 && fabs(leadScEta)<1.5 && leadR9>=0.94)  theFirstSmear = smearEBhighEtaHighR9;
      else if (fabs(leadScEta)>=1 && fabs(leadScEta)<1.5 && leadR9<0.94)   theFirstSmear = smearEBhighEtaLowR9;
      else if (fabs(leadScEta)>=1.5 && fabs(leadScEta)<2. && leadR9>=0.94) theFirstSmear = smearEElowEtaHighR9;
      else if (fabs(leadScEta)>=1.5 && fabs(leadScEta)<2. && leadR9<0.94)  theFirstSmear = smearEElowEtaLowR9;
      else if (fabs(leadScEta)>=2 && fabs(leadScEta)<2.5 && leadR9>=0.94)  theFirstSmear = smearEEhighEtaHighR9;
      else if (fabs(leadScEta)>=2 && fabs(leadScEta)<2.5 && leadR9<0.94)   theFirstSmear = smearEEhighEtaLowR9;
      //
      float theSecondSmear = 0.;   
      if (fabs(subleadScEta)<1 && subleadR9>=0.94)      theSecondSmear = smearEBlowEtaHighR9;
      else if (fabs(subleadScEta)<1 && subleadR9<0.94)  theSecondSmear = smearEBlowEtaLowR9;
      else if (fabs(subleadScEta)>=1 && fabs(subleadScEta)<1.5 && subleadR9>=0.94)  theSecondSmear = smearEBhighEtaHighR9;
      else if (fabs(subleadScEta)>=1 && fabs(subleadScEta)<1.5 && subleadR9<0.94)   theSecondSmear = smearEBhighEtaLowR9;
      else if (fabs(subleadScEta)>=1.5 && fabs(subleadScEta)<2. && subleadR9>=0.94) theSecondSmear = smearEElowEtaHighR9;
      else if (fabs(subleadScEta)>=1.5 && fabs(subleadScEta)<2. && subleadR9<0.94)  theSecondSmear = smearEElowEtaLowR9;
      else if (fabs(subleadScEta)>=2 && fabs(subleadScEta)<2.5 && subleadR9>=0.94)  theSecondSmear = smearEEhighEtaHighR9;
      else if (fabs(subleadScEta)>=2 && fabs(subleadScEta)<2.5 && subleadR9<0.94)   theSecondSmear = smearEEhighEtaLowR9;

      // smearing up / down
      float movedFirstSmear  = 0;
      float movedSecondSmear = 0;
      if (extraSmUp || extraSm0TUp) {
	movedFirstSmear  = sqrt( theFirstSmear*theFirstSmear + 0.005*0.005 );
	movedSecondSmear = sqrt( theSecondSmear*theSecondSmear + 0.005*0.005 );
      } else {
	movedFirstSmear  = sqrt( theFirstSmear*theFirstSmear - 0.005*0.005 );
	movedSecondSmear = sqrt( theSecondSmear*theSecondSmear - 0.005*0.005 );
      }

      // New energy with extra smearings
      float theLeadRatio    = (movedFirstSmear-theFirstSmear)/theFirstSmear;
      float theSubLeadRatio = (movedSecondSmear-theSecondSmear)/theSecondSmear;
      float movedLeadEnergy    = leadEnergy*(1+theLeadRatio) - leadInitialEnergy*theLeadRatio;
      float movedSubLeadEnergy = subLeadEnergy*(1+theSubLeadRatio) - subLeadInitialEnergy*theSubLeadRatio;
      
      // corrected mass
      mgg = mass*sqrt(movedLeadEnergy*movedSubLeadEnergy)/sqrt(leadEnergy*subLeadEnergy);

      // To check ntuples
      lSmear  = 0.;
      slSmear = 0.;
      
    } else {

      mgg = mass;

      // To check ntuples
      lSmear  = 0.;
      slSmear = 0.;
    }

    treeNew->Fill();
  }

  // new info
  fileNew->ls();
  fileNew->cd();
  treeNew->Write();
  fileNew->Close();

  fileOrig->cd();
  fileOrig->Close();  
}
