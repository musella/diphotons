#ifndef diphotons_Utils_RooPowLogPdf_h
#define diphotons_Utils_RooPowLogPdf_h

#include "RooAbsPdf.h"
#include "RooRealProxy.h"

class RooRealVar;
class RooAbsReal;

class RooPowLogPdf : public RooAbsPdf {
public:
	RooPowLogPdf() : analyticIntegral_(true) {} ;
	RooPowLogPdf(const char *name, const char *title,
		     RooAbsReal& _x, RooAbsReal& _c, RooAbsReal& _d);
	RooPowLogPdf(const RooPowLogPdf& other, const char* name=0);
	virtual TObject* clone(const char* newname) const { return new RooPowLogPdf(*this,newname); }
	inline virtual ~RooPowLogPdf() { }
	
	const RooAbsReal & base() const { return x_.arg(); }
	const RooAbsReal & linc() const { return c_.arg(); }
	const RooAbsReal & logc() const { return d_.arg(); }
	
	void doAnalyticIntegral(bool x=true) { analyticIntegral_ = x; }
	Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const ;
	Double_t analyticalIntegral(Int_t code, const char* rangeName) const;
	
protected:
	bool analyticIntegral_;
	RooRealProxy x_;
	RooRealProxy c_, d_;
	
	mutable Double_t val_, logVal_;
	
	Double_t evaluate() const;
	
private:
	ClassDef(RooPowLogPdf,1) // Exponential PDF
};

#endif
