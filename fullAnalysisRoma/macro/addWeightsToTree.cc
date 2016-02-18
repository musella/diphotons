#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1.h>
#include <iostream>

using namespace std;

void addWeights(const char* filename, float lumiForW, float massTrue=1) {

  cout << "Adding weight branch to file " << filename << endl;  

  TFile *fileOrig = 0;
  TTree *treeOrig = 0;
  TH1F  *h_entries = 0;
  TH1F  *h_sumW = 0;
  TH1F  *h_selection = 0;

  fileOrig = TFile::Open(filename);
  if( fileOrig ) {
    fileOrig->cd();
    treeOrig = (TTree*)fileOrig->Get("diPhoAna/DiPhotonTree");
    h_entries = (TH1F*)fileOrig->Get("diPhoAna/h_entries");
    h_sumW    = (TH1F*)fileOrig->Get("diPhoAna/h_sumW");
    h_selection = (TH1F*)fileOrig->Get("diPhoAna/h_selection");
  } else {
    cout << "File " << filename << " not existing !" << endl;
    return;
  }


  fileOrig->cd();
  if (!treeOrig) {
    cout << "Tree diPhoAna/DiPhotonTree not existing !" << endl; 
    return;    
  }
  
  float sampleSumWeight = (float)h_sumW->Integral();    // sum of weights in the dataset we ran on originally

  int nentriesOrig = treeOrig->GetEntries();   // number fo entries saved in the first tree
  
  TFile *fileNew = TFile::Open(filename,"recreate");
  TTree *treeNew = new TTree("DiPhotonTree","tree with 2 photon selection");
  
  std::vector<TTree*> trees; 
  trees.push_back(treeNew);
  
  // original tree leaves
  Int_t           run;
  Int_t           event;
  Int_t           lumi;
  Int_t           nvtx;
  Float_t         rho;
  Int_t           sampleID;
  Float_t         totXsec; 
  Float_t         pu_weight;
  Float_t         pu_n;
  Float_t         sumDataset;
  Float_t         perEveW;
  Float_t         ptgg;
  Float_t         mgg;
  Int_t           eventClass;
  Float_t         pt1;
  Float_t         ptOverM1;
  Float_t         eta1;
  Float_t         phi1;
  Float_t         sceta1;
  Float_t         r91;
  Float_t         sieie1;
  Float_t         hoe1;
  Float_t         scRawEne1;
  Float_t         chiso1;
  Float_t         phoiso1;
  Float_t         neuiso1;
  Float_t         pt2;
  Float_t         ptOverM2;
  Float_t         eta2;
  Float_t         phi2;
  Float_t         sceta2;
  Float_t         r92;
  Float_t         sieie2;
  Float_t         hoe2;
  Float_t         scRawEne2;
  Float_t         chiso2;
  Float_t         phoiso2;
  Float_t         neuiso2;
  Int_t           presel1;
  Int_t           presel2;
  Int_t           sel1;
  Int_t           sel2;
  Float_t         genmgg;
  Int_t           genmatch1;
  Int_t           genmatch2;
  Float_t         geniso1;
  Float_t         geniso2;
  Int_t           vtxIndex;
  Float_t         vtxX;
  Float_t         vtxY;
  Float_t         vtxZ;
  Float_t         genVtxX;
  Float_t         genVtxY;
  Float_t         genVtxZ;
  
  // List of branches - original tree
  TBranch        *b_run; 
  TBranch        *b_event;
  TBranch        *b_lumi;
  TBranch        *b_nvtx;
  TBranch        *b_rho; 
  TBranch        *b_sampleID; 
  TBranch        *b_totXsec;
  TBranch        *b_pu_weight;
  TBranch        *b_pu_n;
  TBranch        *b_sumDataset;
  TBranch        *b_perEveW;
  TBranch        *b_ptgg;
  TBranch        *b_mgg; 
  TBranch        *b_eventClass; 
  TBranch        *b_pt1; 
  TBranch        *b_ptOverM1;
  TBranch        *b_eta1; 
  TBranch        *b_phi1; 
  TBranch        *b_sceta1; 
  TBranch        *b_r91; 
  TBranch        *b_sieie1;
  TBranch        *b_hoe1; 
  TBranch        *b_scRawEne1; 
  TBranch        *b_chiso1;  
  TBranch        *b_phoiso1; 
  TBranch        *b_neuiso1;   
  TBranch        *b_pt2; 
  TBranch        *b_ptOverM2;
  TBranch        *b_eta2; 
  TBranch        *b_phi2;
  TBranch        *b_sceta2;  
  TBranch        *b_r92; 
  TBranch        *b_sieie2;
  TBranch        *b_hoe2; 
  TBranch        *b_scRawEne2; 
  TBranch        *b_chiso2;  
  TBranch        *b_phoiso2; 
  TBranch        *b_neuiso2;   
  TBranch        *b_presel1;
  TBranch        *b_presel2;
  TBranch        *b_sel1;
  TBranch        *b_sel2;
  TBranch        *b_genmgg; 
  TBranch        *b_genmatch1; 
  TBranch        *b_genmatch2; 
  TBranch        *b_geniso1; 
  TBranch        *b_geniso2; 
  TBranch        *b_vtxIndex; 
  TBranch        *b_vtxX;
  TBranch        *b_vtxY;
  TBranch        *b_vtxZ;
  TBranch        *b_genVtxX; 
  TBranch        *b_genVtxY; 
  TBranch        *b_genVtxZ; 

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("run", &run, &b_run);
  treeOrig->SetBranchAddress("event", &event, &b_event);
  treeOrig->SetBranchAddress("lumi", &lumi, &b_lumi);
  treeOrig->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
  treeOrig->SetBranchAddress("rho", &rho, &b_rho);
  treeOrig->SetBranchAddress("sampleID", &sampleID, &b_sampleID);
  treeOrig->SetBranchAddress("totXsec", &totXsec, &b_totXsec);
  treeOrig->SetBranchAddress("pu_weight", &pu_weight, &b_pu_weight);
  treeOrig->SetBranchAddress("pu_n", &pu_n, &b_pu_n);
  treeOrig->SetBranchAddress("sumDataset", &sumDataset, &b_sumDataset);
  treeOrig->SetBranchAddress("perEveW", &perEveW, &b_perEveW);
  treeOrig->SetBranchAddress("ptgg", &ptgg, &b_ptgg);
  treeOrig->SetBranchAddress("mgg", &mgg, &b_mgg);
  treeOrig->SetBranchAddress("eventClass", &eventClass, &b_eventClass);
  treeOrig->SetBranchAddress("pt1", &pt1, &b_pt1);
  treeOrig->SetBranchAddress("ptOverM1", &ptOverM1, &b_ptOverM1);
  treeOrig->SetBranchAddress("eta1", &eta1, &b_eta1);
  treeOrig->SetBranchAddress("phi1", &phi1, &b_phi1);
  treeOrig->SetBranchAddress("sceta1", &sceta1, &b_sceta1);
  treeOrig->SetBranchAddress("r91", &r91, &b_r91);
  treeOrig->SetBranchAddress("sieie1", &sieie1, &b_sieie1);
  treeOrig->SetBranchAddress("hoe1", &hoe1, &b_hoe1);
  treeOrig->SetBranchAddress("scRawEne1", &scRawEne1, &b_scRawEne1);
  treeOrig->SetBranchAddress("chiso1", &chiso1, &b_chiso1);
  treeOrig->SetBranchAddress("phoiso1", &phoiso1, &b_phoiso1);
  treeOrig->SetBranchAddress("neuiso1", &neuiso1, &b_neuiso1);
  treeOrig->SetBranchAddress("pt2", &pt2, &b_pt2);
  treeOrig->SetBranchAddress("ptOverM2", &ptOverM2, &b_ptOverM2);
  treeOrig->SetBranchAddress("eta2", &eta2, &b_eta2);
  treeOrig->SetBranchAddress("phi2", &phi2, &b_phi2);
  treeOrig->SetBranchAddress("sceta2", &sceta2, &b_sceta2);
  treeOrig->SetBranchAddress("r92", &r92, &b_r92);
  treeOrig->SetBranchAddress("sieie2", &sieie2, &b_sieie2);
  treeOrig->SetBranchAddress("hoe2", &hoe2, &b_hoe2);
  treeOrig->SetBranchAddress("scRawEne2", &scRawEne2, &b_scRawEne2);
  treeOrig->SetBranchAddress("chiso2", &chiso2, &b_chiso2);
  treeOrig->SetBranchAddress("phoiso2", &phoiso2, &b_phoiso2);
  treeOrig->SetBranchAddress("neuiso2", &neuiso2, &b_neuiso2);
  treeOrig->SetBranchAddress("presel1",&presel1,&b_presel1);
  treeOrig->SetBranchAddress("presel2",&presel2,&b_presel2);
  treeOrig->SetBranchAddress("sel1",&sel1,&b_sel1);
  treeOrig->SetBranchAddress("sel2",&sel2,&b_sel2);
  treeOrig->SetBranchAddress("genmgg", &genmgg, &b_genmgg);
  treeOrig->SetBranchAddress("genmatch1", &genmatch1, &b_genmatch1);
  treeOrig->SetBranchAddress("genmatch2", &genmatch2, &b_genmatch2);
  treeOrig->SetBranchAddress("geniso1", &geniso1, &b_geniso1);
  treeOrig->SetBranchAddress("geniso2", &geniso2, &b_geniso2);
  treeOrig->SetBranchAddress("vtxIndex", &vtxIndex, &b_vtxIndex);
  treeOrig->SetBranchAddress("vtxX", &vtxX, &b_vtxX);
  treeOrig->SetBranchAddress("vtxY", &vtxY, &b_vtxY);
  treeOrig->SetBranchAddress("vtxZ", &vtxZ, &b_vtxZ);
  treeOrig->SetBranchAddress("genVtxX", &genVtxX, &b_genVtxX);
  treeOrig->SetBranchAddress("genVtxY", &genVtxY, &b_genVtxY);
  treeOrig->SetBranchAddress("genVtxZ", &genVtxZ, &b_genVtxZ);

  // new variables to be added
  Float_t xsecWeight;
  Float_t weight;
  Float_t mggNominal;
  Float_t mggGen;

  // xsec to weight histos
  Float_t xsecToWeight = 0.;

  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches
    theTreeNew->Branch("xsecWeight", &xsecWeight, "xsecWeight/F");
    theTreeNew->Branch("weight", &weight, "weight/F");
    theTreeNew->Branch("mggNominal", &mggNominal, "mggNominal/F");
    theTreeNew->Branch("mggGen", &mggGen, "mggGen/F");
    
    // Copy branches
    theTreeNew->Branch("run", &run, "run/I");
    theTreeNew->Branch("event", &event, "event/I");
    theTreeNew->Branch("lumi", &lumi, "lumi/I");
    theTreeNew->Branch("nvtx", &nvtx, "nvtx/I");
    theTreeNew->Branch("rho", &rho, "rho/F");
    theTreeNew->Branch("sampleID", &sampleID, "sampleID/I");
    theTreeNew->Branch("totXsec", &totXsec, "totXsec/F");
    theTreeNew->Branch("pu_weight", &pu_weight, "pu_weight/F");
    theTreeNew->Branch("pu_n", &pu_n, "pu_n/F");
    theTreeNew->Branch("sumDataset", &sumDataset, "sumDataset/F");
    theTreeNew->Branch("perEveW", &perEveW, "perEveW/F");
    theTreeNew->Branch("ptgg", &ptgg, "ptgg/F");
    theTreeNew->Branch("mgg", &mgg, "mgg/F");
    theTreeNew->Branch("eventClass", &eventClass, "eventClass/I");
    theTreeNew->Branch("pt1", &pt1, "pt1/F");
    theTreeNew->Branch("ptOverM1", &ptOverM1, "ptOverM1/F");
    theTreeNew->Branch("eta1", &eta1, "eta1/F");
    theTreeNew->Branch("phi1", &phi1, "phi1/F");
    theTreeNew->Branch("sceta1", &sceta1, "sceta1/F");
    theTreeNew->Branch("r91", &r91, "r91/F");
    theTreeNew->Branch("sieie1", &sieie1, "sieie1/F");
    theTreeNew->Branch("hoe1", &hoe1, "hoe1/F");
    theTreeNew->Branch("scRawEne1", &scRawEne1, "scRawEne1/F");
    theTreeNew->Branch("chiso1", &chiso1, "chiso1/F");
    theTreeNew->Branch("phoiso1", &phoiso1, "phoiso1/F");
    theTreeNew->Branch("neuiso1", &neuiso1, "neuiso1/F");
    theTreeNew->Branch("pt2", &pt2, "pt2/F");
    theTreeNew->Branch("ptOverM2", &ptOverM2, "ptOverM2/F");
    theTreeNew->Branch("eta2", &eta2, "eta2/F");
    theTreeNew->Branch("phi2", &phi2, "phi2/F");
    theTreeNew->Branch("sceta2", &sceta2, "sceta2/F");
    theTreeNew->Branch("r92", &r92, "r92/F");
    theTreeNew->Branch("sieie2", &sieie2, "sieie2/F");
    theTreeNew->Branch("hoe2", &hoe2, "hoe2/F");
    theTreeNew->Branch("scRawEne2", &scRawEne2, "scRawEne2/F");
    theTreeNew->Branch("chiso2", &chiso2, "chiso2/F");
    theTreeNew->Branch("phoiso2", &phoiso2, "phoiso2/F");
    theTreeNew->Branch("neuiso2", &neuiso2, "neuiso2/F");
    theTreeNew->Branch("presel1",&presel1,"presel1/I");
    theTreeNew->Branch("presel2",&presel2,"presel2/I");
    theTreeNew->Branch("sel1",&sel1,"sel1/I");
    theTreeNew->Branch("sel2",&sel2,"sel2/I");
    theTreeNew->Branch("genmatch1", &genmatch1, "genmatch1/I");
    theTreeNew->Branch("genmatch2", &genmatch2, "genmatch2/I");
    theTreeNew->Branch("geniso1", &geniso1, "geniso1/F");
    theTreeNew->Branch("geniso2", &geniso2, "geniso2/F");
    theTreeNew->Branch("vtxIndex", &vtxIndex, "vtxIndex/I");
    theTreeNew->Branch("vtxX", &vtxX, "vtxX/F");
    theTreeNew->Branch("vtxY", &vtxY, "vtxY/F");
    theTreeNew->Branch("vtxZ", &vtxZ, "vtxZ/F");
    theTreeNew->Branch("genVtxX", &genVtxX, "genVtxX/F");
    theTreeNew->Branch("genVtxY", &genVtxY, "genVtxY/F");
    theTreeNew->Branch("genVtxZ", &genVtxZ, "genVtxZ/F");
  }
  
  for(int i=0; i<nentriesOrig; i++) {

    if (i%10000 == 0) std::cout << ">>> Weighting event # " << i << " / " << nentriesOrig << " entries" << std::endl; 
    treeOrig->GetEntry(i);

    if (i==0) xsecToWeight = totXsec;
   
    // new variables
    if (sampleID>0 && sampleID<10000) {   // MC
      xsecWeight = perEveW * lumiForW * totXsec / sampleSumWeight;             
      weight     = xsecWeight * pu_weight;
      mggNominal = massTrue;
      mggGen     = genmgg;
    } else {   
      xsecWeight = 1.;
      weight     = 1.;
      mggNominal = 1.;
      mggGen     = 1.;
    }

    treeNew->Fill();
  }

  // histo scaling to get the correct normalization
  h_selection->Scale( xsecToWeight / sampleSumWeight );  

  // new info
  fileNew->cd();
  h_entries->Write();
  h_sumW->Write();
  h_selection->Write();
  treeNew->Write();
  fileNew->Close();

  fileOrig->cd();
  fileOrig->Close();
  
}


