//#include "CMS_lumi.C"
//#include "RooDCBShape.h"

void compareSignalShapes(std::string cat){
  TCanvas* c = new TCanvas("c", "c", 1);
  c->cd();
  //pdf from livia parametric model
  TFile* f_livia = TFile::Open("MassParamSpin0/full_analysis_spring16v1_sync_v4_cert_275125_cic2_default_shapes_spin0_wnuis_lumi_3.99_grav_001.root");
  RooWorkspace* w_livia = (RooWorkspace*)f_livia->Get("wtemplates");  
  RooRealVar* var_livia = (RooRealVar*) w_livia->var("MH");
  w_livia->var("MH")->setVal(750);
  RooDCBShape* shape_livia = (RooDCBShape*) w_livia->pdf(("model_signal_grav_001_"+cat).c_str());  
  shape_livia->Print();
  w_livia->var("MH")->Print();
  RooPlot* plot = (w_livia->var(("mgg"+cat).c_str()))->frame();
  shape_livia->plotOn(plot, RooFit::LineColor(kBlue));

  //pdf from pasquale roohistpdf
  TFile* f_pasquale = TFile::Open("MassFixedSpin0/full_analysis_spring16v1_sync_v4_cert_275125_cic2016_default_shapes_spin0_corrshape_lumi_3.99_grav_001_750.root");
  RooWorkspace* w_pasquale = (RooWorkspace*)f_pasquale->Get("wtemplates");
  RooHistPdf* shape_pasquale = (RooHistPdf*) w_pasquale->pdf(("model_signal_grav_001_750_"+cat+"016").c_str());
  shape_pasquale->plotOn(plot, RooFit::LineColor(kRed));
  plot->GetYaxis()->SetTitle("a.u.");
  plot->GetXaxis()->SetTitle("m_{#gamma#gamma} [GeV]");
  plot->GetXaxis()->SetRangeUser(600,900);
  plot->Draw();

  TLegend* leg = new TLegend(0.55, 0.6,0.9,0.9, "","brNDC");
  leg->SetFillColor(kWhite);
  leg->SetTextFont(42);
  leg->AddEntry(plot->getObject(0), "Parametric Model 80X", "L");
  leg->AddEntry(plot->getObject(1), "RooHistPdf Model 76X", "L");
  leg->Draw("same");
  c->SaveAs(("~/www/Pippone/SignalShapeComparison_"+cat+".png").c_str());
  c->SaveAs(("~/www/Pippone/SignalShapeComparison_"+cat+".pdf").c_str());
  c->SetLogy();
  c->SaveAs(("~/www/Pippone/SignalShapeComparison_"+cat+"_LOG.png").c_str());
  c->SaveAs(("~/www/Pippone/SignalShapeComparison_"+cat+"_LOG.pdf").c_str());
 

}
