#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>
#include <TRandom.h>

// To be modified
static const Int_t prompt      = 0;
static const Int_t guessRereco = 0;
static const Int_t rereco      = 1;
static const Int_t scaleUp     = 0;
static const Int_t scaleDown   = 0;

using namespace std;

void signalModelFormat(const char* filename, TString kinGenIso, TString kpl, TString theMass, TString cat) {

  // initialize the random number
  TRandom myRandom(12345);

  cout << "Formatting " << filename << " from Pasquale's to our format for signal model studies" << endl;  
  if (prompt)      cout << "prompt reco smearings applied"    << endl;
  if (guessRereco) cout << "guessed rereco smearings applied" << endl;
  if (rereco)      cout << "74x rereco smearings by Giuseppe applied" << endl;
  if (!prompt && !guessRereco && !rereco) { cout << "choose some smearings! " << endl; }
  if (scaleUp)     cout << "scale 1% up"   << endl;
  if (scaleDown)   cout << "scale 1% down" << endl;
  if (!scaleDown && !scaleUp) cout << "nominal scale" << endl;

  TFile *fileOrig = 0;
  TTree *treeOrig = 0;
  
  fileOrig = TFile::Open(filename);
  if( fileOrig ) {
    fileOrig->cd();
    TString theTree = TString::Format("RSGravToGG_kMpl_"+kpl+"_M_"+theMass+"_TuneCUEP8M1_13TeV_pythia8_13TeV_"+cat);
    TString theDir  = TString::Format(kinGenIso+"/trees/"+theTree);
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

  TString theNewFile = TString::Format("/tmp/crovelli/FormSigMod_"+kinGenIso+"_kpl"+kpl+"_M"+theMass+"_cat"+cat+".root");
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
  
  // List of branches - original tree
  TBranch  *b_puweight;
  TBranch  *b_mass;
  TBranch  *b_genMass;
  TBranch  *b_leadR9;
  TBranch  *b_subleadR9;
  TBranch  *b_leadScEta;
  TBranch  *b_subleadScEta;

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("puweight",     &puweight,     &b_puweight);
  treeOrig->SetBranchAddress("mass",         &mass,         &b_mass);
  treeOrig->SetBranchAddress("genMass",      &genMass,      &b_genMass);
  treeOrig->SetBranchAddress("leadR9",       &leadR9,       &b_leadR9);
  treeOrig->SetBranchAddress("subleadR9",    &subleadR9,    &b_subleadR9);
  treeOrig->SetBranchAddress("leadScEta",    &leadScEta,    &b_leadScEta);
  treeOrig->SetBranchAddress("subleadScEta", &subleadScEta, &b_subleadScEta);

  // New variables
  float mgg, unsmearedMgg, mggGen;
  int eventClass;
  int highR9;
  
  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches
    theTreeNew->Branch("mgg", &mgg, "mgg/F");
    theTreeNew->Branch("unsmearedMgg", &unsmearedMgg, "unsmearedMgg/F");
    theTreeNew->Branch("mggGen", &mggGen, "mggGen/F");
    theTreeNew->Branch("eventClass", &eventClass, "eventClass/I");
    theTreeNew->Branch("highR9", &highR9, "highR9/I");
    theTreeNew->Branch("puweight", &puweight, "puweight/F");
  }

  for(int i=0; i<nentriesOrig; i++) {
    
    treeOrig->GetEntry(i);

    // EE-EE excluded
    if (fabs(leadScEta)>1.5 && fabs(subleadScEta)>1.5) continue;

    // 2 classes
    eventClass = -1;
    if (fabs(leadScEta)<1.5 && fabs(subleadScEta)<1.5) eventClass = 0;
    else eventClass = 1;

    // R9 classes
    if (leadR9>0.94 && subleadR9>0.94) highR9=1;
    else highR9=0;

    // before smearings
    unsmearedMgg = mass;

    // gen level
    mggGen = genMass;

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

    // preliminary by Giuseppe (dec15), photon-trtained regression, 74x rereco      
    if (rereco) { 
      smearEBlowEtaHighR9  = 0.0087;
      smearEBlowEtaLowR9   = 0.0105;
      smearEBhighEtaHighR9 = 0.0130;
      smearEBhighEtaLowR9  = 0.0181;
      smearEElowEtaHighR9  = 0.0204;
      smearEElowEtaLowR9   = 0.0223;
      smearEEhighEtaHighR9 = 0.0231;
      smearEEhighEtaLowR9  = 0.0260;
    }

    float theGaussMean = 1.;
    if (scaleUp) {
      theGaussMean = 1.01;
    } else if (scaleDown) {
      theGaussMean = 0.99;
    }


    // Apply smearings
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
    //
    float theGaussSigma = 0.5 * sqrt (theFirstSmear*theFirstSmear + theSecondSmear*theSecondSmear);
    float fromGauss = myRandom.Gaus(theGaussMean,theGaussSigma);
    mgg = mass*fromGauss;

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
