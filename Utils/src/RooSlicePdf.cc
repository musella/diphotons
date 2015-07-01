#include "RooFit.h"

#include "Riostream.h"
#include "Riostream.h"
#include <cmath>
#include <limits>

#include "../interface/RooSlicePdf.h"
#include "RooRealVar.h"

#include "TMath.h"

ClassImp(RooSlicePdf)

using namespace std;


//_____________________________________________________________________________
RooSlicePdf::RooSlicePdf(const char *name, const char *title, TH2 * histo, 
			 RooAbsReal& _x, RooAbsReal& _p) :
	RooAbsPdf(name, title), 
	histo_(dynamic_cast<TH2*>(histo->Clone(Form("histo_%s",name)))),
	x_("x","Dependent",this,_x), p_("p","Parameter",this,_p)
{
	histo_->SetDirectory(0);
	
	// normalize the slices
	int nbinsx = histo_->GetNbinsX();
	int nbinsy = histo_->GetNbinsY();
	/// cout << "nbinsx " << nbinsx<< " nbinsy " << nbinsy << endl;
	for(int ib=0; ib<nbinsx; ++ib) {
		double sliceint = histo_->Integral(ib+1,ib+1,1,nbinsy);
		/// cout << "sliceint " << ib << " " << sliceint << endl;
		for(int jb=0; jb<nbinsy; ++jb) {
			double bincont = histo_->GetBinContent(ib+1,jb+1)/(sliceint*histo_->GetYaxis()->GetBinWidth(jb+1));
			histo_->SetBinContent(ib+1,jb+1,bincont);
			histo_->SetBinError(ib+1,jb+1,0.); // errors are not used 
		}
		histo_->SetBinContent(ib+1,0,0.); // zero-out underflows and overflows
		histo_->SetBinError(ib+1,0,0.);
		histo_->SetBinContent(ib+1,nbinsy+1,0.);
		histo_->SetBinError(ib+1,nbinsy+1,0.);
	}
	histo_->SetBinContent(0,0,0.); // zero-out underflows and overflows
	histo_->SetBinError(0,0,0.);
	histo_->SetBinContent(nbinsx+1,nbinsy+1,0.);
	histo_->SetBinError(nbinsx+1,nbinsy+1,0.);
	/// histo_->Print("all");
}


//_____________________________________________________________________________
RooSlicePdf::RooSlicePdf(const RooSlicePdf& other, const char* name) :
	RooAbsPdf(other, name), 
	histo_(dynamic_cast<TH2*>(other.histo_->Clone(Form("histo_%s",name)))),
	x_("x",this,other.x_), p_("p",this,other.p_)
{
	histo_->SetDirectory(0);
}

//_____________________________________________________________________________
Int_t RooSlicePdf::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const 
{
	if( matchArgs(allVars,analVars,x_)) return 1 ;
	return 0 ;
}

//_____________________________________________________________________________
Double_t RooSlicePdf::analyticalIntegral(Int_t code, const char* rangeName) const 
{
	switch(code) {
	case 1: 
	{
		Double_t ret(0) ;
		const Double_t eps = 1.e-300;
		Double_t x1 = x_.min(rangeName);
		Double_t x2 = x_.max(rangeName);		

		int slice = histo_->GetXaxis()->FindBin(p_);
		int nbinsy = histo_->GetNbinsY();
		for(int ib=0; ib<nbinsy; ++ib) {
			Double_t low  = histo_->GetYaxis()->GetBinLowEdge(ib+1);
			Double_t high = histo_->GetYaxis()->GetBinUpEdge(ib+1);
			Double_t binc = histo_->GetBinContent(slice,ib+1);
			
			Double_t overlap = min(high,x2) - max(low,x1);
			if( overlap > 0. ) {
				ret += binc * overlap;
			}
		}
		
		//// cout << "Int_RooSlicePdf_dx(c_=" << c_ << " d_= " << d_ <<  ", x1=" << x1 << ", x2=" << x2 << ")=" << ret << endl ;
		return max(ret,eps);
	}
	}
  
	assert(0) ;
	return 0 ;
}

//_____________________________________________________________________________
Double_t RooSlicePdf::evaluate() const{
	int ibin = histo_->FindBin(p_,x_);
	val_ = histo_->GetBinContent( ibin );
	/// cout << p_ << " " << x_ << " " << " " << ibin << " " << val_ << endl;
	return val_;
}
