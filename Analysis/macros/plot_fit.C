{
	using namespace RooFit;

	gSystem->Load("libHiggsAnalysisCombinedLimit");
	gSystem->Load("libdiphotonsUtils");
	
	RooAbsData * fakes = toys->Get("toy_asimov");
	// fakes = fakes->reduce("templateNdim2_unroll<4");	
	_file1->cd();
	RooAbsData * data = toys->Get("toy_asimov");
	// data = data->reduce("templateNdim2_unroll<4");	
	_file0->cd();
	
	w->loadSnapshot("MultiDimFit");
	w->exportToCint("ws");
	RooCategory & cat = ws::CMS_channel;

	RooSimultaneousOpt & sim = ws::model_s;
	
        TList *datasets = data->split(cat, true);
        TIter next(datasets);

        TList *fdatasets = fakes->split(cat, true);
	TIter fnext(fdatasets);
	
        for (RooAbsData *ds = (RooAbsData *) next(); ds != 0; ds = (RooAbsData *) next()) {
            RooAbsPdf *pdfi  = sim->getPdf(ds->GetName());
	    
	    RooAbsData *fds = (RooAbsData *) fnext();
		    
	    RooPlot * framei = ws::mgg.frame(Title(ds->GetName()),Bins(134));
	    ws::templateNdim2_unroll.setRange("sig_region",0,4.);
	    ws::mgg.setRange("sig_region",ws::mgg.getMin(),ws::mgg.getMax());
	    /// RooPlot * framei = ws::templateNdim2_unroll.frame(Title(ds->GetName()));
	    /// if( TString(ds->GetName()).Contains("control") ) {
	    /// 	    continue;
	    /// }
	    
	    if( ! TString(ds->GetName()).Contains("control") ) {
		    fds->plotOn(framei,MarkerStyle(kOpenCircle));//,DataError(RooAbsData::Poisson),LineColor(kRed-1),MarkerColor(kRed-1));
	    }
	    ds->plotOn(framei,DataError(RooAbsData::Poisson));
	    pdfi->plotOn(framei,LineColor(kBlue));//,ProjectionRange("sig_region"));
	    pdfi->plotOn(framei,LineColor(kRed),Components("*pf*"));//,ProjectionRange("sig_region"));
	    pdfi->plotOn(framei,LineColor(kOrange),Components("*ff*"));//,ProjectionRange("sig_region"));
	    
	    TCanvas * canvi = new TCanvas(ds->GetName(),ds->GetName());
	    
	    canvi->SetLogy();
	    canvi->SetLogx();
	    
	    framei->GetYaxis()->SetRangeUser(1e-5,700);
	    
	    framei->Draw();
	    
	    canvi->SaveAs(Form("%s.png", canvi->GetName()));
	    /// canvi->SaveAs(Form("template_%s.png", canvi->GetName()));
	}
}

	    
