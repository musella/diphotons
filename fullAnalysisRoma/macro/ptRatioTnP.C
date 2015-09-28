void ptRatioTnP()
{
    TCanvas* cnv[3][2];

    gStyle->SetLegendBorderSize(0);
    gStyle->SetLegendFont(42);
    gStyle->SetOptStat("");
    gStyle->SetTitleSize(0.7, "X");
    gStyle->SetTitleOffset(0.7, "X");

    map<string, pair<string, int> > samples;
    map<string, TH1F*> histos[3];
    THStack* bkgStack[3];
    bkgStack[0] = new THStack("bkg_ptr", "p_T Ratio: 1 fb^{-1};p_T-ratio");
    bkgStack[1] = new THStack("bkg_m", "Invariant Mass: 1 fb^{-1};M_{e#gamma}");
    bkgStack[2] = new THStack("bkg_Xpt", "Tag and Probe pair boost: 1 fb^{-1};p_{T}");
    TH1F* voids[3];
    voids[0] = new TH1F("void_ptr", "p_T Ratio: 1 fb^{-1};p_T-ratio", 300, 0, 10);
    voids[1] = new TH1F("void_m", "Invariant Mass: 1 fb^{-1};M_{e#gamma}", 300, 0, 2000);
    voids[2] = new TH1F("void_Xpt", "Tag and Probe pair boost: 1 fb^{-1};p_{T}", 300, 0, 200);

    TLegend lg(0.6,0.5,0.95,0.8);
    lg.SetFillStyle(0);
    lg.SetTextSize(0.045);
    
    samples["TTjets_ALL"] = make_pair("tt+jets", kGreen+1);
    samples["ZZ4l_ALL"] = make_pair("ZZ", kCyan+1);
    samples["WZjets_ALL"] = make_pair("WZ", kBlue);
    samples["WW2L2nu_ALL"] = make_pair("WW", kBlue);
    samples["WJetsToLNu_HT-600To800_ALL"] = make_pair("W+jets", kRed+1);
    samples["WJetsToLNu_HT-800To1200_ALL"] = make_pair("W+jets", kRed+1);
    samples["WJetsToLNu_HT-1200To2500_ALL"] = make_pair("W+jets", kRed+1);
    samples["WJetsToLNu_HT-2500ToInf_ALL"] = make_pair("W+jets", kRed+1);
    samples["DYLL_all"] = make_pair("DY", kAzure-7);
    samples["singleEle_ALL"] = make_pair("Data", kBlack);

    TH1F* h_sumW;
    float lumiForW=1000;

    float          pu_weight=0;
    float          perEveW=0;
    float          totXsec=0;
    vector<float>* e_pt_ = new vector<float>;
    vector<float>* e_eta_ = new vector<float>;
    vector<float>* e_phi_ = new vector<float>;
    vector<float>* g_pt_ = new vector<float>;
    vector<float>* g_eta_ = new vector<float>;
    vector<float>* g_phi_ = new vector<float>;
    vector<bool>* e_match_ = new vector<bool>;
    vector<int>* g_presel_ = new vector<int>;
    vector<int>* e_idx_ = new vector<int>;
    vector<int>* g_idx_ = new vector<int>;
    vector<float>* invMass_ = new vector<float>;
    vector<float>* ptRatio_ = new vector<float>;

    for(auto& sample : samples)
    {
        cout << "/afs/cern.ch/work/c/crovelli/public/spring15_v3v8/"+sample.first+".root" << endl;
        TFile* file = TFile::Open(("/afs/cern.ch/work/c/crovelli/public/spring15_v3v8/"+sample.first+".root").c_str(), "READ");
        TTree* tree = (TTree*)file->Get("tnpAna/TaPtree");
        h_sumW = (TH1F*)file->Get("tnpAna/h_sumW");
        if(!file || !tree || !h_sumW)
            continue;

        if(!histos[0][sample.second.first])
        {
            histos[0][sample.second.first] = new TH1F((sample.second.first+"_ptr").c_str(), "", 300, 0, 10);
            histos[1][sample.second.first] = new TH1F((sample.second.first+"_m").c_str(), "", 300, 0, 2000);
            histos[2][sample.second.first] = new TH1F((sample.second.first+"_Xpt").c_str(), "", 300, 0, 200);
        }

        double sampleSumWeight = (double)h_sumW->Integral();       
        tree->SetBranchAddress("pu_weight", &pu_weight);
        tree->SetBranchAddress("perEveW", &perEveW);
        tree->SetBranchAddress("totXsec", &totXsec);
        tree->SetBranchAddress("electron_pt", &e_pt_);
        tree->SetBranchAddress("electron_eta", &e_eta_);
        tree->SetBranchAddress("electron_phi", &e_phi_);
        tree->SetBranchAddress("gamma_pt", &g_pt_);
        tree->SetBranchAddress("gamma_eta", &g_eta_);
        tree->SetBranchAddress("gamma_phi", &g_phi_);
        tree->SetBranchAddress("electron_matchHLT", &e_match_);
        tree->SetBranchAddress("gamma_presel", &g_presel_);
        tree->SetBranchAddress("eleIndex", &e_idx_);
        tree->SetBranchAddress("gammaIndex", &g_idx_);
        tree->SetBranchAddress("invMass", &invMass_);
        tree->SetBranchAddress("ptRatio", &ptRatio_);

        for(unsigned int iEntry=0; iEntry<tree->GetEntriesFast(); ++iEntry)
        {
            tree->GetEntry(iEntry);
            float ptr_tmp=-1;
            for(unsigned int iPair=0; iPair<invMass_->size(); ++iPair)
            {
                TLorentzVector ele, gam;
                ele.SetPtEtaPhiM(e_pt_->at(e_idx_->at(iPair)), e_eta_->at(e_idx_->at(iPair)),
                                 e_phi_->at(e_idx_->at(iPair)), 0);
                gam.SetPtEtaPhiM(g_pt_->at(g_idx_->at(iPair)), g_eta_->at(g_idx_->at(iPair)),
                                 g_phi_->at(g_idx_->at(iPair)), 0);
                float X_pt = (ele+gam).Pt();
                float weight = perEveW * lumiForW * totXsec / sampleSumWeight;
                // if(invMass_->at(iPair) > 150)
                //     histos[2][sample.second.first]->Fill(X_pt, weight);
                if(invMass_->at(iPair) > 150 && X_pt < 30 && 
                   g_presel_->at(g_idx_->at(iPair))==1 &&
                   e_match_->at(e_idx_->at(iPair)))
                {                    
                    ptr_tmp = ptRatio_->at(iPair);
                    if(sample.second.first == "Data")
                        weight = 1;
                    histos[0][sample.second.first]->Fill(ptr_tmp, weight);
                    histos[1][sample.second.first]->Fill(invMass_->at(iPair), weight);
                    histos[2][sample.second.first]->Fill(gam.Pt(), weight);
                }
            }
        }

        for(int i=0; i<3; ++i)
        {
            //histos[i][sample.second.first]->SetFillStyle(0);
            histos[i][sample.second.first]->SetLineColor(sample.second.second);
            histos[i][sample.second.first]->SetFillColor(sample.second.second);
            if(sample.second.first != "Data")
                bkgStack[i]->Add(histos[i][sample.second.first]);
        }
        cout << sample.second.first << " :"
             << histos[0][sample.second.first]->Integral(1, 30) << " / "
             << histos[0][sample.second.first]->Integral(31, 200) << endl;
    }

    lg.AddEntry(histos[0]["tt+jets"], "tt+jets", "f");
    lg.AddEntry(histos[0]["W+jets"], "W+jets", "f");
    lg.AddEntry(histos[0]["WW"], "WW", "f");
    lg.AddEntry(histos[0]["WZ"], "WZ", "f");
    lg.AddEntry(histos[0]["ZZ"], "ZZ", "f");
    lg.AddEntry(histos[0]["DY"], "DY", "f");
    
    for(int i=0; i<3; ++i)
    {
        cnv[i][0] = new TCanvas();
        cnv[i][1] = new TCanvas();
        cnv[i][0]->cd();
        bkgStack[i]->Draw("hist");
        lg.DrawClone("same");
        cnv[i][0]->Modified();
        //histos[i]["Data"]->Draw("Esame");
        cnv[i][1]->cd();
        bkgStack[i]->Draw("nostack");
        lg.DrawClone("same");
        cnv[i][1]->Modified();
    }
}
