#include "TString.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TLatex.h"
#include "CMS_lumi.C"

bool wantSyst = true;

const int nEtaBins = 1;

const TString lumiString = "CMS Preliminary, #sqrt{s}=13 TeV, #intLdt=2.6 fb^{-1}";

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
    8.08134e-01, 8.55612e-01, 8.84874e-01, 8.89166e-01, 8.82828e-01, 8.99915e-01, 8.86947e-01, 8.86066e-01, 9.17447e-01, 9.14671e-01, 8.52987e-01
  }
};

double dataEE[nEtaBins][nPtBinsEE] = {
  { 
    6.76097e-01, 7.34016e-01, 7.76813e-01 , 7.98718e-01, 8.18468e-01, 8.41352e-01, 8.60736e-01, 9.09526e-01, 9.39925e-01
  }
};

// statistical only errors 
double dataErrStatEB[nEtaBins][nPtBinsEB] = {
  { 
    1.96889e-03, 7.12343e-04, 4.43749e-04, 1.22480e-03, 2.28228e-03, 4.65595e-03, 6.46986e-03, 1.24471e-02, 1.59420e-02, 2.75159e-02, 5.24314e-02
  }
};

double dataErrStatEE[nEtaBins][nPtBinsEE] = {
  { 
    2.55719e-03, 1.39765e-03, 1.11814e-03, 2.96063e-03, 5.21855e-03, 9.64626e-03, 1.57999e-02, 2.41369e-02, 2.37021e-02
  }
};


// ----------------------------------------
// alternative fit changing the signal model and keeping nominal background
double dataSystSigEB[nEtaBins][nPtBinsEB] = {
  { 
    8.14595e-01, 8.50123e-01, 8.84583e-01, 8.89930e-01, 8.90693e-01, 9.08531e-01, 8.94165e-01, 8.94204e-01, 9.35519e-01, 9.19174e-01, 8.56413e-01
  }
};

double dataSystSigEE[nEtaBins][nPtBinsEE] = {
  { 
    6.83858e-01, 7.31611e-01, 7.73326e-01, 7.91733e-01, 8.18643e-01, 8.42331e-01, 8.61081e-01, 9.09120e-01, 9.41852e-01
  }
};


// ----------------------------------------
// alternative fit changing the background model and keeping nominal signal
double dataSystBackEB[nEtaBins][nPtBinsEB] = {
  { 
    8.00842e-01, 8.55870e-01, 8.82220e-01, 8.88940e-01, 8.85143e-01, 8.99926e-01, 8.92703e-01, 8.92112e-01, 9.19241e-01, 9.17535e-01, 8.54486e-01 
  }
};

double dataSystBackEE[nEtaBins][nPtBinsEE] = {
  { 
    6.64802e-01, 7.32883e-01, 7.78883e-01, 7.87099e-01, 8.19140e-01, 8.41760e-01, 8.62728e-01, 9.09047e-01, 9.37351e-01 
  }
};
  

// ----------------------------------------
// MC efficiencies and errors - C&C
double mcEB[nEtaBins][nPtBinsEB] = {
  { 
    0.8088, 0.857935, 0.887251, 0.891072, 0.889657, 0.888987, 0.896471, 0.902262, 0.907358, 0.916961, 0.90458
  }
};

double mcEE[nEtaBins][nPtBinsEE] = {
  { 
    0.653038, 0.713392, 0.757357, 0.784137, 0.817278, 0.831368, 0.856555, 0.877245, 0.905188
  }
};

// statistical only errors 
double mcErrEB[nEtaBins][nPtBinsEB] = {
  { 
    0.000672316, 0.000398068, 0.000350577, 0.000656427, 0.000958025, 0.0017922, 0.00290353, 0.00453561, 0.00697337, 0.0125509, 0.0201619
  }
};

double mcErrEE[nEtaBins][nPtBinsEE] = {
  { 
    0.00132486, 0.000909263, 0.000886604, 0.00163089, 0.0022497, 0.0041477, 0.00670113, 0.0109229, 0.0133561
  }
};


void drawResultsMoriond(){

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
    h2->GetXaxis()->SetTitle("p_{T} (GeV)");
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

    TLegend *leg = new TLegend(0.65,0.1,0.9,0.25,"Z #rightarrow ee");
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->SetBorderSize(0);
    leg->AddEntry(grDataEB, "data", "pl");
    leg->AddEntry(grMcEB, "simulation", "pFlE");

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
    CMS_lumi(c1,4,1);
    latEtaEB->Draw("same");
    //latLumi->Draw("same");
    // Draw the scale factors
    pad2->cd();
    h2->Draw();
    grSfEB  ->Draw("2same");
    grSfEB  ->Draw("pEZ,same");
    // Save into a file
    TString fname = cname;
    fname += "_EB.pdf";
    //fname += "_EB.png";
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
    //latLumi->Draw("same");
    CMS_lumi(c1,4,1);
    // Draw the scale factors
    pad2->cd();
    h2->Draw();
    grSfEE  ->Draw("2same");
    grSfEE  ->Draw("pEZ,same");
    // Save into a file
    fname = cname;
    fname += "_EE.pdf";
    //fname += "_EE.png";
    c1->Print(fname);
  }

}



