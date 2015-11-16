#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>

using namespace std;

void signalModelGenonlyFormat(const char* filename, TString genGenIso, TString kpl, TString theMass, TString cat) {

  cout << "Formatting " << filename << " from Pasquale's to our format for signal model studies - gen only scan" << endl;  

  TFile *fileOrig = 0;
  TTree *treeOrig = 0;
  
  fileOrig = TFile::Open(filename);
  if( fileOrig ) {
    fileOrig->cd();
    TString theTree = TString::Format("RSGravitonToGG_kMpl_"+kpl+"_M_"+theMass+"_TuneCUEP8M1_13TeV_pythia8_13TeV_"+cat);
    TString theDir  = TString::Format(genGenIso+"/trees/"+theTree);
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


  TString theNewFile = TString::Format("/tmp/crovelli/FormSigMod_"+genGenIso+"_kpl"+kpl+"_M"+theMass+"_cat"+cat+".root");
  cout << "OutputFile: " << theNewFile << endl;
  TFile *fileNew = TFile::Open(theNewFile,"RECREATE");
  TTree *treeNew = new TTree("DiPhotonTree","reduced tree for signal model studies at gen level only");
  
  std::vector<TTree*> trees; 
  trees.push_back(treeNew);

  // original tree leaves
  Float_t genMass = 0.;
  
  // List of branches - original tree
  TBranch  *b_genMass;

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("genMass",    &genMass,    &b_genMass);

  // New variables
  float mggGen;
  int eventClass;
  
  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches
    theTreeNew->Branch("mggGen", &mggGen, "mggGen/F");
    theTreeNew->Branch("eventClass", &eventClass, "eventClass/I");
  }

  for(int i=0; i<nentriesOrig; i++) {
    
    // if (i%10000 == 0) std::cout << ">>> Event # " << i << " / " << nentriesOrig << " entries" << std::endl; 
    treeOrig->GetEntry(i);

    eventClass = -1;
    if (cat=="EB") eventClass = 0;
    else if(cat=="EE") eventClass = 1;
    else eventClass = -10;

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
