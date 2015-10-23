#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooBinning.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooHistPdf.h"
#include "RooMomentMorph.h"
#include "RooNumConvPdf.h"   
#include "RooFFTConvPdf.h"
#include "RooFitResult.h"     
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
#include "TROOT.h"
#include "TVectorD.h"
#include <iostream>
#include "TCanvas.h"

using namespace RooFit;
using namespace std;

// to be modified:
static const Int_t NCAT = 2;  

// Definition of the variables in the input ntuple
RooArgSet* defineVariables() {

  RooRealVar* mgg        = new RooRealVar("mgg",        "M(gg)",       0, 12000, "GeV");   
  RooRealVar* mggGen     = new RooRealVar("mggGen",     "M(gg) gen",   0, 12000, "GeV");   
  RooRealVar* eventClass = new RooRealVar("eventClass", "eventClass", -10,   10, "");
  RooRealVar* weight     = new RooRealVar("weight",     "weightings",   0, 1000, "");   

  RooArgSet* ntplVars = new RooArgSet(*mgg, *mggGen, *eventClass, *weight);                  
  
  return ntplVars;
}

// Preparing the resolution histograms
void MakeResolutionHisto(RooWorkspace *w, TString filename, bool newFile, int mass, TString coupling) {

  TString myMass = TString::Format("%d",mass);
  Int_t ncat = NCAT;
  
  // the roorealvar 
  RooRealVar* deltaM = w->var("deltaM");    

  // Output file 
  TFile *theResoFile;
  if (newFile) theResoFile = new TFile(filename,"RECREATE");
  else theResoFile = new TFile(filename,"UPDATE");
  
  // Input file and tree
  TString inDir = "../macro/allFiles/";
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");

  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=0 && mgg<=12000 && mggGen>=0 && mggGen<=12000");     

  // Loop over categories
  for (int c=0; c<ncat; ++c) {

    // Histos
    TH1D *resolH = new TH1D("resolH","resolH",200,-1000.,200.);    // chiara: qui va studiato se lasciarlo costante o dipendente dalla massa
    resolH->Sumw2();
  
    // Projecting the tree into the histo
    if (c==0) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==0"));
    if (c==1) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==1"));

    // Now make the roodatahist
    RooDataHist resolRDH("resolRDH","resolRDH",*deltaM,Import(*resolH));        
    cout << "Resolution datahist summary for mass " << mass << endl;  
    resolRDH.Print();
    cout << endl;  

    // Saving in the root file
    theResoFile->cd();
    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
    TString nameH = TString::Format("resolH_mass%d",mass)+TString::Format("_cat")+myCut;
    resolH->Write(nameH);
    TString nameRDH = TString::Format("resolRDH_mass%d",mass)+TString::Format("_cat")+myCut;
    resolRDH.Write(nameRDH);

    // Plot to check
    TCanvas *c1 = new TCanvas("c1","c1",1);
    RooPlot* myPlot = deltaM->frame(Range(-100,100),Bins(200));  
    resolRDH.plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));    
    myPlot->Draw();
    TString canvasName = TString(Form("CheckResol_Cat"+myCut+".png"));
    c1->SaveAs(canvasName);       

    delete resolH;
  }

  // Closing the output file
  theResoFile->Close();
}

// Preparing the intrinsic width histogram
void MakeIntrinsicWidthHisto(RooWorkspace *w, TString filename, bool newFile, int mass, TString coupling) {

  TString myMass = TString::Format("%d",mass);
  Int_t ncat = NCAT;

  // Roorealvar 
  RooRealVar* deltaMgen = w->var("deltaMgen");    

  // Output file 
  TFile *theIntrinsicWFile;
  if (newFile) theIntrinsicWFile = new TFile(filename,"RECREATE");
  else theIntrinsicWFile = new TFile(filename,"UPDATE");
  
  // Input file and tree
  TString inDir = "../macro/allFiles/";
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");

  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=0 && mgg<=12000 && mggGen>=0 && mggGen<=12000");   

  // Loop over categories
  for (int c=0; c<ncat; ++c) {

    // Histos
    TH1D *intWidthH = new TH1D("intWidthH","intWidthH",250,-1000.,2000.);        // chiara: qui andra' studiato se costante o dipendente dalla massa
    intWidthH->Sumw2();
  
    // Projecting the tree
    TString express = TString::Format("mggGen-%d",mass);
    if (c==0) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==0"));
    if (c==1) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==1"));

    // Now make the roodatahist
    RooDataHist intWidthRDH("intWidthRDH","intWidthRDH",*deltaMgen,Import(*intWidthH));   
    cout << "Intrinsic width datahist summary for mass " << mass << endl;  
    intWidthRDH.Print();
    cout << endl;  

    // Saving in the root file
    theIntrinsicWFile->cd();
    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
    TString nameH = TString::Format("intWidthH_mass%d",mass)+TString::Format("_cat")+myCut;
    intWidthH->Write(nameH);
    TString nameRDH = TString::Format("intWidthRDH_mass%d",mass)+TString::Format("_cat")+myCut;
    intWidthRDH.Write(nameRDH);

    // Plot to check
    TCanvas *c1 = new TCanvas("c1","c1",1);
    RooPlot* myPlot = deltaMgen->frame(Range(-100,100),Bins(200));  
    intWidthRDH.plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));    
    myPlot->Draw();
    TString canvasName = TString(Form("CheckIntrinsicWidth_Cat"+myCut+".png"));
    c1->SaveAs(canvasName);       
    
    delete intWidthH;
  }

  // Closing the output file
  theIntrinsicWFile->Close();
}

/*
// Load the morphing and make a convolution
void ConvolutionFromMorphing(RooWorkspace* w){

  // Only for closure test
  float mass = 1500; 
  RooDataSet* sigToFit[NCAT];
  sigToFit[0] = (RooDataSet*) w->data("SigWeight_catEBEB_mass1500");
  sigToFit[1] = (RooDataSet*) w->data("SigWeight_catEBEE_mass1500");
  RooRealVar* mgg = w->var("mgg");    

  // Morphing variable
  RooRealVar* mu = w->var("mu");    

  TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);   
  c1->cd(1);

  // Morphings
  RooAbsPdf *morphInW[NCAT];
  RooAbsPdf *morphRes[NCAT];
  for (int c=0; c<NCAT; ++c) {   
    cout << "---------- Category = " << c << endl; 

    if (c==0) morphInW[c] = (RooAbsPdf*) w->pdf("morphInWCat0");
    if (c==1) morphInW[c] = (RooAbsPdf*) w->pdf("morphInWCat1");
    if (c==0) morphRes[c] = (RooAbsPdf*) w->pdf("morphResCat0");
    if (c==1) morphRes[c] = (RooAbsPdf*) w->pdf("morphResCat1");
    morphInW[c]->Print();
    morphRes[c]->Print();

    mu->setVal(1500);    // chiara
    mu->setConstant();
    
    // convolution
    float fitMin = 1250.;     
    float fitMax = 1750.;     
    
    RooNumConvPdf convol("convol","convol",*mgg,*morphInW[c],*morphRes[c]);
    //RooNumConvPdf convol("convol","convol",*mu,*morphRes[c],*morphInW[c]);
    RooRealVar* W = new RooRealVar("W","W",100);    // ok?
    W->setConstant();        
    RooRealVar* C = new RooRealVar("C","C",-5);    // ok?
    C->setConstant();      
    convol.setConvolutionWindow(*C,*W);       
    convol.Print();
    cout << "done!!" << endl;

    // Plot to check
    TString myCut = "EBEB"; 
    if (c==1) myCut = "EBEE";  
    
    //RooFitResult* fitresults = (RooFitResult* ) convol.fitTo(*sigToFit[c], SumW2Error(kFALSE), Range(fitMin,fitMax), RooFit::Save(kTRUE)); 
    //fitresults->Print();

    RooPlot* myPlot1 = mgg->frame(Range(fitMin,fitMax),Bins(100));  
    morphInW[c]->plotOn(myPlot1, LineColor(kYellow), LineStyle(kDashed));    
    double max = myPlot1->GetMaximum();
    cout << "A" << endl;

    //RooPlot* myPlot = mgg->frame(Range(fitMin,fitMax),Bins(100)); 
    RooPlot* myPlot = mgg->frame();
    myPlot->SetTitle("Convolution, cat"+myCut);     
    sigToFit[c]->plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));
    morphRes[c]->plotOn(myPlot, LineColor(kGreen));
    morphInW[c]->plotOn(myPlot, LineColor(kYellow));
    convol.plotOn(myPlot, LineColor(kBlue));
    cout << "B" << endl;

    myPlot->Draw();
    myPlot->GetYaxis()->SetRangeUser(0.01, max*1.2); 
    TString canvasName = TString(Form("closure_cat"+myCut+".png"));
    c1->SaveAs(canvasName);   
  }
}
*/

// Load the roodatahist and make a convolution 
void ConvolutionFromRDH(RooWorkspace* w, Int_t mass, TString coupling) {     

  // Dataset - only for closure test   
  RooDataSet* sigToFit[NCAT];   

  // RooRealVar
  RooRealVar* mgg = w->var("mgg");  
  RooRealVar* mggGen = w->var("mggGen");  
  RooRealVar* deltaM = w->var("deltaM");    
  RooRealVar* deltaMgen = w->var("deltaMgen");    
  RooRealVar* mH = w->var("mH");
  mH->setConstant();    

  // Resolution centred in zero in mgg
  RooFormulaVar *deltaM_formula = new RooFormulaVar("deltaM_formula","","@0",RooArgList(*w->var("mgg")));
  RooArgList pdfObsRes;
  pdfObsRes.add(*deltaM_formula);  

  // Intrinsic width
  RooFormulaVar *deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0-@1",RooArgList(*w->var("mgg"),*mH));        // centred in mass 
  //RooFormulaVar *deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0",RooArgList(*w->var("mgg")));             // centred in zero
  RooArgList pdfObsInw;
  pdfObsInw.add(*deltaMgen_formula);
  
  RooArgList histObsRes;
  histObsRes.add(*deltaM);

  RooArgList histObsInw;
  histObsInw.add(*deltaMgen);

  // Files with resolution and mgg roodatahists 
  TFile *fileInw = new TFile("IntrinsicWidthHistos.root"); 
  TFile *fileRes = new TFile("ResolutionHistos.root");  

  // Plots to check
  TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);  
  c1->cd(1);    

  for (int c=0; c<NCAT; ++c) {  

    cout << "---------- Category = " << c << endl;    
    TString myCut = "EBEB";   
    if (c==1) myCut = "EBEE";  

    // reading the roodatahists 
    TString myRDHA = TString(Form("resolRDH_mass%d_cat",mass)+myCut);     
    RooDataHist *resRDH = (RooDataHist*)fileRes->Get(myRDHA);    
    resRDH->Print();   
    TString myRDHB = TString(Form("intWidthRDH_mass%d_cat",mass)+myCut);   
    RooDataHist *inwRDH = (RooDataHist*)fileInw->Get(myRDHB);   
    inwRDH->Print();        
    cout << "RooDataHists taken" << endl;       
    
    // creating the roohistpdfs    
    //RooHistPdf *myHistPdfRes = new RooHistPdf("myHistPdfRes","myHistPdfRes",*mgg,*resRDH,0) ;  
    RooHistPdf *myHistPdfRes = new RooHistPdf("myHistPdfRes","myHistPdfRes",pdfObsRes, histObsRes,*resRDH,0) ;  
    myHistPdfRes->Print(); 
    //RooHistPdf *myHistPdfInW = new RooHistPdf("myHistPdfInW","myHistPdfInW",*mgg,*inwRDH,0) ;  
    RooHistPdf *myHistPdfInW = new RooHistPdf("myHistPdfInW","myHistPdfInW",pdfObsInw, histObsInw,*inwRDH,0) ;  
    myHistPdfInW->Print();        
    cout << "RooHistPdfs done" << endl;      

    // convolution    
    float fitMin = 1250.;          
    float fitMax = 1750.;    
    

    // numerical
    /*
    mgg->setRange(-500,500);
    mgg->setBins(1000);
    RooNumConvPdf convol("convol","convol",*mgg,*myHistPdfInW,*myHistPdfRes);          
    RooRealVar* W = new RooRealVar("W","W",200);    
    W->setConstant();    
    RooRealVar* C = new RooRealVar("C","C",0);  
    C->setConstant();      
    convol.setConvolutionWindow(*C,*W);  
    */

    // analytical
    mgg->setBins(10000, "cache");
    RooFFTConvPdf convol("convol","convol",*mgg,*myHistPdfInW,*myHistPdfRes);          

    // Both
    convol.Print();     
    cout << "done!!" << endl;   

    // Fit and Plot 
    TString myUnbDS = TString(Form("SigWeight_cat"+myCut+"_mass%d",mass));   
    sigToFit[c] = (RooDataSet*) w->data(myUnbDS); 
    sigToFit[c]->Print();      

    RooPlot* myPlot1 = mgg->frame(Range(fitMin,fitMax),Bins(100));   
    sigToFit[c]->plotOn(myPlot1, LineColor(kRed), LineStyle(kDashed));           
    double max = myPlot1->GetMaximum(); 

    RooPlot* myPlot = mgg->frame(Range(fitMin,fitMax),Bins(50)); 
    myPlot->SetTitle("Convolution, cat"+myCut);    
    sigToFit[c]->plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));         
    convol.plotOn(myPlot, LineColor(kBlue));  
    myHistPdfRes->plotOn(myPlot, LineColor(kRed));
    myHistPdfInW->plotOn(myPlot, LineColor(kYellow));

    myPlot->Draw();           
    myPlot->GetYaxis()->SetRangeUser(0.01, max*3.);      
    TString canvasName = TString(Form("closure_cat"+myCut+".png"));    
    c1->SetLogy(0);
    c1->SaveAs(canvasName); 
    c1->SetLogy(1);
    canvasName = TString(Form("closure_cat"+myCut+"_log.png"));    
    c1->SaveAs(canvasName); 

    delete myHistPdfRes;    
    delete myHistPdfInW;     
  }

  // deleting
  delete fileInw;   
  delete fileRes;      
}

//-------------------------------------------------------
/*
// Load the morphing and make a convolution by hand tossing toy mcs
void ConvolutionWithToysFromMorphing(RooWorkspace* w){

  // Only for closure test: original dataset and variable
  float mass = 1500; 
  RooDataSet* sigToFit[NCAT];
  sigToFit[0] = (RooDataSet*) w->data("SigWeight_catEBEB_mass1500");
  sigToFit[1] = (RooDataSet*) w->data("SigWeight_catEBEE_mass1500");
  RooRealVar* mgg = w->var("mgg");    

  // Toy dataset for real work     
  RooDataSet* pseudoData[NCAT];    

  // Toy dataset for checks  
  RooDataSet* pseudoDataCB[NCAT];   
  RooDataSet* pseudoDataBW[NCAT];    

  // Other RooRealVar   
  RooRealVar* mggGenCB = new RooRealVar("mggGenCB", "M(gg)", -1000., 100., "GeV");  
  RooRealVar* mggGenBW = new RooRealVar("mggGenBW", "M(gg)", 10., 9000., "GeV");  

  // Morphing variable
  RooRealVar* mu = w->var("mu");    

  // Control plots
  TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);   
  c1->cd(1);

  // Morphings
  RooAbsPdf *morphInW[NCAT];
  RooAbsPdf *morphRes[NCAT];
  for (int c=0; c<NCAT; ++c) {   
    cout << "---------- Category = " << c << endl; 

    if (c==0) morphInW[c] = (RooAbsPdf*) w->pdf("morphInWCat0");
    if (c==1) morphInW[c] = (RooAbsPdf*) w->pdf("morphInWCat1");
    if (c==0) morphRes[c] = (RooAbsPdf*) w->pdf("morphResCat0");
    if (c==1) morphRes[c] = (RooAbsPdf*) w->pdf("morphResCat1");
    morphInW[c]->Print();
    morphRes[c]->Print();
    mu->setVal(1500); // chiara
    
    // Pseudodata from our generation  
    pseudoData[c]   = new RooDataSet("pseudoData",  "pseudoData",   RooArgSet(*mgg));       
    pseudoDataCB[c] = new RooDataSet("pseudoDataCB","pseudoDataCB", RooArgSet(*mggGenCB));    
    pseudoDataBW[c] = new RooDataSet("pseudoDataBW","pseudoDataBW", RooArgSet(*mggGenBW)); 

    // to check the generation   
    TH1F *H_orig = new TH1F("H_orig","H_orig",45,1250,1750);  
    TH1F *H_toys = new TH1F("H_toys","H_toys",45,1250,1750);  
    H_orig->Sumw2();   
    H_toys->Sumw2();   
    H_orig->SetTitle("");     
    H_toys->SetTitle("");     
    H_orig->SetLineColor(1);    
    H_orig->SetLineColor(1);      
    H_toys->SetLineWidth(2); 
    H_orig->SetLineWidth(2);   

    // Generation from the two morphings
    int origEntries = sigToFit[c]->numEntries(); 
    cout << "generating " << origEntries << " entries" << endl; 
    for (int ii=0; ii<origEntries; ii++) {  
      if (ii%500==0) cout << ii << endl; 
      RooDataSet* dataCB = morphRes[c]->generate(*mggGenCB,1);  
      RooDataSet* dataBW = morphInW[c]->generate(*mggGenBW,1);  
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

      delete dataCB;  
      delete dataBW;    
    }

    // Noi build a roodatahist and a rohistpdf for the generated toys
    mgg->setRange(10,9000);   // chiara
    mgg->setBins(890);        // chiara 
    RooDataHist bPseudodata("bPseudodata","bPseudodata",RooArgList(*mgg),*pseudoData[c]); 
    bPseudodata.Print();  
    RooHistPdf rhpdf("rhpdf","rhpdf",*mgg,bPseudodata,0);
    cout << "done!!" << endl;

    // Plot to check
    float fitMin = 1250.;     
    float fitMax = 1750.;     

    TString myCut = "EBEB"; 
    if (c==1) myCut = "EBEE";  

    RooPlot* myPlot1 = mgg->frame(Range(fitMin,fitMax),Bins(100));  
    morphInW[c]->plotOn(myPlot1, LineColor(kYellow), LineStyle(kDashed));    
    double max = myPlot1->GetMaximum();
    cout << "A" << endl;

    RooPlot* myPlot = mgg->frame(Range(fitMin,fitMax),Bins(100)); 
    myPlot->SetTitle("Convolution, cat"+myCut);     
    sigToFit[c]->plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));
    morphRes[c]->plotOn(myPlot, LineColor(kGreen));
    morphInW[c]->plotOn(myPlot, LineColor(kYellow));
    bPseudodata.plotOn(myPlot, LineColor(kBlue));
    cout << "B" << endl;

    myPlot->Draw();
    myPlot->GetYaxis()->SetRangeUser(0.01, max*1.2); 
    TString canvasName = TString(Form("closure_cat"+myCut+".png"));
    c1->SaveAs(canvasName);   
  }
}
*/
//-------------------------------------------------------

// Loading signal data and making roodatasets
void AddSigData(RooWorkspace* w, int mass, TString coupling) {
  
  TString myMass = TString::Format("%d",mass);

  Int_t ncat = NCAT;
  
  // Variables
  RooArgSet* ntplVars = defineVariables();

  // Files
  TString inDir = "../macro/allFiles/";  
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");
  
  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=0 && mgg<=12000 && mggGen>=0 && mggGen<=12000");  
  RooDataSet sigWeighted("sigWeighted","dataset",sigTree,*ntplVars,mainCut,"weight");   
  sigWeighted.Print();

  // defining the reco - gen mass difference
  RooFormulaVar *deltaM_formula = new RooFormulaVar("deltaM_formula","","@0-@1",RooArgList(*w->var("mgg"),*w->var("mggGen"))); 
  RooRealVar* deltaM = (RooRealVar*) sigWeighted.addColumn(*deltaM_formula);       
  deltaM->SetName("deltaM");
  deltaM->SetTitle("deltaM");
  w->import(*deltaM);

  // nominal graviton mass
  RooRealVar* mH = new RooRealVar("mH", "M(G)", 0, 20000, "GeV");    
  mH->setVal(mass);
  mH->setConstant();
  w->import(*mH);

  // defining the gen - mH difference 
  RooFormulaVar *deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0-@1",RooArgList(*w->var("mggGen"),*w->var("mH"))); 
  RooRealVar* deltaMgen = (RooRealVar*) sigWeighted.addColumn(*deltaMgen_formula);       
  deltaMgen->SetName("deltaMgen");
  deltaMgen->SetTitle("deltaMgen");
  w->import(*deltaMgen);

  // Split in categories - mgg as observable
  cout << endl;
  cout << "preparing dataset with observable mgg" << endl;
  RooDataSet* signal[NCAT];
  for (int c=0; c<ncat; ++c) {
    if (c==0) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==0"));
    if (c==1) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==1"));

    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
    w->import(*signal[c],Rename("SigWeight_cat"+myCut+"_mass"+myMass));
    
    cout << "cat " << c << ", signal[c]: " << endl;
    signal[c]->Print("v");
    cout << "---- for category " << c << ", nX for signal[c]:  " << signal[c]->sumEntries() << endl; 
    cout << endl;
  }

  cout << endl;
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << endl;
  cout << endl;

  // Split in categories - deltaM as observable
  cout << endl;
  cout << "preparing dataset with observable deltaM" << endl;
  RooDataSet* signalDm[NCAT];
  for (int c=0; c<ncat; ++c) {
    if (c==0) signalDm[c] = (RooDataSet*) sigWeighted.reduce(*w->var("deltaM"),mainCut+TString::Format("&& eventClass==0"));
    if (c==1) signalDm[c] = (RooDataSet*) sigWeighted.reduce(*w->var("deltaM"),mainCut+TString::Format("&& eventClass==1"));

    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
    w->import(*signalDm[c],Rename("SigWeightDeltaM_cat"+myCut+"_mass"+myMass));
    
    cout << "cat " << c << ", signal[c]: " << endl;
    signalDm[c]->Print("v");
    cout << "---- for category " << c << ", nX for signal[c]:  " << signalDm[c]->sumEntries() << endl; 
    cout << endl;
  }

  cout << endl;
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << endl;
  cout << endl;

  // Split in categories - deltaMgen as observable
  cout << endl;
  cout << "preparing dataset with observable deltaMgen" << endl;
  RooDataSet* signalDmgen[NCAT];
  for (int c=0; c<ncat; ++c) {
    if (c==0) signalDmgen[c] = (RooDataSet*) sigWeighted.reduce(*w->var("deltaMgen"),mainCut+TString::Format("&& eventClass==0"));
    if (c==1) signalDmgen[c] = (RooDataSet*) sigWeighted.reduce(*w->var("deltaMgen"),mainCut+TString::Format("&& eventClass==1"));

    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
    w->import(*signalDm[c],Rename("SigWeightDeltaMgen_cat"+myCut+"_mass"+myMass));
    
    cout << "cat " << c << ", signal[c]: " << endl;
    signalDmgen[c]->Print("v");
    cout << "---- for category " << c << ", nX for signal[c]:  " << signalDm[c]->sumEntries() << endl; 
    cout << endl;
  }

  cout << endl;
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << endl;
  cout << endl;

  cout << "workspace summary" << endl;
  w->Print();
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << endl;
}

/*
// Interpolation
void Interpolation(RooWorkspace* w, vector<int> masses) {

  // Variables
  RooRealVar* mgg = w->var("mgg");    
  RooArgList varlist;
  varlist.add(*mgg);

  // For them morphing
  RooRealVar* mu;
  w->factory("mu[10,9000]");      // chiara
  mu = w->var("mu");

  // PDFs
  RooArgList pdfsResCat0, pdfsResCat1;
  RooArgList pdfsInWCat0, pdfsInWCat1;

  // Files with resolution and mgg roodatahists
  TFile *fileInw = new TFile("IntrinsicWidthHistos.root");
  TFile *fileRes = new TFile("ResolutionHistos.root");

  // Reference points
  int numMass = (int)masses.size();
  TVectorD paramVec(numMass); 

  // To plot
  RooPlot *frameResCat0 = mgg->frame(Range(-200,100));   
  RooPlot *frameResCat1 = mgg->frame(Range(-200,100));   
  RooPlot *frameInWCat0 = mgg->frame(Range(1000,2000));   // chiara  
  RooPlot *frameInWCat1 = mgg->frame(Range(1000,2000));   // chiara  

  // Loop over masses
  for (int ii=0; ii<(int)masses.size(); ii++) {  // loop over masses
    int theMass = masses[ii];
    TString myMass = TString::Format("%d",theMass);

    paramVec[ii] = (double)theMass; 

    for (int c=0; c<NCAT; ++c) {   

      cout << "---------- Category = " << c << ", mass = " << myMass << endl; 
      TString myCut = "EBEB"; 
      if (c==1) myCut = "EBEE";  

      // reading the resolution roodatahist for this category and this mass   
      TString myRDHA = TString(Form("resolRDH_mass%d_cat",theMass)+myCut);
      RooDataHist *resRDH = (RooDataHist*)fileRes->Get(myRDHA);
      resRDH->Print();

      // creating the roohistpdfs
      TString myHistPdfResName = TString::Format("myHistPdfResName%d",theMass)+TString::Format("_%d",c);
      RooHistPdf *myHistPdfRes = new RooHistPdf(myHistPdfResName,myHistPdfResName,*mgg,*resRDH,0) ;
      myHistPdfRes->Print();
      if (c==0) pdfsResCat0.add(*myHistPdfRes);         
      if (c==1) pdfsResCat1.add(*myHistPdfRes);         
      if (c==0) myHistPdfRes->plotOn(frameResCat0,LineColor(kBlue), LineStyle(kSolid));
      if (c==1) myHistPdfRes->plotOn(frameResCat1,LineColor(kBlue), LineStyle(kSolid));
      cout << "roohistpdfs for resolution added" << endl;

      // reading the width roodatahist for this category and this mass    
      TString myRDHB = TString(Form("intWidthRDH_mass%d_cat",theMass)+myCut);
      RooDataHist *inwRDH = (RooDataHist*)fileInw->Get(myRDHB);
      inwRDH->Print();

      // creating the roohistpdfs       
      TString myHistPdfInWName = TString::Format("myHistPdfInWName%d",theMass)+TString::Format("_%d",c);
      RooHistPdf *myHistPdfInW = new RooHistPdf(myHistPdfInWName,myHistPdfInWName,*mgg,*inwRDH,0) ;
      myHistPdfInW->Print();
      if (c==0) pdfsInWCat0.add(*myHistPdfInW);
      if (c==1) pdfsInWCat1.add(*myHistPdfInW);
      if (c==0) myHistPdfInW->plotOn(frameInWCat0,LineColor(kBlue), LineStyle(kSolid));      
      if (c==1) myHistPdfInW->plotOn(frameInWCat1,LineColor(kBlue), LineStyle(kSolid));      
      cout << "roohistpdfs for intrinsic width added" << endl;
      
    } // loop over cat
  }   // loop over masses
  

  // Now morphing for the two categories and the two pdfs
  cout << endl;
  cout << "morphing for the resolution functions" << endl;
  pdfsResCat0.Print();
  pdfsResCat1.Print();
  RooMomentMorph *morphResCat0 = new RooMomentMorph("morphResCat0","morphResCat0",*mu,varlist,pdfsResCat0,paramVec,RooMomentMorph::Linear);
  morphResCat0->Print();
  RooMomentMorph *morphResCat1 = new RooMomentMorph("morphResCat1","morphResCat1",*mu,varlist,pdfsResCat1,paramVec,RooMomentMorph::Linear);
  morphResCat1->Print();
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;
  cout << endl;
  cout << "morphing for the intrinsic width functions" << endl;
  pdfsInWCat0.Print();
  pdfsInWCat1.Print();
  RooMomentMorph *morphInWCat0 = new RooMomentMorph("morphInWCat0","morphInWCat0",*mu,varlist,pdfsInWCat0,paramVec,RooMomentMorph::Linear);
  morphInWCat0->Print();
  RooMomentMorph *morphInWCat1 = new RooMomentMorph("morphInWCat1","morphInWCat1",*mu,varlist,pdfsInWCat1,paramVec,RooMomentMorph::Linear);
  morphInWCat1->Print();
  cout << endl;
  cout << endl;


  // Importing in the workspace
  w->import(*morphResCat0);
  w->import(*morphResCat1);
  w->import(*morphInWCat0);
  w->import(*morphInWCat1);
  w->Print();


  // Evaluating
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;
  cout << endl;
  cout << "now evaluating the resolution morphing" << endl;
  mu->setVal(1500);
  cout << endl;
  morphResCat0->Print();
  cout << endl;
  morphResCat1->Print();

  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;
  cout << endl;
  cout << "now evaluating the intrinsic width morphing" << endl;
  mu->setVal(1500);
  cout << endl;
  morphInWCat0->Print();
  cout << endl;
  morphInWCat1->Print();


  // Making control plots
  TCanvas *c1 = new TCanvas("c1","c1",1);
  morphResCat0->plotOn(frameResCat0, LineColor(kRed), LineStyle(kDashed));
  frameResCat0->Draw();
  c1->SaveAs("testCat0res.pdf");
  c1->SetLogy(1);
  c1->SaveAs("testCat0resLog.pdf");

  TCanvas *c2 = new TCanvas("c2","c2",1);
  morphResCat1->plotOn(frameResCat1, LineColor(kGreen), LineStyle(kDashed));
  frameResCat1->Draw();
  c2->SaveAs("testCat1res.pdf");
  c2->SetLogy(1);
  c2->SaveAs("testCat1resLog.pdf");

  TCanvas *c11 = new TCanvas("c11","c11",1);
  morphInWCat0->plotOn(frameInWCat0, LineColor(kRed), LineStyle(kDashed));
  frameInWCat0->Draw();
  c11->SaveAs("testCat0inW.pdf");
  c11->SetLogy(1);
  c11->SaveAs("testCat0inWLog.pdf");

  TCanvas *c12 = new TCanvas("c12","c12",1);
  morphInWCat1->plotOn(frameInWCat1, LineColor(kGreen), LineStyle(kDashed));
  frameInWCat1->Draw();
  c12->SaveAs("testCat1inW.pdf");
  c12->SetLogy(1);
  c12->SaveAs("testCat1inWLog.pdf");
}
*/

// To run the analysis. Pdfs obtained here
void runfits(string coupling="001") {

  RooWorkspace *w = new RooWorkspace("w");
 
  // range for the variables
  w->factory("mgg[0,12000]");
  w->factory("mggGen[0,12000]");
  w->Print("v");

  // range of masses - chiara: questo va splittato tra quelle che uso x la risoluzione e quelle per l'analisis
  vector<int> masses;
  ///masses.push_back(500);
  ///masses.push_back(750);
  //masses.push_back(1000);
  //masses.push_back(1250);
  masses.push_back(1500);
  //masses.push_back(1750);
  //masses.push_back(2000);
  ///masses.push_back(2250);
  /////masses.push_back(2500);
  ///masses.push_back(2750);
  ///masses.push_back(3000);
  ///masses.push_back(3500);
  ///masses.push_back(4000);
  ///masses.push_back(4500);
  ///masses.push_back(5000);
  /////masses.push_back(5500);
  ///masses.push_back(6000);
  /////  masses.push_back(6500);
  ///masses.push_back(7000);


  // loading data - here the coupling is the requested one
  // make the roodatasets with minimal selection
  cout << endl; 
  cout << "------------------------------------------" << endl; 
  cout << endl; 
  cout << "Now add signal data" << endl;
  for (int ii=0; ii<(int)masses.size(); ii++) {
    int theMass = masses[ii];
    cout << "adding mass " << theMass << endl;
    AddSigData(w, theMass, coupling);   
  }


  // make resolution histograms and roodatahists.
  // Here the coupling is k=0.1 or k=0.01 according to the mass
  // and independent of what you choose when running the macro
  cout << endl; 
  cout << endl; 
  cout << "------------------------------------------" << endl; 
  cout << endl; 
  string fileResol = "ResolutionHistos.root";
  cout << "Now prepare resolution histograms and save in root file " << fileResol << endl;
  for (int ii=0; ii<(int)masses.size(); ii++) {
    int theMass = masses[ii];
    string myResKpl = "01";
    if (theMass>5000) myResKpl = "001";                   // chiara
    cout << "resolution at mass " << theMass << " with coupling " << myResKpl << endl;
    if (ii==0) MakeResolutionHisto(w, fileResol, 1, theMass, myResKpl);    
    else MakeResolutionHisto(w, fileResol, 0, theMass, myResKpl);
  }

  // make intrinsic width histograms and roodatahists.
  // Here the coupling is the requested one
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl; 
  string fileIntrinsic = "IntrinsicWidthHistos.root";
  cout << "Now prepare intrinsic width histograms and save in root file " << fileIntrinsic << endl;
  for (int ii=0; ii<(int)masses.size(); ii++) {
    int theMass = masses[ii];
    cout << "resolution at mass " << theMass << " with coupling " << coupling << endl;
    if (ii==0) MakeIntrinsicWidthHisto(w, fileIntrinsic, 1, theMass, coupling);    
    else MakeIntrinsicWidthHisto(w, fileIntrinsic, 0, theMass, coupling);
  }

  // Now make the convolution 
  cout << endl;    
  cout << endl;    
  cout << "--------------------------------------------------------------------------" << endl;     
  cout << endl;    
  cout << "Now make the convolution of the two histograms" << endl;  
  for (int ii=0; ii<(int)masses.size(); ii++) {  
    int theMass = masses[ii];    
    cout << "convolution for mass " << theMass << " and coupling " << coupling << endl;      
    ConvolutionFromRDH(w, theMass,coupling);        
  }

  /*
  // interpolation
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl;    
  cout << "Now make the interpolation" << endl; 
  Interpolation(w, masses);

  // Now make the convolution
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl; 
  cout << "Now make the convolution of the two interpolated function" << endl;
  ConvolutionFromMorphing(w); //;, Int_t mass, TString coupling) { 
  // ConvolutionWithToysFromMorphing(w);
  */

  return;
}

