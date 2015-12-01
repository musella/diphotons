#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>
#include <TRandom.h>

// To be modified
static const Int_t nominal     = 1;
static const Int_t smearDown   = 0;
static const Int_t smearUp     = 0;
static const Int_t scaleDown   = 0;
static const Int_t scaleUp     = 0;
static const Int_t checkRereco = 0;

using namespace std;

void signalModelFormat(const char* filename, TString kinGenIso, TString kpl, TString theMass, TString cat) {

  // initialize the random number
  TRandom myRandom(12345);

  cout << "Formatting " << filename << " from Pasquale's to our format for signal model studies" << endl;  
  if (nominal)     cout << "nominal smearings applied"   << endl;
  if (smearUp)     cout << "smearUp smearings applied"   << endl;
  if (smearDown)   cout << "smearDown smearings applied" << endl;
  if (scaleUp)     cout << "scaleUp applied"   << endl;
  if (scaleDown)   cout << "scaleDown applied" << endl;
  if (checkRereco) cout << "[potential] rereco smearings applied"   << endl;

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
  Float_t puweight   = 0.;
  Float_t mass       = 0.;
  Float_t genMass    = 0.;
  Float_t leadEta    = 0.;
  Float_t subleadEta = 0.;
  Float_t leadR9     = 0.;
  Float_t subleadR9  = 0.;
  
  // List of branches - original tree
  TBranch  *b_puweight;
  TBranch  *b_mass;
  TBranch  *b_genMass;
  TBranch  *b_leadEta;
  TBranch  *b_subleadEta;
  TBranch  *b_leadR9;
  TBranch  *b_subleadR9;

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("puweight",   &puweight,   &b_puweight);
  treeOrig->SetBranchAddress("mass",       &mass,       &b_mass);
  treeOrig->SetBranchAddress("genMass",    &genMass,    &b_genMass);
  treeOrig->SetBranchAddress("leadEta",    &leadEta,    &b_leadEta);
  treeOrig->SetBranchAddress("subleadEta", &subleadEta, &b_subleadEta);
  treeOrig->SetBranchAddress("leadR9",     &leadR9,     &b_leadR9);
  treeOrig->SetBranchAddress("subleadR9",  &subleadR9,  &b_subleadR9);

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

    if (fabs(leadEta)>1.5 && fabs(subleadEta)>1.5) continue;

    eventClass = -1;
    if (fabs(leadEta)<1.5 && fabs(subleadEta)<1.5) eventClass = 0;
    else eventClass = 1;

    if (leadR9>0.94 && subleadR9>0.94) highR9=1;
    else highR9=0;

    unsmearedMgg = mass;
    mggGen = genMass;


    // Nominal or systematics
    // hardcoded! - init    
    float smearEBlowEta     = 0.013898;
    float smearEBhighEta    = 0.0189895;
    float smearEElowEta     = 0.027686;
    float smearEEhighEta    = 0.031312;
    float smearEBlowEtaErr  = 0.000153567;
    float smearEBhighEtaErr = 0.000518756;
    float smearEElowEtaErr  = 0.000445363;
    float smearEEhighEtaErr = 0.000372622;
    // hardcoded! - end 
    float theGaussMean = 1.;
    if (smearUp) {
      smearEBlowEta  = smearEBlowEta  + smearEBlowEtaErr;
      smearEBhighEta = smearEBhighEta + smearEBhighEtaErr;
      smearEElowEta  = smearEElowEta  + smearEElowEtaErr;
      smearEEhighEta = smearEEhighEta + smearEEhighEtaErr;
    } else if (smearDown) {
      smearEBlowEta  = smearEBlowEta  - smearEBlowEtaErr;
      smearEBhighEta = smearEBhighEta - smearEBhighEtaErr;
      smearEElowEta  = smearEElowEta  - smearEElowEtaErr;
      smearEEhighEta = smearEEhighEta - smearEEhighEtaErr;
    } else if (scaleUp) {
      theGaussMean = 1.01;
    } else if (scaleDown) {
      theGaussMean = 0.99;
    } else if (checkRereco) {
      smearEBlowEta  = 0.009;
      smearEBhighEta = 0.015;
      smearEElowEta  = 0.021;
      smearEEhighEta = 0.021;
    }

    // Apply smearings
    float theFirstSmear = 0.;   
    if (fabs(leadEta)<1) theFirstSmear = smearEBlowEta;
    else if (fabs(leadEta)>=1 && fabs(leadEta)<1.5)  theFirstSmear = smearEBhighEta;
    else if (fabs(leadEta)>=1.5 && fabs(leadEta)<2.) theFirstSmear = smearEElowEta;
    else if (fabs(leadEta)>=2 && fabs(leadEta)<2.5)  theFirstSmear = smearEEhighEta;
    //
    float theSecondSmear = 0.;   
    if (fabs(subleadEta)<1) theSecondSmear = smearEBlowEta;
    else if (fabs(subleadEta)>=1 && fabs(subleadEta)<1.5)  theSecondSmear = smearEBhighEta;
    else if (fabs(subleadEta)>=1.5 && fabs(subleadEta)<2.) theSecondSmear = smearEElowEta;
    else if (fabs(subleadEta)>=2 && fabs(subleadEta)<2.5)  theSecondSmear = smearEEhighEta;
    //
    float theGaussSigma = 0.5 * sqrt (theFirstSmear*theFirstSmear + theSecondSmear*theSecondSmear);
    float fromGauss = myRandom.Gaus(theGaussMean,theGaussSigma);
    mgg = mass*fromGauss;

    ////mgg = mass;

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
