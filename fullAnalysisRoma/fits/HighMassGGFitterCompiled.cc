#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooCBShape.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooAddPdf.h"
#include "RooBreitWigner.h"
#include "RooFFTConvPdf.h"
#include "RooHistFunc.h"
#include "RooHistPdf.h"
#include "RooFitResult.h"
#include "RooStats/HLFactory.h"

#include "HiggsAnalysis/CombinedLimit/interface/HZZ2L2QRooPdfs.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphAsymmErrors.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TPaveText.h"

#include <fstream>

using namespace RooFit;
using namespace RooStats;

// ============================================
// to be modified:
static const Int_t NCAT = 4;  
Int_t MINmass= 500;
Int_t MAXmass= 6000;
Float_t Lum = 19500.0;    
bool wantResponse = 0;
bool wantGenLevel = 0;
// ============================================


// Definition of the variables in the input ntuple
RooArgSet* defineVariables() {

  RooRealVar* mgg        = new RooRealVar("mgg",        "M(gg)",       MINmass, MAXmass, "GeV");
  RooRealVar* mggGen     = new RooRealVar("mggGen",     "M(gg) gen",   MINmass, MAXmass, "GeV");
  RooRealVar* eventClass = new RooRealVar("eventClass", "eventClass",    -10,      10,   "");
  RooRealVar* weight     = new RooRealVar("weight",     "weightings",      0,     1000,  "");   // chiara

  RooArgSet* ntplVars = new RooArgSet(*mgg, *mggGen, *eventClass, *weight);                     // chiara
  // RooArgSet* ntplVars = new RooArgSet(*mgg, *mggGen, *eventClass);
  
  return ntplVars;
}

void SetConstantParams(const RooArgSet* params) {
  
  cout << endl; cout << "Entering SetConstantParams" << endl;
  TIterator* iter(params->createIterator());
  for (TObject *a = iter->Next(); a != 0; a = iter->Next()) {
    RooRealVar *rrv = dynamic_cast<RooRealVar *>(a);
    if (rrv) { rrv->setConstant(true); std::cout << " " << rrv->GetName(); }
  }  
}

// CMS stuffs
TPaveText* get_labelCMS( int legendQuadrant, std::string year, bool sim) {
  
  if( legendQuadrant!=0 && legendQuadrant!=1 && legendQuadrant!=2 && legendQuadrant!=3 ) {
    std::cout << "WARNING! Legend quadrant '" << legendQuadrant << "' not yet implemented for CMS label. Using 2." << std::endl;
    legendQuadrant = 2;
  }
  
  float x1=0.;
  float y1=0.;
  float x2=0.;
  float y2=0.;
  if( legendQuadrant==1 ) {
    x1 = 0.63;
    y1 = 0.83;
    x2 = 0.8;
    y2 = 0.87;
  } else if( legendQuadrant==0 ) {
    x1 = 0.175;
    y1 = 0.953;
    x2 = 0.6;
    y2 = 0.975;
  } else if( legendQuadrant==2 ) {
    x1 = 0.25;
    y1 = 0.2;
    x2 = 0.42;
  }
 
  TPaveText* cmslabel = new TPaveText( x1, y1, x2, y2, "" );
  cmslabel->SetFillColor(kWhite);
  cmslabel->SetTextSize(0.038);
  if( legendQuadrant==0 ) cmslabel->SetTextAlign(11);
  cmslabel->SetTextSize(0.038);
  cmslabel->SetTextFont(42);
  
  std::string leftText;
   
  if (sim)  leftText = "CMS Simulation"; 
  else {
    leftText = "CMS Preliminary, xxx fb^{-1}";
  }
  cmslabel->AddText(leftText.c_str());
  return cmslabel;
}

TPaveText* get_labelSqrt( int legendQuadrant ) {

  if( legendQuadrant!=0 && legendQuadrant!=1 && legendQuadrant!=2 ) {
    std::cout << "WARNING! Legend quadrant '" << legendQuadrant << "' not yet implemented for Sqrt label. Using 2." << std::endl;
    legendQuadrant = 2;
  }

  float x1=0.;
  float y1=0.;
  float x2=0.;
  float y2=0.;
  if( legendQuadrant==1 ) {
    x1 = 0.63;
    y1 = 0.78;
    x2 = 0.8;
    y2 = 0.82;
  } else if( legendQuadrant==2 ) {
    x1 = 0.25;
    y1 = 0.78;
    x2 = 0.42;
    y2 = 0.82;
  } else if( legendQuadrant==0 ) {
    x1 = 0.65;
    y1 = 0.953;
    x2 = 0.87;
    y2 = 0.975;
  }

  TPaveText* label_sqrt = new TPaveText(x1,y1,x2,y2, "");
  label_sqrt->SetFillColor(kWhite);
  label_sqrt->SetTextSize(0.038);
  label_sqrt->SetTextFont(42);
  label_sqrt->SetTextAlign(31); 
  label_sqrt->AddText("#sqrt{s} = 13 TeV");
  return label_sqrt;
}

// loading signal data and making roodatasets
void AddSigData(RooWorkspace* w, Float_t mass, TString coupling) {
  
  Int_t ncat = NCAT;
  
  // Variables
  RooArgSet* ntplVars = defineVariables();

  // -------------------------  
  // Files
  int iMass = abs(mass);   
  TString inDir = "data/newSelection/mergedFinal/";
  TChain* sigTree = new TChain();
  cout << "reading file " << inDir+TString(Form("RSGravToGG_kMpl-"))+coupling+TString(Form("_M-%d.root/DiPhotonTree", iMass)) << endl;
  sigTree->Add(inDir+TString(Form("RSGravToGG_kMpl-"))+coupling+TString(Form("_M-%d.root/DiPhotonTree", iMass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");


  // -------------------------
  // common preselection cut on mgg and mggGen
  TString mainCut1 = TString::Format("mgg>=500 && mgg<=6000 && mggGen>=500 && mggGen<=6000");   
  RooDataSet sigWeighted("sigWeighted","dataset",sigTree,*ntplVars,mainCut1,"weight");   // chiara!
  // RooDataSet sigWeighted("sigWeighted","dataset",sigTree,*ntplVars,mainCut1);


  // -------------------------
  // reduced mass
  RooFormulaVar *massReduced_formula = new RooFormulaVar("massReduced_formula","","@0/@1",RooArgList(*w->var("mgg"),*w->var("mggGen")));
  RooRealVar* massReduced = (RooRealVar*) sigWeighted.addColumn(*massReduced_formula);
  massReduced->SetName("massReduced");
  massReduced->SetTitle("massReduced");
  w->import(*massReduced);  
  massReduced->setRange(0.5, 1.5);
  
  // common preselection cut on the reduced mass
  TString mainCut = TString::Format("massReduced>0.5 && massReduced <1.5"); 

  
  // -------------------------
  // split in categories, wrt mgg - this is the dataset to be used for the convolution
  cout << endl;
  cout << "preparing dataset with observable mgg" << endl;
  RooDataSet* signal[NCAT];
  for (int c=0; c<ncat; ++c) {
    if (c==0) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==0"));
    if (c==1) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==1"));
    if (c==2) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==2"));
    if (c==3) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==3"));

    TString myCut;
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    
    w->import(*signal[c],Rename("SigWeight_cat"+myCut));
    
    cout << "cat " << c << ", signal[c]: " << endl;
    signal[c]->Print("v");
    cout << "---- for category " << c << ", nX for signal[c]:  " << signal[c]->sumEntries() << endl; 
    cout << endl;
  }

  // Create full weighted signal data set without categorization
  RooDataSet* signalAll = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut);
  w->import(*signalAll, Rename("SigWeight"));
  cout << "now signalAll" << endl;
  signalAll->Print("v");
  cout << "---- nX for signalAll:  " << signalAll->sumEntries() << endl; 
  cout << endl;


  // -------------------------
  // split in categories, wrt massReduced - to study the detector response
  if (wantResponse) {
    cout << endl;
    cout << endl;
    cout << "preparing dataset with observable massReduced" << endl;
    RooDataSet* signalR[NCAT];
    for (int c=0; c<ncat; ++c) {
      if (c==0) signalR[c] = (RooDataSet*) sigWeighted.reduce(*w->var("massReduced"),mainCut+TString::Format("&& eventClass==0"));
      if (c==1) signalR[c] = (RooDataSet*) sigWeighted.reduce(*w->var("massReduced"),mainCut+TString::Format("&& eventClass==1"));
      if (c==2) signalR[c] = (RooDataSet*) sigWeighted.reduce(*w->var("massReduced"),mainCut+TString::Format("&& eventClass==2"));
      if (c==3) signalR[c] = (RooDataSet*) sigWeighted.reduce(*w->var("massReduced"),mainCut+TString::Format("&& eventClass==3"));

      TString myCut;
      if (c==0)      myCut = "EBHighR9";
      else if (c==1) myCut = "EBLowR9";
      else if (c==2) myCut = "EEHighR9";
      else if (c==3) myCut = "EELowR9";    
      w->import(*signalR[c],Rename("SigWeightReduced_cat"+myCut));
    }
    cout << endl;
  }


  // -------------------------
  // split in categories, wrt genMass - to study the theory width
  if (wantGenLevel) { 
    cout << endl;
    cout << endl;
    cout << "preparing dataset with observable mggGen, no split in categories since they're all the same" << endl;
    RooDataSet* signalGAll = (RooDataSet*) sigWeighted.reduce(*w->var("mggGen"),mainCut);
    w->import(*signalGAll, Rename("SigWeightGen"));
    cout << endl;
  }

  cout << "workspace summary" << endl;
  w->Print();
}

// Signal model: doubleCB. To describe the detector resolution, reco mgg used
void SigModelResponseDoubleCBFit(RooWorkspace* w, Float_t mass, TString coupling) {
  
  int iMass = abs(mass);   
  
  // dataset
  RooDataSet* signal[NCAT];
  RooRealVar* mgg = w->var("mgg");     
  
  // fit function
  RooDoubleCB* ResponseDoubleCB[NCAT];

  TCanvas* c1 = new TCanvas("c1", "c1", 1);
  c1->cd();

  for(int c = 0; c<NCAT; c++){

    TString myCut = "EBHighR9";
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    
    
    // loading the dataset
    signal[c] = (RooDataSet*) w->data("SigWeight_cat"+myCut); 

    // DoubleCB
    RooFormulaVar CBmean("CB_sig_mean_cat"+myCut,"", "@0", *w->var("Sig_mean_cat"+myCut));
    RooFormulaVar CBsigma("CB_sig_sigma_cat"+myCut, "", "sqrt(@0*@0)", *w->var("Sig_sigma_cat"+myCut));
    RooFormulaVar CBalpha1("CB_sig_alpha1_cat"+myCut,"", "@0", *w->var("Sig_alpha1_cat"+myCut));
    RooFormulaVar CBn1("CB_sig_n1_cat"+myCut,"", "@0", *w->var("Sig_n1_cat"+myCut));
    RooFormulaVar CBalpha2("CB_sig_alpha2_cat"+myCut,"", "@0", *w->var("Sig_alpha2_cat"+myCut));
    RooFormulaVar CBn2("CB_sig_n1_cat"+myCut,"", "@0", *w->var("Sig_n2_cat"+myCut));
    ResponseDoubleCB[c] = new RooDoubleCB("ResponseDoubleCB_cat"+myCut,"ResponseDoubleCB_cat"+myCut, *mgg, CBmean, CBsigma, CBalpha1, CBn1, CBalpha2, CBn2) ;    
    w->import(*ResponseDoubleCB[c]);

    // Fit with ResponseDoubleCB
    RooFitResult* fitresults = 0;
    if (mass==1500) 
      fitresults = (RooFitResult* ) ResponseDoubleCB[c]->fitTo(*signal[c], SumW2Error(kFALSE), Range(1250, 1700), RooFit::Save(kTRUE));
    else if (mass==750) 
      fitresults = (RooFitResult* ) ResponseDoubleCB[c]->fitTo(*signal[c], SumW2Error(kFALSE), Range(500, 1200), RooFit::Save(kTRUE));
    else if (mass==5000) 
      fitresults = (RooFitResult* ) ResponseDoubleCB[c]->fitTo(*signal[c], SumW2Error(kFALSE), Range(4000, 5500), RooFit::Save(kTRUE));
    
    std::cout<<TString::Format("******************************** Signal Fit results DoubleCB mass %f cat %d***********************************", mass, c)<<std::endl;
    fitresults->Print("V");
   


    // Plot
    RooPlot* plotG = mgg->frame(Range(1250, 1650),Title("Mgg, response"),Bins(60));
    plotG->GetXaxis()->SetTitle("m_{reco}");
    plotG->GetXaxis()->SetTitleFont(42);
    plotG->GetXaxis()->SetTitleSize(0.04);
    plotG->GetXaxis()->SetTitleOffset(1.40);
    plotG->GetYaxis()->SetTitle("Events");
    plotG->GetYaxis()->SetTitleFont(42);
    plotG->GetYaxis()->SetTitleSize(0.04);

    TLegend* legmc = new TLegend(0.6, 0.58, 0.91, 0.91, "", "");
    legmc->SetTextSize(0.0286044);  
    legmc->SetTextFont(42);
    legmc->SetBorderSize(0);
    legmc->SetFillStyle(0);
    legmc->AddEntry(plotG->getObject(0),"Simulation","LP");    
    legmc->AddEntry(plotG->getObject(1),"DoubleCB ","L");
    
    TLatex* latex = new TLatex(0.21, 0.76, TString::Format("#splitline{m_{X}=%d GeV}{#splitline{}{Class %d}}",iMass,c));
    latex->SetTextSize(0.038);
    latex->SetTextAlign(11);
    latex->SetTextFont(42); 
    latex->SetNDC();

    // Lin scale
    if (mass==1500) {
      plotG = mgg->frame(Range(1400, 1550),Title("Mgg, response"),Bins(60));           
    } else if (mass==750) {
      plotG = mgg->frame(Range(710, 780),Title("Mgg, response"),Bins(60));          
    } else if (mass==5000) {
      plotG = mgg->frame(Range(4500, 5300),Title("Mgg, response"),Bins(60));          
    }
    signal[c]->plotOn(plotG);
    ResponseDoubleCB[c]->plotOn(plotG, LineColor(kBlue));
    plotG->Draw();
    latex->Draw("same");
    legmc->Draw("same");
    c1->SetLogy(0);
    c1->SaveAs("plots/responseDoubleCB_cat"+myCut+".png");

    // The Log scale
    if (mass==1500) {
      plotG = mgg->frame(Range(1250, 1650),Title("Mgg, response"),Bins(60));        
    } else if (mass==750) {
      plotG = mgg->frame(Range(550, 850),Title("Mgg, response"),Bins(60));             
    } else if (mass==5000) {
      plotG = mgg->frame(Range(4000, 5500),Title("Mgg, response"),Bins(60));           
    }
    signal[c]->plotOn(plotG);
    ResponseDoubleCB[c]->plotOn(plotG, LineColor(kBlue));
    plotG->Draw();
    latex->Draw("same");
    legmc->Draw("same");
    c1->SetLogy();
    c1->SaveAs("plots/responseDoubleCB_cat"+myCut+"_LOG.png");


    // saving as constant in the WS    
    w->defineSet("ResponseDoubleCBPdfParam_cat"+myCut,RooArgSet(*w->var("Sig_sigma_cat"+myCut),
								*w->var("Sig_alpha1_cat"+myCut),
								*w->var("Sig_alpha2_cat"+myCut),
								*w->var("Sig_n1_cat"+myCut),
								*w->var("Sig_n2_cat"+myCut),	   
								*w->var("Sig_mean_cat"+myCut)));
    SetConstantParams(w->set("ResponseDoubleCBPdfParam_cat"+myCut));

    w->Print();
  }
}

// Signal model: sum of 2 CBs. To describe the detector resolution, reco mgg used
void SigModelResponseCBCBFit(RooWorkspace* w, Float_t mass, TString coupling) {
  
  int iMass = abs(mass);   
  
  // Dataset
  RooDataSet* signal[NCAT];
  RooRealVar* mgg = w->var("mgg");     

  // fit functions
  RooCBShape* ResponseCBpos[NCAT];
  RooCBShape* ResponseCBneg[NCAT];
  RooAddPdf* ResponseAdd[NCAT];
  
  TCanvas* c1 = new TCanvas("c1", "c1", 1);
  c1->cd();

  for(int c = 0; c<NCAT; c++){

    TString myCut = "EBHighR9";
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    
    
    // taking the dataset
    signal[c] = (RooDataSet*) w->data("SigWeight_cat"+myCut);

    // cb pos                                                               
    RooFormulaVar CBpos_mean("MassCB_sig_mean_cat"+myCut,"", "@0", *w->var("Mass_sig_mean_cat"+myCut));
    RooFormulaVar CBpos_sigma("MassCB_sig_sigma_cat"+myCut, "", "sqrt(@0*@0)", *w->var("Mass_sig_sigma_cat"+myCut));
    RooFormulaVar CBpos_alphaCB("MassCB_sig_alphaCBpos_cat"+myCut,"", "@0", *w->var("Mass_sig_alphaCBpos_cat"+myCut));
    RooFormulaVar CBpos_n("MassCB_sig_nCBpos_cat"+myCut,"", "@0", *w->var("Mass_sig_nCBpos_cat"+myCut));
    ResponseCBpos[c] = new RooCBShape("ResponseCBpos_cat"+myCut,"ResponseCBpos_cat"+myCut, *mgg, CBpos_mean, CBpos_sigma,  CBpos_alphaCB, CBpos_n) ;
    
    // cb neg (same mean and sigma)
    RooFormulaVar CBneg_n("MassCB_sig_nCBneg_cat"+myCut,"", "@0", *w->var("Mass_sig_nCBneg_cat"+myCut));
    RooFormulaVar CBneg_alphaCB("MassCB_sig_alphaCBneg_cat"+myCut,"", "@0", *w->var("Mass_sig_alphaCBneg_cat"+myCut));
    ResponseCBneg[c] = new RooCBShape("ResponseCBneg_cat"+myCut,"ResponseCBneg_cat"+myCut, *mgg, CBpos_mean, CBpos_sigma, CBneg_alphaCB, CBneg_n) ;
   
    // combination pos and neg
    RooFormulaVar CB_frac("MassCB_sig_frac_cat"+myCut,"", "@0", *w->var("Mass_sig_frac_cat"+myCut));
    ResponseAdd[c]= new RooAddPdf("ResponseAddPdf_cat"+myCut,"ResponseAddPdf_cat"+myCut, RooArgList(*ResponseCBneg[c], *ResponseCBpos[c]), CB_frac);
    w->import(*ResponseAdd[c]);
   
    // Fit with ResponseAdd
    RooFitResult* fitresults = 0;
    if (mass==1500) 
      fitresults = (RooFitResult* ) ResponseAdd[c]->fitTo(*signal[c], SumW2Error(kFALSE), Range(1250, 1700), RooFit::Save(kTRUE));
    else if (mass==750) 
      fitresults = (RooFitResult* ) ResponseAdd[c]->fitTo(*signal[c], SumW2Error(kFALSE), Range(500, 1200), RooFit::Save(kTRUE));
    else if (mass==5000) 
      fitresults = (RooFitResult* ) ResponseAdd[c]->fitTo(*signal[c], SumW2Error(kFALSE), Range(4000, 5500), RooFit::Save(kTRUE));
    
    std::cout<<TString::Format("******************************** Signal Fit results CB+CB  mass %f cat %d***********************************", mass, c)<<std::endl;
    fitresults->Print("V");
   
    
    
    // Plot
    RooPlot* plotG = mgg->frame(Range(1250, 1650),Title("Mgg, response"),Bins(60));
    plotG->GetXaxis()->SetTitle("m_{reco}");
    plotG->GetXaxis()->SetTitleFont(42);
    plotG->GetXaxis()->SetTitleSize(0.04);
    plotG->GetXaxis()->SetTitleOffset(1.40);
    plotG->GetYaxis()->SetTitle("Events");
    plotG->GetYaxis()->SetTitleFont(42);
    plotG->GetYaxis()->SetTitleSize(0.04);
    TLegend* legmc = new TLegend(0.6, 0.58, 0.91, 0.91, "", "");
    legmc->SetTextSize(0.0286044);  
    legmc->SetTextFont(42);
    legmc->SetBorderSize(0);
    legmc->SetFillStyle(0);
    legmc->AddEntry(plotG->getObject(0),"Simulation","LP");    
    legmc->AddEntry(plotG->getObject(1),"Sum of two CB ","L");
    legmc->AddEntry(plotG->getObject(2),"CB 1","L");   
    legmc->AddEntry(plotG->getObject(3),"CB 2","L");   
    
    TLatex* latex = new TLatex(0.21, 0.76, TString::Format("#splitline{m_{X}=%d GeV}{#splitline{}{Class %d}}",iMass,c));
    latex->SetTextSize(0.038);
    latex->SetTextAlign(11);
    latex->SetTextFont(42); 
    latex->SetNDC();


    // First lin scale
    if (mass==1500) {
      plotG = mgg->frame(Range(1400, 1550),Title("Mgg, response"),Bins(60));           
    } else if (mass==750) {
      plotG = mgg->frame(Range(710, 780),Title("Mgg, response"),Bins(60));               
    } else if (mass==5000) {
      plotG = mgg->frame(Range(4500, 5300),Title("Mgg, response"),Bins(60));          
    }
    signal[c]->plotOn(plotG);
    ResponseAdd[c]->plotOn(plotG, LineColor(kBlue));
    ResponseAdd[c]->plotOn(plotG,Components("ResponseCBneg_cat"+myCut), LineColor(kGreen), LineStyle(kDotted));
    ResponseAdd[c]->plotOn(plotG,Components("ResponseCBpos_cat"+myCut), LineColor(kRed), LineStyle(kDashed));
    plotG->Draw();
    latex->Draw("same");
    legmc->Draw("same");
    c1->SetLogy(0);
    c1->SaveAs("plots/responseAbsoluteFitCBCB_cat"+myCut+".png");

    // The log scale
    if (mass==1500) {
      plotG = mgg->frame(Range(1250, 1650),Title("Mgg, response"),Bins(60));              
    } else if (mass==750) {
      plotG = mgg->frame(Range(550, 850),Title("Mgg, response"),Bins(60));             
    } else if (mass==5000) {
      plotG = mgg->frame(Range(4000, 5500),Title("Mgg, response"),Bins(60));           
    }
    signal[c]->plotOn(plotG);
    ResponseAdd[c]->plotOn(plotG, LineColor(kBlue));
    ResponseAdd[c]->plotOn(plotG,Components("ResponseCBneg_cat"+myCut), LineColor(kGreen), LineStyle(kDotted));
    ResponseAdd[c]->plotOn(plotG,Components("ResponseCBpos_cat"+myCut), LineColor(kRed), LineStyle(kDashed));
    plotG->Draw();
    latex->Draw("same");
    legmc->Draw("same");
    c1->SetLogy();
    c1->SaveAs("plots/responseAbsoluteFitCBCB_cat"+myCut+"_LOG.png");


    // saving as constant in the WS
    w->defineSet("ResponseAddPdfParam_cat"+myCut,RooArgSet(*w->var("Mass_sig_sigma_cat"+myCut),
							   *w->var("Mass_sig_alphaCBpos_cat"+myCut),
							   *w->var("Mass_sig_alphaCBneg_cat"+myCut),
							   *w->var("Mass_sig_nCBpos_cat"+myCut),
							   *w->var("Mass_sig_nCBneg_cat"+myCut),
							   *w->var("Mass_sig_frac_cat"+myCut),
							   *w->var("Mass_sig_mean_cat"+myCut)));
    SetConstantParams(w->set("ResponseAddPdfParam_cat"+myCut));

    w->Print();
  }
}

// Signal model: sum of two CBs. Detector resolution function based on mreco/mgen -1
void SigModelResponseReducedCBCBFit(RooWorkspace* w, Float_t mass, TString coupling) {
  
  int iMass = abs(mass);   
  
  // Dataset 
  RooDataSet* signal[NCAT];
  RooRealVar* mgg = w->var("mgg");     
  RooRealVar* massReduced = w->var("massReduced");
    
  // fit functions
  RooCBShape* ResponseCBpos[NCAT];
  RooCBShape* ResponseCBneg[NCAT];
  RooAddPdf* ResponseAdd[NCAT];
  
  TCanvas* c1 = new TCanvas("c1", "c1", 1);
  c1->cd();
  
  for(int c = 0; c<NCAT; c++){

    TString myCut = "EBHighR9";
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    

    // taking the dataset  
    signal[c] = (RooDataSet*) w->data("SigWeightReduced_cat"+myCut);
    
    // cb pos                                                               
    RooFormulaVar CBpos_mean("ReducedMassCB_sig_mean_cat"+myCut,"", "@0", *w->var("ReducedMass_sig_mean_cat"+myCut));
    RooFormulaVar CBpos_sigma("ReducedMassCB_sig_sigma_cat"+myCut, "", "sqrt(@0*@0)", *w->var("ReducedMass_sig_sigma_cat"+myCut));
    RooFormulaVar CBpos_alphaCB("ReducedMassCB_sig_alphaCBpos_cat"+myCut,"", "@0", *w->var( "ReducedMass_sig_alphaCBpos_cat"+myCut));
    RooFormulaVar CBpos_n("ReducedMassCB_sig_nCBpos_cat"+myCut,"", "@0", *w->var( "ReducedMass_sig_nCBpos_cat"+myCut));
    ResponseCBpos[c] = new RooCBShape("ResponseCBpos_cat"+myCut,"ResponseCBpos_cat"+myCut , *massReduced, CBpos_mean, CBpos_sigma,  CBpos_alphaCB, CBpos_n) ;
    
    // cb neg (same mean and sigma)
    RooFormulaVar CBneg_n("ReducedMassCB_sig_nCBneg_cat"+myCut,"", "@0", *w->var( "ReducedMass_sig_nCBneg_cat"+myCut));
    RooFormulaVar CBneg_alphaCB("ReducedMassCB_sig_alphaCBneg_cat"+myCut,"", "@0", *w->var( "ReducedMass_sig_alphaCBneg_cat"+myCut));
    ResponseCBneg[c] = new RooCBShape("ResponseCBneg_cat"+myCut,"ResponseCBneg_cat"+myCut , *massReduced, CBpos_mean, CBpos_sigma, CBneg_alphaCB, CBneg_n) ;
   
    // combination pos and neg
    RooFormulaVar CB_frac("ReducedMassCB_sig_frac_cat"+myCut,"", "@0", *w->var("ReducedMass_sig_frac_cat"+myCut));
    ResponseAdd[c]= new RooAddPdf("ResponseAddPdf_cat"+myCut,"ResponseAddPdf_cat"+myCut , RooArgList(*ResponseCBneg[c], *ResponseCBpos[c]), CB_frac);

    w->import(*ResponseAdd[c]);
   
    // Fit with ResponseAdd
    RooFitResult* fitresults = (RooFitResult* ) ResponseAdd[c]->fitTo(*signal[c], SumW2Error(kFALSE), Range(0.95, 1.05), RooFit::Save(kTRUE));
    std::cout<<TString::Format("******************************** Signal Fit results CB+CB  mass %f cat %d***********************************", mass, c)<<std::endl;
    fitresults->Print("V");
   

    // Plot
    RooPlot* plotG = massReduced->frame(Range(0.95, 1.05),Title("Mass Reduced"),Bins(60));
    signal[c]->plotOn(plotG);

    ResponseAdd[c]->plotOn(plotG, LineColor(kBlue));
    ResponseAdd[c]->plotOn(plotG,Components("ResponseCBneg_cat"+myCut), LineColor(kGreen), LineStyle(kDotted));
    ResponseAdd[c]->plotOn(plotG,Components("ResponseCBpos_cat"+myCut), LineColor(kRed), LineStyle(kDashed));
  
    plotG->GetXaxis()->SetTitle("#frac{m_{reco}}{m_{true}}");
    plotG->GetXaxis()->SetTitleFont(42);
    plotG->GetXaxis()->SetTitleSize(0.04);
    plotG->GetXaxis()->SetTitleOffset(1.40);
    plotG->GetYaxis()->SetTitle("Events/0.0024 units");
    plotG->GetYaxis()->SetTitleFont(42);
    plotG->GetYaxis()->SetTitleSize(0.04);

    TLegend* legmc = new TLegend(0.6, 0.58, 0.91, 0.91, "", "");
    legmc->SetTextSize(0.0286044);  
    legmc->SetTextFont(42);
    legmc->SetBorderSize(0);
    legmc->SetFillStyle(0);
    legmc->AddEntry(plotG->getObject(0),"Simulation","LP");    
    legmc->AddEntry(plotG->getObject(1),"Sum of two CB ","L");
    legmc->AddEntry(plotG->getObject(2),"CB 1","L");   
    legmc->AddEntry(plotG->getObject(3),"CB 2","L");   
    
    TLatex* latex = new TLatex(0.21, 0.76, TString::Format("#splitline{m_{X}=%d GeV}{#splitline{}{Class %d}}",iMass,c));
    latex->SetTextSize(0.038);
    latex->SetTextAlign(11);
    latex->SetTextFont(42); 
    latex->SetNDC();
   
    plotG->Draw();
    
    latex->Draw("same");
    legmc->Draw("same");
    int iPos=11 ;

    c1->SetLogy(0);
    c1->SaveAs("plots/responseFitCBCB_cat"+myCut+".png");
    c1->SetLogy();
    c1->SaveAs("plots/responseFitCBCB_cat"+myCut+"_LOG.png");    

    // saving as constant in the WS  
    w->defineSet("ResponseAddPdfParam_cat"+myCut,RooArgSet(*w->var("ReducedMass_sig_sigma_cat"+myCut),
							   *w->var("ReducedMass_sig_alphaCBpos_cat"+myCut),
							   *w->var("ReducedMass_sig_alphaCBneg_cat"+myCut),
							   *w->var("ReducedMass_sig_nCBpos_cat"+myCut),
							   *w->var("ReducedMass_sig_nCBneg_cat"+myCut),	   
							   *w->var("ReducedMass_sig_frac_cat"+myCut),  
							   *w->var("ReducedMass_sig_mean_cat"+myCut)));
    SetConstantParams(w->set("ResponseAddPdfParam_cat"+myCut));

    w->Print();
  }
}

// Signal model: doubleCB. To describe the detector resolution, mgg/mggGen -1 used
void SigModelResponseReducedDoubleCBFit(RooWorkspace* w, Float_t mass, TString coupling) {
  
  int iMass = abs(mass);   
  
  // Dataset 
  RooDataSet* signal[NCAT];
  RooRealVar* mgg = w->var("mgg");     
  RooRealVar* massReduced = w->var("massReduced");
  
  // fit function
  RooDoubleCB* ResponseDoubleCB[NCAT];

  // to write down the results in a file
  ofstream *outTxtFile = new ofstream("HighMassGG_temp.rs",ios::out);
  *outTxtFile << "mgg[500,6000];" << endl;
  *outTxtFile << "mggGen[500,6000];" << endl;
  *outTxtFile << "weight[0, 1000];" << endl;
  *outTxtFile << "eventClass[-10,10];" << endl;
  *outTxtFile << endl;
  
  TCanvas* c1 = new TCanvas("c1", "c1", 1);
  c1->cd();
  
  for(int c = 0; c<NCAT; c++){

    TString myCut = "EBHighR9";
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    

    // loading the dataset  
    signal[c] = (RooDataSet*) w->data("SigWeightReduced_cat"+myCut);
    
    // DoubleCB 
    RooFormulaVar CBmean("ReducedMassCB_sig_mean_cat"+myCut,"", "@0", *w->var("ReducedMass_sig_mean_cat"+myCut));
    RooFormulaVar CBsigma("ReducedMassCB_sig_sigma_cat"+myCut, "", "sqrt(@0*@0)", *w->var("ReducedMass_sig_sigma_cat"+myCut));
    RooFormulaVar CBalpha1("ReducedMassCB_sig_alpha1_cat"+myCut,"", "@0", *w->var( "ReducedMass_sig_alpha1_cat"+myCut));
    RooFormulaVar CBn1("ReducedMassCB_sig_n1cat"+myCut,"", "@0", *w->var( "ReducedMass_sig_n1_cat"+myCut));
    RooFormulaVar CBalpha2("ReducedMassCB_sig_alpha2_cat"+myCut,"", "@0", *w->var( "ReducedMass_sig_alpha2_cat"+myCut));
    RooFormulaVar CBn2("ReducedMassCB_sig_n2cat"+myCut,"", "@0", *w->var( "ReducedMass_sig_n2_cat"+myCut));
    ResponseDoubleCB[c] = new RooDoubleCB("ResponseDoubleCB_cat"+myCut,"ResponseDoubleCB_cat"+myCut, *massReduced, CBmean, CBsigma, CBalpha1, CBn1, CBalpha2, CBn2);
    w->import(*ResponseDoubleCB[c]);
    
    // Fit with ResponseDoubleCB      
    RooFitResult* fitresults = (RooFitResult* ) ResponseDoubleCB[c]->fitTo(*signal[c], SumW2Error(kFALSE), Range(0.95, 1.05), RooFit::Save(kTRUE));
    std::cout<<TString::Format("******************************** Signal Fit results doubleCB  mass %f cat %d***********************************", mass, c)<<std::endl;
    fitresults->Print("V");

    // saving fit parameters
    *outTxtFile << "Sig_sigma_cat"+myCut+"["  << CBsigma.getVal()  << "];" << endl;
    *outTxtFile << "Sig_alpha1_cat"+myCut+"[" << CBalpha1.getVal() << "];" << endl;
    *outTxtFile << "Sig_alpha2_cat"+myCut+"[" << CBalpha2.getVal() << "];" << endl;
    *outTxtFile << "Sig_n1_cat"+myCut+"["     << CBn1.getVal()     << "];" << endl;
    *outTxtFile << "Sig_n2_cat"+myCut+"["     << CBn2.getVal()     << "];" << endl;
    *outTxtFile << "Sig_mean_cat"+myCut+"["   << CBmean.getVal()   << "];" << endl;
    *outTxtFile << endl;

    // Plot
    RooPlot* plotG = massReduced->frame(Range(0.95, 1.05),Title("Mass Reduced"),Bins(60));
    plotG->GetXaxis()->SetTitle("#frac{m_{reco}}{m_{true}}");
    plotG->GetXaxis()->SetTitleFont(42);
    plotG->GetXaxis()->SetTitleSize(0.04);
    plotG->GetXaxis()->SetTitleOffset(1.40);
    plotG->GetYaxis()->SetTitle("Events/0.0024 units");
    plotG->GetYaxis()->SetTitleFont(42);
    plotG->GetYaxis()->SetTitleSize(0.04);

    TLegend* legmc = new TLegend(0.6, 0.58, 0.91, 0.91, "", "");
    legmc->SetTextSize(0.0286044);  
    legmc->SetTextFont(42);
    legmc->SetBorderSize(0);
    legmc->SetFillStyle(0);
    legmc->AddEntry(plotG->getObject(0),"Simulation","LP");    
    legmc->AddEntry(plotG->getObject(1),"DoubleCB ","L");
    
    TLatex* latex = new TLatex(0.21, 0.76, TString::Format("#splitline{m_{X}=%d GeV}{#splitline{}{Class %d}}",iMass,c));
    latex->SetTextSize(0.038);
    latex->SetTextAlign(11);
    latex->SetTextFont(42); 
    latex->SetNDC();

    signal[c]->plotOn(plotG);
    ResponseDoubleCB[c]->plotOn(plotG, LineColor(kBlue));
    plotG->Draw();
    latex->Draw("same");
    legmc->Draw("same");

    c1->SetLogy(0);
    c1->SaveAs("plots/responseDoubleCB_cat"+myCut+".png");
    c1->SetLogy();
    c1->SaveAs("plots/responseDoubleCB_cat"+myCut+"_LOG.png");
    
    // saving as constant in the WS  
    w->defineSet("ResponseDoubleCBPdfParam_cat"+myCut,RooArgSet(*w->var("ReducedMass_sig_sigma_cat"+myCut), 
								*w->var("ReducedMass_sig_alpha1_cat"+myCut),
								*w->var("ReducedMass_sig_alpha2_cat"+myCut),
								*w->var("ReducedMass_sig_n1_cat"+myCut),
								*w->var("ReducedMass_sig_n2_cat"+myCut),	   
								*w->var("ReducedMass_sig_mean_cat"+myCut)));
    SetConstantParams(w->set("ResponseDoubleCBPdfParam_cat"+myCut));

    w->Print();
  }
}

//-------------------------------------------------------------------------
// Signal model: BW only fit to the gen level mass - to check it is doable or not due to the PDFs
// Fit all categories together
void SigModelBWFit(RooWorkspace* w, Float_t mass, TString coupling) {

  int iMass = abs(mass);   
  
  // Variables
  RooRealVar* mggGen = w->var("mggGen");     

  // dataset
  RooDataSet* signal = (RooDataSet*) w->data(TString::Format("SigWeightGen"));

  // fit function
  RooFormulaVar meanBW("meanBWgen","","@0",*w->var("meanBW"));   
  RooFormulaVar sigmaBW("sigmaBWgen","","@0",*w->var("sigmaBW"));     
  RooBreitWigner *genMassBW = new RooBreitWigner("genMassBW","genMassBW",*mggGen,meanBW,sigmaBW);  
  w->import(*genMassBW);
  
  TCanvas* c1 = new TCanvas("c1", "c1", 1);
  c1->cd();

  std::cout<<TString::Format("******************************** gen level mass fit with BW, %f ***********************************",mass)<<std::endl;
  float fitMin = 1350.;
  float fitMax = 1650.;
  if (coupling=="001") {
    if (mass==750)  { fitMin = 740.;  fitMax = 760.; }
    if (mass==1500) { fitMin = 1490.; fitMax = 1510.; }
    if (mass==5000) { fitMin = 4980.; fitMax = 5020.; }
  } else if (coupling=="01") {
    if (mass==1500) { fitMin = 1200.;  fitMax = 1800.; }
    if (mass==3000) { fitMin = 2750.;  fitMax = 3250.; }
  } else if (coupling=="02") {
    if (mass==1500) { fitMin = 1000.;  fitMax = 2000.; }
    if (mass==3000) { fitMin = 2000.;  fitMax = 4000.; }
    if (mass==5000) { fitMin = 4000.;  fitMax = 6000.; }
  }
  RooFitResult* fitresults = (RooFitResult* ) genMassBW->fitTo(*signal, SumW2Error(kFALSE), Range(fitMin,fitMax), RooFit::Save(kFALSE));    

  // Plot
  RooPlot* plotG = mggGen->frame(Range(fitMin,fitMax),Title("Gen Level mgg"),Bins(60));
  plotG->GetXaxis()->SetTitle("m_{true}");
  plotG->GetXaxis()->SetTitleFont(42);
  plotG->GetXaxis()->SetTitleSize(0.04);
  plotG->GetXaxis()->SetTitleOffset(1.40);
  plotG->GetYaxis()->SetTitleFont(42);
  plotG->GetYaxis()->SetTitleSize(0.04);

  TLegend* legmc = new TLegend(0.6, 0.58, 0.91, 0.91, "", "");
  legmc->SetTextSize(0.0286044);  
  legmc->SetTextFont(42);
  legmc->SetBorderSize(0);
  legmc->SetFillStyle(0);
  legmc->AddEntry(plotG->getObject(0),"Simulation","LP");    
  legmc->AddEntry(plotG->getObject(1),"BW fit","L");
  
  TLatex* latex = new TLatex(0.21, 0.76, TString::Format("m_{X}=%d GeV",iMass));
  latex->SetTextSize(0.038);
  latex->SetTextAlign(11);
  latex->SetTextFont(42); 
  latex->SetNDC();

  signal->plotOn(plotG);
  genMassBW->plotOn(plotG, LineColor(kBlue));
  plotG->Draw();
  latex->Draw("same");
  legmc->Draw("same");

  c1->SetLogy(0);
  c1->SaveAs("plots/mggGenFitBW.png");
  c1->SetLogy();
  c1->SaveAs("plots/mggGenFitBW_LOG.png");
  
  w->defineSet("genMassBWPdfParam",RooArgSet(*w->var("meanBW"),*w->var("sigmaBW")));  
  SetConstantParams(w->set("genMassBWPdfParam"));

  // now put the dataset into a roodatahist and save
  TString myName= TString::Format("bSigWeightGen_mass%d",iMass)+TString("_")+TString(coupling);
  mggGen->setRange(fitMin,fitMax);
  mggGen->setBins(60);
  RooDataHist bSigWeightGen("bSigWeightGen","bSigWeightGen",RooArgList(*mggGen),*signal);
  bSigWeightGen.Print();
  w->import(bSigWeightGen,Rename(myName));
  w->Print();
  
  TFile myOutFile("outputHits.root","UPDATE");
  myOutFile.cd();
  bSigWeightGen.Write(myName);
}

//-------------------------------------------------------------------------

// Fit signal with model with CBCB convoluted with BW
void SigModelFitConvBW(RooWorkspace* w, Float_t mass) {

  Float_t MASS(mass);  

  // Dataset
  RooDataSet* sigToFit[NCAT];
  RooRealVar* mgg = w->var("mgg"); 

  // Fit to Signal 
  for (int c=0; c<NCAT; ++c) {
    cout << "---------- Category = " << c << endl;

    TString myCut = "EBHighR9";
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    
    
    // CB
    RooFormulaVar CBpos_mean("CBpos_mean_cat"+myCut,"","@0",*w->var("ConvMass_sig_mean_cat"+myCut));
    RooFormulaVar CBpos_sigma("CBpos_sigma_cat"+myCut,"","@0",*w->var("Mass_sig_sigma_cat"+myCut));
    RooFormulaVar CBpos_alphaCB("CBpos_alphaCB_cat"+myCut,"","@0",*w->var("Mass_sig_alphaCBpos_cat"+myCut));
    RooFormulaVar CBneg_alphaCB("CBneg_alphaCB_cat"+myCut,"","@0",*w->var("Mass_sig_alphaCBneg_cat"+myCut));
    RooFormulaVar CBpos_n("CBpos_n_cat"+myCut,"","@0",*w->var("Mass_sig_nCBpos_cat"+myCut));
    RooFormulaVar CBneg_n("CBneg_n_cat"+myCut,"","@0",*w->var("Mass_sig_nCBneg_cat"+myCut));
    RooFormulaVar CBpos_frac("CBpos_frac_cat"+myCut,"","@0",*w->var("Mass_sig_frac_cat"+myCut));

    RooCBShape ResCBpos("ResCBpos_cat"+myCut,"ResCBpos_cat"+myCut, *mgg, CBpos_mean, CBpos_sigma,CBpos_alphaCB, CBpos_n);
    RooCBShape ResCBneg("ResCBneg_cat"+myCut,"ResCBneg_cat"+myCut, *mgg, CBpos_mean, CBpos_sigma,CBneg_alphaCB, CBneg_n);
    mgg->setBins(5000, "cache");  
    
    RooAddPdf ResAddPdf("ResAddPdf_cat"+myCut,"ResAddPdf_cat"+myCut, RooArgList(ResCBneg, ResCBpos), CBpos_frac);
  
    // BW 
    RooFormulaVar meanBW("massBW_cat"+myCut,"","@0",*w->var("meanBW_cat"+myCut));  
    RooFormulaVar sigmaBW("widthBW_cat"+myCut,"","@0",*w->var("sigmaBW_cat"+myCut));
    RooBreitWigner SigModelBW("BW_cat"+myCut,"BW_cat"+myCut, *mgg, meanBW, sigmaBW);
    
    // Convolution
    RooFFTConvPdf* ConvolutedRes_CB;
    ConvolutedRes_CB = new RooFFTConvPdf("mggSig_cat"+myCut,"mggSig_cat"+myCut, *mgg,SigModelBW, ResAddPdf);
    w->import(*ConvolutedRes_CB);


    // Fit and Plot
    sigToFit[c] = (RooDataSet*) w->data("SigWeight_cat"+myCut);
    
    float fitMin = 1250.;
    float fitMax = 1700.;
    if (mass==750)  {fitMin = 500.;   fitMax = 1200.; }
    if (mass==1500) {fitMin = 1250.;  fitMax = 1700.; }
    if (mass==5000) {fitMin = 4000.;  fitMax = 5500.; }

    RooFitResult* fitresults_CB = (RooFitResult* ) ConvolutedRes_CB->fitTo(*sigToFit[c], SumW2Error(kFALSE), Range(fitMin,fitMax), RooFit::Save(kTRUE));
    fitresults_CB->Print("V");
    
    RooPlot* plotOnlyResPdf = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    sigToFit[c]->plotOn(plotOnlyResPdf, LineColor(kRed), LineStyle(kDashed));
    double max = plotOnlyResPdf->GetMaximum();
    
    RooPlot* plotPhotonsMassAll = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    sigToFit[c]->plotOn(plotPhotonsMassAll);
    SigModelBW.plotOn(plotPhotonsMassAll, LineColor(kGreen), LineStyle(kDashed));
    ResAddPdf.plotOn(plotPhotonsMassAll, LineColor(kRed), LineStyle(kDashed));
    ConvolutedRes_CB->plotOn(plotPhotonsMassAll, LineColor(kBlue));
    
    TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);
    c1->cd(1);
    plotPhotonsMassAll->Draw();  
    plotPhotonsMassAll->GetYaxis()->SetRangeUser(0.01, max*1.2);
    plotPhotonsMassAll->GetXaxis()->SetRangeUser(210, 290);
    
    TLatex *lat  = new TLatex(0.55,0.9,TString::Format("Cat: %d", c));  
    lat->SetTextSize(0.038);
    lat->SetTextAlign(11);
    lat->SetTextFont(42); 
    lat->SetNDC();
    
    TLegend *legmc = new TLegend(0.55, 0.6, 0.87, 0.88, "");
    legmc->AddEntry(plotPhotonsMassAll->getObject(0),"Simulation","LPE");
    legmc->AddEntry(plotPhotonsMassAll->getObject(1),"BW","L");
    legmc->AddEntry(plotPhotonsMassAll->getObject(2)," CB + CB ","L");
    legmc->AddEntry(plotPhotonsMassAll->getObject(3),"BW #otimes Resolution","L");
    legmc->SetTextSize(0.0206044);
    legmc->SetTextFont(42);
    legmc->SetBorderSize(0);
    legmc->SetFillStyle(0);
    legmc->Draw();
    lat->Draw("same");
    
    int massI(mass);
    c1->SetLogy();
    plotPhotonsMassAll->GetXaxis()->SetTitle("m_{#gamma #gamma}[GeV]");
    
    c1->SetLogy(0);
    c1->SaveAs("plots/signalCBCconvBW"+TString::Format(("_M%d_cat%d_LOG_free.png"),massI,c));
    
    c1->SetLogy();
    plotPhotonsMassAll->GetYaxis()->SetRangeUser(0.01,max*10. );
    plotPhotonsMassAll->GetXaxis()->SetRangeUser(210, 290);
    c1->SaveAs("plots/signalCBCconvBW"+TString::Format(("_M%d_cat%d_LOG_free.png"),massI,c));
    
    
    // IMPORTANT: fix all pdf parameters to constant
    w->defineSet("ConvolutedPdfParam_cat"+myCut,RooArgSet( *w->var("Mass_sig_sigma_cat"+myCut), 
							   *w->var("Mass_sig_alphaCBpos_cat"+myCut),
							   *w->var("Mass_sig_alphaCBneg_cat"+myCut),
							   *w->var("Mass_sig_nCBpos_cat"+myCut),
							   *w->var("Mass_sig_nCBneg_cat"+myCut),	   
							   *w->var("Mass_sig_frac_cat"+myCut),  
							   *w->var("ConvMass_sig_mean_cat"+myCut),
							   *w->var("meanBW_cat"+myCut),
							   *w->var("sigmaBW_cat"+myCut)));

    SetConstantParams(w->set("ConvolutedPdfParam_cat"+myCut));
    
    w->Print("V");
  }
}

// Fit signal with model with DoubleCB convoluted with BW
void SigModelFitDoubleCBConvBW(RooWorkspace* w, Float_t mass) {

  Float_t MASS(mass);  
  
  // Dataset
  RooDataSet* sigToFit[NCAT];
  RooRealVar* mgg = w->var("mgg"); 

  // Fit to Signal 
  for (int c=0; c<NCAT; ++c) {
    cout << "---------- Category = " << c << endl;

    TString myCut = "EBHighR9";
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    
    
    // DoubleCB
    RooFormulaVar CBmean("CBmean_cat"+myCut,"","@0",*w->var("ConvSig_mean_cat"+myCut));
    RooFormulaVar CBsigma("CBsigma_cat"+myCut,"","@0",*w->var("Sig_sigma_cat"+myCut));
    RooFormulaVar CBalpha1("CBalpha1_cat"+myCut,"","@0",*w->var("Sig_alpha1_cat"+myCut) );
    RooFormulaVar CBn1("CBn1_cat"+myCut,"","@0",*w->var("Sig_n1_cat"+myCut) );
    RooFormulaVar CBalpha2("CBalpha2_cat"+myCut,"","@0",*w->var("Sig_alpha2_cat"+myCut) );
    RooFormulaVar CBn2("CBn2_cat"+myCut,"","@0",*w->var("Sig_n2_cat"+myCut) );
    RooDoubleCB ResponseDoubleCB("ResponseDoubleCB_cat"+myCut,"ResponseDoubleCB_cat"+myCut , *mgg, CBmean, CBsigma, CBalpha1, CBn1, CBalpha2, CBn2);
    mgg->setBins(5000, "cache");  

    // BW 
    RooFormulaVar meanBW("massBW_cat"+myCut,"","@0",*w->var("meanBW_cat"+myCut) );  
    RooFormulaVar sigmaBW("widthBW_cat"+myCut,"","@0",*w->var("sigmaBW_cat"+myCut) );
    RooBreitWigner SigModelBW("BW_cat"+myCut,"BW_cat"+myCut, *mgg, meanBW, sigmaBW);
      
    // Convolution
    RooFFTConvPdf* ConvolutedRes_CB;
    ConvolutedRes_CB = new RooFFTConvPdf("mggSig_cat"+myCut,"mggSig_cat"+myCut, *mgg,SigModelBW, ResponseDoubleCB);
    w->import(*ConvolutedRes_CB);


    // Fit and Plot
    sigToFit[c] = (RooDataSet*) w->data("SigWeight_cat"+myCut);
    
    float fitMin = 1250.;
    float fitMax = 1700.;
    if (mass==750)  {fitMin = 500.;   fitMax = 1200.; }
    if (mass==1500) {fitMin = 1250.;  fitMax = 1700.; }
    if (mass==5000) {fitMin = 4000.;  fitMax = 5500.; }

    RooFitResult* fitresults_CB = (RooFitResult* ) ConvolutedRes_CB->fitTo(*sigToFit[c], SumW2Error(kFALSE), Range(fitMin,fitMax), RooFit::Save(kTRUE));
    fitresults_CB->Print("V");
    
    RooPlot* plotOnlyResPdf = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    sigToFit[c]->plotOn(plotOnlyResPdf, LineColor(kRed), LineStyle(kDashed));
    double max = plotOnlyResPdf->GetMaximum();
    
    RooPlot* plotPhotonsMassAll = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    plotPhotonsMassAll->SetTitle("DoubleCB conv BW, cat"+myCut);
    sigToFit[c]->plotOn(plotPhotonsMassAll);
    SigModelBW.plotOn(plotPhotonsMassAll, LineColor(kGreen), LineStyle(kDashed));
    ResponseDoubleCB.plotOn(plotPhotonsMassAll, LineColor(kRed), LineStyle(kDashed));
    ConvolutedRes_CB->plotOn(plotPhotonsMassAll, LineColor(kBlue));
    
    TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);
    c1->cd(1);
    plotPhotonsMassAll->Draw();  
    plotPhotonsMassAll->GetYaxis()->SetRangeUser(0.01, max*1.2);
    plotPhotonsMassAll->GetXaxis()->SetRangeUser(210, 290);
    
    TLegend *legmc = new TLegend(0.6, 0.58, 0.091, 0.91, "");
    legmc->AddEntry(plotPhotonsMassAll->getObject(0),"Simulation","LPE");
    legmc->AddEntry(plotPhotonsMassAll->getObject(1),"BW","L");
    legmc->AddEntry(plotPhotonsMassAll->getObject(2),"DoubleCB","L");
    legmc->AddEntry(plotPhotonsMassAll->getObject(3),"BW #otimes Resolution","L");
    legmc->SetTextSize(0.0206044);
    legmc->SetTextFont(42);
    legmc->SetBorderSize(0);
    legmc->SetFillStyle(0);
    legmc->Draw();
    
    int massI(mass);
    c1->SetLogy();
    plotPhotonsMassAll->GetXaxis()->SetTitle("m_{#gamma #gamma}[GeV]");
    
    c1->SetLogy(0);
    c1->SaveAs("plots/signalCBCconvBW"+TString::Format(("_M%d_cat%d_free.png"),massI, c));
    
    c1->SetLogy();
    plotPhotonsMassAll->GetYaxis()->SetRangeUser(0.01,max*10. );
    plotPhotonsMassAll->GetXaxis()->SetRangeUser(210, 290);
    c1->SaveAs("plots/signalCBCconvBW"+TString::Format(("_M%d_cat%d_LOG_free.png"),massI,c));
    
    
    // IMPORTANT: fix all pdf parameters to constant
    w->defineSet("ConvolutedPdfParam_cat"+myCut,RooArgSet( *w->var("Sig_sigma_cat"+myCut),
							   *w->var("Sig_alpha1_cat"+myCut),
							   *w->var("Sig_alpha2_cat"+myCut),
							   *w->var("Sig_n1_cat"+myCut),
							   *w->var("Sig_n2_cat"+myCut),	   
							   *w->var("ConvSig_mean_cat"+myCut),
							   *w->var("sigmaBW_cat"+myCut),
							   *w->var("meanBW_cat"+myCut)));
    SetConstantParams(w->set("ConvolutedPdfParam_cat"+myCut));
    
    w->Print("V");
  }
}

// Fit signal using toys from fitted doubleCB and nominal BW
void SigModelFromToys(RooWorkspace* w, Float_t mass) {

  Float_t MASS(mass);  
  
  // Original dataset and variable
  RooDataSet* sigToFit[NCAT];
  RooRealVar* mgg = w->var("mgg"); 
  
  // Toy dataset for real work
  RooDataSet* pseudoData[NCAT];

  // Toy dataset for checks
  RooDataSet* pseudoDataCB[NCAT];
  RooDataSet* pseudoDataBW[NCAT];

  // Other RooRealVar
  RooRealVar* mggGenCB = new RooRealVar("mggGenCB", "M(gg)", -1., 2., "GeV");      
  RooRealVar* mggGenBW = new RooRealVar("mggGenBW", "M(gg)", 500., 6000., "GeV");

  // Fit to Signal 
  for (int c=0; c<NCAT; ++c) {
    cout << "---------- Category = " << c << endl;

    TString myCut = "EBHighR9";
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    
    
    // DoubleCB - must be centred at 1 with all parameters fixed to the result of the fit for this mass at k=0.01 (or no resonance)
    RooFormulaVar CBmean("CBmean_cat"+myCut,"","@0",*w->var("Sig_mean_cat"+myCut));
    RooFormulaVar CBsigma("CBsigma_cat"+myCut,"","@0",*w->var("Sig_sigma_cat"+myCut));
    RooFormulaVar CBalpha1("CBalpha1_cat"+myCut,"","@0",*w->var("Sig_alpha1_cat"+myCut));
    RooFormulaVar CBn1("CBn1_cat"+myCut,"","@0",*w->var("Sig_n1_cat"+myCut));
    RooFormulaVar CBalpha2("CBalpha2_cat"+myCut,"","@0",*w->var("Sig_alpha2_cat"+myCut));
    RooFormulaVar CBn2("CBn2_cat"+myCut,"","@0",*w->var("Sig_n2_cat"+myCut));
    RooDoubleCB ResponseDoubleCB("ResponseDoubleCB_cat"+myCut,"ResponseDoubleCB_cat"+myCut , *mggGenCB, CBmean, CBsigma, CBalpha1, CBn1, CBalpha2, CBn2);

    // BW - all parameters fixed to the expected values
    RooFormulaVar meanBW("massBW_cat"+myCut,"","@0",*w->var("meanBW_cat"+myCut));  
    RooFormulaVar sigmaBW("widthBW_cat"+myCut,"","@0",*w->var("sigmaBW_cat"+myCut));
    RooBreitWigner SigModelBWG("BWG_cat"+myCut,"BWG_cat"+myCut, *mggGenBW, meanBW, sigmaBW);

    // Original dataset with reco
    sigToFit[c] = (RooDataSet*) w->data("SigWeight_cat"+myCut); 
    int origEntries = sigToFit[c]->numEntries();

    // Pseudodata from our generation
    pseudoData[c]   = new RooDataSet("pseudoData",  "pseudoData",   RooArgSet(*mgg));
    pseudoDataCB[c] = new RooDataSet("pseudoDataCB","pseudoDataCB", RooArgSet(*mggGenCB));
    pseudoDataBW[c] = new RooDataSet("pseudoDataBW","pseudoDataBW", RooArgSet(*mggGenBW));

    // to check the generation 
    TH1F *H_orig = new TH1F("H_orig","H_orig",45,1250,1700);
    TH1F *H_toys = new TH1F("H_toys","H_toys",45,1250,1700);
    if(mass==750) {
      H_orig = new TH1F("H_orig","H_orig",14,710,780);
      H_toys = new TH1F("H_toys","H_toys",14,710,780);
    }
    H_orig->Sumw2();
    H_toys->Sumw2();
    H_toys->SetTitle("");
    H_orig->SetTitle("");
    H_toys->SetLineColor(2);
    H_orig->SetLineColor(1);
    H_toys->SetLineWidth(2);
    H_orig->SetLineWidth(2);
      
    // Generation from the two pdfs
    cout << "generating " << origEntries << " entries" << endl;
    for (int ii=0; ii<origEntries; ii++) {
      if (ii%500==0) cout << ii << endl;

      RooDataSet* dataCB = ResponseDoubleCB.generate(*mggGenCB,1);  
      RooDataSet* dataBW = SigModelBWG.generate(*mggGenBW,1);     
      RooArgSet setCB = *dataCB->get(0);
      RooArgSet setBW = *dataBW->get(0);
      RooRealVar* varCB = (RooRealVar*)setCB.find("mggGenCB");      
      RooRealVar* varBW = (RooRealVar*)setBW.find("mggGenBW");      
      float mggCB = varCB->getVal();
      float mggBW = varBW->getVal();

      *mgg      = mggCB*mggBW;  
      *mggGenCB = mggCB;  
      *mggGenBW = mggBW;  

      pseudoData[c]->add(RooArgSet(*mgg));  
      pseudoDataCB[c]->add(RooArgSet(*mggGenCB));  
      pseudoDataBW[c]->add(RooArgSet(*mggGenBW));  

      // Fill histos
      RooArgSet setOriginal = *sigToFit[c]->get(ii);
      RooRealVar* varOriginal = (RooRealVar*)setOriginal.find("mgg");
      H_orig->Fill(varOriginal->getVal());
      H_toys->Fill(mggCB*mggBW);
    }

    w->import(*pseudoData[c],Rename("SigPseudodata_cat"+myCut));
    pseudoData[c]->Print();

    // Now build a new CB centred in 0 and with free parameters
    RooFormulaVar CBmeanF("CBmeanF_cat"+myCut,"","@0",*w->var("SigF_mean_cat"+myCut));
    RooFormulaVar CBsigmaF("CBsigmaF_cat"+myCut,"","@0",*w->var("SigF_sigma_cat"+myCut));
    RooFormulaVar CBalpha1F("CBalpha1F_cat"+myCut,"","@0",*w->var("SigF_alpha1_cat"+myCut));
    RooFormulaVar CBn1F("CBn1F_cat"+myCut,"","@0",*w->var("SigF_n1_cat"+myCut));
    RooFormulaVar CBalpha2F("CBalpha2F_cat"+myCut,"","@0",*w->var("SigF_alpha2_cat"+myCut));
    RooFormulaVar CBn2F("CBn2F_cat"+myCut,"","@0",*w->var("SigF_n2_cat"+myCut));
    RooDoubleCB ResponseDoubleCBF("ResponseDoubleCBF_cat"+myCut,"ResponseDoubleCBF_cat"+myCut, *mgg, CBmeanF, CBsigmaF, CBalpha1F, CBn1F, CBalpha2F, CBn2F);    
    mgg->setBins(5000, "cache");  

    // Now build a BW with all fixed parameters (same as above, correct roorealvar here) 
    RooBreitWigner SigModelBW("BW_cat"+myCut,"BW_cat"+myCut, *mgg, meanBW, sigmaBW);

    // And convolve with the nominal BW - everything fixed
    RooFFTConvPdf* ConvolutedRes;
    ConvolutedRes = new RooFFTConvPdf("mggSig_cat"+myCut,"mggSig_cat"+myCut, *mgg,SigModelBW, ResponseDoubleCBF);
    w->import(*ConvolutedRes);
    



    // Fit and Plot    
    float fitMin = 1250.;
    float fitMax = 1700.;
    // if (mass==750)  {fitMin = 500.;   fitMax = 1200.; }
    if (mass==750)  {fitMin = 680.;   fitMax = 800.; }
    if (mass==1500) {fitMin = 1250.;  fitMax = 1700.; }
    if (mass==5000) {fitMin = 4000.;  fitMax = 5500.; }

    RooFitResult* fitresults_CB = (RooFitResult* ) ConvolutedRes->fitTo(*pseudoData[c], SumW2Error(kFALSE), Range(fitMin,fitMax), RooFit::Save(kTRUE));
    fitresults_CB->Print("V");


    // plot
    RooPlot* plotOnlyResPdf = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    pseudoData[c]->plotOn(plotOnlyResPdf, LineColor(kRed), LineStyle(kDashed));
    double max = plotOnlyResPdf->GetMaximum();

    RooPlot* plotPhotonsMassAll = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    plotPhotonsMassAll->GetXaxis()->SetTitle("m_{#gamma #gamma}[GeV]");
    plotPhotonsMassAll->SetTitle("DoubleCB conv BW from toys, cat"+myCut);
    pseudoData[c]->plotOn(plotPhotonsMassAll);
    SigModelBW.plotOn(plotPhotonsMassAll, LineColor(kGreen), LineStyle(kDashed));
    ResponseDoubleCBF.plotOn(plotPhotonsMassAll, LineColor(kRed), LineStyle(kDashed));
    ConvolutedRes->plotOn(plotPhotonsMassAll, LineColor(kBlue));

    TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);
    c1->cd(1);
    int massI(mass);    
    c1->SetLogy();
    plotPhotonsMassAll->GetYaxis()->SetRangeUser(0.01, max*10.);
    plotPhotonsMassAll->Draw();

    TLegend *legmc = new TLegend(0.6, 0.58, 0.091, 0.91, "");
    legmc->AddEntry(plotPhotonsMassAll->getObject(0),"Pseudodata","LPE");
    legmc->AddEntry(plotPhotonsMassAll->getObject(1),"BW","L");
    legmc->AddEntry(plotPhotonsMassAll->getObject(2),"DoubleCB","L");
    legmc->AddEntry(plotPhotonsMassAll->getObject(3),"BW #otimes Resolution","L");
    legmc->SetTextSize(0.0206044);
    legmc->SetTextFont(42);
    legmc->SetBorderSize(0);
    legmc->SetFillStyle(0);
    legmc->Draw();
    c1->SaveAs("plots/signalCBCconvBW"+TString::Format(("_M%d_cat%d_LOG_fromPseudoData.png"),massI,c));

    if(mass==750)  plotPhotonsMassAll = w->var("mgg")->frame(Range(710,780),Bins(100));
    if(mass==1500) plotPhotonsMassAll = w->var("mgg")->frame(Range(1400,1600),Bins(100));
    plotPhotonsMassAll->GetXaxis()->SetTitle("m_{#gamma #gamma}[GeV]");
    plotPhotonsMassAll->SetTitle("DoubleCB conv BW from toys, cat"+myCut);
    pseudoData[c]->plotOn(plotPhotonsMassAll);
    max = plotPhotonsMassAll->GetMaximum();
    SigModelBW.plotOn(plotPhotonsMassAll, LineColor(kGreen), LineStyle(kDashed));
    ResponseDoubleCBF.plotOn(plotPhotonsMassAll, LineColor(kRed), LineStyle(kDashed));
    ConvolutedRes->plotOn(plotPhotonsMassAll, LineColor(kBlue));

    c1->SetLogy(0);
    plotPhotonsMassAll->GetYaxis()->SetRangeUser(0.01, max*1.2);
    plotPhotonsMassAll->Draw();  
    legmc->Draw();
    c1->SaveAs("plots/signalCBCconvBW"+TString::Format(("_M%d_cat%d_fromPseudoData.png"),massI, c));


    // check that the generation was ok
    RooPlot* plotDatasets = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    if(mass==750) plotDatasets = w->var("mgg")->frame(Range(550,900),Bins(100));
    c1->cd(1);
    c1->SetLogy(1);
    plotDatasets->SetTitle("Pseudodata vs sim, cat"+myCut);
    pseudoData[c]->plotOn(plotDatasets,MarkerColor(kRed));
    sigToFit[c]->plotOn(plotDatasets);
    plotDatasets->Draw();  
    c1->SaveAs("plots/compareDataset_cat"+myCut+"_LOG.png");
    c1->SetLogy(0);
    if(mass==750)  plotDatasets = w->var("mgg")->frame(Range(710,780),Bins(100));
    if(mass==1500) plotDatasets = w->var("mgg")->frame(Range(1400,1600),Bins(100));
    plotDatasets->SetTitle("Pseudodata vs sim, cat"+myCut);
    pseudoData[c]->plotOn(plotDatasets,MarkerColor(kRed));
    sigToFit[c]->plotOn(plotDatasets);
    plotDatasets->Draw();
    c1->SaveAs("plots/compareDataset_cat"+myCut+".png");

    // 2 components separated for checks, CB
    RooPlot* plotDatasetsCB = mggGenCB->frame(Range(0.7,1.3),Bins(100));
    c1->cd(1);
    c1->SetLogy(1);
    plotDatasetsCB->SetTitle("Pseudodata vs PDF, CB part, cat"+myCut);
    pseudoDataCB[c]->plotOn(plotDatasetsCB,MarkerColor(kRed));
    ResponseDoubleCB.plotOn(plotDatasetsCB,LineColor(kAzure+1)); 
    plotDatasetsCB->Draw();  
    c1->SaveAs("plots/compareDatasetCB_cat"+myCut+"_LOG.png");
    if(mass==750)  plotDatasetsCB = mggGenCB->frame(Range(0.9,1.1),Bins(100));
    if(mass==1500) plotDatasetsCB = mggGenCB->frame(Range(0.9,1.1),Bins(100));
    c1->cd(1);
    c1->SetLogy(0);
    plotDatasetsCB->SetTitle("Pseudodata vs PDF, CB part, cat"+myCut);
    pseudoDataCB[c]->plotOn(plotDatasetsCB,MarkerColor(kRed));
    ResponseDoubleCB.plotOn(plotDatasetsCB,LineColor(kAzure+1)); 
    plotDatasetsCB->Draw();
    c1->SaveAs("plots/compareDatasetCB_cat"+myCut+".png");

    // 2 components separated for checks, BW
    RooPlot* plotDatasetsBW = mggGenBW->frame(Range(fitMin,fitMax),Bins(100));
    c1->cd(1);
    c1->SetLogy(1);
    if(mass==750)  plotDatasetsBW = mggGenBW->frame(Range(745,755),Bins(100));    
    plotDatasetsBW->SetTitle("Pseudodata vs PDF, BW part, cat"+myCut);
    pseudoDataBW[c]->plotOn(plotDatasetsBW,MarkerColor(kRed));
    SigModelBWG.plotOn(plotDatasetsBW,LineColor(kGreen+1)); 
    plotDatasetsBW->Draw();  
    c1->SaveAs("plots/compareDatasetBW_cat"+myCut+"_LOG.png");
    c1->SetLogy(0);
    if(mass==750)  plotDatasetsBW = mggGenBW->frame(Range(749,751),Bins(100));    
    if(mass==1500) plotDatasetsBW = mggGenBW->frame(Range(1400,1600),Bins(100));    
    plotDatasetsBW->SetTitle("Pseudodata vs PDF, BW part, cat"+myCut);
    pseudoDataBW[c]->plotOn(plotDatasetsBW,MarkerColor(kRed));
    max = plotDatasetsBW->GetMaximum();
    plotDatasetsBW->GetYaxis()->SetRangeUser(0.01, max*1.2);
    SigModelBWG.plotOn(plotDatasetsBW,LineColor(kGreen+1)); 
    plotDatasetsBW->Draw();
    c1->SaveAs("plots/compareDatasetBW_cat"+myCut+".png");

    // Ratio plot
    gStyle->SetOptStat(0);
    TCanvas *cr = new TCanvas("cr", "cr", 10,10,700,700);
    cr->SetFillColor(kWhite);
    cr->Draw();
    TPad *pad1 = new TPad("main","",0, 0.3, 1.0, 1.0);
    pad1->SetTopMargin(0.20);
    pad1->SetBottomMargin(0.02);
    pad1->SetGrid();
    TPad *pad2 = new TPad("ratio", "", 0, 0, 1.0, 0.3);
    pad2->SetTopMargin(0.05);
    pad2->SetBottomMargin(0.30);
    pad2->SetGrid();
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    H_orig->Draw("hist");
    H_toys->Draw("samehist");
    TH1F *H_ratio = (TH1F*)H_orig->Clone();
    H_ratio->Divide(H_toys);
    H_ratio->SetLineColor(4);
    H_ratio->SetLineWidth(2);
    H_ratio->SetMinimum(0);
    H_ratio->SetMaximum(3);
    pad2->cd();
    H_ratio->Draw("histE");
    cr->SaveAs("plots/ratioDatasets_cat"+myCut+".png");
    cr->SaveAs("plots/ratioDatasets_cat"+myCut+".root");


    // IMPORTANT: fix all pdf parameters to constant
    w->defineSet("ConvolutedPdfParam_cat"+myCut,RooArgSet( *w->var("SigF_sigma_cat"+myCut), 
							   *w->var("SigF_alpha1_cat"+myCut),
							   *w->var("SigF_alpha2_cat"+myCut),
							   *w->var("SigF_n1_cat"+myCut),
							   *w->var("SigF_n2_cat"+myCut),	   
							   *w->var("SigF_mean_cat"+myCut),
							   *w->var("sigmaBW_cat"+myCut),
							   *w->var("meanBW_cat"+myCut)));
    
    SetConstantParams(w->set("ConvolutedPdfParam_cat"+myCut));
    w->Print("V");
  }
}

//------------------------------------------------------------------------------------------------------
// Fit signal using toys from fitted doubleCB and binned theory distribution
void SigModelFromBinnedToys(RooWorkspace* w, Float_t mass, TString coupling) {

  Float_t MASS(mass);  
  int iMass = (int)mass;
  
  // Original dataset 
  RooDataSet* sigToFitReco[NCAT];

  // RooRealVars from the WS
  RooRealVar* mgg    = w->var("mgg"); 
  RooRealVar* mggGen = w->var("mggGen");

  // RooRealVars to generate
  RooRealVar* mggGenCB = new RooRealVar("mggGenCB", "M(gg)", -1., 2., "GeV");  
  
  // Toy dataset for real work
  RooDataSet* pseudoData[NCAT];

  // RooDataHist with mgg at gen level
  TFile *myRDHistFile = new TFile("outputHits.root","READ");
  TString myName= TString::Format("bSigWeightGen_mass%d",iMass)+TString("_")+coupling;
  cout << "loading " << myName << endl;
  RooDataHist *bSigWeightGen = (RooDataHist*)myRDHistFile->Get(myName);  
  RooHistPdf hSigWeightGen("hSigWeightGen","hSigWeightGen",*mggGen,*bSigWeightGen,0) ;

  // Fit to Signal 
  for (int c=0; c<NCAT; ++c) {
    cout << "---------- Category = " << c << endl;

    TString myCut = "EBHighR9";
    if (c==0)      myCut = "EBHighR9";
    else if (c==1) myCut = "EBLowR9";
    else if (c==2) myCut = "EEHighR9";
    else if (c==3) myCut = "EELowR9";    

    // Original dataset
    sigToFitReco[c] = (RooDataSet*) w->data("SigWeight_cat"+myCut);
    int origEntries = sigToFitReco[c]->numEntries();

    // Pseudodata from our generation
    pseudoData[c] = new RooDataSet("pseudoData","pseudoData", RooArgSet(*mgg));  
    
    // DoubleCB - must be centred at 1 with all parameters fixed to the result of the fit for this mass at k=0.01 (or no resonance)
    RooFormulaVar CBmean("CBmean_cat"+myCut,"","@0",*w->var("Sig_mean_cat"+myCut));
    RooFormulaVar CBsigma("CBsigma_cat"+myCut,"","@0",*w->var("Sig_sigma_cat"+myCut));
    RooFormulaVar CBalpha1("CBalpha1_cat"+myCut,"","@0",*w->var("Sig_alpha1_cat"+myCut));
    RooFormulaVar CBn1("CBn1_cat"+myCut,"","@0",*w->var("Sig_n1_cat"+myCut));
    RooFormulaVar CBalpha2("CBalpha2_cat"+myCut,"","@0",*w->var("Sig_alpha2_cat"+myCut));
    RooFormulaVar CBn2("CBn2_cat"+myCut,"","@0",*w->var("Sig_n2_cat"+myCut));
    RooDoubleCB ResponseDoubleCB("ResponseDoubleCB_cat"+myCut,"ResponseDoubleCB_cat"+myCut, *mggGenCB, CBmean, CBsigma, CBalpha1, CBn1, CBalpha2, CBn2);


    // to check the generation
    TH1F *H_orig = new TH1F("H_orig","H_orig",45,1250,1700);
    TH1F *H_toys = new TH1F("H_toys","H_toys",45,1250,1700);
    H_orig->Sumw2();
    H_toys->Sumw2();
    H_toys->SetTitle("");
    H_orig->SetTitle("");
    H_toys->SetLineColor(2);
    H_orig->SetLineColor(1);
    H_toys->SetLineWidth(2);
    H_orig->SetLineWidth(2);
    if(mass==750) {
      H_orig = new TH1F("H_orig","H_orig",40,500,900);
      H_toys = new TH1F("H_toys","H_toys",40,500,900);
    }

    // Generation from the two pdfs
    cout << "generating " << origEntries << " entries" << endl;
    for (int ii=0; ii<origEntries; ii++) {
      if (ii%500==0) cout << ii << endl;
      
      RooDataSet* dataCB = ResponseDoubleCB.generate(*mggGenCB,1);  
      RooDataSet* dataBW = hSigWeightGen.generate(*mggGen,1,AutoBinned(false));
      RooArgSet setCB = *dataCB->get(0);
      RooArgSet setBW = *dataBW->get(0);
      RooRealVar* varCB = (RooRealVar*)setCB.find("mggGenCB");      
      RooRealVar* varBW = (RooRealVar*)setBW.find("mggGen");      
      float mggCB = varCB->getVal();
      float mggBW = varBW->getVal();

      *mgg = mggCB*mggBW;  

      pseudoData[c]->add(RooArgSet(*mgg));   

      // Fill histos
      RooArgSet setOriginal = *sigToFitReco[c]->get(ii);
      RooRealVar* varOriginal = (RooRealVar*)setOriginal.find("mgg");
      H_orig->Fill(varOriginal->getVal());
      H_toys->Fill(mggCB*mggBW);
    }

    cout << "my pseudodata" << endl;
    pseudoData[c]->Print("V");
    w->import(*pseudoData[c],Rename("SigPseudodata_cat"+myCut));
  

    // check that the generation was ok
    TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);
    RooPlot* plotDatasets = w->var("mgg")->frame(Range(1250,1700),Bins(45));
    if(mass==750)  plotDatasets = w->var("mgg")->frame(Range(500,900),Bins(40));
    if(mass==1500) plotDatasets = w->var("mgg")->frame(Range(1250,1700),Bins(45));
    plotDatasets->SetTitle("Pseudodata vs sim, cat"+myCut);
    pseudoData[c]->plotOn(plotDatasets,MarkerColor(kRed));
    sigToFitReco[c]->plotOn(plotDatasets);
    c1->cd(1);
    c1->SetLogy(0);
    plotDatasets->Draw();  
    c1->SaveAs("plots/compareDataset_cat"+myCut+"_binned.png");
    c1->SetLogy();
    plotDatasets->Draw();
    c1->SaveAs("plots/compareDataset_cat"+myCut+"_LOG_binned.png");


    // Ratio plot
    gStyle->SetOptStat(0);
    TCanvas *cr = new TCanvas("cr", "cr", 10,10,700,700);
    cr->SetFillColor(kWhite);
    cr->Draw();
    TPad *pad1 = new TPad("main","",0, 0.3, 1.0, 1.0);
    pad1->SetTopMargin(0.20);
    pad1->SetBottomMargin(0.02);
    pad1->SetGrid();
    TPad *pad2 = new TPad("ratio", "", 0, 0, 1.0, 0.3);
    pad2->SetTopMargin(0.05);
    pad2->SetBottomMargin(0.30);
    pad2->SetGrid();
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    H_orig->Draw("hist");
    H_toys->Draw("samehist");
    pad2->cd();
    TH1F *H_ratio = (TH1F*)H_orig->Clone();
    H_ratio->Divide(H_toys);
    H_ratio->SetLineColor(4);
    H_ratio->SetLineWidth(2);
    H_ratio->SetMinimum(0);
    H_ratio->SetMaximum(3);
    H_ratio->Draw("histE");
    cr->SaveAs("plots/ratioDatasets_cat"+myCut+"_binned.png");
    cr->SaveAs("plots/ratioDatasets_cat"+myCut+"_binned.root");


    /*
    // Now build a new CB centred in 0 and with free parameters
    RooFormulaVar CBmeanF(TString::Format("CBmeanF_cat%d",c),"","@0",*w->var(TString::Format("SigF_mean_cat%d",c)) );
    RooFormulaVar CBsigmaF(TString::Format("CBsigmaF_cat%d",c),"","@0",*w->var(TString::Format("SigF_sigma_cat%d",c)) );
    RooFormulaVar CBalpha1F(TString::Format("CBalpha1F_cat%d",c),"","@0",*w->var(TString::Format("SigF_alpha1_cat%d",c)) );
    RooFormulaVar CBn1F(TString::Format("CBn1F_cat%d",c),"","@0",*w->var(TString::Format("SigF_n1_cat%d",c)) );
    RooFormulaVar CBalpha2F(TString::Format("CBalpha2F_cat%d",c),"","@0",*w->var(TString::Format("SigF_alpha2_cat%d",c)) );
    RooFormulaVar CBn2F(TString::Format("CBn2F_cat%d",c),"","@0",*w->var(TString::Format("SigF_n2_cat%d",c)) );
    RooDoubleCB ResponseDoubleCBF(TString::Format("ResponseDoubleCBF_cat%d",c),TString::Format("ResponseDoubleCBF_cat%d",c) , *mgg, CBmeanF, CBsigmaF, CBalpha1F, CBn1F, CBalpha2F, CBn2F);    
    mgg->setBins(5000, "cache");  

    // And convolve with the nominal BW - everything fixed
    //RooFormulaVar meanBW(TString::Format("massBW_cat%d",c),"","@0",*w->var(TString::Format("meanBW_cat%d",c)) );  
    //RooFormulaVar sigmaBW(TString::Format("widthBW_cat%d",c),"","@0",*w->var(TString::Format("sigmaBW_cat%d",c)) );
    //RooBreitWigner SigModelBW(TString::Format("BW_cat%d",c),TString::Format("BW_cat%d",c), *mgg, meanBW, sigmaBW);



    RooFFTConvPdf* ConvolutedRes;
    ConvolutedRes = new RooFFTConvPdf(TString::Format("mggSig_cat%d",c),TString::Format("mggSig_cat%d",c), *mgg,SigModelBW, ResponseDoubleCBF);
4    w->import(*ConvolutedRes);
    
    // Fit and Plot    
    float fitMin = 1250.;
    float fitMax = 1700.;
    if (mass==750)  {fitMin = 500.;   fitMax = 1200.; }
    if (mass==1500) {fitMin = 1250.;  fitMax = 1700.; }
    if (mass==5000) {fitMin = 4000.;  fitMax = 5500.; }

    RooFitResult* fitresults_CB = (RooFitResult* ) ConvolutedRes->fitTo(*pseudoData[c], SumW2Error(kFALSE), Range(fitMin,fitMax), RooFit::Save(kTRUE));
    fitresults_CB->Print("V");

    RooPlot* plotOnlyResPdf = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    pseudoData[c]->plotOn(plotOnlyResPdf, LineColor(kRed), LineStyle(kDashed));
    double max = plotOnlyResPdf->GetMaximum();
    

    RooPlot* plotPhotonsMassAll = w->var("mgg")->frame(Range(fitMin,fitMax),Bins(100));
    plotPhotonsMassAll->SetTitle(TString::Format("DoubleCB conv BW from toys, cat%d",c));
    pseudoData[c]->plotOn(plotPhotonsMassAll);
    SigModelBW.plotOn(plotPhotonsMassAll, LineColor(kGreen), LineStyle(kDashed));
    ResponseDoubleCBF.plotOn(plotPhotonsMassAll, LineColor(kRed), LineStyle(kDashed));
    ConvolutedRes->plotOn(plotPhotonsMassAll, LineColor(kBlue));
    
    TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);
    c1->cd(1);
    plotPhotonsMassAll->Draw();  
    plotPhotonsMassAll->GetYaxis()->SetRangeUser(0.01, max*1.2);
    plotPhotonsMassAll->GetXaxis()->SetRangeUser(210, 290);
    
    TLegend *legmc = new TLegend(0.6, 0.58, 0.091, 0.91, "");
    legmc->AddEntry(plotPhotonsMassAll->getObject(0),"Pseudodata","LPE");
    legmc->AddEntry(plotPhotonsMassAll->getObject(1),"BW","L");
    legmc->AddEntry(plotPhotonsMassAll->getObject(2),"DoubleCB","L");
    legmc->AddEntry(plotPhotonsMassAll->getObject(3),"BW #otimes Resolution","L");
    legmc->SetTextSize(0.0206044);
    legmc->SetTextFont(42);
    legmc->SetBorderSize(0);
    legmc->SetFillStyle(0);
    legmc->Draw();
    
    int massI(mass);
    c1->SetLogy();
    plotPhotonsMassAll->GetXaxis()->SetTitle("m_{#gamma #gamma}[GeV]");

    c1->SetLogy(0);
    c1->SaveAs("plots/signalCBCconvBW"+TString::Format(("_M%d_cat%d_fromPseudoData.png"),massI, c));
    
    c1->SetLogy();
    plotPhotonsMassAll->GetYaxis()->SetRangeUser(0.01,max*10. );
    plotPhotonsMassAll->GetXaxis()->SetRangeUser(210, 290);
    c1->SaveAs("plots/signalCBCconvBW"+TString::Format(("_M%d_cat%d_LOG_fromPseudoData.png"),massI,c));


    // IMPORTANT: fix all pdf parameters to constant
    w->defineSet(TString::Format("ConvolutedPdfParam_cat%d",c),RooArgSet( *w->var(TString::Format("SigF_sigma_cat%d",c)), 
									  *w->var(TString::Format("SigF_alpha1_cat%d",c)),
									  *w->var(TString::Format("SigF_alpha2_cat%d",c)),
									  *w->var(TString::Format("SigF_n1_cat%d",c)),
									  *w->var(TString::Format("SigF_n2_cat%d",c)),	   
									  *w->var(TString::Format("SigF_mean_cat%d",c)),
									  *w->var(TString::Format("sigmaBW_cat%d",c)),
									  *w->var(TString::Format("meanBW_cat%d",c))));
    
    SetConstantParams(w->set(TString::Format("ConvolutedPdfParam_cat%d",c)));
    w->Print("V");
    */

  }
}
//------------------------------------------------------------------------------------------------------


// Write signal pdfs and datasets into the workspace 
void MakeSigWS(RooWorkspace* w, const char* fileBaseName, Float_t mass, std::string coupling){
  
  TString wsDir = "workspaces/";
  Int_t ncat = NCAT;
  
  RooWorkspace *wAll = new RooWorkspace("w_all","w_all");  

  // Retrieve stuff
  w->Print("V");

  // convolution
  wAll->import(*w->pdf("mggSig_catEBHighR9"));
  wAll->import(*w->pdf("mggSig_catEBLowR9"));
  wAll->import(*w->pdf("mggSig_catEEHighR9"));
  wAll->import(*w->pdf("mggSig_catEELowR9"));
  
  // real dataset
  wAll->import(*w->data("SigWeight_catEBHighR9"));   
  wAll->import(*w->data("SigWeight_catEBLowR9"));   
  wAll->import(*w->data("SigWeight_catEEHighR9"));   
  wAll->import(*w->data("SigWeight_catEELowR9"));   

  // dataset from toys
  // wAll->import(*w->data("SigPseudodata_catEBHighR9")); 
  // wAll->import(*w->data("SigPseudodata_catEBLowR9"));   
  // wAll->import(*w->data("SigPseudodata_catEEHighR9"));   
  // wAll->import(*w->data("SigPseudodata_catEELowR9"));   

  std::cout << "done with importing signal pdf and datasets" << std::endl;

  wAll->import(*w->var("massReduced"));
  wAll->import(*w->var("mggGen"));

  // pdf normalization
  RooRealVar *mggSig_catEBHighR9_norm = new RooRealVar("mggSig_catEBHighR9_norm", "mggSig_catEBHighR9_norm", (*w->data("SigWeight_catEBHighR9")).sumEntries(), (*w->data("SigWeight_catEBHighR9")).sumEntries(), "");
  RooRealVar *mggSig_catEBLowR9_norm  = new RooRealVar("mggSig_catEBLowR9_norm",  "mggSig_catEBLowR9_norm",  (*w->data("SigWeight_catEBLowR9")).sumEntries(), (*w->data("SigWeight_catEBLowR9")).sumEntries(), "");
  RooRealVar *mggSig_catEEHighR9_norm = new RooRealVar("mggSig_catEEHighR9_norm", "mggSig_catEEHighR9_norm", (*w->data("SigWeight_catEEHighR9")).sumEntries(), (*w->data("SigWeight_catEEHighR9")).sumEntries(), "");
  RooRealVar *mggSig_catEELowR9_norm  = new RooRealVar("mggSig_catEELowR9_norm",  "mggSig_catEELowR9_norm",  (*w->data("SigWeight_catEELowR9")).sumEntries(), (*w->data("SigWeight_catEELowR9")).sumEntries(), "");
  wAll->import(*mggSig_catEBHighR9_norm);
  wAll->import(*mggSig_catEBLowR9_norm);
  wAll->import(*mggSig_catEEHighR9_norm);
  wAll->import(*mggSig_catEELowR9_norm);

  // saving
  int imass = (int)mass;
  TString filename(wsDir+TString(fileBaseName)+TString::Format("_m%d",imass)+"_"+TString(coupling)+".root");
  TFile fileWs(filename,"RECREATE");
  fileWs.cd();
  wAll->writeToFile(filename);
  cout << "Write signal workspace in: " << filename << " file" << endl;
  
  return;
}

void runfits(const Float_t mass=1500, string coupling="001") {

  //******************************************************************//
  //  Steps:
  //     - create signal and background data sets 
  //     - make and fit signal and background  models 
  //     - write signal and background workspaces in root files
  //     - write data card
  //*******************************************************************//
  
  TString fileBaseName("HighMassGG");    
  TString fileBkgName("HighMassGG.inputbkg");
  HLFactory hlf("HLFactory", "HighMassGG.rs", false);
  RooWorkspace* w = hlf.GetWs();
 
  // import luminosity in the ws
  RooRealVar lumi("lumi","lumi",Lum);
  w->import(lumi); 
  
  // range for the variables
  w->var("mgg")->setMin(MINmass);
  w->var("mgg")->setMax(MAXmass);
  w->var("mggGen")->setMin(MINmass);
  w->var("mggGen")->setMax(MAXmass);
  w->Print("v");
  
  cout << endl; 
  cout << "Now add signal data" << endl;
  AddSigData(w, mass, coupling);   

  cout << endl; 
  if (wantResponse) {
    cout << "Now prepare signal model fit - resolution function" << endl;  
    // SigModelResponseDoubleCBFit(w, mass, coupling); 
    SigModelResponseReducedDoubleCBFit(w, mass, coupling); 
  }    

  cout << endl;
  if (wantGenLevel) {
    cout << "Now try BW only on gen level mgg" << endl;
    SigModelBWFit(w, mass, coupling);     
  }
  
  cout << endl;
  cout << "Now prepare signal model fit - resolution function x BW" << endl;  
  SigModelFitDoubleCBConvBW(w, mass);

  cout << endl;
  cout << "Alternative: generate following nominal BW and fitted doubleCB and build another dataset" << endl;
  //SigModelFromToys(w, mass);  
  //SigModelFromBinnedToys(w, mass,coupling); 
  
  cout << endl;
  cout << "Now preparing signal WS" << endl;
  MakeSigWS(w, fileBaseName, mass, coupling);

  return;
}
