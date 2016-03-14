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

//enum samp { iWJets, iWW, iWZ, iZZ, iGJ, iGG, iQCD, iTT, iDY, nSamples };
enum samp { iDY, nSamples };

//float xPos[nSamples+1] = {0.70,0.70,0.70,0.70,0.70,0.70,0.70,0.70,0.70,0.70};
//float yOff[nSamples+1] = {0,1,2,3,4,5,6,7,8,9};
float xPos[nSamples+1] = {0.65,0.65};
float yOff[nSamples+1] = {0,1};

//const Float_t _tsize   = 0.03;
const Float_t _tsize   = 0.04;
const Float_t _xoffset = 0.20;
const Float_t _yoffset = 0.08;


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
		TString option,
		TString title="")
{
  TLegend* legend = new TLegend(x1,
				y1,
				x1 + _xoffset,
				y1 + _yoffset,
				title);
  
  legend->SetBorderSize(     0);
  legend->SetFillColor (     0);
  legend->SetTextAlign (    12);
  legend->SetTextFont  (    42);
  legend->SetTextSize  (_tsize);
 
  legend->AddEntry(hist, label.Data(), option.Data());
  
  legend->Draw();
}

class TnPPlot {
  
public: 
  TnPPlot() { _hist.resize(nSamples,0); _data = 0; _breakdown = false; _mass = 0; }
  void setMCHist   (const samp &s, TH1F * h)  { _hist[s]       = h;  } 
  void setDataHist (TH1F * h)                 { _data          = h;  }

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
    _sampleColor[iDY  ]   = kAzure-9;
    //_sampleColor[iDY  ]   = kBlue+1;
    //_sampleColor[iWJets ] = kViolet-9;
    //_sampleColor[iWW    ] = kGreen-9;
    //_sampleColor[iWZ   ]  = kAzure-9;
    //_sampleColor[iZZ   ]  = kOrange-9;
    //_sampleColor[iGJ   ]  = kPink-9;
    //_sampleColor[iGG   ]  = kGreen-5;
    //_sampleColor[iQCD  ]  = kYellow-9;
    //_sampleColor[iTT   ]  = kRed-9;

    Color_t _lineColor[nSamples];
    _lineColor[iDY  ]  = kBlue+1;
    //_lineColor[iWJets] = kViolet;   
    //_lineColor[iWW   ] = kGreen;
    //_lineColor[iWZ  ]  = kAzure;
    //_lineColor[iZZ  ]  = kOrange;
    //_lineColor[iGJ  ]  = kPink;
    //_lineColor[iGG  ]  = kGreen+4;
    //_lineColor[iQCD ]  = kYellow;
    //_lineColor[iTT  ]  = kRed;
    
    if(!gPad) new TCanvas();

    THStack* hstack = new THStack();
    for (int i=0; i<nSamples; i++) {

      // in case the user doesn't set it
      if( !_hist[i] ) continue;
      
      _hist[i]->Rebin(rebin);
      _hist[i]->SetLineColor(_lineColor[i]);
      _hist[i]->SetFillColor(_sampleColor[i]);
      _hist[i]->SetFillStyle(1001);
      
      hstack->Add(_hist[i]);
    }
  
    if(_data) _data->Rebin(rebin);
    if(_data) _data->SetLineColor  (kBlack);
    if(_data) _data->SetMarkerStyle(kFullCircle);
  
    hstack->Draw("hist");
    if(_data) _data->Draw("ep,same");
    
    //hstack->SetTitle("CMS preliminary");  
    
    Float_t theMax = hstack->GetMaximum();
    Float_t theMin = hstack->GetMinimum();
    
    if (_data) {
      Float_t dataMax = GetMaximumIncludingErrors(_data);
      if (dataMax > theMax) theMax = dataMax;
    }

    if (gPad->GetLogy()) {
      hstack->SetMaximum(1.2 * theMax);
      hstack->SetMinimum(0.1);  
      if (hstack->GetMinimum()==0) hstack->SetMinimum(0.00005);  
    } else {
      hstack->SetMaximum(1.55 * theMax);
    }
    
    if(_breakdown) {
      THStackAxisFonts(hstack, "y", "Events");
      hstack->GetHistogram()->LabelsOption("v");
    } else {
      THStackAxisFonts(hstack, "x", TString::Format("%s (%s)",_xLabel.Data(),_units.Data()));
      if(_units.Sizeof() == 1) {
	THStackAxisFonts(hstack, "x", _xLabel.Data());
	THStackAxisFonts(hstack, "y", "Events");
      } else {
	THStackAxisFonts(hstack, "x", TString::Format("%s (%s)",_xLabel.Data(),_units.Data()));
	// THStackAxisFonts(hstack, "y", TString::Format("entries / %.0f %s", _hist[iWJets]->GetBinWidth(0),_units.Data()));
	//THStackAxisFonts(hstack, "y", TString::Format("Events / %.0f %s", _hist[iDY]->GetBinWidth(0),_units.Data()));
	THStackAxisFonts(hstack, "y", TString::Format("Events / (%.1f %s)", _hist[iDY]->GetBinWidth(0),_units.Data()));
      }
    }

    // total mess to get it nice, should be redone
    size_t j=0;
    if(_data        ) { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _data,         " data",    "lp", "Z #rightarrow ee"); j++; }
    if(_hist[iDY   ]) { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iDY  ],   "simulation", "f" ); j++; }
    //if(_hist[iDY   ]) { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iDY  ],   "DY", "f" ); j++; }
    //if(_hist[iWJets]) { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iWJets   ], " W+jets",      "f" ); j++; }
    //if(_hist[iWW])    { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iWW],   " WW",  "f" ); j++; }
    //if(_hist[iWZ])    { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iWZ],   " WZ",  "f" ); j++; }
    //if(_hist[iZZ])    { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iZZ],   " ZZ",  "f" ); j++; }
    //if(_hist[iGJ])    { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iGJ],   " g+jets",  "f" ); j++; }
    //if(_hist[iGG])    { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iGG],   " gg+jets",  "f" ); j++; }
    //if(_hist[iQCD])   { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iQCD],  " qcd",  "f" ); j++; }
    //if(_hist[iTT])    { DrawLegend(xPos[j], 0.75 - yOff[j]*_yoffset, _hist[iTT],   " ttjets",  "f" ); j++; }

    /*
    TLatex* luminosity = new TLatex(0.86, 0.620, TString::Format("L = %.1f pb^{-1}",_lumi));
    //TLatex* luminosity = new TLatex(0.86, 0.620, TString::Format("L = 2.4 fb^{-1}"));
    luminosity->SetNDC();
    luminosity->SetTextAlign(32);
    luminosity->SetTextFont(42);
    luminosity->SetTextSize(_tsize);
    luminosity->Draw("same");
    */
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
};


