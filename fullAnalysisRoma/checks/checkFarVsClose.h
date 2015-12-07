//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Dec  1 16:45:53 2015 by ROOT version 6.02/05
// from TTree Data_13TeV_EEEE/Data_13TeV_EEEE
// found on file: /afs/cern.ch/user/c/crovelli/myWorkspace/public/EEEE/output.root
//////////////////////////////////////////////////////////

#ifndef checkFarVsClose_h
#define checkFarVsClose_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class checkFarVsClose {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           candidate_id;
   Float_t         weight;
   Float_t         mass;
   Float_t         pt;
   Float_t         rapidity;
   Float_t         satRegressedMass;
   Float_t         regressedMass;
   Float_t         genMass;
   Float_t         leadSatRegressedEnergy;
   Float_t         subLeadSatRegressedEnergy;
   Float_t         leadRegressedEnergy;
   Float_t         subLeadRegressedEnergy;
   Float_t         leadEnergy;
   Float_t         subLeadEnergy;
   Float_t         leadIsSat;
   Float_t         subLeadIsSat;
   Float_t         leadIsWeird;
   Float_t         subLeadIsWeird;
   Float_t         genLeadPt;
   Float_t         genSubLeadPt;
   Float_t         deltaEta;
   Float_t         cosDeltaPhi;
   Float_t         leadPt;
   Float_t         subleadPt;
   Float_t         leadEta;
   Float_t         subleadEta;
   Float_t         leadR9;
   Float_t         subleadR9;
   Float_t         leadScEta;
   Float_t         subleadScEta;
   Float_t         leadPhi;
   Float_t         subleadPhi;
   Float_t         leadCShapeMVA;
   Float_t         subleadCShapeMVA;
   Float_t         minR9;
   Float_t         maxEta;
   Float_t         leadBlockChIso;
   Float_t         leadBlockPhoIso;
   Float_t         leadPhoIsoEA;
   Float_t         subleadPhoIsoEA;
   Float_t         leadMatchType;
   Float_t         leadGenIso;
   Float_t         subleadMatchType;
   Float_t         subleadGenIso;
   Float_t         leadChIso;
   Float_t         leadPhoIso;
   Float_t         leadNeutIso;
   Float_t         leadHoE;
   Float_t         leadSigmaIeIe;
   Float_t         leadPixSeed;
   Float_t         leadPassEleVeto;
   Float_t         subleadBlockPhoIso;
   Float_t         subleadChIso;
   Float_t         subleadPhoIso;
   Float_t         subleadNeutIso;
   Float_t         subleadHoE;
   Float_t         subleadSigmaIeIe;
   Float_t         subleadPixSeed;
   Float_t         subleadPassEleVeto;
   Float_t         leadRndConeChIso;
   Float_t         leadRndConeChIso0;
   Float_t         leadRndConeChIso1;
   Float_t         leadRndConeChIso2;
   Float_t         leadRndConeChIso3;
   Float_t         leadRndConeChIso4;
   Float_t         leadRndConeChIso5;
   Float_t         leadRndConeChIso6;
   Float_t         leadRndConeChIso7;
   Float_t         leadRndConeChIso8;
   Float_t         subleadRndConeChIso;
   Float_t         subleadRndConeChIso0;
   Float_t         subleadRndConeChIso1;
   Float_t         subleadRndConeChIso2;
   Float_t         subleadRndConeChIso3;
   Float_t         subleadRndConeChIso4;
   Float_t         subleadRndConeChIso5;
   Float_t         subleadRndConeChIso6;
   Float_t         subleadRndConeChIso7;
   Float_t         subleadRndConeChIso8;
   Float_t         rho;
   UInt_t          event;
   UChar_t         lumi;
   UInt_t          run;
   Int_t           nvtx;
   Float_t         npu;
   Bool_t          eeBadScFilter;
   Bool_t          goodVertices;

   // List of branches
   TBranch        *b_candidate_id;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_mass;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_rapidity;   //!
   TBranch        *b_satRegressedMass;   //!
   TBranch        *b_regressedMass;   //!
   TBranch        *b_genMass;   //!
   TBranch        *b_leadSatRegressedEnergy;   //!
   TBranch        *b_subLeadSatRegressedEnergy;   //!
   TBranch        *b_leadRegressedEnergy;   //!
   TBranch        *b_subLeadRegressedEnergy;   //!
   TBranch        *b_leadEnergy;   //!
   TBranch        *b_subLeadEnergy;   //!
   TBranch        *b_leadIsSat;   //!
   TBranch        *b_subLeadIsSat;   //!
   TBranch        *b_leadIsWeird;   //!
   TBranch        *b_subLeadIsWeird;   //!
   TBranch        *b_genLeadPt;   //!
   TBranch        *b_genSubLeadPt;   //!
   TBranch        *b_deltaEta;   //!
   TBranch        *b_cosDeltaPhi;   //!
   TBranch        *b_leadPt;   //!
   TBranch        *b_subleadPt;   //!
   TBranch        *b_leadEta;   //!
   TBranch        *b_subleadEta;   //!
   TBranch        *b_leadR9;   //!
   TBranch        *b_subleadR9;   //!
   TBranch        *b_leadScEta;   //!
   TBranch        *b_subleadScEta;   //!
   TBranch        *b_leadPhi;   //!
   TBranch        *b_subleadPhi;   //!
   TBranch        *b_leadCShapeMVA;   //!
   TBranch        *b_subleadCShapeMVA;   //!
   TBranch        *b_minR9;   //!
   TBranch        *b_maxEta;   //!
   TBranch        *b_leadBlockChIso;   //!
   TBranch        *b_leadBlockPhoIso;   //!
   TBranch        *b_leadPhoIsoEA;   //!
   TBranch        *b_subleadPhoIsoEA;   //!
   TBranch        *b_leadMatchType;   //!
   TBranch        *b_leadGenIso;   //!
   TBranch        *b_subleadMatchType;   //!
   TBranch        *b_subleadGenIso;   //!
   TBranch        *b_leadChIso;   //!
   TBranch        *b_leadPhoIso;   //!
   TBranch        *b_leadNeutIso;   //!
   TBranch        *b_leadHoE;   //!
   TBranch        *b_leadSigmaIeIe;   //!
   TBranch        *b_leadPixSeed;   //!
   TBranch        *b_leadPassEleVeto;   //!
   TBranch        *b_subleadBlockPhoIso;   //!
   TBranch        *b_subleadChIso;   //!
   TBranch        *b_subleadPhoIso;   //!
   TBranch        *b_subleadNeutIso;   //!
   TBranch        *b_subleadHoE;   //!
   TBranch        *b_subleadSigmaIeIe;   //!
   TBranch        *b_subleadPixSeed;   //!
   TBranch        *b_subleadPassEleVeto;   //!
   TBranch        *b_leadRndConeChIso;   //!
   TBranch        *b_leadRndConeChIso0;   //!
   TBranch        *b_leadRndConeChIso1;   //!
   TBranch        *b_leadRndConeChIso2;   //!
   TBranch        *b_leadRndConeChIso3;   //!
   TBranch        *b_leadRndConeChIso4;   //!
   TBranch        *b_leadRndConeChIso5;   //!
   TBranch        *b_leadRndConeChIso6;   //!
   TBranch        *b_leadRndConeChIso7;   //!
   TBranch        *b_leadRndConeChIso8;   //!
   TBranch        *b_subleadRndConeChIso;   //!
   TBranch        *b_subleadRndConeChIso0;   //!
   TBranch        *b_subleadRndConeChIso1;   //!
   TBranch        *b_subleadRndConeChIso2;   //!
   TBranch        *b_subleadRndConeChIso3;   //!
   TBranch        *b_subleadRndConeChIso4;   //!
   TBranch        *b_subleadRndConeChIso5;   //!
   TBranch        *b_subleadRndConeChIso6;   //!
   TBranch        *b_subleadRndConeChIso7;   //!
   TBranch        *b_subleadRndConeChIso8;   //!
   TBranch        *b_rho;   //!
   TBranch        *b_event;   //!
   TBranch        *b_lumi;   //!
   TBranch        *b_run;   //!
   TBranch        *b_nvtx;   //!
   TBranch        *b_npu;   //!
   TBranch        *b_eeBadScFilter;   //!
   TBranch        *b_goodVertices;   //!

   checkFarVsClose(TTree *tree=0);
   virtual ~checkFarVsClose();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef checkFarVsClose_cxx
checkFarVsClose::checkFarVsClose(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/afs/cern.ch/user/c/crovelli/myWorkspace/public/EEEE/output.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/afs/cern.ch/user/c/crovelli/myWorkspace/public/EEEE/output.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("/afs/cern.ch/user/c/crovelli/myWorkspace/public/EEEE/output.root:/cic/trees");
      dir->GetObject("Data_13TeV_EELowR9",tree);

   }
   Init(tree);
}

checkFarVsClose::~checkFarVsClose()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t checkFarVsClose::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t checkFarVsClose::LoadTree(Long64_t entry)
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

void checkFarVsClose::Init(TTree *tree)
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

   fChain->SetBranchAddress("candidate_id", &candidate_id, &b_candidate_id);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   fChain->SetBranchAddress("mass", &mass, &b_mass);
   fChain->SetBranchAddress("pt", &pt, &b_pt);
   fChain->SetBranchAddress("rapidity", &rapidity, &b_rapidity);
   fChain->SetBranchAddress("satRegressedMass", &satRegressedMass, &b_satRegressedMass);
   fChain->SetBranchAddress("regressedMass", &regressedMass, &b_regressedMass);
   fChain->SetBranchAddress("genMass", &genMass, &b_genMass);
   fChain->SetBranchAddress("leadSatRegressedEnergy", &leadSatRegressedEnergy, &b_leadSatRegressedEnergy);
   fChain->SetBranchAddress("subLeadSatRegressedEnergy", &subLeadSatRegressedEnergy, &b_subLeadSatRegressedEnergy);
   fChain->SetBranchAddress("leadRegressedEnergy", &leadRegressedEnergy, &b_leadRegressedEnergy);
   fChain->SetBranchAddress("subLeadRegressedEnergy", &subLeadRegressedEnergy, &b_subLeadRegressedEnergy);
   fChain->SetBranchAddress("leadEnergy", &leadEnergy, &b_leadEnergy);
   fChain->SetBranchAddress("subLeadEnergy", &subLeadEnergy, &b_subLeadEnergy);
   fChain->SetBranchAddress("leadIsSat", &leadIsSat, &b_leadIsSat);
   fChain->SetBranchAddress("subLeadIsSat", &subLeadIsSat, &b_subLeadIsSat);
   fChain->SetBranchAddress("leadIsWeird", &leadIsWeird, &b_leadIsWeird);
   fChain->SetBranchAddress("subLeadIsWeird", &subLeadIsWeird, &b_subLeadIsWeird);
   fChain->SetBranchAddress("genLeadPt", &genLeadPt, &b_genLeadPt);
   fChain->SetBranchAddress("genSubLeadPt", &genSubLeadPt, &b_genSubLeadPt);
   fChain->SetBranchAddress("deltaEta", &deltaEta, &b_deltaEta);
   fChain->SetBranchAddress("cosDeltaPhi", &cosDeltaPhi, &b_cosDeltaPhi);
   fChain->SetBranchAddress("leadPt", &leadPt, &b_leadPt);
   fChain->SetBranchAddress("subleadPt", &subleadPt, &b_subleadPt);
   fChain->SetBranchAddress("leadEta", &leadEta, &b_leadEta);
   fChain->SetBranchAddress("subleadEta", &subleadEta, &b_subleadEta);
   fChain->SetBranchAddress("leadR9", &leadR9, &b_leadR9);
   fChain->SetBranchAddress("subleadR9", &subleadR9, &b_subleadR9);
   fChain->SetBranchAddress("leadScEta", &leadScEta, &b_leadScEta);
   fChain->SetBranchAddress("subleadScEta", &subleadScEta, &b_subleadScEta);
   fChain->SetBranchAddress("leadPhi", &leadPhi, &b_leadPhi);
   fChain->SetBranchAddress("subleadPhi", &subleadPhi, &b_subleadPhi);
   fChain->SetBranchAddress("leadCShapeMVA", &leadCShapeMVA, &b_leadCShapeMVA);
   fChain->SetBranchAddress("subleadCShapeMVA", &subleadCShapeMVA, &b_subleadCShapeMVA);
   fChain->SetBranchAddress("minR9", &minR9, &b_minR9);
   fChain->SetBranchAddress("maxEta", &maxEta, &b_maxEta);
   fChain->SetBranchAddress("leadBlockChIso", &leadBlockChIso, &b_leadBlockChIso);
   fChain->SetBranchAddress("leadBlockPhoIso", &leadBlockPhoIso, &b_leadBlockPhoIso);
   fChain->SetBranchAddress("leadPhoIsoEA", &leadPhoIsoEA, &b_leadPhoIsoEA);
   fChain->SetBranchAddress("subleadPhoIsoEA", &subleadPhoIsoEA, &b_subleadPhoIsoEA);
   fChain->SetBranchAddress("leadMatchType", &leadMatchType, &b_leadMatchType);
   fChain->SetBranchAddress("leadGenIso", &leadGenIso, &b_leadGenIso);
   fChain->SetBranchAddress("subleadMatchType", &subleadMatchType, &b_subleadMatchType);
   fChain->SetBranchAddress("subleadGenIso", &subleadGenIso, &b_subleadGenIso);
   fChain->SetBranchAddress("leadChIso", &leadChIso, &b_leadChIso);
   fChain->SetBranchAddress("leadPhoIso", &leadPhoIso, &b_leadPhoIso);
   fChain->SetBranchAddress("leadNeutIso", &leadNeutIso, &b_leadNeutIso);
   fChain->SetBranchAddress("leadHoE", &leadHoE, &b_leadHoE);
   fChain->SetBranchAddress("leadSigmaIeIe", &leadSigmaIeIe, &b_leadSigmaIeIe);
   fChain->SetBranchAddress("leadPixSeed", &leadPixSeed, &b_leadPixSeed);
   fChain->SetBranchAddress("leadPassEleVeto", &leadPassEleVeto, &b_leadPassEleVeto);
   fChain->SetBranchAddress("subleadBlockPhoIso", &subleadBlockPhoIso, &b_subleadBlockPhoIso);
   fChain->SetBranchAddress("subleadChIso", &subleadChIso, &b_subleadChIso);
   fChain->SetBranchAddress("subleadPhoIso", &subleadPhoIso, &b_subleadPhoIso);
   fChain->SetBranchAddress("subleadNeutIso", &subleadNeutIso, &b_subleadNeutIso);
   fChain->SetBranchAddress("subleadHoE", &subleadHoE, &b_subleadHoE);
   fChain->SetBranchAddress("subleadSigmaIeIe", &subleadSigmaIeIe, &b_subleadSigmaIeIe);
   fChain->SetBranchAddress("subleadPixSeed", &subleadPixSeed, &b_subleadPixSeed);
   fChain->SetBranchAddress("subleadPassEleVeto", &subleadPassEleVeto, &b_subleadPassEleVeto);
   fChain->SetBranchAddress("leadRndConeChIso", &leadRndConeChIso, &b_leadRndConeChIso);
   fChain->SetBranchAddress("leadRndConeChIso0", &leadRndConeChIso0, &b_leadRndConeChIso0);
   fChain->SetBranchAddress("leadRndConeChIso1", &leadRndConeChIso1, &b_leadRndConeChIso1);
   fChain->SetBranchAddress("leadRndConeChIso2", &leadRndConeChIso2, &b_leadRndConeChIso2);
   fChain->SetBranchAddress("leadRndConeChIso3", &leadRndConeChIso3, &b_leadRndConeChIso3);
   fChain->SetBranchAddress("leadRndConeChIso4", &leadRndConeChIso4, &b_leadRndConeChIso4);
   fChain->SetBranchAddress("leadRndConeChIso5", &leadRndConeChIso5, &b_leadRndConeChIso5);
   fChain->SetBranchAddress("leadRndConeChIso6", &leadRndConeChIso6, &b_leadRndConeChIso6);
   fChain->SetBranchAddress("leadRndConeChIso7", &leadRndConeChIso7, &b_leadRndConeChIso7);
   fChain->SetBranchAddress("leadRndConeChIso8", &leadRndConeChIso8, &b_leadRndConeChIso8);
   fChain->SetBranchAddress("subleadRndConeChIso", &subleadRndConeChIso, &b_subleadRndConeChIso);
   fChain->SetBranchAddress("subleadRndConeChIso0", &subleadRndConeChIso0, &b_subleadRndConeChIso0);
   fChain->SetBranchAddress("subleadRndConeChIso1", &subleadRndConeChIso1, &b_subleadRndConeChIso1);
   fChain->SetBranchAddress("subleadRndConeChIso2", &subleadRndConeChIso2, &b_subleadRndConeChIso2);
   fChain->SetBranchAddress("subleadRndConeChIso3", &subleadRndConeChIso3, &b_subleadRndConeChIso3);
   fChain->SetBranchAddress("subleadRndConeChIso4", &subleadRndConeChIso4, &b_subleadRndConeChIso4);
   fChain->SetBranchAddress("subleadRndConeChIso5", &subleadRndConeChIso5, &b_subleadRndConeChIso5);
   fChain->SetBranchAddress("subleadRndConeChIso6", &subleadRndConeChIso6, &b_subleadRndConeChIso6);
   fChain->SetBranchAddress("subleadRndConeChIso7", &subleadRndConeChIso7, &b_subleadRndConeChIso7);
   fChain->SetBranchAddress("subleadRndConeChIso8", &subleadRndConeChIso8, &b_subleadRndConeChIso8);
   fChain->SetBranchAddress("rho", &rho, &b_rho);
   fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
   fChain->SetBranchAddress("run", &run, &b_run);
//    fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
   fChain->SetBranchAddress("npu", &npu, &b_npu);
   fChain->SetBranchAddress("eeBadScFilter", &eeBadScFilter, &b_eeBadScFilter);
   fChain->SetBranchAddress("goodVertices", &goodVertices, &b_goodVertices);
   Notify();
}

Bool_t checkFarVsClose::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void checkFarVsClose::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t checkFarVsClose::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef checkFarVsClose_cxx
