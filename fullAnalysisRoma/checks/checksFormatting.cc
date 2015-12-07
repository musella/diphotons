#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>

using namespace std;

void checksFormatting(const char* filename, bool isSignal, bool isDY, bool isData, TString cat, TString kinGenIso="kinGenIso", TString kpl="001", TString theMass="750") {

  TFile *fileOrig = 0;
  TTree *treeOrig = 0;
  
  fileOrig = TFile::Open(filename);
  if( fileOrig ) {
    fileOrig->cd();
    if (isSignal) {
      TString theTree = TString::Format("RSGravToGG_kMpl_"+kpl+"_M_"+theMass+"_TuneCUEP8M1_13TeV_pythia8_13TeV_"+cat);
      TString theDir  = TString::Format(kinGenIso+"/trees/"+theTree);
      cout << "Reading MC, " << theDir << endl;
      treeOrig = (TTree*)fileOrig->Get(theDir);
    } else if (isDY) {
      TString theTree = TString::Format("DYJetsToLL_M_50_TuneCUETP8M1_amcatnloFXFX_pythia8_13TeV_"+cat);
      TString theDir  = TString::Format("cic/trees/"+theTree);
      cout << "Reading DY, " << theDir << endl;
      treeOrig = (TTree*)fileOrig->Get(theDir);
    } else if (isData) {
      TString theTree = TString::Format("Data_13TeV_"+cat);
      TString theDir  = TString::Format("cic/trees/"+theTree);
      cout << "Reading data, " << theDir << endl;
      treeOrig = (TTree*)fileOrig->Get(theDir);
    }
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
  
  TString theNewFile;
  if (isSignal) theNewFile = TString::Format("/tmp/crovelli/FormChecks_"+kinGenIso+"_kpl"+kpl+"_M"+theMass+"_cat"+cat+".root");
  if (isDY)     theNewFile = TString::Format("/tmp/crovelli/FormChecks_DY_cat"+cat+".root");
  if (isData)   theNewFile = TString::Format("/tmp/crovelli/FormChecks_data_cat"+cat+".root");
  cout << "OutputFile: " << theNewFile << endl;
  TFile *fileNew = TFile::Open(theNewFile,"RECREATE");
  TTree *treeNew = new TTree("DiPhotonTree","reduced tree for signal model studies");
  
  std::vector<TTree*> trees; 
  trees.push_back(treeNew);

  // original tree leaves
  Float_t puweight   = 0.;
  Float_t mass       = 0.;
  Float_t leadEta    = 0.;
  Float_t subleadEta = 0.;
  Float_t leadPhi    = 0.;
  Float_t subleadPhi = 0.;
  Float_t leadPt     = 0.;
  Float_t subleadPt  = 0.;
  
  // List of branches - original tree
  TBranch  *b_puweight;
  TBranch  *b_mass;
  TBranch  *b_leadEta;
  TBranch  *b_subleadEta;
  TBranch  *b_leadPhi;
  TBranch  *b_subleadPhi;
  TBranch  *b_leadPt;
  TBranch  *b_subleadPt;

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("puweight",   &puweight,   &b_puweight);
  treeOrig->SetBranchAddress("mass",       &mass,       &b_mass);
  treeOrig->SetBranchAddress("leadEta",    &leadEta,    &b_leadEta);
  treeOrig->SetBranchAddress("subleadEta", &subleadEta, &b_subleadEta);
  treeOrig->SetBranchAddress("leadPhi",    &leadPhi,    &b_leadPhi);
  treeOrig->SetBranchAddress("subleadPhi", &subleadPhi, &b_subleadPhi);
  treeOrig->SetBranchAddress("leadPt",     &leadPt,     &b_leadPt);
  treeOrig->SetBranchAddress("subleadPt",  &subleadPt,  &b_subleadPt);

  // New variables
  int eventClass;
  
  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches
    theTreeNew->Branch("puweight",   &puweight,    "puweight/F");
    theTreeNew->Branch("mass",       &mass,       "mass/F");
    theTreeNew->Branch("leadEta",    &leadEta,    "leadEta/F");
    theTreeNew->Branch("subleadEta", &subleadEta, "subleadEta/F");
    theTreeNew->Branch("leadPhi",    &leadPhi,    "leadPhi/F");
    theTreeNew->Branch("subleadPhi", &subleadPhi, "subleadPhi/F");
    theTreeNew->Branch("leadPt",     &leadPt,     "leadPt/F");
    theTreeNew->Branch("subleadPt",  &subleadPt,  "subleadPt/F");
    theTreeNew->Branch("eventClass", &eventClass, "eventClass/I");
  }

  for(int i=0; i<nentriesOrig; i++) {
    
    treeOrig->GetEntry(i);

    if (fabs(leadEta)>1.5 && fabs(subleadEta)>1.5) continue;

    eventClass = -1;
    if (fabs(leadEta)<1.5 && fabs(subleadEta)<1.5) eventClass = 0;
    else eventClass = 1;

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
