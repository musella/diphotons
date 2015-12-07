//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Nov 29 00:17:36 2015 by ROOT version 6.02/05
// from TTree DiPhotonTree/reduced tree for signal model studies
// found on file: FormSigMod_kpl001_M750.root
//////////////////////////////////////////////////////////

#ifndef angular_h
#define angular_h

#include <TLorentzVector.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class angular {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Float_t         puweight;
   Float_t         mass;
   Float_t         leadEta;
   Float_t         subleadEta;
   Float_t         leadPhi;
   Float_t         subleadPhi;
   Float_t         leadPt;
   Float_t         subleadPt;
   Int_t           eventClass;

   // List of branches
   TBranch        *b_puweight;   //!
   TBranch        *b_mass;   //!
   TBranch        *b_leadEta;   //!
   TBranch        *b_subleadEta;   //!
   TBranch        *b_leadPhi;   //!
   TBranch        *b_subleadPhi;   //!
   TBranch        *b_leadPt;   //!
   TBranch        *b_subleadPt;   //!
   TBranch        *b_eventClass;   //!

   angular(TTree *tree=0);
   virtual ~angular();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef angular_cxx
angular::angular(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
     TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("FormChecks_kpl001_M750.root");
     //TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("FormChecks_data.root");
     if (!f || !f->IsOpen()) {
       f = new TFile("FormChecks_kpl001_M750.root");
       //f = new TFile("FormChecks_data.root");
     }
     f->GetObject("DiPhotonTree",tree);
   }
   Init(tree);
}

angular::~angular()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t angular::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t angular::LoadTree(Long64_t entry)
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

void angular::Init(TTree *tree)
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

   fChain->SetBranchAddress("puweight", &puweight, &b_puweight);
   fChain->SetBranchAddress("mass", &mass, &b_mass);
   fChain->SetBranchAddress("leadEta", &leadEta, &b_leadEta);
   fChain->SetBranchAddress("subleadEta", &subleadEta, &b_subleadEta);
   fChain->SetBranchAddress("leadPhi", &leadPhi, &b_leadPhi);
   fChain->SetBranchAddress("subleadPhi", &subleadPhi, &b_subleadPhi);
   fChain->SetBranchAddress("leadPt", &leadPt, &b_leadPt);
   fChain->SetBranchAddress("subleadPt", &subleadPt, &b_subleadPt);
   fChain->SetBranchAddress("eventClass", &eventClass, &b_eventClass);
   Notify();
}

Bool_t angular::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void angular::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t angular::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef angular_cxx
