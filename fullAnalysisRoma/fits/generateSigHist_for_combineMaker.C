#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooAbsReal.h"
#include "TGraph.h"
#include "TF1.h"
#include "RooWorkspace.h"
#include "RooFormulaVar.h"
#include "TFile.h"
#include "RooFFTConvPdf.h"
#include "RooAddPdf.h"
#include "TSystem.h"
#include "RooNumConvPdf.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooDataSet.h"
#include "TString.h"
#include "TObjString.h"

using namespace RooFit;

void generateSigHist_for_combineMaker(Float_t lum =10, Int_t cc1 = 0, Int_t cc2 = 9){

	const Int_t NCAT(4); // 4 categories in the source file
	TString myCut[NCAT] = {"EBHighR9","EBLowR9","EEHighR9","EELowR9"};
	//Float_t lum =10;
	TString lum_string = TString::Format("%g", lum);
	TFile *file=TFile::Open("workspaces/HighMassGG_m1500_01_mgg_lum_"+lum_string+".root");
	RooWorkspace *w_all = (RooWorkspace*)file->Get("w_all");

	RooRealVar *mgg = (RooRealVar*)w_all->var("mgg");
	RooRealVar *MH = (RooRealVar*)w_all->var("MH");
	RooRealVar *kpl = (RooRealVar*)w_all->var("kpl");

	RooNumConvPdf* numConv[NCAT];
	RooFormulaVar* norm[NCAT];

	// necessary to set a window as it is not saved when RooNumConvPdf is saved
	RooRealVar* W = new RooRealVar("W","W",500);
	W->setConstant();
	RooRealVar* C = new RooRealVar("C","C",0);
	C->setConstant();

	cout << "READ PDFs FROM WORKSPACE : SIGNAL FROM 4 CATEGORIES" << endl;
	cout << "READ ROOFORMULAs FROM WORKSPACE : NORMALIZATION OF 4 CATEGORIES" << endl;
	cout << "SET WINDOW FOR THE 4 CONVOLUTIONS (PDFs)" << endl;
	cout << "..." << endl;
	for(Int_t c = 0; c<NCAT; c++){
		
		numConv[c] = (RooNumConvPdf*)w_all->pdf("mggSig_cat"+myCut[c]);
		norm[c] = (RooFormulaVar*)w_all->function("mggSig_cat"+myCut[c]+"_norm");		
		numConv[c]->setConvolutionWindow(*C,*W);
	}
	cout << endl;
	cout << "ADD PDFs TO FORM THE TWO CATEGORIES EBEB AND EBEE" << endl;
	RooFormulaVar *mggSig_catEBEB_norm = new RooFormulaVar("mggSig_catEBEB_norm","mggSig_catEBEB_norm","@0+@1",RooArgList(*norm[0],*norm[1]));
	RooFormulaVar *mggSig_catEBEE_norm = new RooFormulaVar("mggSig_catEBEE_norm","mggSig_catEBEE_norm","@0+@1",RooArgList(*norm[2],*norm[3]));

	RooFormulaVar *frac1EBEB = new RooFormulaVar("frac1EBEB","frac1EBEB","@0/@1",RooArgList(*norm[0],*mggSig_catEBEB_norm));
	RooFormulaVar *frac2EBEB = new RooFormulaVar("frac2EBEB","frac2EBEB","@0/@1",RooArgList(*norm[1],*mggSig_catEBEB_norm));
	RooFormulaVar *frac1EBEE = new RooFormulaVar("frac1EBEE","frac1EBE","@0/@1",RooArgList(*norm[2],*mggSig_catEBEE_norm));
	RooFormulaVar *frac2EBEE = new RooFormulaVar("frac2EBEE","frac2EBEE","@0/@1",RooArgList(*norm[3],*mggSig_catEBEE_norm));
	
	RooAddPdf* mggSig_catEBEB = new RooAddPdf("mggSig_catEBEB","mggSig_catEBEB",RooArgList(*numConv[0],*numConv[1]),RooArgList(*frac1EBEB,*frac2EBEB));
	RooAddPdf* mggSig_catEBEE = new RooAddPdf("mggSig_catEBEE","mggSig_catEBEE",RooArgList(*numConv[2],*numConv[3]),RooArgList(*frac1EBEE,*frac2EBEE));

	cout << endl << endl ;
	cout << "READ CFG FROM BACKGROUND_FINAL TO BE USED IN COMBINE_MAKER.PY" << endl;
	TFile *fcfg = TFile::Open("../../Analysis/macros/workspace_cic2_dijet_lum_10/full_analysis_anv1_v18_bkg_ws_300.root");
	TObjString *cfg = (TObjString*)fcfg->Get("cfg");

   	const Int_t nCouplings(9);
	Double_t couplings[nCouplings] = {0.01, 0.025, 0.05, 0.075, 0.1, 0.125, 0.15, 0.175, 0.2};

	cout << endl << endl ;
	cout << "SCAN OVER MH AND KAPPA VALUES TO GET DIFFERENT PDFs FOR EACH SET OF PARAMETERS..." << endl;

   	for(Int_t icoupling = cc1; icoupling < cc2; icoupling++){

		//CREATE FOLDER FOR THE COUPLING
		TString coupling_string = TString::Format("%g",couplings[icoupling]);
		coupling_string.ReplaceAll(".","");
		TString inDir("../../Analysis/macros/workspace_cic2_signalDataHist_lum_"+lum_string+"/");
		gSystem->mkdir(inDir);

		for(Int_t mass = 1000; mass < 5100 ; mass+=100){
			
			TString mass_string = TString::Format("%d",mass);
			TString signame("grav_"+coupling_string+"_"+mass_string);
			cout << endl;
			cout << "********************************************" << endl;
			cout << "SET MH and KAPPA values" << endl << "kappa: " << couplings[icoupling] << endl << "mass: " << mass << endl;
			cout << "********************************************" << endl;
			MH->setVal(mass);
			kpl->setVal(couplings[icoupling]);
				
			cout << "CREATE ROODATAHIST WITH NORM..." << endl;
			RooDataHist* dhEBEB = mggSig_catEBEB->generateBinned(RooArgSet(*mgg),mggSig_catEBEB_norm->getVal(), RooFit::ExpectedData());
			RooDataHist* dhEBEE = mggSig_catEBEE->generateBinned(RooArgSet(*mgg),mggSig_catEBEE_norm->getVal(), RooFit::ExpectedData());

			dhEBEB->SetName("signal_"+signame+"_EBEB");
			dhEBEB->SetTitle("signal_"+signame+"_EBEB");
			dhEBEE->SetName("signal_"+signame+"_EBEE");
			dhEBEE->SetTitle("signal_"+signame+"_EBEE");

			//CREATE ROOHISTPDF
			//RooHistPdf *fhEBEB = new RooHistPdf("mggHistPdf_catEBEB","mggHistPdf_catEBEB",RooArgSet(*mgg),*dhEBEB);
			//RooHistPdf *fhEBEE = new RooHistPdf("mggHistPdf_catEBEE","mggHistPdf_catEBEE",RooArgSet(*mgg),*dhEBEE);

			//SAVE ALL IN A WORKSPACE WITH FILE NAME corresponding to MASS AND COUPLING VALUE 
			RooWorkspace *wtemplates = new RooWorkspace("wtemplates","wtemplates");

			//wtemplates->import(*fhEBEB);
			//wtemplates->import(*fhEBEE);
			//wtemplates->import(*mggSig_catEBEB_norm,RecycleConflictNodes());
			//wtemplates->import(*mggSig_catEBEE_norm,RecycleConflictNodes());
			wtemplates->import(*dhEBEB);
			wtemplates->import(*dhEBEE);
			//wtemplates->import(*numConv[0],RecycleConflictNodes());

			cout << "******************************************" << endl;
			cout << "WORKSPACE CONTENT FOR COUPLING " << couplings[icoupling] << " AND MASS " << mass << endl;
			wtemplates->Print("V");
			
			//WRITE IN FILE
			TString filename = inDir+"300_"+signame+".root";
			TFile *fileOutput = TFile::Open(filename,"recreate");
			cfg->Write("cfg");
			wtemplates->Write("wtemplates");
			cout << "workspace imported to " << filename << endl;
			
		}
	}
}
