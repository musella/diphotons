{
gSystem->Load("libdiphotonsUtils");
wtemplates->exportToCint("w");
TCanvas *c2=new TCanvas("c2","c2")       ;                                                     
c2.cd();
gStyle->SetOptStat(111111);
w::mgg->setRange(300.,5999);
RooPlot * frame = w::mgg->frame() ;                                                                       
//w::signal_grav_02_1500_EBEE_pdf->plotOn(frame);
w::model_templateNdim2_unroll_pp_EBEE->plotOn(frame);
w::model_signalEBEE->plotOn(frame);
frame->Draw();
TCanvas *c3=new TCanvas("c3","c3")       ;                                                     
c3.cd();
gStyle->SetOptStat(111111);
w::templateNdim2_unroll->setRange(0.,9.);
RooPlot * frame2 = w::templateNdim2_unroll->frame() ;                                                                       
w::signal_grav_02_1500_EBEE_pdf->plotOn(frame2);
w::model_templateNdim2_unroll_pp_EBEE->plotOn(frame2);
w::model_signalEBEE->plotOn(frame2);
frame2->Draw();
}
