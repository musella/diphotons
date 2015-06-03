#define saturationStudies_cxx
#include "saturationStudies.h"
#include <TH2.h>
#include <TF1.h>
#include <TProfile2D.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TLegend.h>
#include <TProfile.h>
#include <TGraphAsymmErrors.h>
#include <iostream> 

#include "myFunctions.cc"

using namespace std;

void saturationStudies::Loop()
{
  if (fChain == 0) return;

  // chiara: to be set
  bool wantFit = false;

  Long64_t nentries = fChain->GetEntriesFast();
  
  Long64_t nbytes = 0, nb = 0;
  
  // to make the eta spectrum flat
  // TH1F *H_ETA_befW = new TH1F("H_ETA_befW","H_ETA_befW",50,-2.5,2.5);
  // TH1F *H_ETA_aftW = new TH1F("H_ETA_aftW","H_ETA_aftW",50,-2.5,2.5);

  // E/Etrue with saturation
  TH1F *H_EOverEtrue_EB = new TH1F("H_EOverEtrue_EB",   "H_EOverEtrue_EB",   60,0.80,1.1);
  TH1F *H_EOverEtrue_EE = new TH1F("H_EOverEtrue_EE",   "H_EOverEtrue_EE",   60,0.80,1.1);
  H_EOverEtrue_EB->Sumw2();
  H_EOverEtrue_EE->Sumw2();
  H_EOverEtrue_EB->SetTitle("barrel");
  H_EOverEtrue_EE->SetTitle("endcap");
  H_EOverEtrue_EB->GetXaxis()->SetTitle("E / E_{true}");
  H_EOverEtrue_EE->GetXaxis()->SetTitle("E / E_{true}");

  TH1F *H_EOverEtrue_sat_EB = new TH1F("H_EOverEtrue_sat_EB",   "H_EOverEtrue_sat_EB",   60,0.80,1.1);
  TH1F *H_EOverEtrue_sat_EE = new TH1F("H_EOverEtrue_sat_EE",   "H_EOverEtrue_sat_EE",   60,0.80,1.1);
  H_EOverEtrue_sat_EB->Sumw2();
  H_EOverEtrue_sat_EE->Sumw2();
  H_EOverEtrue_sat_EB->SetTitle("barrel");
  H_EOverEtrue_sat_EE->SetTitle("endcap");
  H_EOverEtrue_sat_EB->GetXaxis()->SetTitle("E / E_{true}");
  H_EOverEtrue_sat_EE->GetXaxis()->SetTitle("E / E_{true}");

  TH1F *H_EOverEtrue_rec_EB = new TH1F("H_EOverEtrue_rec_EB",   "H_EOverEtrue_rec_EB",   60,0.80,1.1);
  TH1F *H_EOverEtrue_rec_EE = new TH1F("H_EOverEtrue_rec_EE",   "H_EOverEtrue_rec_EE",   60,0.80,1.1);
  H_EOverEtrue_rec_EB->Sumw2();
  H_EOverEtrue_rec_EE->Sumw2();
  H_EOverEtrue_rec_EB->SetTitle("barrel");
  H_EOverEtrue_rec_EE->SetTitle("endcap");
  H_EOverEtrue_rec_EB->GetXaxis()->SetTitle("E / E_{true}");
  H_EOverEtrue_rec_EE->GetXaxis()->SetTitle("E / E_{true}");


  // recovery by hand: comparisons between LErecover and NN recover - amplitude in the central xtal only
  TH2F *H2_recLeVsNN_EB = new TH2F("H2_recLeVsNN_EB", "H2_recLeVsNN_EB", 100, 0., 4000., 100, 0., 4000.); 
  TH2F *H2_recLeVsNN_EE = new TH2F("H2_recLeVsNN_EE", "H2_recLeVsNN_EE", 100, 0., 4000., 100, 0., 4000.); 
  H2_recLeVsNN_EB->SetTitle("central xtal amplitude, barrel");
  H2_recLeVsNN_EE->SetTitle("central xtal amplitude, endcap");
  H2_recLeVsNN_EB->GetXaxis()->SetTitle("NN recovered");
  H2_recLeVsNN_EB->GetYaxis()->SetTitle("LE recovered");
  H2_recLeVsNN_EE->GetXaxis()->SetTitle("NN recovered");
  H2_recLeVsNN_EE->GetYaxis()->SetTitle("LE recovered");

  // recovery by hand: comparisons between NN recover and do nothing - amplitude in the central xtal only
  TH2F *H2_noRecVsNN_EB = new TH2F("H2_noRecVsNN_EB", "H2_noRecVsNN_EB", 100, 0., 4000., 100, 0., 4000.); 
  TH2F *H2_noRecVsNN_EE = new TH2F("H2_noRecVsNN_EE", "H2_noRecVsNN_EE", 100, 0., 4000., 100, 0., 4000.); 
  H2_noRecVsNN_EB->SetTitle("central xtal amplitude, barrel");
  H2_noRecVsNN_EE->SetTitle("central xtal amplitude, endcap");
  H2_noRecVsNN_EB->GetXaxis()->SetTitle("NN recovered");
  H2_noRecVsNN_EB->GetYaxis()->SetTitle("No recovery");
  H2_noRecVsNN_EE->GetXaxis()->SetTitle("NN recovered");
  H2_noRecVsNN_EE->GetYaxis()->SetTitle("No recovery");


  // E5x5/Etrue with saturation - to compare with recovery by hand
  TH1F *H_E5x5OverEtrue_EB = new TH1F("H_E5x5OverEtrue_EB",   "H_E5x5OverEtrue_EB",   60,0.80,1.1);
  TH1F *H_E5x5OverEtrue_EE = new TH1F("H_E5x5OverEtrue_EE",   "H_E5x5OverEtrue_EE",   60,0.80,1.1);
  H_E5x5OverEtrue_EB->Sumw2();
  H_E5x5OverEtrue_EE->Sumw2();
  H_E5x5OverEtrue_EB->SetTitle("barrel");
  H_E5x5OverEtrue_EE->SetTitle("endcap");
  H_E5x5OverEtrue_EB->GetXaxis()->SetTitle("E5x5 / Etrue");
  H_E5x5OverEtrue_EE->GetXaxis()->SetTitle("E5x5 / Etrue");

  TH1F *H_E5x5OverEtrue_sat_EB = new TH1F("H_E5x5OverEtrue_sat_EB",   "H_E5x5OverEtrue_sat_EB",   60,0.80,1.1);
  TH1F *H_E5x5OverEtrue_sat_EE = new TH1F("H_E5x5OverEtrue_sat_EE",   "H_E5x5OverEtrue_sat_EE",   60,0.80,1.1);
  H_E5x5OverEtrue_sat_EB->Sumw2();
  H_E5x5OverEtrue_sat_EE->Sumw2();
  H_E5x5OverEtrue_sat_EB->SetTitle("barrel");
  H_E5x5OverEtrue_sat_EE->SetTitle("endcap");
  H_E5x5OverEtrue_sat_EB->GetXaxis()->SetTitle("E5x5 / Etrue");
  H_E5x5OverEtrue_sat_EE->GetXaxis()->SetTitle("E5x5 / Etrue");

  TH1F *H_E5x5OverEtrue_rec_EB = new TH1F("H_E5x5OverEtrue_rec_EB",   "H_E5x5OverEtrue_rec_EB",   60,0.80,1.1);
  TH1F *H_E5x5OverEtrue_rec_EE = new TH1F("H_E5x5OverEtrue_rec_EE",   "H_E5x5OverEtrue_rec_EE",   60,0.80,1.1);
  H_E5x5OverEtrue_rec_EB->Sumw2();
  H_E5x5OverEtrue_rec_EE->Sumw2();
  H_E5x5OverEtrue_rec_EB->SetTitle("barrel");
  H_E5x5OverEtrue_rec_EE->SetTitle("endcap");
  H_E5x5OverEtrue_rec_EB->GetXaxis()->SetTitle("E5x5 / Etrue");
  H_E5x5OverEtrue_rec_EE->GetXaxis()->SetTitle("E5x5 / Etrue");

  TH1F *H_E5x5OverEtrue_NNrec_EB = new TH1F("H_E5x5OverEtrue_NNrec_EB",   "H_E5x5OverEtrue_NNrec_EB",   60,0.80,1.1);
  TH1F *H_E5x5OverEtrue_NNrec_EE = new TH1F("H_E5x5OverEtrue_NNrec_EE",   "H_E5x5OverEtrue_NNrec_EE",   60,0.80,1.1);
  H_E5x5OverEtrue_NNrec_EB->Sumw2();
  H_E5x5OverEtrue_NNrec_EE->Sumw2();
  H_E5x5OverEtrue_NNrec_EB->SetTitle("barrel");
  H_E5x5OverEtrue_NNrec_EE->SetTitle("endcap");
  H_E5x5OverEtrue_NNrec_EB->GetXaxis()->SetTitle("E5x5 / Etrue");
  H_E5x5OverEtrue_NNrec_EE->GetXaxis()->SetTitle("E5x5 / Etrue");

  // saturation flags distribution
  TH1F *H_SatFlag = new TH1F("H_SatFlag","saturation flag distribution", 25,0.,25.);
  TH1F *H_LeRFlag = new TH1F("H_LeRFlag","LE recovering flag distribution", 25,0.,25.);
  H_SatFlag->GetXaxis()->SetTitle("crystal in the matrix");
  H_LeRFlag->GetXaxis()->SetTitle("crystal in the matrix");

  // saturation flags frequency
  TH1F *H_SatFlagNum = new TH1F("H_SatFlagNum","number of saturated xtals",   25,0.,25.);
  TH1F *H_LeRFlagNum = new TH1F("H_LeRFlagNum","number of LErecovered xtals", 25,0.,25.); 
  H_SatFlagNum->GetXaxis()->SetTitle("# of saturated xtals");
  H_LeRFlagNum->GetXaxis()->SetTitle("# of LE recovered xtals");

  // counters
  float totEvents   = 0;
  float satEvents   = 0;
  float LErecEvents = 0;
  float satEventsClose   = 0;
  float LErecEventsClose = 0;
  float satXtal[25], lerXtal[25];
  for (int ii=0; ii<25; ii++) {
    satXtal[ii]=0.;
    lerXtal[ii]=0.;
  }


  /*
  // 1st loop over entries: to compute weights
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    // restricting the analysis to photons matching MC truth
    if (mctruth_trueEta<-800) continue;

    // true photon must be in the ECAL fiducial region
    if (fabs(mctruth_trueEta)>2.5) continue;
    if (fabs(mctruth_trueEta)>1.4442 && fabs(mctruth_trueEta)<1.566) continue;

    // also the supercluster of the reco photon must be in the ECAL fiducial region
    if (fabs(supercluster_scEta)>2.5) continue;
    if (fabs(supercluster_scEta)>1.4442 &&fabs(supercluster_scEta)<1.566) continue;

    // filling the histo to cpmpute the eta weights
    H_ETA_befW->Fill(kinematics_eta);
  }
  */

  // preparing to compute weights
  // int xPeak   = H_ETA_befW -> GetMaximumBin();   
  // double xMax = H_ETA_befW -> GetBinContent(xPeak);  

  // 2nd loop over entries: analysis
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    // restricting the analysis to photons matching MC truth
    if (mctruth_trueEta<-800) continue;

    // true photon must be in the ECAL fiducial region
    if (fabs(mctruth_trueEta)>2.5) continue;
    if (fabs(mctruth_trueEta)>1.4442 && fabs(mctruth_trueEta)<1.566) continue;

    // also the supercluster of the reco photon must be in the ECAL fiducial region
    if (fabs(supercluster_scEta)>2.5) continue;
    if (fabs(supercluster_scEta)>1.4442 &&fabs(supercluster_scEta)<1.566) continue;

    // chiara
    // computing the weight to make eta flat
    // int thisBinEta = (int)( (kinematics_eta+2.5)/0.1 +1 );
    // double thisWeightEta = 1./(H_ETA_befW->GetBinContent(thisBinEta)/xMax);
    double thisWeightEta = 1.;


    // saturation studies => restricting to photons with pT>1000
    if (kinematics_pt<1000) continue;   // chiara 




    // counting events for saturation studies - no eta weight for the moment
    totEvents++;
      
    // # events with central xtal saturated
    if (tree5x5_kSaturated[12]==1)   satEvents++;    
    if (tree5x5_kLeRecovered[12]==1) LErecEvents++;
    
    // # events with another xtal saturated (but not the central one)
    bool satClose = false;
    bool leClose  = false;
    for (int ii=0; ii<25; ii++) {
      if (ii==12) continue;
      if (tree5x5_kSaturated[ii]==1 && tree5x5_kSaturated[12]==0)     satClose = true;
      if (tree5x5_kLeRecovered[ii]==1 && tree5x5_kLeRecovered[12]==0) leClose = true; 
    }
    if (satClose) satEventsClose++;
    if (leClose)  LErecEventsClose++;
    
    // # events with a given crystal in the matrix saturated
    for (int ii=0; ii<25; ii++) {
      if (tree5x5_kSaturated[ii]==1)   satXtal[ii]++;
      if (tree5x5_kLeRecovered[ii]==1) lerXtal[ii]++;
    }
    
    // # xtals with at least one flag per event
    float numSat   = 0;
    float numLeR   = 0;
    for (int ii=0; ii<25; ii++) {
      if (tree5x5_kSaturated[ii]==1)   numSat++;
      if (tree5x5_kLeRecovered[ii]==1) numLeR++;
    }
    H_SatFlagNum->Fill(numSat);
    H_LeRFlagNum->Fill(numLeR);


    // checks!
    if (numLeR>1) {
      for (int ii=0; ii<25; ii++) {
	if (tree5x5_kLeRecovered[ii]==1) 
	  cout << "more than 1 LE recovered xtal => ii " << ii 
	       << ", kLeRecovered[ii] = " << tree5x5_kLeRecovered[ii] << ", amplit = " << tree5x5_amplit[ii] << endl;
      }
    }
    if (numSat>1) {
      for (int ii=0; ii<25; ii++) {
	if (tree5x5_kSaturated[ii]==1) 
	  cout << "more than 1 saturated and not LE recovered xtal => ii " << ii 
	       << ", kSaturated[ii] = " << tree5x5_kSaturated[ii] << ", amplit = " << tree5x5_amplit[ii] << endl;
      }
    }


    // filling histos
    
    // check
    // H_ETA_aftW->Fill(kinematics_eta,thisWeightEta);

    // chiara: vedere se si puo controllare l'ampiezza saturazione su uncalibRH

    // 5x5 to study the corrections with the NN method
    // chiara: verificato che e' sempre uguale a e5x5 calcolata da lazy tools senza usare le frazioni
    // a parte quando ci sono delle flag che lazy tools esclude (per il caso noZS): kTowerRecovered, kWeird e kDiWeird (solo in EB)
    float myE5x5 = 0.;
    float myE5x5corr = 0.;
    for (int ii=0; ii<25; ii++) { 
      if (tree5x5_amplit[ii]>=-300)   // to esclude patological cases
	myE5x5 = myE5x5 + tree5x5_amplit[ii];
    }
    if (tree5x5_kSaturated[12]==1) 
      myE5x5corr = myE5x5 - tree5x5_amplit[12] + tree5x5_amplitRecovered[12];
    else
      myE5x5corr = myE5x5;
    
    // EB / EE
    if (fabs(kinematics_eta)<1.5) {

      // not saturated
      if (tree5x5_kSaturated[12]==0 && tree5x5_kLeRecovered[12]==0) {
	H_EOverEtrue_EB    -> Fill(energy_energy/mctruth_trueEnergy,thisWeightEta);
	H_E5x5OverEtrue_EB -> Fill(myE5x5/mctruth_trueEnergy,thisWeightEta);
      }      

      // central saturated and not recovered
      if (tree5x5_kSaturated[12]==1) {
	H_EOverEtrue_sat_EB      -> Fill(energy_energy/mctruth_trueEnergy,thisWeightEta);
	H_E5x5OverEtrue_sat_EB   -> Fill(myE5x5/mctruth_trueEnergy,thisWeightEta);
	H_E5x5OverEtrue_NNrec_EB -> Fill(myE5x5corr/mctruth_trueEnergy,thisWeightEta);
      }

      // central recovered with LE
      if (tree5x5_kLeRecovered[12]==1) {
	H_EOverEtrue_rec_EB    -> Fill(energy_energy/mctruth_trueEnergy,thisWeightEta);
	H_E5x5OverEtrue_rec_EB -> Fill(myE5x5/mctruth_trueEnergy,thisWeightEta);
      }

      // central recovered - comparison between LErecover and DeadchannelsNN recovered amplitude in the central xtal only
      // chiara: ci sono degli overflow da capire...
      if (tree5x5_kLeRecovered[12]==1) 
	H2_recLeVsNN_EB -> Fill(tree5x5_amplitRecovered[12], tree5x5_amplit[12]);  

      // central recovered - comparison between no recovery and DeadchannelsNN recovered amplitude in the central xtal only
      if (tree5x5_kSaturated[12]==1) 
	H2_noRecVsNN_EB -> Fill(tree5x5_amplitRecovered[12], tree5x5_amplit[12]);  

    } else {      // endcaps
      
      // all
      if (tree5x5_kSaturated[12]==0 && tree5x5_kLeRecovered[12]==0) {
	H_EOverEtrue_EE    -> Fill(energy_energy/mctruth_trueEnergy,thisWeightEta);
	H_E5x5OverEtrue_EE -> Fill(myE5x5/mctruth_trueEnergy,thisWeightEta);
      }

      // central saturated and not recovered      
      if (tree5x5_kSaturated[12]==1) { 
	H_EOverEtrue_sat_EE      -> Fill(energy_energy/mctruth_trueEnergy,thisWeightEta);
	H_E5x5OverEtrue_sat_EE   -> Fill(myE5x5/mctruth_trueEnergy,thisWeightEta);
	H_E5x5OverEtrue_NNrec_EE -> Fill(myE5x5corr/mctruth_trueEnergy,thisWeightEta);
      }

      // central recovered 
      if (tree5x5_kLeRecovered[12]==1) { 
	H_EOverEtrue_rec_EE -> Fill(energy_energy/mctruth_trueEnergy,thisWeightEta);
	H_E5x5OverEtrue_rec_EE -> Fill(myE5x5/mctruth_trueEnergy,thisWeightEta);
      }

      // central recovered - comparison between LErecover and DeadchannelsNN recovered amplitude in the central xtal only
      if (tree5x5_kLeRecovered[12]==1)
	H2_recLeVsNN_EE -> Fill(tree5x5_amplitRecovered[12], tree5x5_amplit[12]);  

      // central recovered - comparison between no recovery and DeadchannelsNN recovered amplitude in the central xtal only
      if (tree5x5_kSaturated[12]==1) 
	H2_noRecVsNN_EE -> Fill(tree5x5_amplitRecovered[12], tree5x5_amplit[12]);  
    }

  }  // loop over entries


  // preparing histo with saturation flags
  for (int ii=0; ii<25; ii++) {
    H_SatFlag ->SetBinContent(ii+1, satXtal[ii]);
    H_LeRFlag ->SetBinContent(ii+1, lerXtal[ii]);
  }

  cout << endl;
  cout << "# of events " << totEvents << endl;
  cout << "fraction of events with central xtal saturated: "    << satEvents/totEvents   << endl;
  cout << "fraction of events with central xtal LE recovered: " << LErecEvents/totEvents << endl;
  cout << endl;
  cout << "fraction of events with a xtal close to central one saturated (but not the central one): "    << satEventsClose/totEvents   << endl;
  cout << "fraction of events with a xtal close to central one LE recovered (but not the central one): " << LErecEventsClose/totEvents << endl;
  cout << endl;


  // Fits - barrel only
  if (wantFit) {
    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);
    for (int ii=2; ii<3; ii++) {
      
      TCanvas c1("c1","c1",1);
      c1.cd();
      
      TH1F *myH;
      if (ii==0) { myH = (TH1F*)H_EOverEtrue_EB->Clone();     myH->SetTitle("not saturated"); } 
      if (ii==1) { myH = (TH1F*)H_EOverEtrue_sat_EB->Clone(); myH->SetTitle("saturated, not recovered"); }
      if (ii==2) { myH = (TH1F*)H_EOverEtrue_rec_EB->Clone(); myH->SetTitle("LE recovered"); }
      myH->Draw("hist");
      
      // histo parameters                  
      int peakBin = myH->GetMaximumBin();
      double rms  = myH->GetRMS();
      double peak = myH->GetBinCenter(peakBin);
      double norm = myH->GetMaximum();
      
      // gaussian fit to initialize                                                                                                      
      TF1 *gausa;
      gausa = new TF1 ("gausa","[0]*exp(-1*(x-[1])*(x-[1])/2/[2]/[2])",peak-10*rms,peak+10*rms);
      gausa -> SetParameters(norm,peak,rms);
      myH->Fit("gausa","","",peak-0.5*rms,peak+3*rms);
      double gausNorm  = gausa->GetParameter(0);
      double gausMean  = gausa->GetParameter(1);
      double gausSigma = fabs(gausa->GetParameter(2));
      double gausChi2  = gausa->GetChisquare()/gausa->GetNDF();
      if (gausChi2>100) cout << "too large chi2" << endl;
      
      // crystalball limits                                                                                                              
      double myXmin = gausMean - 3.*gausSigma;
      double myXmax = gausMean + 2.*gausSigma;
      
      // crystalball fit                                                                                                                 
      TF1 *cb_p;
      cb_p = new TF1 ("cb_p",crystalball,myXmin,myXmax, 5) ;
      cb_p->SetParNames ("Mean","Sigma","alpha","n","Norm","Constant");
      cb_p->SetParameter(0, gausMean);
      cb_p->SetParameter(1, gausSigma);
      cb_p->FixParameter(3, 5.);
      cb_p->SetParameter(4, gausNorm);
      cb_p->SetParLimits(2, 0.1, 5.);
      myH->Fit("cb_p","lR","",myXmin,myXmax);
      
      myH->Draw("histsame");
      
      double matrix_gmean      = cb_p->GetParameter(0);
      double matrix_gsigma     = cb_p->GetParameter(1);
      double matrix_gmean_err  = cb_p->GetParError(0);
      double matrix_gsigma_err = cb_p->GetParError(1);
      
      cout << endl;
      cout << "CF fit: " << endl;
      cout << "mean: "  << matrix_gmean  << " +- " << matrix_gmean_err  << endl;
      cout << "sigma: " << matrix_gsigma << " +- " << matrix_gsigma_err << endl;
      cout << "chi2 = " << cb_p->GetChisquare()/cb_p->GetNDF();
      cout << endl;
      cout << "Effective Sigma = " << effectiveSigma(myH) << endl;
      
      c1.SaveAs("fit.png");
      
      delete cb_p;
      delete gausa;
    }
  }




  // Plots
  gStyle->SetOptStat(111111);
  // gStyle->SetOptStat(0);

  // cosmetics
  H_EOverEtrue_EB->SetLineColor(2);
  H_EOverEtrue_EE->SetLineColor(2);
  H_EOverEtrue_sat_EB->SetLineColor(3);
  H_EOverEtrue_sat_EE->SetLineColor(3);
  H_EOverEtrue_rec_EB->SetLineColor(4);
  H_EOverEtrue_rec_EE->SetLineColor(4);
  H_EOverEtrue_EB->SetLineWidth(2);
  H_EOverEtrue_EE->SetLineWidth(2);
  H_EOverEtrue_sat_EB->SetLineWidth(2);
  H_EOverEtrue_sat_EE->SetLineWidth(2);
  H_EOverEtrue_rec_EB->SetLineWidth(2);
  H_EOverEtrue_rec_EE->SetLineWidth(2);

  H_E5x5OverEtrue_EB->SetLineColor(2);
  H_E5x5OverEtrue_EE->SetLineColor(2);
  H_E5x5OverEtrue_sat_EB->SetLineColor(3);
  H_E5x5OverEtrue_sat_EE->SetLineColor(3);
  H_E5x5OverEtrue_rec_EB->SetLineColor(4);
  H_E5x5OverEtrue_rec_EE->SetLineColor(4);
  H_E5x5OverEtrue_NNrec_EB->SetLineColor(7);
  H_E5x5OverEtrue_NNrec_EE->SetLineColor(7);
  H_E5x5OverEtrue_EB->SetLineWidth(2);
  H_E5x5OverEtrue_EE->SetLineWidth(2);
  H_E5x5OverEtrue_sat_EB->SetLineWidth(2);
  H_E5x5OverEtrue_sat_EE->SetLineWidth(2);
  H_E5x5OverEtrue_rec_EB->SetLineWidth(2);
  H_E5x5OverEtrue_rec_EE->SetLineWidth(2);
  H_E5x5OverEtrue_NNrec_EB->SetLineWidth(2);
  H_E5x5OverEtrue_NNrec_EE->SetLineWidth(2);

  TLegend *leg;
  leg = new TLegend(0.15,0.6,0.40,0.85);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->AddEntry(H_EOverEtrue_EB,     "not saturated", "l");
  leg->AddEntry(H_EOverEtrue_sat_EB, "central xtal saturated", "l");
  leg->AddEntry(H_EOverEtrue_rec_EB, "central xtal LE recovered", "l");

  TLegend *leg2;
  leg2 = new TLegend(0.15,0.6,0.40,0.85);
  leg2->SetFillStyle(0);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.05);
  leg2->SetFillColor(0);
  leg2->AddEntry(H_E5x5OverEtrue_EB, "not saturated", "l");
  leg2->AddEntry(H_E5x5OverEtrue_sat_EB, "saturated", "l");
  leg2->AddEntry(H_E5x5OverEtrue_rec_EB, "LE recovered", "l");
  leg2->AddEntry(H_E5x5OverEtrue_NNrec_EB,  "NN recovered", "l");

  // ------------------------
  // TCanvas c0("c0","c0",1);  
  // c0.Divide(1,2);
  // c0.cd(1); H_ETA_befW->Draw();
  // c0.cd(2); H_ETA_aftW->Draw();
  // c0.SaveAs("etaCheck.png");


  // ------------------------
  // E/Etrue
  TCanvas c1a("c1a","",1);  
  H_EOverEtrue_EB     -> DrawNormalized("hist");
  H_EOverEtrue_sat_EB -> DrawNormalized("samehist");
  H_EOverEtrue_rec_EB -> DrawNormalized("samehist");
  leg->Draw();
  c1a.SaveAs("EoEtrue_saturation_EB.png");

  TCanvas c1d("c1d","",1);  
  H_EOverEtrue_rec_EE -> DrawNormalized("hist");
  H_EOverEtrue_EE     -> DrawNormalized("samehist");
  H_EOverEtrue_sat_EE -> DrawNormalized("samehist");
  leg->Draw();
  c1d.SaveAs("EoEtrue_saturation_EE.png");


  // ------------------------
  // flags
  TCanvas c2a("c2a","c2a",1);    
  H_SatFlag -> Draw();
  c2a.SaveAs("saturationByXtal.png");

  TCanvas c2b("c2b","c2b",1);    
  H_LeRFlag -> Draw();
  c2b.SaveAs("leRecoveryByXtal.png");

  TCanvas c2d("c2d","c2d",1);
  H_SatFlagNum -> Draw();
  c2d.SetLogy();
  c2d.SaveAs("numberOfXtalsWithSaturationFlag.png");

  TCanvas c2e("c2e","c2e",1);
  H_LeRFlagNum -> Draw();
  c2e.SetLogy();
  c2e.SaveAs("numberOfXtalsWithLErecoveryFlag.png");


  // ------------------------  
  // By hand recovery
  TCanvas c3a("c3a","c3a",1);
  H2_recLeVsNN_EB -> Draw("colz");
  c3a.SaveAs("LeVsNN_EB.png");

  TCanvas c3b("c3b","c3b",1);
  H2_recLeVsNN_EE -> Draw("colz");
  c3b.SaveAs("LeVsNN_EE.png");

  TCanvas c3c("c3c","c3c",1);
  H2_noRecVsNN_EB -> Draw("colz");
  c3c.SaveAs("noRecVsNN_EB.png");

  TCanvas c3d("c3d","c3d",1);
  H2_noRecVsNN_EE -> Draw("colz");
  c3d.SaveAs("noRecVsNN_EE.png");

  // E5x5/Etrue
  TCanvas c4a("c4a","",1);  
  H_E5x5OverEtrue_EB       -> DrawNormalized("hist");
  H_E5x5OverEtrue_sat_EB   -> DrawNormalized("samehist");
  H_E5x5OverEtrue_rec_EB   -> DrawNormalized("samehist");
  H_E5x5OverEtrue_NNrec_EB -> DrawNormalized("samehist");
  leg2->Draw();
  c4a.SaveAs("E5x5oEtrue_saturation_EB.png");

  TCanvas c4d("c4d","",1);  
  H_E5x5OverEtrue_rec_EE   -> DrawNormalized("hist");
  H_E5x5OverEtrue_EE       -> DrawNormalized("samehist");
  H_E5x5OverEtrue_sat_EE   -> DrawNormalized("samehist");
  H_E5x5OverEtrue_NNrec_EE -> DrawNormalized("samehist");
  leg2->Draw();
  c4d.SaveAs("E5x5oEtrue_saturation_EE.png");
}
