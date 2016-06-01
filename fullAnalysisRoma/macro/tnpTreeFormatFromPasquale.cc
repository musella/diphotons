#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TGraph.h>
#include <iostream>
#include <TRandom.h>

using namespace std;

void tnpTreeFormatFromPasquale(const char* filename, TString cat) {

  // Options
  cout << "Formatting " << filename << " from Pasquale's to our format for Z plots" << endl;  

  // Pasquale's ntuples
  TFile *fileOrig = 0;
  TTree *treeOrig = 0;
  fileOrig = TFile::Open(filename);
  if( fileOrig ) {
    fileOrig->cd();
    TString theTree = TString::Format("DYJetsToLL_M_50_TuneCUETP8M1_13TeV_madgraphMLM_pythia8_13TeV_"+cat);      // 3.8T
    //TString theTree = TString::Format("DYToEE_NNPDF30_13TeV_powheg_pythia8_13TeV_"+cat);                       // 0T 
    //TString theTree = TString::Format("Data_13TeV_"+cat);                                                      // both
    TString theDir  = TString::Format("cic/trees/"+theTree);
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
  TString theNewFile = TString::Format("/tmp/crovelli/DYJetsToLL_M_50_TuneCUETP8M1_13TeV_madgraphMLM_pythia8_13TeV_"+cat+".root");   //  3.8T  
  //TString theNewFile = TString::Format("/tmp/crovelli/DYToEE_NNPDF30_13TeV_powheg_pythia8_13TeV_"+cat+".root");                        //  0T
  //TString theNewFile = TString::Format("/tmp/crovelli/Data_13TeV_"+cat+".root");                                                     // both
  cout << "OutputFile: " << theNewFile << endl;
  TFile *fileNew = TFile::Open(theNewFile,"RECREATE");
  TTree *treeNew = new TTree("DiPhotonTree","reduced tree for TnP plots");
  
  std::vector<TTree*> trees; 
  trees.push_back(treeNew);

  // original tree leaves
  Float_t weight = 0.;
  Float_t mass   = 0.;
  Float_t leadScEta    = 0.; 
  Float_t subleadScEta = 0.;          
  
  // List of branches - original tree
  TBranch  *b_weight;
  TBranch  *b_mass;
  TBranch  *b_leadScEta; 
  TBranch  *b_subleadScEta;  

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("weight",  &weight,  &b_weight);
  treeOrig->SetBranchAddress("mass",    &mass,    &b_mass);
  treeOrig->SetBranchAddress("leadScEta",    &leadScEta,    &b_leadScEta);     
  treeOrig->SetBranchAddress("subleadScEta", &subleadScEta, &b_subleadScEta);

  // New variables
  float mgg;
  int eventClass;

  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches
    theTreeNew->Branch("mgg", &mgg, "mgg/F");
    theTreeNew->Branch("eventClass", &eventClass, "eventClass/I");
    theTreeNew->Branch("weight", &weight, "weight/F");
  }

  for(int i=0; i<nentriesOrig; i++) {
    
    treeOrig->GetEntry(i);

    // EE-EE excluded
    if (fabs(leadScEta)>1.5 && fabs(subleadScEta)>1.5) continue;

    // classes
    eventClass = -1;
    if (fabs(leadScEta)<1.5 && fabs(subleadScEta)<1.5) eventClass = 0;
    else eventClass = 1;

    // 3.8T
    mgg = mass; 

    // 0T - to 'calibrate' the mass peak
    //if (eventClass==0) mgg = mass-0.01*mass;
    //if (eventClass==1) mgg = mass-0.005*mass;

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
