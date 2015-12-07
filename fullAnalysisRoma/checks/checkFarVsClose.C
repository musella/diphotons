#define checkFarVsClose_cxx
#include "checkFarVsClose.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>

void checkFarVsClose::Loop()
{
  TH1F *myH_all   = new TH1F("myH_all",  "myH_all"  ,68,300,2000);
  TH1F *myH_far   = new TH1F("myH_far",  "myH_far",  68,300,2000);
  TH1F *myH_close = new TH1F("myH_close","myH_close",68,300,2000);

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      myH_all->Fill(mass,weight); 

      //if (leadEta*subleadEta>0) myH_close->Fill(mass,weight);
      //else myH_far->Fill(mass,weight); 
      
      if (fabs(leadEta-subleadEta)<1.) myH_close->Fill(mass,weight);
      else myH_far->Fill(mass,weight); 
   }

   myH_all->SetLineColor(1);
   myH_far->SetLineColor(2);
   myH_close->SetLineColor(4); 
   myH_all->SetLineWidth(2);
   myH_far->SetLineWidth(2);
   myH_close->SetLineWidth(2); 
   myH_close->SetTitle("");
   myH_close->GetXaxis()->SetTitle("m_{#gamma#gamma}");

   gStyle->SetOptStat(0);

   TLegend *leg;
   leg = new TLegend(0.65,0.65,0.80,0.85);
   leg->SetFillStyle(0);
   leg->SetBorderSize(0);
   leg->SetTextSize(0.05);
   leg->SetFillColor(0);
   leg->AddEntry(myH_all,"all events","lp");
   leg->AddEntry(myH_close,"|#Delta#eta|<1","lp");
   leg->AddEntry(myH_far,"|#Delta#eta|>1","lp");
   //leg->AddEntry(myH_close,"same EE","lp");
   //leg->AddEntry(myH_far,"different EE","lp");

   TCanvas c("c","c",1);
   c.SetLogy();
   c.SetLogx();
   myH_close->DrawNormalized("pE");  
   myH_all->DrawNormalized("samepE");
   myH_far->DrawNormalized("samepE");  
   leg->Draw();
   c.SaveAs("test.png");
}
