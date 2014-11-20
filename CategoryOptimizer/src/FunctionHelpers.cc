#include "../interface/FunctionHelpers.h"

using namespace std;

// ------------------------------------------------------------------------------------------------
TH1 * integrate1D(TH1 * h, bool normalize) {
	TH1 * ret= (TH1*)h->Clone( Form("%s_cdf", h->GetName() ) );
	ret->SetDirectory(0);
	for(int xx=ret->GetNbinsX()-1; xx>=0; --xx) {
		ret->SetBinContent( xx, ret->GetBinContent(xx) + ret->GetBinContent(xx+1) );
	}
	if( normalize ) { ret->Scale( 1./h->Integral() ); }
	return ret;
}

// ------------------------------------------------------------------------------------------------
TH2 * integrate2D(TH2 * h, bool normalize) {
	TH2 * ret= (TH2*)h->Clone( Form("%s_cdf", h->GetName() ) );
	ret->SetDirectory(0);
	for(int xx=ret->GetNbinsX(); xx>=0; --xx) {
		for(int yy=ret->GetNbinsY()-1; yy>=0; --yy) {
			ret->SetBinContent( xx, yy, ret->GetBinContent(xx,yy) + ret->GetBinContent(xx,yy+1) );
		}
	}
	for(int yy=ret->GetNbinsY(); yy>=0; --yy) {
		for(int xx=ret->GetNbinsX()-1; xx>=0; --xx) {
			ret->SetBinContent( xx, yy, ret->GetBinContent(xx,yy) + ret->GetBinContent(xx+1,yy) );
		}
	}
	if( normalize ) { ret->Scale( 1./h->Integral() ); }
	return ret;
}

// ------------------------------------------------------------------------------------------------
TF1 * GraphToTF1::asTF1(TString name)
{
	return new TF1(name,this,xmin_,xmax_,0);
}

// ------------------------------------------------------------------------------------------------
DecorrTransform::DecorrTransform(TH2 * histo, float ref, bool doRatio) : doRatio_(doRatio)
{
	refbin_ = histo->GetXaxis()->FindBin(ref);
	hist_ = histo;
	double miny = histo->GetYaxis()->GetXmin();
	double maxy = histo->GetYaxis()->GetXmax();
	for(int ii=0; ii<histo->GetNbinsX()+1; ++ii) {
		TH1 * proj = histo->ProjectionY(Form("%s_%d",histo->GetName(),ii),ii,ii);
		/// dirtr_.push_back(cdf<GraphToTF1>(proj,miny,maxy));
		dirtr_.push_back(cdf(proj,miny,maxy));
		if( ii == refbin_ ) {
			/// invtr_ = cdfInv<GraphToTF1>(proj,miny,maxy);
			invtr_ = cdfInv(proj,miny,maxy);
			cout << invtr_->eval(0.) << " " << invtr_->eval(0.5)  << " " << invtr_->eval(1.) << endl;
			
		}
		delete proj;
	}
}

// ------------------------------------------------------------------------------------------------
double DecorrTransform::operator() (double *x, double *p)
{
	if( x[1] < -2.7 ) { return x[1]; }
	double ret = x[1];
	ret = invtr_->eval(getConverter(x[0])->eval(x[1]));
	return (doRatio_?ret/x[1]:ret);
}


// ------------------------------------------------------------------------------------------------
HistoConverter * DecorrTransform::clone() const
{
	return new DecorrTransform(*this);
}

// ------------------------------------------------------------------------------------------------
WrapDecorr::WrapDecorr(DecorrTransform *tr) : tr_(tr) 
{
}

// ------------------------------------------------------------------------------------------------
double WrapDecorr::operator() (double *x, double *p)
{
	std::vector<double> xp(2);
	xp[0] = p[0];
	xp[1] = x[0];
	return (*tr_)(&xp[0],0)-x[0];
}

// ------------------------------------------------------------------------------------------------
HistoConverter * WrapDecorr::clone() const
{
	return new WrapDecorr(*this);
}

// ------------------------------------------------------------------------------------------------
WrapDecorr::~WrapDecorr()
{
}
