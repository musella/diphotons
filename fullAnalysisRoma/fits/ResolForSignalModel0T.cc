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

// Preparing the resolution histograms
void MakeResolutionHisto(TString filename, bool newFile, int mass, TString coupling) {

  TString myMass = TString::Format("%d",mass);
  Int_t ncat = NCAT;
  
  // the roorealvar 
  RooRealVar* deltaM = new RooRealVar("deltaM", "", -1000, 200, "GeV");   

  // Output file 
  TFile *theResoFile;
  if (newFile) theResoFile = new TFile(filename,"RECREATE");
  else theResoFile = new TFile(filename,"UPDATE");
  
  // Input file and tree
  TString inDir = "../macro/allFilesWithResolAtZ_rereco76x_2classes_0T_v6/";   // chiara
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");

  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("(mgg>=0 && mgg<=12000 && mggGen>=0 && mggGen<=12000");     

  // Loop over categories
  for (int c=0; c<ncat; ++c) {

    // Histos
    TH1D *resolH = new TH1D("resolH","resolH",600,-1000.,200.);    // chiara
    resolH->Sumw2();
  
    // Projecting the tree into the histo
    if (ncat==2) {
      if (c==0) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==0)*puweight"));
      if (c==1) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==1)*puweight"));
    } else if (ncat==3) {
      if (c==0) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==0)*puweight"));
      if (c==1) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==1)*puweight"));
      if (c==2) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==2)*puweight"));
    } else if (ncat==4) {
      if (c==0) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==0)*puweight"));
      if (c==1) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==1)*puweight"));
      if (c==2) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==2)*puweight"));
      if (c==3) sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format("&& eventClass==3)*puweight"));
    }

    // Now make the roodatahist
    RooDataHist resolRDH("resolRDH","resolRDH",*deltaM,Import(*resolH));        
    cout << "Resolution datahist summary for mass " << mass << endl;  
    resolRDH.Print();
    cout << endl;  

    // Saving in the root file
    theResoFile->cd();
    TString myCut;
    if (ncat==2) {
      if (c==0)      myCut = "EBEB";  
      else if (c==1) myCut = "EBEE";
    } else if (ncat==3) {
      if (c==0)      myCut = "EBHighR9";  
      else if (c==1) myCut = "EBLowR9";
      else if (c==2) myCut = "EBEE";  
    } else if (ncat==4) {
      if (c==0)      myCut = "EBHighR9";  
      else if (c==1) myCut = "EBLowR9";
      else if (c==2) myCut = "EEHighR9";  
      else if (c==3) myCut = "EELowR9";
    }
    TString nameRDH = TString::Format("resolRDH_mass%d",mass)+TString::Format("_cat")+myCut;
    resolRDH.Write(nameRDH);

    // Plot to check
    TCanvas *c1 = new TCanvas("c1","c1",1);
    RooPlot* myPlot = deltaM->frame(Range(-1000,200),Bins(600));    
    resolRDH.plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));    
    myPlot->Draw();
    TString canvasName = TString(Form("CheckResol_Cat"+myCut+"_mass"+myMass+".png"));
    c1->SaveAs(canvasName);       
    c1->SetLogy();
    canvasName = TString(Form("CheckResol_Cat"+myCut+"_mass"+myMass+"_LOG.png"));
    c1->SaveAs(canvasName);
    RooPlot* myPlotZoom = deltaM->frame(Range(-100,100),Bins(100));   
    resolRDH.plotOn(myPlotZoom, LineColor(kBlue), LineStyle(kDashed));    
    myPlotZoom->Draw();
    canvasName = TString(Form("CheckResolZoom_Cat"+myCut+"_mass"+myMass+".png"));
    c1->SaveAs(canvasName);       

    delete c1;
    delete resolH;
  }

  // Closing the output file
  theResoFile->Close();
}

//-------------------------------------------------------

// Interpolation of resolution functions 
void ResolInterpolation(RooWorkspace* w, vector<int> masses) {

  // Variable for resolution
  RooRealVar* deltaM = new RooRealVar("deltaM", "", -1000, 200, "GeV");   
  RooArgList varlist;
  varlist.add(*deltaM);

  // Morphing variable
  RooRealVar* muRes = new RooRealVar("muRes", "", 0, 12000, "GeV");

  // PDFs
  RooArgList pdfsCat0, pdfsCat1;
  RooArgList pdfsCat2, pdfsCat3;

  // Reference points
  int numMass = (int)masses.size();
  TVectorD paramVec(numMass); 

  // To plot
  RooPlot *frameCat0 = deltaM->frame();
  RooPlot *frameCat1 = deltaM->frame();
  RooPlot *frameCat2 = deltaM->frame();
  RooPlot *frameCat3 = deltaM->frame();

  // Files with the roodatahists 
  TFile *fileRes = new TFile("ResolutionHistos.root");

  // Loop over masses
  for (int ii=0; ii<(int)masses.size(); ii++) { 
    int theMass = masses[ii];
    TString myMass = TString::Format("%d",theMass);

    // Reference points
    paramVec[ii] = (double)theMass; 

    for (int c=0; c<NCAT; ++c) {   

      cout << "---------- Category = " << c << ", mass = " << myMass << endl; 
      TString myCut = "EBEB"; 
      if (c==1) myCut = "EBEE";  

      if (NCAT==3) {
	if (c==0) myCut = "EBHighR9";  
	if (c==1) myCut = "EBLowR9";  
	if (c==2) myCut = "EBEE";
      }

      if (NCAT==4) {
	if (c==0) myCut = "EBHighR9";  
	if (c==1) myCut = "EBLowR9";  
	if (c==2) myCut = "EEHighR9";  
	if (c==3) myCut = "EELowR9";  
      }

      // reading the roodatahist from the file
      TString myRDH = TString(Form("resolRDH_mass%d_cat",theMass)+myCut);
      RooDataHist *resRDH = (RooDataHist*)fileRes->Get(myRDH);
      resRDH->Print();
      cout << "Just got the resolution RooDataHist " << myRDH << endl;
      cout << endl;

      // moving to the corresponding roohistpdf
      TString myRHpdfName = TString(Form("resolRHPDF_mass%d_cat",theMass)+myCut);
      RooHistPdf *myHistPdfRes = new RooHistPdf("myHistPdfRes","myHistPdfRes",*deltaM,*resRDH,0) ;  
      myHistPdfRes->SetTitle(myRHpdfName);
      myHistPdfRes->SetName(myRHpdfName);
      myHistPdfRes->Print();
      cout << "Just got the resolution RooHistPdf " << myRHpdfName << endl;
      cout << endl;

      // adding to the list of pdfs
      if (c==0) pdfsCat0.add(*myHistPdfRes);        
      if (c==1) pdfsCat1.add(*myHistPdfRes); 
      if (NCAT==3) {
	if (c==2) pdfsCat2.add(*myHistPdfRes);
      }
      if (NCAT==4) {
	if (c==2) pdfsCat2.add(*myHistPdfRes);
	if (c==3) pdfsCat3.add(*myHistPdfRes);
      }
      cout << "RooHistPdfs added to the pdf list" << endl;
      cout << endl;

      // plot to check
      if (c==0) myHistPdfRes->plotOn(frameCat0,LineColor(kBlue), LineStyle(kSolid));
      if (c==1) myHistPdfRes->plotOn(frameCat1,LineColor(kBlue), LineStyle(kSolid));
      if (NCAT==3) {
	if (c==2) myHistPdfRes->plotOn(frameCat2,LineColor(kBlue), LineStyle(kSolid));
      }
      if (NCAT==4) {
	if (c==2) myHistPdfRes->plotOn(frameCat2,LineColor(kBlue), LineStyle(kSolid));
	if (c==3) myHistPdfRes->plotOn(frameCat3,LineColor(kBlue), LineStyle(kSolid));
      }

    } // loop over cat
  }   // loop over masses
  
  
  // Now morphing for the two categories
  cout << endl;
  cout << "morphing for the resolution functions" << endl;
  pdfsCat0.Print();
  pdfsCat1.Print();
  if (NCAT==3) {
    pdfsCat2.Print();
  }
  if (NCAT==4) {
    pdfsCat2.Print();
    pdfsCat3.Print();
  }
  RooMomentMorph *morphResCat0 = new RooMomentMorph("morphResCat0","morphResCat0",*muRes,varlist,pdfsCat0,paramVec,RooMomentMorph::Linear);
  morphResCat0->Print();
  RooMomentMorph *morphResCat1 = new RooMomentMorph("morphResCat1","morphResCat1",*muRes,varlist,pdfsCat1,paramVec,RooMomentMorph::Linear);
  morphResCat1->Print();

  RooMomentMorph *morphResCat2;
  if (NCAT==3) {
    morphResCat2 = new RooMomentMorph("morphResCat2","morphResCat2",*muRes,varlist,pdfsCat2,paramVec,RooMomentMorph::Linear);
    morphResCat2->Print();
  }

  RooMomentMorph *morphResCat3;
  if (NCAT==4) {
    morphResCat2 = new RooMomentMorph("morphResCat2","morphResCat2",*muRes,varlist,pdfsCat2,paramVec,RooMomentMorph::Linear);
    morphResCat2->Print();
    morphResCat3 = new RooMomentMorph("morphResCat3","morphResCat3",*muRes,varlist,pdfsCat3,paramVec,RooMomentMorph::Linear);
    morphResCat3->Print();
  }
  cout << endl;
  cout << endl;

  // Close the input file
  fileRes->Close();


  // Now we evaluate with a fine mass scan samples and save the corresponding roodatahists
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;
  cout << endl;
  cout << "now evaluating the morphing every 50GeV, from 500GeV to 5000GeV: doing mass " << endl;


  // This is to have 2GeV steps between 500 and 1000 GeV
  /*
  for (int iGenMass=0; iGenMass<250; iGenMass++) {    
    int thisMass = 500 + iGenMass*2;
    cout << "Fine (2GeV) scan: " << thisMass << endl;    

    muRes->setVal(thisMass);
    for (int c=0; c<NCAT; ++c) {   
      TString myCut = "EBEB"; 
      if (c==1) myCut = "EBEE";  
      if (NCAT==3) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EBEE";
      }
      if (NCAT==4) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EEHighR9";
	if (c==3) myCut = "EELowR9";
      }
      RooDataHist *fittResolRDH; 
      deltaM->setBins(600);       // chiara
      if(c==0) fittResolRDH = morphResCat0->generateBinned(*deltaM,10000,kTRUE);
      if(c==1) fittResolRDH = morphResCat1->generateBinned(*deltaM,10000,kTRUE);
      if (NCAT==3) {
	if(c==2) fittResolRDH = morphResCat2->generateBinned(*deltaM,10000,kTRUE);
      }
      if (NCAT==4) {
	if(c==2) fittResolRDH = morphResCat2->generateBinned(*deltaM,10000,kTRUE);
	if(c==3) fittResolRDH = morphResCat3->generateBinned(*deltaM,10000,kTRUE);
      }
      fittResolRDH->Print();
      TString myFitRDH = TString(Form("resolRDH_mass%d_cat",thisMass)+myCut);
      fittResolRDH->SetTitle(myFitRDH);
      fittResolRDH->SetName(myFitRDH);
      w->import(*fittResolRDH);
    }   
  }
  */

  // Then 4GeV steps between 1000 and 1600 GeV
  for (int iGenMass=0; iGenMass<150; iGenMass++) {    
    int thisMass = 1000 + iGenMass*4.;
    cout << "Medium (4GeV) scan: " << thisMass << endl;
    muRes->setVal(thisMass);
    for (int c=0; c<NCAT; ++c) {   
      TString myCut = "EBEB"; 
      if (c==1) myCut = "EBEE";
      if (NCAT==3) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EBEE";
      }      
      if (NCAT==4) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EEHighR9";
	if (c==3) myCut = "EELowR9";
      }      
      RooDataHist *fittResolRDH; 
      deltaM->setBins(600);       // chiara
      if(c==0) fittResolRDH = morphResCat0->generateBinned(*deltaM,10000,kTRUE);
      if(c==1) fittResolRDH = morphResCat1->generateBinned(*deltaM,10000,kTRUE);
      if (NCAT==3) {
	if(c==2) fittResolRDH = morphResCat2->generateBinned(*deltaM,10000,kTRUE);
      }
      if (NCAT==4) {
	if(c==2) fittResolRDH = morphResCat2->generateBinned(*deltaM,10000,kTRUE);
	if(c==3) fittResolRDH = morphResCat3->generateBinned(*deltaM,10000,kTRUE);
      }
      fittResolRDH->Print();
      TString myFitRDH = TString(Form("resolRDH_mass%d_cat",thisMass)+myCut);
      fittResolRDH->SetTitle(myFitRDH);
      fittResolRDH->SetName(myFitRDH);
      w->import(*fittResolRDH);
    }   
  }

  // Then 100GeV steps between 1600 and 5000 GeV
  for (int iGenMass=0; iGenMass<34; iGenMass++) {    
    int thisMass = 1600 + iGenMass*100.;
    cout << "Coarse (100GeV) scan: " << thisMass << endl;
    muRes->setVal(thisMass);
    for (int c=0; c<NCAT; ++c) {   
      TString myCut = "EBEB"; 
      if (c==1) myCut = "EBEE";  
      if (NCAT==3) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EBEE";
      }      
      if (NCAT==4) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EEHighR9";
	if (c==3) myCut = "EELowR9";
      }      
      RooDataHist *fittResolRDH; 
      deltaM->setBins(600);       // chiara
      if(c==0) fittResolRDH = morphResCat0->generateBinned(*deltaM,10000,kTRUE);
      if(c==1) fittResolRDH = morphResCat1->generateBinned(*deltaM,10000,kTRUE);
      if (NCAT==3) {
	if(c==2) fittResolRDH = morphResCat2->generateBinned(*deltaM,10000,kTRUE);
      }
      if (NCAT==4) {
	if(c==2) fittResolRDH = morphResCat2->generateBinned(*deltaM,10000,kTRUE);
	if(c==3) fittResolRDH = morphResCat3->generateBinned(*deltaM,10000,kTRUE);
      }
      fittResolRDH->Print();
      TString myFitRDH = TString(Form("resolRDH_mass%d_cat",thisMass)+myCut);
      fittResolRDH->SetTitle(myFitRDH);
      fittResolRDH->SetName(myFitRDH);
      w->import(*fittResolRDH);
    }   
  }

  // Finally saving in a second rootfile
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;
  cout << endl;
  cout << "Now salving the histos in a root file" << endl;
  TFile fileFittoRes("ResHistosGenOnlyScan.root","RECREATE");
  fileFittoRes.cd();

  /*
  for (int iGenMass=0; iGenMass<250; iGenMass++) {    
    int thisMass = 500 + iGenMass*2;
    for (int c=0; c<NCAT; ++c) {
      TString myCut = "EBEB";
      if (c==1) myCut = "EBEE";
      if (NCAT==3) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EBEE";
      }      
      if (NCAT==4) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EEHighR9";
	if (c==3) myCut = "EELowR9";
      }      
      TString myFitRDH = TString(Form("resolRDH_mass%d_cat",thisMass)+myCut);
      RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);
      RDH->Write();
    }
  }
  */
  
  for (int iGenMass=0; iGenMass<150; iGenMass++) {    
    int thisMass = 1000 + iGenMass*4.;
    for (int c=0; c<NCAT; ++c) {
      TString myCut = "EBEB";
      if (c==1) myCut = "EBEE";
      if (NCAT==4) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EEHighR9";
	if (c==3) myCut = "EELowR9";
      }      
      if (NCAT==3) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EBEE";
      }      
      TString myFitRDH = TString(Form("resolRDH_mass%d_cat",thisMass)+myCut);
      RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);
      RDH->Write();
    }
  }
  for (int iGenMass=0; iGenMass<34; iGenMass++) {    
    int thisMass = 1600 + iGenMass*100.;
    for (int c=0; c<NCAT; ++c) {
      TString myCut = "EBEB";
      if (c==1) myCut = "EBEE";
      if (NCAT==4) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EEHighR9";
	if (c==3) myCut = "EELowR9";
      }      
      if (NCAT==3) {
	if (c==0) myCut = "EBHighR9";
	if (c==1) myCut = "EBLowR9";
	if (c==2) myCut = "EBEE";
      }      
      TString myFitRDH = TString(Form("resolRDH_mass%d_cat",thisMass)+myCut);
      RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);
      RDH->Write();
    }
  }

  fileFittoRes.Close();
}

// To run the analysis
void runfits() {

  cout << endl;
  cout << endl;
  cout << "================ this is 0T ===============" << endl;
  cout << endl;
  cout << endl;

  RooWorkspace *w = new RooWorkspace("w");
 
  // range of masses - to be used to compute the resolution
  vector<int> masses;
  masses.push_back(500);
  masses.push_back(750);
  masses.push_back(1000);
  masses.push_back(1250);
  masses.push_back(1500);
  masses.push_back(1750);
  masses.push_back(2000);
  masses.push_back(2250);
  masses.push_back(2500);
  masses.push_back(2750);
  masses.push_back(3000);
  //masses.push_back(3500);
  masses.push_back(4000);
  masses.push_back(4500);
  masses.push_back(5000);

  // make resolution histograms and roodatahists using k=0.1 or k=0.01 according to the mass
  cout << endl; 
  cout << endl; 
  cout << "------------------------------------------" << endl; 
  cout << endl; 
  string fileResol = "ResolutionHistos.root";
  cout << "Now prepare resolution histograms and save in root file " << fileResol << endl;
  for (int ii=0; ii<(int)masses.size(); ii++) {
    int theMass = masses[ii];
    string myResKpl = "01";
    if (theMass==2000 || theMass==4000 || theMass==500 || theMass==750) myResKpl = "001";  

    cout << "resolution at mass " << theMass << " with coupling " << myResKpl << endl;
    if (ii==0) MakeResolutionHisto(fileResol, 1, theMass, myResKpl);    
    else MakeResolutionHisto(fileResol, 0, theMass, myResKpl);
  }

  // make the interpolation of the resolutions and save in another file the roodatahists
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl;    
  cout << "Now make the interpolation" << endl; 
  ResolInterpolation(w, masses);

  return;
}

