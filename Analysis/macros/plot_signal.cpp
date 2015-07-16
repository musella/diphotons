{
TString name="grav_001_5000_EBEB";
gSystem->Load("libdiphotonsUtils");
gSystem->Load("libHiggsAnalysisCombinedLimit");
wtemplates->exportToCint("w");
TCanvas *c1=new TCanvas("c1","c1");
RooPlot *comp = w::mgg.frame(3000.,6000.);
w::model_signal_mgg_grav_001_5000_EBEB->plotOn(comp);
//w::model_signal_grav_001_5000_EBEE->plotOn(comp);
w::signal_grav_001_5000_EBEB->plotOn(comp);
comp->Draw();
cout << "signalpdf " <<  w::model_signal_mgg_grav_001_5000_EBEB->createIntegral(RooArgSet("mgg"),"templateBinningEBEB").getVal() << " combined " <<
w::model_signal_grav_001_5000_EBEB->createIntegral(RooArgSet("mgg","templateNdim2_unroll"),"templateBinningEBEB").getVal() << endl;
//w::model_signal_grav_001_5000_EBEE->createIntegral(mgg).getVal();   
}
