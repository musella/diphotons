//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 19 18:13:45 2016 by ROOT version 6.02/13
// from TTree TaPTree/reduced tree for T&P
// found on file: /afs/cern.ch/user/c/crovelli/myWorkspace/public/TaP_fall15_Moriond16_v1/Formatted_DYLL_all1pb.root
//////////////////////////////////////////////////////////

#ifndef templateMaker_h
#define templateMaker_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class templateMaker {
 public :
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain
  
  // Fixed size dimensions of array or collections stored in the TTree if any.
  
  // Declaration of leaf types
  Int_t           run;
  Int_t           event;
  Int_t           lumi;
  Int_t           nvtx;
  Float_t         pu_weight;
  Float_t         tag_pt;
  Float_t         tag_absEta;
  Int_t           tag_matchMC;
  Float_t         probe_pt;
  Float_t         probe_absEta;
  Int_t           probe_fullsel;
  Int_t           probe_matchMC;
  Int_t           probe_kSaturated;
  Int_t           probe_eleveto;
  Float_t         mass;
  Float_t         massRaw;
  Float_t         xsecWeight;
  Float_t         weight;
  
  // List of branches
  TBranch        *b_run;   //!
  TBranch        *b_event;   //!
  TBranch        *b_lumi;   //!
  TBranch        *b_nvtx;   //!
  TBranch        *b_pu_weight;   //!
  TBranch        *b_tag_pt;   //!
  TBranch        *b_tag_absEta;   //!
  TBranch        *b_tag_matchMC;   //!
  TBranch        *b_probe_pt;   //!
  TBranch        *b_probe_absEta;   //!
  TBranch        *b_probe_fullsel;   //!
  TBranch        *b_probe_matchMC;   //!
  TBranch        *b_probe_kSaturated;   //!
  TBranch        *b_probe_eleveto;   //!
  TBranch        *b_mass;   //!
  TBranch        *b_massRaw;   //!
  TBranch        *b_xsecWeight;   //!
  TBranch        *b_weight;   //!
  
  templateMaker(TTree *tree=0);
  virtual ~templateMaker();
  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual void     Loop();
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef templateMaker_cxx
templateMaker::templateMaker(TTree *tree) : fChain(0) 
{
  // if parameter tree is not specified (or zero), connect the file
  // used to generate this class and read the Tree.
  if (tree == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/afs/cern.ch/user/c/crovelli/myWorkspace/public/TaP_fall15_Moriond16_v1/formattedZ/Formatted_DYLL_largeAndExtension__all1pb.root");
    if (!f || !f->IsOpen()) {
      f = new TFile("/afs/cern.ch/user/c/crovelli/myWorkspace/public/TaP_fall15_Moriond16_v1/formattedZ/Formatted_DYLL_largeAndExtension__all1pb.root");
    }
    TDirectory * dir = (TDirectory*)f->Get("/afs/cern.ch/user/c/crovelli/myWorkspace/public/TaP_fall15_Moriond16_v1/formattedZ/Formatted_DYLL_largeAndExtension__all1pb.root:/tnpAna");
    dir->GetObject("TaPTree",tree);
  }
  Init(tree);
}

templateMaker::~templateMaker()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

Int_t templateMaker::GetEntry(Long64_t entry)
{
  // Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}
Long64_t templateMaker::LoadTree(Long64_t entry)
{
  // Set the environment to read one entry
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (fChain->GetTreeNumber() != fCurrent) {
    fCurrent = fChain->GetTreeNumber();
    Notify();
  }
  return centry;
}

void templateMaker::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses and branch
  // pointers of the tree will be set.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).
  
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);
  
  fChain->SetBranchAddress("run", &run, &b_run);
  fChain->SetBranchAddress("event", &event, &b_event);
  fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
  fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
  fChain->SetBranchAddress("pu_weight", &pu_weight, &b_pu_weight);
  fChain->SetBranchAddress("tag_pt", &tag_pt, &b_tag_pt);
  fChain->SetBranchAddress("tag_absEta", &tag_absEta, &b_tag_absEta);
  fChain->SetBranchAddress("tag_matchMC", &tag_matchMC, &b_tag_matchMC);
  fChain->SetBranchAddress("probe_pt", &probe_pt, &b_probe_pt);
  fChain->SetBranchAddress("probe_absEta", &probe_absEta, &b_probe_absEta);
  fChain->SetBranchAddress("probe_fullsel", &probe_fullsel, &b_probe_fullsel);
  fChain->SetBranchAddress("probe_matchMC", &probe_matchMC, &b_probe_matchMC);
  fChain->SetBranchAddress("probe_kSaturated", &probe_kSaturated, &b_probe_kSaturated);
  fChain->SetBranchAddress("probe_eleveto", &probe_eleveto, &b_probe_eleveto);
  fChain->SetBranchAddress("mass", &mass, &b_mass);
  fChain->SetBranchAddress("massRaw", &massRaw, &b_massRaw);
  fChain->SetBranchAddress("xsecWeight", &xsecWeight, &b_xsecWeight);
  fChain->SetBranchAddress("weight", &weight, &b_weight);
  Notify();
}

Bool_t templateMaker::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.
  
  return kTRUE;
}

void templateMaker::Show(Long64_t entry)
{
  // Print contents of entry.
  // If entry is not specified, print current entry
  if (!fChain) return;
  fChain->Show(entry);
}

Int_t templateMaker::Cut(Long64_t entry)
{
  // This function may be called from Loop.
  // returns  1 if entry is accepted.
  // returns -1 otherwise.
  return 1;
}

#endif // #ifdef templateMaker_cxx
