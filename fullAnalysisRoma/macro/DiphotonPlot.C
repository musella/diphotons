#include<vector>

#if !defined (__CINT__) || defined (__MAKECINT__)
#include "THStack.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TLatex.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TFrame.h"
#include <iostream>
#endif

enum samp { iRS, iQCD, iGJets, iGG, nSamples };

float xPos[nSamples+1] = {0.70,0.70,0.70,0.70};
float yOff[nSamples+1] = {0,1,2,3};

const Float_t _tsize   = 0.03;
const Float_t _xoffset = 0.20;
const Float_t _yoffset = 0.05;


//------------------------------------------------------------------------------
// GetMaximumIncludingErrors
//------------------------------------------------------------------------------
Float_t GetMaximumIncludingErrors(TH1F* h)
{
  Float_t maxWithErrors = 0;

  for (Int_t i=1; i<=h->GetNbinsX(); i++) {
    
    Float_t binHeight = h->GetBinContent(i) + h->GetBinError(i);
    
    if (binHeight > maxWithErrors) maxWithErrors = binHeight;
  }
  
  return maxWithErrors;
}


//------------------------------------------------------------------------------
// AxisFonts
//------------------------------------------------------------------------------
void AxisFonts(TAxis*  axis,
	       TString coordinate,
	       TString title)
{
  axis->SetLabelFont  (   42);
  axis->SetLabelOffset(0.015);
  axis->SetLabelSize  (0.050);
  axis->SetNdivisions (  505);
  axis->SetTitleFont  (   42);
  axis->SetTitleOffset(  1.5);
  axis->SetTitleSize  (0.050);
  
  if (coordinate == "y") axis->SetTitleOffset(1.6);
  
  axis->SetTitle(title);
}


//------------------------------------------------------------------------------
// THStackAxisFonts
//------------------------------------------------------------------------------
void THStackAxisFonts(THStack* h,
		      TString  coordinate,
		      TString  title)
{
  TAxis* axis = NULL;
  
  if (coordinate.Contains("x")) axis = h->GetHistogram()->GetXaxis();
  if (coordinate.Contains("y")) axis = h->GetHistogram()->GetYaxis();
  
  AxisFonts(axis, coordinate, title);
}

//------------------------------------------------------------------------------
// DrawLegend
//------------------------------------------------------------------------------
void DrawLegend(Float_t x1,
		Float_t y1,
		TH1F*   hist,
		TString label,
		TString option)
{
  TLegend* legend = new TLegend(x1,
				y1,
				x1 + _xoffset,
				y1 + _yoffset);
  
  legend->SetBorderSize(     0);
  legend->SetFillColor (     0);
  legend->SetTextAlign (    12);
  legend->SetTextFont  (    42);
  legend->SetTextSize  (_tsize);
  
  legend->AddEntry(hist, label.Data(), option.Data());
  
  legend->Draw();
}


class DiphotonPlot {
  
public: 
  DiphotonPlot() { _hist.resize(nSamples,0); _data = 0; _breakdown = false; _mass = 0; _nostack = false; }
  void setMCHist   (const samp &s, TH1F * h)  { _hist[s]       = h;  } 
  void setDataHist (TH1F * h)                 { _data          = h;  } 
  void setRSHist   (TH1F * h)                 { setMCHist(iRS   ,h); } 
  void setGGHist   (TH1F * h)                 { setMCHist(iGG   ,h); } 
  void setGJetsHist(TH1F * h)                 { setMCHist(iGJets,h); } 
  void setQCDHist  (TH1F * h)                 { setMCHist(iQCD,h);   } 
  
  void setNoStack() { _nostack = true; }

  TH1F* getDataHist() { return _data; }

  void setMass(const int &m) {_mass=m;}

  void DrawAndRebinTo(const int &rebinTo) {

    if(rebinTo == 0) return Draw();
    int rebin = 0, nbins = 0;
    for (int i=0; i<nSamples; i++) {

      // in case the user doesn't set it
      if( !_hist[i] ) continue;

      nbins = _hist[i]->GetNbinsX();
    }
    if (nbins == 0) return Draw();
    
    rebin = nbins / rebinTo;
    while(nbins % rebin != 0) rebin--;
    return Draw(rebin);
    
  }

  void Draw(const int &rebin=1) {
    
    Color_t _sampleColor[nSamples];
    _sampleColor[iRS  ]   = kBlue+1;
    _sampleColor[iGJets ] = kViolet-9;
    _sampleColor[iGG    ] = kGreen-9;
    _sampleColor[iQCD   ] = kAzure-9;

    Color_t _lineColor[nSamples];
    _lineColor[iRS  ]  = kBlue+1;
    _lineColor[iGJets] = kViolet;   
    _lineColor[iGG   ] = kGreen;
    _lineColor[iQCD  ] = kAzure;
    
    if(!gPad) new TCanvas();
    
    THStack* hstack = new THStack();
    for (int i=0; i<nSamples; i++) {

      // in case the user doesn't set it
      if( !_hist[i] ) continue;
      
      _hist[i]->Rebin(rebin);
      _hist[i]->SetLineColor(_lineColor[i]);
      
      // signal gets overlaid
      if (i == iRS) continue;
      _hist[i]->SetFillColor(_sampleColor[i]);
      _hist[i]->SetFillStyle(1001);
      
      hstack->Add(_hist[i]);
    }
    
    if(_hist[iRS]) _hist[iRS]->SetLineWidth(3);

    if(_data) _data->Rebin(rebin);
    if(_data) _data->SetLineColor  (kBlack);
    if(_data) _data->SetMarkerStyle(kFullCircle);

    if(_nostack) {
      for(int ihist=0;ihist<(int)_hist.size();ihist++) {
	_hist[ihist]->SetFillStyle(0);
	_hist[ihist]->SetLineWidth(2);
      }
      hstack->Draw("hist,nostack");
      if(_hist[iRS]) _hist[iRS]->Draw("hist,same");
      if(_data) _data->Draw("ep,same");
    } else {
      hstack->Draw("hist");
      if(_hist[iRS]) _hist[iRS]->Draw("hist,same");
      if(_data) _data->Draw("ep,same");
    }
    hstack->SetTitle("CMS preliminary");  
    
    Float_t theMax = hstack->GetMaximum();
    Float_t theMin = hstack->GetMinimum();
    
    if (_hist[iRS]) {
      if (_hist[iRS]->GetMaximum() > theMax) theMax = _hist[iRS]->GetMaximum();
      if (_hist[iRS]->GetMinimum() < theMin) theMin = _hist[iRS]->GetMinimum();
    }
    
    if (_data) {
            
      Float_t dataMax = GetMaximumIncludingErrors(_data);
      
      if (dataMax > theMax) theMax = dataMax;
    }

    if (gPad->GetLogy()) {
      // hstack->SetMaximum(500 * theMax);
      // if (hstack->GetMinimum()==0) hstack->SetMinimum(0.05);   
      hstack->SetMaximum(1.2 * theMax);
      if (hstack->GetMinimum()==0) hstack->SetMinimum(0.00005);   // chiara
    } else {
      hstack->SetMaximum(1.55 * theMax);
    }
    
    if(_breakdown) {
      THStackAxisFonts(hstack, "y", "entries");
      hstack->GetHistogram()->LabelsOption("v");
    } else {
      THStackAxisFonts(hstack, "x", TString::Format("%s [%s]",_xLabel.Data(),_units.Data()));
      if(_units.Sizeof() == 1) {
	THStackAxisFonts(hstack, "x", _xLabel.Data());
	THStackAxisFonts(hstack, "y", "entries");
      } else {
	THStackAxisFonts(hstack, "x", TString::Format("%s [%s]",_xLabel.Data(),_units.Data()));
	THStackAxisFonts(hstack, "y", TString::Format("entries / %.0f %s", _hist[iGG]->GetBinWidth(0),_units.Data()));
      }
    }

    // total mess to get it nice, should be redone
    size_t j=0;
    TString rsLabel = " RS Graviton";
    if(_mass != 0) rsLabel.Form(" m_{G}=%d",_mass);

    if(_data        ) { DrawLegend(xPos[j], 0.84 - yOff[j]*_yoffset, _data,         " data",    "lp"); j++; }
    if(_hist[iRS   ]) { DrawLegend(xPos[j], 0.84 - yOff[j]*_yoffset, _hist[iRS  ],   rsLabel, "l" ); j++; }
    if(_hist[iGG   ]) { DrawLegend(xPos[j], 0.84 - yOff[j]*_yoffset, _hist[iGG   ], " gg",      "f" ); j++; }
    if(_hist[iGJets]) { DrawLegend(xPos[j], 0.84 - yOff[j]*_yoffset, _hist[iGJets], " g+jets",  "f" ); j++; }
    if(_hist[iQCD])   { DrawLegend(xPos[j], 0.84 - yOff[j]*_yoffset, _hist[iQCD],   " QCD",  "f" ); j++; }

    // TLatex* luminosity = new TLatex(0.9, 0.815, TString::Format("L = %.1f pb^{-1}",_lumi));
    TLatex* luminosity = new TLatex(0.9, 0.615, TString::Format("L = %.1f pb^{-1}",_lumi));
    luminosity->SetNDC();
    luminosity->SetTextAlign(32);
    luminosity->SetTextFont(42);
    luminosity->SetTextSize(_tsize);
    luminosity->Draw("same");
  }

  void setLumi(const float &l) { _lumi = l; }
  void setLabel(const TString &s) { _xLabel = s; }
  void setUnits(const TString &s) { _units = s; }
  void setBreakdown(const bool &b = true) { _breakdown = b; }
  void addLabel(const std::string &s) {
    _extraLabel = new TLatex(0.9, 0.77, TString(s));
    _extraLabel->SetNDC();
    _extraLabel->SetTextAlign(32);
    _extraLabel->SetTextFont(42);
    _extraLabel->SetTextSize(_tsize);
  }
  
private: 
  std::vector<TH1F*> _hist;
  TH1F* _data;
  
  //MWL
  float    _lumi;
  TString  _xLabel;
  TString  _units;
  TLatex * _extraLabel;
  bool     _breakdown;
  int      _mass;
  bool     _nostack;
};


