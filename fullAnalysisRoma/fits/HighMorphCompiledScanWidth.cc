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
#include "RooCustomizer.h"     
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

// Load the roodatahists and make a convolution 
void ConvolutionFromRDH(RooWorkspace* w, Int_t mass, float coupling) {     

  TString myMass     = TString::Format("%d",mass);
  TString myCoupling = TString::Format("%f",coupling);
  cout << "mass = " << myMass << ", coupling = " << myCoupling << endl;

  // RooRealVars
  RooRealVar* mgg = w->var("mgg");  
  cout << "chiara, mgg " << endl;
  mgg->Print();

  TString deltaMname = TString::Format("deltaM_kpl%f",coupling);
  RooRealVar* deltaM = w->var(deltaMname);    
  deltaM->SetTitle("deltaM");     
  deltaM->SetName("deltaM");
  cout << "chiara, deltaM " << endl;
  deltaM->Print();
  
  TString deltaMgenName = TString::Format("deltaMgen_kpl%f",coupling);
  RooRealVar* deltaMgen = w->var(deltaMgenName);    
  deltaMgen->SetTitle("deltaMgen");  
  deltaMgen->SetName("deltaMgen");   
  cout << "chiara, deltaMgen " << endl;
  deltaMgen->Print();

  TString mHname = TString::Format("mH_mass%d",mass);
  RooRealVar* mH = w->var(mHname);
  mH->setConstant();   
  cout << "chiara: mH" << endl;
  mH->Print();

  TString kHname = TString::Format("kH_kpl%f",coupling);        
  RooRealVar* kH = w->var(kHname);                          
  kH->setConstant();   
  cout << "chiara: mk" << endl;
  kH->Print();

  // Resolution centred in zero
  RooFormulaVar *deltaM_formula = new RooFormulaVar("deltaM_formula","","@0",RooArgList(*w->var("mgg")));

  // Intrinsic width centred in mH or in zero
  RooFormulaVar *deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0-@1",RooArgList(*w->var("mgg"),*mH)); 

  // To move from deltaM to mgg
  RooArgList histObsRes;
  histObsRes.add(*deltaM);

  // To move from deltaMgen to mgg
  RooArgList histObsInw;
  histObsInw.add(*deltaMgen);

  // Files with resolution and intrinsic width RDHs
  TFile *fileRes = new TFile("ResHistosGenOnlyScan.root");
  TFile *fileInw = new TFile("WidthHistosGenOnlyScan.root");

  for (int c=0; c<NCAT; ++c) {  

    cout << "---------- Category = " << c << endl;    
    TString myCut   = "EBEB";       
    if (c==1) myCut = "EBEE";  

    // for resolution
    TString myDeltaM_formulaA = TString(Form("deltaM_formula_cat"+myCut+"_mass%d",mass));
    TString myDeltaM_formula  = TString(Form(myDeltaM_formulaA+"_kpl"+myCoupling));   
    deltaM_formula->SetTitle(myDeltaM_formula);
    deltaM_formula->SetName(myDeltaM_formula);
    RooArgList pdfObsRes;
    pdfObsRes.add(*deltaM_formula);  
    
    // for intrinsic width
    TString myDeltaMgen_formulaA = TString(Form("deltaMgen_formula_cat"+myCut+"_mass%d",mass));
    TString myDeltaMgen_formula  = TString(Form(myDeltaMgen_formulaA+"_kpl"+myCoupling));   
    deltaMgen_formula->SetTitle(myDeltaMgen_formula);
    deltaMgen_formula->SetName(myDeltaMgen_formula);
    RooArgList pdfObsInw;
    pdfObsInw.add(*deltaMgen_formula);

    // reading the roodatahists
    TString myRDHA = TString(Form("resolRDH_mass%d_cat",mass)+myCut);     
    RooDataHist *resRDH = (RooDataHist*)fileRes->Get(myRDHA);    
    resRDH->Print(); 
    //
    TString myRDHBa = TString(Form("widthRDH_mass%d_cat",mass)+myCut);   
    TString myRDHB = TString(Form(myRDHBa))+TString(Form("_kpl%f",coupling));
    cout << "Reading " << myRDHB << endl;
    RooDataHist *inwRDH = (RooDataHist*)fileInw->Get(myRDHB);   
    cout << myRDHB << endl;
    inwRDH->Print();        
    //
    cout << "RooDataHists taken" << endl;       

    // creating the roohistpdfs    
    TString myHistPdfResNameA = TString(Form("myHistPdfRes_cat"+myCut+"_mass%d",mass));
    TString myHistPdfResName  = TString(Form(myHistPdfResNameA+"_kpl"+myCoupling));   
    RooHistPdf *myHistPdfRes = new RooHistPdf("myHistPdfRes","myHistPdfRes",pdfObsRes, histObsRes,*resRDH,0) ;  
    myHistPdfRes->SetTitle(myHistPdfResName);
    myHistPdfRes->SetName(myHistPdfResName);
    myHistPdfRes->Print(); 
    //
    TString myHistPdfInwNameA = TString(Form("myHistPdfInw_cat"+myCut+"_mass%d",mass));
    TString myHistPdfInwName  = TString(Form(myHistPdfInwNameA+"_kpl"+myCoupling));   
    RooHistPdf *myHistPdfInw = new RooHistPdf("myHistPdfInw","myHistPdfInw",pdfObsInw, histObsInw,*inwRDH,0) ;  
    myHistPdfInw->SetTitle(myHistPdfInwName);
    myHistPdfInw->SetName(myHistPdfInwName);
    myHistPdfInw->Print(); 
    //
    cout << "RooHistPdfs done" << endl;      


    // Now make the convolution    
    mgg->setBins(10000, "cache");
    TString myConvNameA = TString(Form("Convolution_cat"+myCut+"_mass%d",mass));
    TString myConvName  = TString(Form(myConvNameA+"_kpl"+myCoupling));   
    RooFFTConvPdf *convol = new RooFFTConvPdf("convol","convol",*mgg,*myHistPdfInw,*myHistPdfRes);          
    if (myCoupling=="0.010000" && mass==750)  {cout << "Eccolo" << endl; convol->setBufferFraction(0.08);}        // chiaraaa
    convol->SetTitle(myConvName);
    convol->SetName(myConvName);
    cout << "Convolution done" << endl;   

    // Both
    convol->Print();     
    cout << "Convolution done and printed. now plot" << endl;   

    // Importing the convolution in the workspace
    w->import(*convol);   

    // moving to the corresponding roohistpdf
    mgg->setBins(10000);
    RooDataHist *convRDH = convol->generateBinned(*mgg,10000,kTRUE);
    TString myConvRdhNameA = TString(Form("ConvolutionRDH_cat"+myCut+"_mass%d",mass));
    TString myConvRdhName  = TString(Form(myConvRdhNameA+"_kpl"+myCoupling));   
    convRDH->SetTitle(myConvRdhName);
    convRDH->SetName(myConvRdhName);
    cout << "done with RooDataHist from conv" << endl;
    convRDH->Print();
    cout << endl;
    
    RooHistPdf *convRhPdf = new RooHistPdf("convRhPdf","convRHhPdf",*mgg,*convRDH,0);
    TString myConvRhPdfNameA = TString(Form("ConvolutionRhPdf_cat"+myCut+"_mass%d",mass));  
    TString myConvRhPdfName  = TString(Form(myConvRhPdfNameA+"_kpl"+myCoupling));   // chiara
    //TString myConvRhPdfName  = TString(Form(myConvRhPdfNameA+"_kpl_smearDown"+myCoupling));   // chiara
    convRhPdf->SetTitle(myConvRhPdfName);   
    convRhPdf->SetName(myConvRhPdfName);    
    cout << "done with RooHistPdf from conv"<< endl;
    convRhPdf->Print();
    w->import(*convRhPdf);
    cout << endl;
    
    delete convol;
    delete convRhPdf;
    delete myHistPdfRes;    
    delete myHistPdfInw;     
  }

  // deleting
  delete fileInw;   
  delete fileRes;      
  delete deltaM_formula;
  delete deltaMgen_formula;

  // Saving the WS
  cout<< endl; 
  TString filename("/tmp/crovelli/myWSwithMorphing.root"); 
  TFile fileWs(filename,"RECREATE");
  fileWs.cd(); 
  w->writeToFile(filename);       
  cout << "Write signal workspace in: " << filename << " file" << endl;  
  cout << endl;  

  // checking the workspaces
  w->Print();
}

// To run the analysis
void runfits(int mass=750) {

  RooWorkspace *w = new RooWorkspace("w");
 
  // range for the variables
  w->factory("mgg[300,1600]");          // chiara: restringo il range, con il nuovo metodo ho tagliato
  w->factory("mggGen[300,1600]");       // chiara: restringo il range, con il nuovo metodo ho tagliato
  
  // range of couplings
  vector<float> couplings;
  //for (int iCoupl=0; iCoupl<30; iCoupl++) {    // chiara
  for (int iCoupl=4; iCoupl<30; iCoupl++) {
    float thisCoupl = 0.01 + iCoupl*0.01;
    couplings.push_back(thisCoupl); 
  }

  // preparing roorealvars
  cout << endl; 
  cout << "------------------------------------------" << endl; 
  cout << endl; 
  cout << "Now add coupling dependent roorealvar - for gen level scan analysis" << endl;
  for (int ii=0; ii<(int)couplings.size(); ii++) {

    float coupling = couplings[ii];
    
    RooRealVar* deltaM = new RooRealVar("deltaM", "deltaM", -200, 200, "GeV");              // chiara
    TString deltaMname = TString::Format("deltaM_kpl%f",coupling);
    deltaM->SetName(deltaMname);
    deltaM->SetTitle(deltaMname);
    w->import(*deltaM);

    RooRealVar* deltaMgen = new RooRealVar("deltaMgen", "deltaMgen", -400, 400, "GeV");     // chiara
    TString deltaMgenName = TString::Format("deltaMgen_kpl%f",coupling);
    deltaMgen->SetName(deltaMgenName);
    deltaMgen->SetTitle(deltaMgenName);
    w->import(*deltaMgen);
    
    RooRealVar* mH = new RooRealVar("mH", "mH", 700, 800, "GeV");   
    TString mHname = TString::Format("mH_mass%d",mass);
    mH->SetName(mHname);
    mH->SetTitle(mHname);
    mH->setVal(mass);
    mH->setConstant();
    if (ii==0) w->import(*mH);
    
    RooRealVar* kH = new RooRealVar("kH", "kH", 0, 0.5, "");   
    TString kHname = TString::Format("kH_kpl%f",coupling);
    kH->SetName(kHname);
    kH->SetTitle(kHname);
    kH->setVal(coupling);
    kH->setConstant();
    w->import(*kH);
  }

  w->Print();

  // Now make the convolution 
  cout << endl;    
  cout << endl;    
  cout << "--------------------------------------------------------------------------" << endl;     
  cout << endl;    
  cout << "Now make the convolution of the two histograms" << endl;  
  for (int ii=0; ii<(int)couplings.size(); ii++) {  
    float theCoupling = couplings[ii];    
    cout << "convolution for mass " << mass << " and coupling " << theCoupling << endl;      
    ConvolutionFromRDH(w, mass,theCoupling);        
  }

  return;
}

