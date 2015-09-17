#ifndef diphotons_Utils_RooSlicePdf_h
#define diphotons_Utils_RooSlicePdf_h

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "TH2.h"
#include "RVersion.h"
// #include <memory>

class RooRealVar;
class RooAbsReal;

class RooSlicePdf : public RooAbsPdf {
public:
	RooSlicePdf() : histo_(0) ///,  pdf_(0) 
		{} ;
	RooSlicePdf(const char *name, const char *title, TH2 * histo, 
		    RooAbsReal& _x, RooAbsReal& _p, 		     
		    Double_t * widths=0 //, RooAbsPdf * _ppdf=0
		);
	RooSlicePdf(const RooSlicePdf& other, const char* name=0);
	virtual TObject* clone(const char* newname) const { return new RooSlicePdf(*this,newname); }
	inline virtual ~RooSlicePdf() { if(histo_) { delete histo_; } }
	
	const RooAbsReal & x() const { return x_.arg(); }
	const RooAbsReal & p() const { return p_.arg(); }
	
	Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const ;
	Double_t analyticalIntegral(Int_t code, const char* rangeName) const;
	
protected:
	/// std::shared_ptr<TH2> histo_;
	TH2 * histo_;
	std::vector<double> widths_;
	/// RooAbsPdf * pdf_;
	/// RooRealProxy binw_;
	RooRealProxy x_;
	RooRealProxy p_;
	
	mutable Double_t val_;
	
	Double_t evaluate() const;
	
private:
	ClassDef(RooSlicePdf,1) // Exponential PDF
};

#endif
