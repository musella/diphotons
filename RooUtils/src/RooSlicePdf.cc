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
			 RooAbsReal& _x, RooAbsReal& _p, Double_t * _widths //, RooAbsPdf * __ppdf
	) :
	RooAbsPdf(name, title), 
	histo_(dynamic_cast<TH2*>(histo->Clone(Form("histo_%s",name)))),	
	/// pdf_(__ppdf),
	// widths_(&_widths[0],&_widths[histo_->GetNbinsY()]),
	x_("x","Dependent",this,_x), p_("p","Parameter",this,_p)
{
	histo_->SetDirectory(0);
	
	// normalize the slices
	int nbinsx = histo_->GetNbinsX();
	int nbinsy = histo_->GetNbinsY();
	/// cout << "nbinsx " << nbinsx<< " nbinsy " << nbinsy << endl;
	for(int yb=0; yb<histo_->GetNbinsY(); ++yb) {
		if( _widths != 0 ) { 
			widths_.push_back(_widths[yb]); 
		} else { 
			widths_.push_back(histo_->GetYaxis()->GetBinWidth(yb+1) ); 
		}
	}
	for(int ib=0; ib<nbinsx; ++ib) {
		double sliceint = histo_->Integral(ib+1,ib+1,1,nbinsy);		
		if( sliceint == 0. ) { 
			cout << "Warning: slice has " << sliceint << " integral: " << histo_->GetXaxis()->GetBinLowEdge(ib+1) << " " << histo_->GetXaxis()->GetBinUpEdge(ib+1) << endl;
			continue; 
		}
		/// double slicewidth = histo_->GetXaxis()->GetBinWidth(ib+1);
		/// cout << "sliceint " << ib << " " << sliceint << endl;		
		for(int jb=0; jb<nbinsy; ++jb) {
			double bincont = histo_->GetBinContent(ib+1,jb+1)/(sliceint*widths_[jb]);
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
}


//_____________________________________________________________________________
RooSlicePdf::RooSlicePdf(const RooSlicePdf& other, const char* name) :
	RooAbsPdf(other, name), 
	histo_(dynamic_cast<TH2*>(other.histo_->Clone(Form("histo_%s",name)))),
	/// pdf_(other.pdf_),
	widths_(other.widths_),
	x_("x",this,other.x_), p_("p",this,other.p_)
{
	histo_->SetDirectory(0);
}

//_____________________________________________________________________________
Int_t RooSlicePdf::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const 
{
	int code = matchArgs(allVars,analVars,x_);
	/// if( code > 0 ) { 
	/// 	code += matchArgs(allVars,analVars,p_);
	/// }
	
	/// if( analytic 
	///     /// && (pdf_ == 0 || pdf_->getAnalyticalIntegral(allVars,analVars,rangeName) ) ) return 1 ;
	return code;
}

//_____________________________________________________________________________
Double_t RooSlicePdf::analyticalIntegral(Int_t code, const char* rangeName) const 
{
	assert( code == 1 || code == 2);

	Double_t ret(0) ;
	const Double_t eps = 1.e-300;
	Double_t x1 = x_.min(rangeName);
	Double_t x2 = x_.max(rangeName);		

	int slice1;
	int slice2;
	if( code == 2 ) {
		slice1 = histo_->GetXaxis()->FindBin(p_.min(rangeName));
		slice2 = histo_->GetXaxis()->FindBin(p_.max(rangeName));
	} else {
		slice1 = histo_->GetXaxis()->FindBin(p_);
		slice2 = slice1;
	}
	
	int nbinsy = histo_->GetNbinsY();
	if( x1 <= histo_->GetYaxis()->GetBinLowEdge(1) && x2 >= histo_->GetYaxis()->GetBinUpEdge(nbinsy) ) { 
	 	ret = 1.;
	} else { 
		/// cout << "analyticalIntegral " << code << " " << x1 << " " << x2 << " "<< slice1 << " "<< slice2 << endl;
		for(int slice = slice1; slice<=slice2; ++slice ) {
			Double_t slicesum = 0.;
			for(int ib=0; ib<nbinsy; ++ib) {
				Double_t low  = histo_->GetYaxis()->GetBinLowEdge(ib+1);
				Double_t high = histo_->GetYaxis()->GetBinUpEdge(ib+1);
				Double_t binc = histo_->GetBinContent(slice,ib+1);
				
				Double_t xhigh = min(high,x2);
				Double_t xlow  = max(low,x1);
				Double_t overlap = (xhigh - xlow)/(high-low);
				//// cout << "low=" << low << " high=" << high << " xlow=" << xlow << " xhigh=" << xhigh << " binc=" << binc << " overlap="<<overlap 
				////      << " width=" << widths_[ib] << endl;
				if( overlap > 0. ) {
					slicesum += binc * overlap * widths_[ib];
				}
			}
			Double_t wei = 1.;
			ret += slicesum * wei;
		}
	}
	return max(ret,eps);
}

//_____________________________________________________________________________
Double_t RooSlicePdf::evaluate() const{
	int ibin = histo_->FindBin(p_,x_);
	val_ = histo_->GetBinContent( ibin );
	//// if( pdf_ ) { val_ *= pdf_->getVal(); };
	/// cout << p_ << " " << x_ << " " << " " << ibin << " " << val_ << endl;
	return val_;
}
