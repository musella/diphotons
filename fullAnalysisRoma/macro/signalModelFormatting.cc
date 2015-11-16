#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>

using namespace std;

void signalModelFormat(const char* filename, TString kinGenIso, TString kpl, TString theMass, TString cat) {

  cout << "Formatting " << filename << " from Pasquale's to our format for signal model studies" << endl;  

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
  
  // List of branches - original tree
  TBranch  *b_puweight;
  TBranch  *b_mass;
  TBranch  *b_genMass;
  TBranch  *b_leadEta;
  TBranch  *b_subleadEta;

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("puweight",   &puweight,   &b_puweight);
  treeOrig->SetBranchAddress("mass",       &mass,       &b_mass);
  treeOrig->SetBranchAddress("genMass",    &genMass,    &b_genMass);
  treeOrig->SetBranchAddress("leadEta",    &leadEta,    &b_leadEta);
  treeOrig->SetBranchAddress("subleadEta", &subleadEta, &b_subleadEta);

  // New variables
  float mgg, mggGen;
  int eventClass;
  
  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches
    theTreeNew->Branch("mgg", &mgg, "mgg/F");
    theTreeNew->Branch("mggGen", &mggGen, "mggGen/F");
    theTreeNew->Branch("eventClass", &eventClass, "eventClass/I");
    theTreeNew->Branch("puweight", &puweight, "puweight/F");
  }

  for(int i=0; i<nentriesOrig; i++) {
    
    // if (i%10000 == 0) std::cout << ">>> Event # " << i << " / " << nentriesOrig << " entries" << std::endl; 
    treeOrig->GetEntry(i);

    if (fabs(leadEta)>1.5 && fabs(subleadEta)>1.5) continue;

    eventClass = -1;
    if (fabs(leadEta)<1.5 && fabs(subleadEta)<1.5) eventClass = 0;
    else eventClass = 1;

    mgg = mass;
    mggGen = genMass;
    
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
