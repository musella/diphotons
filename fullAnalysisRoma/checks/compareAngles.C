void compareAngles() {

  TFile fileData("outfile_data.root");
  TFile fileSignal("outfile_signal.root");
  
  TH1F *Hin_cosThetaStarLead_sig    = (TH1F*)fileSignal.Get("Hin_cosThetaStarLead");
  TH1F *Hin_cosThetaStarSublead_sig = (TH1F*)fileSignal.Get("Hin_cosThetaStarSublead");
  TH1F *Hin_deltaEta_sig            = (TH1F*)fileSignal.Get("Hin_deltaEta");
  TH1F *Hin_deltaPhi_sig            = (TH1F*)fileSignal.Get("Hin_deltaPhi");
  
  TH1F *Hin_cosThetaStarLead_data    = (TH1F*)fileData.Get("Hin_cosThetaStarLead");
  TH1F *Hin_cosThetaStarSublead_data = (TH1F*)fileData.Get("Hin_cosThetaStarSublead");
  TH1F *Hin_deltaEta_data            = (TH1F*)fileData.Get("Hin_deltaEta");
  TH1F *Hin_deltaPhi_data            = (TH1F*)fileData.Get("Hin_deltaPhi");

  TH1F *Hout_cosThetaStarLead_data    = (TH1F*)fileData.Get("Hout_cosThetaStarLead");
  TH1F *Hout_cosThetaStarSublead_data = (TH1F*)fileData.Get("Hout_cosThetaStarSublead");
  TH1F *Hout_deltaEta_data            = (TH1F*)fileData.Get("Hout_deltaEta");
  TH1F *Hout_deltaPhi_data            = (TH1F*)fileData.Get("Hout_deltaPhi");

  Hin_cosThetaStarLead_sig->Rebin(4);
  Hin_cosThetaStarSublead_sig->Rebin(4);
  Hin_deltaEta_sig->Rebin(4);

  Hin_cosThetaStarLead_data->Rebin(4);
  Hin_cosThetaStarSublead_data->Rebin(4);
  Hin_deltaEta_data->Rebin(4);

  Hout_cosThetaStarLead_data->Rebin(4);
  Hout_cosThetaStarSublead_data->Rebin(4);
  Hout_deltaEta_data->Rebin(4);

  Hin_cosThetaStarLead_sig->SetLineColor(4);
  Hin_cosThetaStarLead_data->SetLineColor(1);
  Hout_cosThetaStarLead_data->SetLineColor(2);
  Hin_cosThetaStarLead_sig->SetLineWidth(2);
  Hin_cosThetaStarLead_data->SetLineWidth(2);
  Hout_cosThetaStarLead_data->SetLineWidth(2);

  Hin_cosThetaStarSublead_sig->SetLineColor(4);
  Hin_cosThetaStarSublead_data->SetLineColor(1);
  Hout_cosThetaStarSublead_data->SetLineColor(2);
  Hin_cosThetaStarSublead_sig->SetLineWidth(2);
  Hin_cosThetaStarSublead_data->SetLineWidth(2);
  Hout_cosThetaStarSublead_data->SetLineWidth(2);

  Hin_deltaEta_sig->SetLineColor(4);
  Hin_deltaEta_data->SetLineColor(1);
  Hout_deltaEta_data->SetLineColor(2);
  Hin_deltaEta_sig->SetLineWidth(2);
  Hin_deltaEta_data->SetLineWidth(2);
  Hout_deltaEta_data->SetLineWidth(2);

  Hin_deltaPhi_sig->SetLineColor(4);
  Hin_deltaPhi_data->SetLineColor(1);
  Hout_deltaPhi_data->SetLineColor(2);
  Hin_deltaPhi_sig->SetLineWidth(2);
  Hin_deltaPhi_data->SetLineWidth(2);
  Hout_deltaPhi_data->SetLineWidth(2);


  gStyle->SetOptStat(0);

  TCanvas cLead("cLead","cosTheta*, leading gamma",1);
  cLead.Divide(2,1);
  cLead.cd(1);
  Hin_cosThetaStarLead_sig->SetTitle("");
  Hout_cosThetaStarLead_data->SetTitle("");
  Hin_cosThetaStarLead_sig->GetXaxis()->SetTitle("cos #theta* (gamma1)");
  Hout_cosThetaStarLead_data->GetXaxis()->SetTitle("cos #theta* (gamma1)");
  Hin_cosThetaStarLead_sig->DrawNormalized("hist");
  Hout_cosThetaStarLead_data->DrawNormalized("histsame");
  cLead.cd(2);
  Hin_cosThetaStarLead_data->SetTitle("");
  Hin_cosThetaStarLead_data->GetXaxis()->SetTitle("cos #theta* (gamma1)");
  Hin_cosThetaStarLead_data->DrawNormalized("pE");
  cLead.SaveAs("cosThetaStarLeading.png");

  TCanvas cSublead("cSublead","cosTheta*, subleading gamma",1);
  cSublead.Divide(2,1);
  cSublead.cd(1);
  Hin_cosThetaStarSublead_sig->SetTitle("");
  Hout_cosThetaStarSublead_data->SetTitle("");
  Hin_cosThetaStarSublead_sig->GetXaxis()->SetTitle("cos #theta* (gamma2)");
  Hout_cosThetaStarSublead_data->GetXaxis()->SetTitle("cos #theta* (gamma2)");
  Hin_cosThetaStarSublead_sig->DrawNormalized("hist");
  Hout_cosThetaStarSublead_data->DrawNormalized("histsame");
  cSublead.cd(2);
  Hin_cosThetaStarSublead_data->SetTitle("");
  Hin_cosThetaStarSublead_data->GetXaxis()->SetTitle("cos #theta* (gamma1)");
  Hin_cosThetaStarSublead_data->DrawNormalized("pE");
  cSublead.SaveAs("cosThetaStarSubleading.png");

  TCanvas cDeta("cDeta","|#Delta #eta|",1);
  cDeta.Divide(2,1);
  cDeta.cd(1);
  Hin_deltaEta_sig->SetTitle("");
  Hout_deltaEta_data->SetTitle("");
  Hin_deltaEta_sig->GetXaxis()->SetTitle("#Delta #eta");
  Hout_deltaEta_data->GetXaxis()->SetTitle("#Delta #eta");
  Hout_deltaEta_data->DrawNormalized("hist");
  Hin_deltaEta_sig->DrawNormalized("histsame");
  cDeta.cd(2);
  Hin_deltaEta_data->SetTitle("");
  Hin_deltaEta_data->GetXaxis()->SetTitle("#Delta #eta");
  Hin_deltaEta_data->DrawNormalized("pE");
  cDeta.SaveAs("deltaEta.png");

  TCanvas cDphi("cDphi","cos(#Delta #phi)",1);
  cDphi.Divide(2,1);
  cDphi.cd(1);
  cDphi.SetLogy(1);
  Hin_deltaPhi_sig->SetTitle("");
  Hout_deltaPhi_data->SetTitle("");
  Hin_deltaPhi_sig->GetXaxis()->SetTitle("cos(#Delta #phi)");
  Hout_deltaPhi_data->GetXaxis()->SetTitle("cos(#Delta #phi)");
  Hin_deltaPhi_sig->DrawNormalized("hist");
  Hout_deltaPhi_data->DrawNormalized("histsame");
  cDphi.cd(2);
  cDphi.SetLogy(1);
  Hin_deltaPhi_data->SetTitle("");
  Hin_deltaPhi_data->GetXaxis()->SetTitle("cos(#Delta #phi)");
  Hin_deltaPhi_data->DrawNormalized("pE");
  cDphi.SaveAs("cosDeltaPhi.png");
  cDphi.SaveAs("cosDeltaPhi.root");

}
