#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1.h>
#include <iostream>

using namespace std;

void tnpTreeFormat(const char* filename, float lumiForW) {

  cout << "Formatting " << filename << endl;  

  TFile *fileOrig = 0;
  TTree *treeOrig = 0;
  TH1F  *h_sumW = 0;
  
  fileOrig = TFile::Open(filename);
  if( fileOrig ) {
    fileOrig->cd();
    treeOrig = (TTree*)fileOrig->Get("tnpAna/TaPtree");
    h_sumW = (TH1F*)fileOrig->Get("tnpAna/h_sumW");
  } else {
    cout << "File " << filename << " not existing !" << endl;
    return;
  }

  fileOrig->cd();
  if (!treeOrig) {
    cout << "Tree tnpAna/TaPTree not existing !" << endl; 
    return;    
  }

  treeOrig->SetMakeClass(0);
  
  // number of entries saved in the first tree
  int nentriesOrig = treeOrig->GetEntries();   
  
  TFile *fileNew = TFile::Open(TString("Formatted_")+TString(filename),"recreate");
  TTree *treeNew = new TTree("TaPTree","reduced tree for T&P");
  
  std::vector<TTree*> trees; 
  trees.push_back(treeNew);

  // sum of weights in the dataset we ran on originally   
  float sampleSumWeight = (float)h_sumW->Integral();   

  // original tree leaves
  Int_t           run   = 0;
  Int_t           event = 0;
  Int_t           lumi  = 0;
  Int_t           nvtx  = 0;
  Float_t         pu_weight = 0;
  Float_t         perEveW   = 0;
  float           totXsec = 0;
  vector<float>   *electron_eta = 0;
  vector<float>   *electron_pt = 0;
  vector<bool>    *isTagMediumEle    = 0;
  vector<bool>    *isTagTightEle     = 0;
  vector<bool>    *electron_matchHLT = 0;
  vector<bool>    *electron_matchMC  = 0;
  vector<float>   *gamma_pt  = 0;
  vector<float>   *gamma_eta = 0;
  vector<int>     *gamma_presel   = 0;
  vector<int>     *gamma_fullsel  = 0;
  vector<bool>    *gamma_matchMC  = 0;
  vector<bool>    *gamma_kSaturated = 0;
  vector<float>   *invMass    = 0;
  vector<int>     *eleIndex   = 0;
  vector<int>     *gammaIndex = 0;

  // List of branches - original tree
  TBranch        *b_run; 
  TBranch        *b_event;
  TBranch        *b_lumi;
  TBranch        *b_nvtx;
  TBranch        *b_pu_weight;
  TBranch        *b_perEveW;
  TBranch        *b_totXsec;
  TBranch        *b_electron_eta;   //!   
  TBranch        *b_electron_pt;   //!   
  TBranch        *b_isTagMediumEle;  
  TBranch        *b_isTagTightEle;
  TBranch        *b_electron_matchHLT; 
  TBranch        *b_electron_matchMC;
  TBranch        *b_gamma_pt;   //!  
  TBranch        *b_gamma_eta;   //!   
  TBranch        *b_gamma_presel; 
  TBranch        *b_gamma_fullsel; 
  TBranch        *b_gamma_matchMC;
  TBranch        *b_gamma_kSaturated;
  TBranch        *b_invMass; 
  TBranch        *b_eleIndex;  
  TBranch        *b_gammaIndex;

  // Set branch addresses and branch pointers 
  treeOrig->SetBranchAddress("run", &run, &b_run);
  treeOrig->SetBranchAddress("event", &event, &b_event);
  treeOrig->SetBranchAddress("lumi", &lumi, &b_lumi);
  treeOrig->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
  treeOrig->SetBranchAddress("pu_weight", &pu_weight, &b_pu_weight);
  treeOrig->SetBranchAddress("perEveW", &perEveW, &b_perEveW);
  treeOrig->SetBranchAddress("totXsec", &totXsec, &b_totXsec);
  treeOrig->SetBranchAddress("electron_eta", &electron_eta, &b_electron_eta);
  treeOrig->SetBranchAddress("electron_pt", &electron_pt, &b_electron_pt);
  treeOrig->SetBranchAddress("isTagMediumEle", &isTagMediumEle, &b_isTagMediumEle);
  treeOrig->SetBranchAddress("isTagTightEle", &isTagTightEle, &b_isTagTightEle);
  treeOrig->SetBranchAddress("electron_matchHLT", &electron_matchHLT, &b_electron_matchHLT);
  treeOrig->SetBranchAddress("electron_matchMC", &electron_matchMC, &b_electron_matchMC);
  treeOrig->SetBranchAddress("gamma_pt", &gamma_pt, &b_gamma_pt);
  treeOrig->SetBranchAddress("gamma_eta", &gamma_eta, &b_gamma_eta);
  treeOrig->SetBranchAddress("gamma_presel", &gamma_presel, &b_gamma_presel);
  treeOrig->SetBranchAddress("gamma_fullsel", &gamma_fullsel, &b_gamma_fullsel);
  treeOrig->SetBranchAddress("gamma_matchMC", &gamma_matchMC, &b_gamma_matchMC);        
  treeOrig->SetBranchAddress("gamma_kSaturated", &gamma_kSaturated, &b_gamma_kSaturated);        
  treeOrig->SetBranchAddress("invMass", &invMass, &b_invMass);
  treeOrig->SetBranchAddress("eleIndex", &eleIndex, &b_eleIndex);
  treeOrig->SetBranchAddress("gammaIndex", &gammaIndex, &b_gammaIndex);

  // New variables
  float tag_pt, tag_absEta;
  int tag_matchMC;
  float probe_pt, probe_absEta;
  int probe_matchMC, probe_kSaturated;
  int probe_fullsel;
  float mass;
  float xsecWeight, weight;
  
  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches
    theTreeNew->Branch("run", &run, "run/I");
    theTreeNew->Branch("event", &event, "event/I");
    theTreeNew->Branch("lumi", &lumi, "lumi/I");
    theTreeNew->Branch("nvtx", &nvtx, "nvtx/I");
    theTreeNew->Branch("pu_weight", &pu_weight, "pu_weight/F");
    theTreeNew->Branch("tag_pt",&tag_pt,"tag_pt/F");
    theTreeNew->Branch("tag_absEta",&tag_absEta,"tag_absEta/F");
    theTreeNew->Branch("tag_matchMC",&tag_matchMC,"tag_matchMC/I");
    theTreeNew->Branch("probe_pt",&probe_pt,"probe_pt/F");
    theTreeNew->Branch("probe_absEta",&probe_absEta,"probe_absEta/F");
    theTreeNew->Branch("probe_fullsel", &probe_fullsel, "probe_fullsel/I");
    theTreeNew->Branch("probe_matchMC",&probe_matchMC,"probe_matchMC/I");
    theTreeNew->Branch("probe_kSaturated",&probe_kSaturated,"probe_kSaturated/I");
    theTreeNew->Branch("mass", &mass, "mass/F");
    theTreeNew->Branch("xsecWeight", &xsecWeight, "xsecWeight/F");
    theTreeNew->Branch("weight", &weight, "weight/F");
  }

  for(int i=0; i<nentriesOrig; i++) {
    
    if (i%10000 == 0) std::cout << ">>> Event # " << i << " / " << nentriesOrig << " entries" << std::endl; 
    treeOrig->GetEntry(i);

    for (unsigned int ii=0; ii<invMass->size(); ii++) {
      
      mass = (float)(invMass->at(ii));
      if (mass<60 || mass>120) continue;
      // if (mass<60 || mass>4000) continue;
      
      // further selection on tag 
      if (electron_pt->at(eleIndex->at(ii))<30)     continue;
      if (!isTagTightEle->at(eleIndex->at(ii)))     continue;
      if (!electron_matchHLT->at(eleIndex->at(ii))) continue;   
      
      // further selection on probe
      if (!gamma_presel->at(gammaIndex->at(ii)))   continue;
      
      // now making flat tree
      tag_absEta = fabs(electron_eta->at(eleIndex->at(ii)));
      tag_pt = electron_pt->at(eleIndex->at(ii));
      tag_matchMC = electron_matchMC->at(eleIndex->at(ii));
      probe_pt = gamma_pt->at(gammaIndex->at(ii));
      probe_absEta  = fabs(gamma_eta->at(gammaIndex->at(ii)));
      probe_fullsel = gamma_fullsel->at(gammaIndex->at(ii));  
      probe_matchMC = gamma_matchMC->at(gammaIndex->at(ii));
      probe_kSaturated = gamma_kSaturated->at(gammaIndex->at(ii));

      // weights
      if (run==1) {   // MC                                                                                                                   
	xsecWeight = perEveW * lumiForW * totXsec / sampleSumWeight;
	weight     = xsecWeight * pu_weight;
      } else {
	xsecWeight = 1.;
	weight     = 1.;
      }

      treeNew->Fill();
      // treeNew->GetEntries();
    }
  }


  // new info
  fileNew->ls();
  fileNew->cd();
  TDirectory *myDir = (TDirectory*)fileNew->mkdir("tnpAna");
  myDir->cd();
  treeNew->Write();
  fileNew->Close();
  fileNew->ls();

  fileOrig->cd();
  fileOrig->Close();  
}
