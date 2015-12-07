#define angular_cxx
#include "angular.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void angular::Loop()
{
   if (fChain == 0) return;

   TH1F *Hin_cosThetaStarLead = new TH1F("Hin_cosThetaStarLead","Hin_cosThetaStarLead",100,-1,1);
   TH1F *Hin_cosThetaStarSublead = new TH1F("Hin_cosThetaStarSublead","Hin_cosThetaStarSublead",100,-1,1);
   TH1F *Hin_deltaEta = new TH1F("Hin_deltaEta","Hin_deltaEta",100,-5,5);
   TH1F *Hin_deltaPhi = new TH1F("Hin_deltaPhi","Hin_deltaPhi",50,-1,1);
   
   TH1F *Hout_cosThetaStarLead = new TH1F("Hout_cosThetaStarLead","Hout_cosThetaStarLead",100,-1,1);
   TH1F *Hout_cosThetaStarSublead = new TH1F("Hout_cosThetaStarSublead","Hout_cosThetaStarSublead",100,-1,1);
   TH1F *Hout_deltaEta = new TH1F("Hout_deltaEta","Hout_deltaEta",100,-5,5);
   TH1F *Hout_deltaPhi = new TH1F("Hout_deltaPhi","Hout_deltaPhi",50,-1,1);

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      TLorentzVector myLeadGamma(0,0,0,0);  
      myLeadGamma.SetPtEtaPhiM(leadPt, leadEta, leadPhi, 0.);
      TLorentzVector mySubleadGamma(0,0,0,0);  
      mySubleadGamma.SetPtEtaPhiM(subleadPt, subleadEta, subleadPhi, 0.);

      TLorentzVector grav = myLeadGamma + mySubleadGamma;

      TLorentzVector lead_Gstar(myLeadGamma);
      lead_Gstar.Boost(-grav.BoostVector());
      TLorentzVector sublead_Gstar(mySubleadGamma);
      sublead_Gstar.Boost(-grav.BoostVector());

      float leadCosThStar = lead_Gstar.CosTheta();
      float subleadCosThStar = sublead_Gstar.CosTheta();

      float deltaEta = leadEta-subleadEta;
      float deltaPhi = myLeadGamma.DeltaPhi(mySubleadGamma);
      float cosDPhi = cos(deltaPhi);

      // for data
      if (puweight==0) puweight=1;
      
      if (fabs(mass-755)<20){ 
	Hin_cosThetaStarLead    -> Fill(leadCosThStar,puweight);
	Hin_cosThetaStarSublead -> Fill(subleadCosThStar,puweight);
	Hin_deltaEta->Fill(deltaEta,puweight);
	Hin_deltaPhi->Fill(cosDPhi,puweight);
      } else {
	Hout_cosThetaStarLead    -> Fill(leadCosThStar,puweight);
	Hout_cosThetaStarSublead -> Fill(subleadCosThStar,puweight);
	Hout_deltaEta->Fill(deltaEta,puweight);
	Hout_deltaPhi->Fill(cosDPhi,puweight);
      }
   }

   TFile outfile("outfile.root","RECREATE");
   Hout_cosThetaStarLead->Write();
   Hout_cosThetaStarSublead->Write();
   Hout_deltaEta->Write();
   Hout_deltaPhi->Write();
   Hin_cosThetaStarLead->Write();
   Hin_cosThetaStarSublead->Write();
   Hin_deltaEta->Write();
   Hin_deltaPhi->Write();
}
