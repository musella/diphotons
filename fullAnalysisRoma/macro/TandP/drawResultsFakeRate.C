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
const int nPtBinsEE = 10;
const double ptBinLimitsEE[nPtBinsEE+1]  = {20., 30., 40., 50., 60., 80.,  105.,  140., 180., 250., 500.};
const double ptBinCentersEE[nPtBinsEE]   = {25., 35., 45., 55., 70., 92.5, 122.5, 160., 215., 375.};
const double ptBinHalfWidthEE[nPtBinsEE] = { 5.,  5.,  5.,  5., 10., 12.5, 17.5,   20.,  35., 125.};
const TString etaLimitsStringArrayEE[nEtaBins] = { "1.566 < |#eta| < 2.5" };


// ----------------------------------------
// Data efficiencies and statistical errors
double dataEB[nEtaBins][nPtBinsEB] = {
  { 
    9.64248e-02, 8.46439e-02, 7.86777e-02, 7.97509e-02, 7.74308e-02, 7.62234e-02, 7.29922e-02, 7.68911e-02, 5.79410e-02, 7.59770e-02 , 4.64477e-02
  }
};

double dataEE[nEtaBins][nPtBinsEE] = {
  { 
    1.89705e-01, 1.79365e-01, 1.73145e-01, 1.64579e-01, 1.70300e-01, 1.50223e-01, 1.68517e-01, 1.86414e-01, 1.34606e-01, 1.32030e-01
  }
};

// statistical only errors 
double dataErrStatEB[nEtaBins][nPtBinsEB] = {
  { 
    1.27854e-03, 5.97950e-04, 4.37001e-04, 1.06132e-03, 1.45938e-03, 3.51554e-03, 5.41978e-03, 8.13871e-03, 1.08557e-02, 2.30611e-02, 4.30591e-02
  }
};

double dataErrStatEE[nEtaBins][nPtBinsEE] = {
  { 
    2.21425e-03, 1.06163e-03, 1.10949e-03, 2.86260e-03, 5.04743e-03, 9.95471e-03, 1.44762e-02, 2.40547e-02, 3.36653e-02, 5.05220e-02
  }
};


// ----------------------------------------
// MC efficiencies and errors - C&C
double mcEB[nEtaBins][nPtBinsEB] = {
  { 
    0.0584838, 0.050945, 0.0487157, 0.0501778, 0.0475708, 0.0466449, 0.0533911, 0.0446743, 0.0434292, 0.0645161, 0.0127119
  }
};

double mcEE[nEtaBins][nPtBinsEE] = {
  { 
    0.176548, 0.166628, 0.159936, 0.156196, 0.151863, 0.145293, 0.144914, 0.14578, 0.131849, 0.146226
  }
};

// statistical only errors 
double mcErrEB[nEtaBins][nPtBinsEB] = {
  { 
    0.000430203, 0.000260834, 0.000244765, 0.000470677, 0.000669679, 0.00124076, 0.00219222, 0.00330397, 0.00515281, 0.0110362, 0.00960293
  }
};

double mcErrEE[nEtaBins][nPtBinsEE] = {
  { 
    0.0011496, 0.000779501, 0.000776887, 0.00145779, 0.0021261, 0.00409174, 0.00628131, 0.0107858, 0.0149308, 0.0267305 
  }
};


void drawResultsFakeRate(){

  cout << "only statistical error" << endl;
  double dataErrEB[nEtaBins][nPtBinsEB];
  double dataErrEE[nEtaBins][nPtBinsEE];
  for (int ii=0; ii<nPtBinsEB; ii++ ) {
    dataErrEB[0][ii] = dataErrStatEB[0][ii];
  }
  for (int ii=0; ii<nPtBinsEE; ii++ ) {
    dataErrEE[0][ii] = dataErrStatEE[0][ii];
  }


  cout << "================================" << endl;
  cout << "EB" << endl;
  for (int ii=0; ii<nPtBinsEB; ii++ ) 
    cout << ii << ", nominal = " << dataEB[0][ii] << ", statErr = " << dataErrStatEB[0][ii] << endl;
  cout << "================================" << endl;
  cout << "EE" << endl;
  for (int ii=0; ii<nPtBinsEE; ii++ ) 
    cout << ii << ", nominal = " << dataEE[0][ii] << ", statErr = " << dataErrStatEE[0][ii] << endl;
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

    TString cname = "sfFakeRate_";
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
    grMcEB->SetFillColor(kPink-8);
    ci = TColor::GetColor("#3399ff");
    grMcEB->SetLineColor(kPink+4);
    grMcEB->SetMarkerStyle(22);
    grMcEB->SetMarkerColor(kPink+4);
    grMcEB->SetMarkerSize(1.);

    ci = TColor::GetColor("#99ccff");
    grMcEE->SetFillColor(kPink-8);
    ci = TColor::GetColor("#3399ff");
    grMcEE->SetLineColor(kPink+4);
    grMcEE->SetMarkerStyle(22);
    grMcEE->SetMarkerColor(kPink+4);
    grMcEE->SetMarkerSize(1.);

    ci = TColor::GetColor("#99ccff");
    grSfEB->SetFillColor(kPink-8);
    ci = TColor::GetColor("#3399ff");
    grSfEB->SetLineColor(kPink+4);
    grSfEB->SetMarkerStyle(20);
    grSfEB->SetMarkerColor(kPink+4);
    grSfEB->SetMarkerSize(1.);

    ci = TColor::GetColor("#99ccff");
    grSfEE->SetFillColor(kPink-8);
    ci = TColor::GetColor("#3399ff");
    grSfEE->SetLineColor(kPink+4);
    grSfEE->SetMarkerStyle(20);
    grSfEE->SetMarkerColor(kPink+4);
    grSfEE->SetMarkerSize(1.);

    // Create and configure the dummy histograms on which to draw the graphs
    TH2F *h1 = new TH2F("dummy1","", 100, 0, 500, 100, 0., 0.3);
    h1->GetYaxis()->SetTitle("Fake rate");
    h1->SetStats(0);
    h1->GetXaxis()->SetLabelSize(0);
    h1->GetXaxis()->SetNdivisions(505);
    h1->GetXaxis()->SetDecimals();
    h1->GetYaxis()->SetTitleOffset(0.8);
    h1->GetYaxis()->SetTitleSize(0.05);
    TH2F *h2 = new TH2F("dummy2","", 100, 0, 500, 100, 0.2, 2.2);
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

    TLegend *leg = new TLegend(0.65,0.5,0.9,0.75);
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
    latLumi->Draw("same");
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



