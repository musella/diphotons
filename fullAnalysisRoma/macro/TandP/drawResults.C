#include "TString.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TLatex.h"

bool wantSyst = true;

const int nEtaBins = 1;

const TString lumiString = "CMS Preliminary, #sqrt{s}=13 TeV, #intLdt=2.4 fb^{-1}";

// EB
const int nPtBinsEB = 11;   
const double ptBinLimitsEB[nPtBinsEB+1]  = {20., 30., 40., 50., 60., 80., 110., 150., 200., 270., 350., 500.};
const double ptBinCentersEB[nPtBinsEB]   = {25., 35., 45., 55., 70., 95., 130., 175., 235., 310., 425.};
const double ptBinHalfWidthEB[nPtBinsEB] = { 5.,  5.,  5.,  5., 10., 15.,  20.,  25.,  35.,  40.,  75.};
const TString etaLimitsStringArrayEB[nEtaBins] = { "0. < |#eta| < 1.4442" };

// EE
const int nPtBinsEE = 9;
const double ptBinLimitsEE[nPtBinsEE+1]  = {20., 30., 40., 50., 60., 80., 110., 150., 200., 500.};
const double ptBinCentersEE[nPtBinsEE]   = {25., 35., 45., 55., 70., 95., 130., 175., 350.};
const double ptBinHalfWidthEE[nPtBinsEE] = { 5.,  5.,  5.,  5., 10., 15.,  20.,  25., 150.};
const TString etaLimitsStringArrayEE[nEtaBins] = { "1.566 < |#eta| < 2.5" };


// ----------------------------------------
// Data efficiencies and statistical errors
double dataEB[nEtaBins][nPtBinsEB] = {
  { 
    8.14784e-01, 8.63670e-01 , 8.92883e-01, 8.93278e-01, 8.88792e-01, 8.99810e-01, 8.93464e-01, 8.93338e-01, 9.22840e-01, 9.24046e-01, 9.26053e-01
  }
};

double dataEE[nEtaBins][nPtBinsEE] = {
  { 
    6.91538e-01, 7.57889e-01, 7.98537e-01, 8.09125e-01, 8.29769e-01, 8.50851e-01, 8.68422e-01, 8.79020e-01, 9.19384e-01
  }
};

// statistical only errors 
double dataErrStatEB[nEtaBins][nPtBinsEB] = {
  { 
    2.03773e-03, 7.25389e-04, 5.28764e-04, 1.28216e-03, 2.42247e-03, 4.93865e-03, 7.87431e-03, 1.13276e-02, 1.59638e-02, 2.77824e-02, 5.31681e-02
  }
};

double dataErrStatEE[nEtaBins][nPtBinsEE] = {
  { 
    2.74125e-03, 1.40759e-03, 1.28101e-03, 3.08376e-03, 5.69046e-03, 1.01103e-02, 1.70185e-02, 2.63264e-02, 2.66311e-02
  }
};


// ----------------------------------------
// alternative fit changing the signal model and keeping nominal background
double dataSystSigEB[nEtaBins][nPtBinsEB] = {
  { 
    8.15170e-01, 8.60076e-01, 8.93184e-01, 8.96896e-01, 8.96205e-01, 9.11385e-01, 8.92919e-01, 9.04949e-01, 9.32198e-01, 9.30125e-01, 8.47152e-01 
  }
};

double dataSystSigEE[nEtaBins][nPtBinsEE] = {
  { 
    7.05637e-01, 7.57202e-01, 7.96266e-01, 8.06905e-01, 8.37237e-01, 8.60022e-01, 8.50684e-01, 8.91316e-01, 9.03238e-01 
  }
};


// ----------------------------------------
// alternative fit changing the background model and keeping nominal signal
double dataSystBackEB[nEtaBins][nPtBinsEB] = {
  { 
    8.23609e-01, 8.64064e-01, 8.90442e-01, 8.90713e-01, 8.94145e-01, 8.96122e-01, 9.05821e-01, 8.92899e-01, 9.22727e-01, 9.24031e-01, 8.68644e-01 
  }
};

double dataSystBackEE[nEtaBins][nPtBinsEE] = {
  { 
    6.91353e-01, 7.66270e-01, 7.98943e-01, 8.05622e-01, 8.21131e-01, 8.26827e-01, 8.97592e-01, 9.24601e-01, 9.17364e-01
  }
};
  

// ----------------------------------------
// MC efficiencies and errors - C&C
double mcEB[nEtaBins][nPtBinsEB] = {
  { 
    0.82562, 0.874696, 0.90277, 0.903433, 0.902497, 0.904019, 0.9104, 0.908949, 0.913793, 0.904762, 0.885932
  }
};

double mcEE[nEtaBins][nPtBinsEE] = {
  { 
    0.695045, 0.761412, 0.797101, 0.819717, 0.832361, 0.851242, 0.860569, 0.867294, 0.958333
  }
};

// statistical only errors 
double mcErrEB[nEtaBins][nPtBinsEB] = {
  { 
    0.00065036, 0.000376655, 0.000327907, 0.000620222, 0.000906508, 0.00168081,  0.00271831, 0.00443906, 0.00682152, 0.012784, 0.0216047
  }
};

double mcErrEE[nEtaBins][nPtBinsEE] = {
  { 
    0.00112741, 0.000851825, 0.000830018, 0.00152089, 0.0021914, 0.0039127, 0.0066184, 0.0115489, 0.0130177
  }
};


void drawResults(){

  // Syst error: take the max difference
  double dataSystErrEB[nEtaBins][nPtBinsEB];
  for (int ii=0; ii<nPtBinsEB; ii++ ) {
    if ( fabs(dataEB[0][ii]-dataSystSigEB[0][ii]) > fabs(dataEB[0][ii]-dataSystBackEB[0][ii]) ) dataSystErrEB[0][ii] = fabs(dataEB[0][ii]-dataSystSigEB[0][ii]);
    else dataSystErrEB[0][ii] = fabs(dataEB[0][ii]-dataSystBackEB[0][ii]);
  }

  double dataSystErrEE[nEtaBins][nPtBinsEE];
  for (int ii=0; ii<nPtBinsEE; ii++ ) {
    if ( fabs(dataEE[0][ii]-dataSystSigEE[0][ii]) > fabs(dataEE[0][ii]-dataSystBackEE[0][ii]) ) dataSystErrEE[0][ii] = fabs(dataEE[0][ii]-dataSystSigEE[0][ii]);
    else dataSystErrEE[0][ii] = fabs(dataEE[0][ii]-dataSystBackEE[0][ii]);
  }

  // Tot error: stat + syst
  double dataErrEB[nEtaBins][nPtBinsEB];
  double dataErrEE[nEtaBins][nPtBinsEE];

  if (wantSyst) {
    cout << "systematics added" << endl;
    for (int ii=0; ii<nPtBinsEB; ii++ ) {
      dataErrEB[0][ii] = sqrt( dataSystErrEB[0][ii]*dataSystErrEB[0][ii] + dataErrStatEB[0][ii]*dataErrStatEB[0][ii] );
    }
    for (int ii=0; ii<nPtBinsEE; ii++ ) {
      dataErrEE[0][ii] = sqrt( dataSystErrEE[0][ii]*dataSystErrEE[0][ii] + dataErrStatEE[0][ii]*dataErrStatEE[0][ii] );
    }
  } else {
    cout << "only statistical error" << endl;
    for (int ii=0; ii<nPtBinsEB; ii++ ) {
      dataErrEB[0][ii] = dataErrStatEB[0][ii];
    }
    for (int ii=0; ii<nPtBinsEE; ii++ ) {
      dataErrEE[0][ii] = dataErrStatEE[0][ii];
    }
  }

  cout << "================================" << endl;
  cout << "EB" << endl;
  for (int ii=0; ii<nPtBinsEB; ii++ ) 
    cout << ii << ", nominal = " << dataEB[0][ii] << ", forSigSyst = " << dataSystSigEB[0][ii] << ", forBkgSyst = " << dataSystBackEB[0][ii] 
	 << ", statErr = " << dataErrStatEB[0][ii] << ", systErr = " <<dataSystErrEB[0][ii] << endl; 
  cout << "================================" << endl;
  cout << "EE" << endl;
  for (int ii=0; ii<nPtBinsEE; ii++ ) 
    cout << ii << ", nominal = " << dataEE[0][ii] << ", forSigSyst = " << dataSystSigEE[0][ii] << ", forBkgSyst = " << dataSystBackEE[0][ii] 
	 << ", statErr = " << dataErrStatEE[0][ii] << ", systErr = " <<dataSystErrEE[0][ii] << endl; 
  cout << "================================" << endl;
  

  cout << endl;
  cout << endl;
  cout << "================================" << endl;  
  cout << "scale factors: EB" << endl;
  // Scale factors and errors
  double sfEB[nEtaBins][nPtBinsEB];
  double sfErrTotEB[nEtaBins][nPtBinsEB];
  for (int iEta=0; iEta<nEtaBins; iEta++){ 
    for (int iPt=0; iPt<nPtBinsEB; iPt++){ 
      sfEB[iEta][iPt] = dataEB[iEta][iPt]/mcEB[iEta][iPt];
      float sigmaDoDEB   = dataErrEB[iEta][iPt]/dataEB[iEta][iPt];
      float sigmaMCoMCEB = mcErrEB[iEta][iPt]/mcEB[iEta][iPt];
      sfErrTotEB[iEta][iPt] = sfEB[iEta][iPt]*sqrt( (sigmaDoDEB*sigmaDoDEB) + (sigmaMCoMCEB*sigmaMCoMCEB) );
      cout << sfEB[iEta][iPt] << " +/- " << sfErrTotEB[iEta][iPt] << endl;
    }
  }

  cout << endl;
  cout << "================================" << endl;  
  cout << "scale factors: EE" << endl;
  double sfEE[nEtaBins][nPtBinsEE];
  double sfErrTotEE[nEtaBins][nPtBinsEE];
  for (int iEta=0; iEta<nEtaBins; iEta++){ 
    for (int iPt=0; iPt<nPtBinsEE; iPt++){ 
      sfEE[iEta][iPt] = dataEE[iEta][iPt]/mcEE[iEta][iPt];
      float sigmaDoDEE   = dataErrEE[iEta][iPt]/dataEE[iEta][iPt];
      float sigmaMCoMCEE = mcErrEE[iEta][iPt]/mcEE[iEta][iPt];
      sfErrTotEE[iEta][iPt] = sfEE[iEta][iPt]*sqrt( (sigmaDoDEE*sigmaDoDEE) + (sigmaMCoMCEE*sigmaMCoMCEE) );
      cout << sfEE[iEta][iPt] << " +/- " << sfErrTotEE[iEta][iPt] << endl;
    }
  }


  // Draw all canvases
  for(int ieta = 0; ieta<nEtaBins; ieta++){

    TString cname = "sfEff_";
    TCanvas *c1 = new TCanvas(cname, cname, 10,10,700,700);
    c1->SetFillColor(kWhite);
    c1->Draw();
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

    // Create and fill arrays for graphs for this eta bin
    double *dataSliceEB    = new double[nPtBinsEB];
    double *dataSliceErrEB = new double[nPtBinsEB];
    double *mcSliceEB      = new double[nPtBinsEB];
    double *mcSliceErrEB   = new double[nPtBinsEB];
    double *sfSliceEB      = new double[nPtBinsEB];
    double *sfSliceErrEB   = new double[nPtBinsEB];
    for(int ipt = 0; ipt<nPtBinsEB; ipt++){
      dataSliceEB   [ipt] = dataEB     [ieta][ipt];
      dataSliceErrEB[ipt] = dataErrEB  [ieta][ipt];
      mcSliceEB     [ipt] = mcEB       [ieta][ipt];
      mcSliceErrEB  [ipt] = mcErrEB    [ieta][ipt];
      sfSliceEB     [ipt] = sfEB       [ieta][ipt];
      sfSliceErrEB  [ipt] = sfErrTotEB [ieta][ipt];
    }

    double *dataSliceEE    = new double[nPtBinsEE];
    double *dataSliceErrEE = new double[nPtBinsEE];
    double *mcSliceEE      = new double[nPtBinsEE];
    double *mcSliceErrEE   = new double[nPtBinsEE];
    double *sfSliceEE      = new double[nPtBinsEE];
    double *sfSliceErrEE   = new double[nPtBinsEE];
    for(int ipt = 0; ipt<nPtBinsEE; ipt++){
      dataSliceEE   [ipt] = dataEE     [ieta][ipt];
      dataSliceErrEE[ipt] = dataErrEE  [ieta][ipt];
      mcSliceEE     [ipt] = mcEE       [ieta][ipt];
      mcSliceErrEE  [ipt] = mcErrEE    [ieta][ipt];
      sfSliceEE     [ipt] = sfEE       [ieta][ipt];
      sfSliceErrEE  [ipt] = sfErrTotEE [ieta][ipt];
    }

    // Create and configure the graphs   
    TGraphErrors *grDataEB = new TGraphErrors(nPtBinsEB, ptBinCentersEB, dataSliceEB, ptBinHalfWidthEB, dataSliceErrEB);
    TGraphErrors *grMcEB   = new TGraphErrors(nPtBinsEB, ptBinCentersEB, mcSliceEB, ptBinHalfWidthEB, mcSliceErrEB);
    TGraphErrors *grSfEB   = new TGraphErrors(nPtBinsEB, ptBinCentersEB, sfSliceEB, ptBinHalfWidthEB, sfSliceErrEB);

    TGraphErrors *grDataEE = new TGraphErrors(nPtBinsEE, ptBinCentersEE, dataSliceEE, ptBinHalfWidthEE, dataSliceErrEE);
    TGraphErrors *grMcEE   = new TGraphErrors(nPtBinsEE, ptBinCentersEE, mcSliceEE, ptBinHalfWidthEE, mcSliceErrEE);
    TGraphErrors *grSfEE   = new TGraphErrors(nPtBinsEE, ptBinCentersEE, sfSliceEE, ptBinHalfWidthEE, sfSliceErrEE);
    
    grDataEB->SetLineColor(kBlack);
    grDataEB->SetMarkerColor(kBlack);
    grDataEB->SetMarkerStyle(20);
    grDataEB->SetMarkerSize(1.);
    grDataEE->SetLineColor(kBlack);
    grDataEE->SetMarkerColor(kBlack);
    grDataEE->SetMarkerStyle(20);
    grDataEE->SetMarkerSize(1.);

    int ci = TColor::GetColor("#99ccff");
    grMcEB->SetFillColor(kGreen-8);
    ci = TColor::GetColor("#3399ff");
    grMcEB->SetLineColor(kGreen+4);
    grMcEB->SetMarkerStyle(22);
    grMcEB->SetMarkerColor(kGreen+4);
    grMcEB->SetMarkerSize(1.);

    ci = TColor::GetColor("#99ccff");
    grMcEE->SetFillColor(kGreen-8);
    ci = TColor::GetColor("#3399ff");
    grMcEE->SetLineColor(kGreen+4);
    grMcEE->SetMarkerStyle(22);
    grMcEE->SetMarkerColor(kGreen+4);
    grMcEE->SetMarkerSize(1.);

    ci = TColor::GetColor("#99ccff");
    grSfEB->SetFillColor(kGreen-8);
    ci = TColor::GetColor("#3399ff");
    grSfEB->SetLineColor(kGreen+4);
    grSfEB->SetMarkerStyle(20);
    grSfEB->SetMarkerColor(kGreen+4);
    grSfEB->SetMarkerSize(1.);

    ci = TColor::GetColor("#99ccff");
    grSfEE->SetFillColor(kGreen-8);
    ci = TColor::GetColor("#3399ff");
    grSfEE->SetLineColor(kGreen+4);
    grSfEE->SetMarkerStyle(20);
    grSfEE->SetMarkerColor(kGreen+4);
    grSfEE->SetMarkerSize(1.);

    // Create and configure the dummy histograms on which to draw the graphs
    TH2F *h1 = new TH2F("dummy1","", 100, 0, 500, 100, 0.6, 1.1);
    h1->GetYaxis()->SetTitle("Efficiency");
    h1->SetStats(0);
    h1->GetXaxis()->SetLabelSize(0);
    h1->GetXaxis()->SetNdivisions(505);
    h1->GetXaxis()->SetDecimals();
    h1->GetYaxis()->SetTitleOffset(0.8);
    h1->GetYaxis()->SetTitleSize(0.05);
    TH2F *h2 = new TH2F("dummy2","", 100, 0, 500, 100, 0.8, 1.2);
    h2->GetXaxis()->SetTitle("p_{T} [GeV]");
    h2->GetYaxis()->SetTitle("Scale Factor");
    h2->GetXaxis()->SetTitleOffset(1.0);
    h2->GetXaxis()->SetTitleSize(0.1);
    h2->GetYaxis()->SetTitleOffset(0.4);
    h2->GetYaxis()->SetTitleSize(0.1);
    h2->GetXaxis()->SetLabelSize(0.08);
    h2->GetYaxis()->SetLabelSize(0.08);
    h2->GetYaxis()->SetNdivisions(505);
    h2->GetYaxis()->SetDecimals();
    h2->SetStats(0);

    TLegend *leg = new TLegend(0.65,0.1,0.9,0.25);
    leg->SetFillColor(kWhite);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->AddEntry(grDataEB, "Data", "pl");
    leg->AddEntry(grMcEB, "Simulation DY", "pFlE");

    TLatex *latLumi = new TLatex(0, 1.15, lumiString);

    TLatex *latEtaEB = new TLatex(60.0, 0.5, etaLimitsStringArrayEB[ieta]);
    TLatex *latEtaEE = new TLatex(60.0, 0.5, etaLimitsStringArrayEE[ieta]);


    // --------------------------------------
    // EB
    // Draw the efficiencies
    pad1->cd();
    h1->Draw();
    grMcEB  ->Draw("2same");
    grMcEB  ->Draw("pZ,same");
    grDataEB->Draw("PEZ,same");
    leg->Draw("same");
    latEtaEB->Draw("same");
    latLumi->Draw("same");
    // Draw the scale factors
    pad2->cd();
    h2->Draw();
    grSfEB  ->Draw("2same");
    grSfEB  ->Draw("pEZ,same");
    // Save into a file
    TString fname = cname;
    fname += "_EB.pdf";
    c1->Print(fname);

    // --------------------------------------
    // EE
    // Draw the efficiencies
    pad1->cd();
    h1->Draw();
    grMcEE  ->Draw("2same");
    grMcEE  ->Draw("pZ,same");
    grDataEE->Draw("PEZ,same");
    leg->Draw("same");
    latEtaEE->Draw("same");
    latLumi->Draw("same");
    // Draw the scale factors
    pad2->cd();
    h2->Draw();
    grSfEE  ->Draw("2same");
    grSfEE  ->Draw("pEZ,same");
    // Save into a file
    fname = cname;
    fname += "_EE.pdf";
    c1->Print(fname);
  }

}



