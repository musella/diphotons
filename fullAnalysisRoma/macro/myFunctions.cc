// -------------------------------------------
// fitting functions
// -------------------------------------------

// crystal ball fit
double crystalball(double *x, double *par) {
  // par[0]:  mean
  // par[1]:  sigma
  // par[2]:  alpha, crossover point
  // par[3]:  n, length of tail
  // par[4]:  N, normalization
                                
  double cb = 0.0;
  double exponent = 0.0;
  
  if (x[0] > par[0] - par[2]*par[1]) {
    exponent = (x[0] - par[0])/par[1];
    cb = exp(-exponent*exponent/2.);
  } else {
    double nenner  = pow(par[3]/par[2], par[3])*exp(-par[2]*par[2]/2.);
    double zaehler = (par[0] - x[0])/par[1] + par[3]/par[2] - par[2];
    zaehler = pow(zaehler, par[3]);
    cb = nenner/zaehler;
  }
  
  if (par[4] > 0.) {
    cb *= par[4];
  }
  return cb;
}

double shapeFunction(double *x, double *par) {

  // par[0] = constant value
  // par[1], par[2], par[3]: a3, a2 x>0 pol3  
  // par[4], par[5], par[6]: a3, a2 x<=0 pol3  
  
  double ret_val;
  if(x[0]>0.)
    ret_val = par[0] + par[1]*x[0]*x[0] + par[2]*x[0]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0]*x[0];
  else
    ret_val = par[0] + par[4]*x[0]*x[0] + par[5]*x[0]*x[0]*x[0] + par[6]*x[0]*x[0]*x[0]*x[0];

  return ret_val;
}

// cruijff fit
double cruijffFunction(double *x, double *par) {

  double dx    = 0.0; 
  double sigma = 0.0; 
  double alpha = 0.0; 
  double f     = 0.0; 
  double func  = 0.0;

  dx = (x[0] - par[0]);  
  sigma = dx<0 ? par[1]: par[1] ;  
  alpha = dx<0 ? par[2]: par[3] ;  
  f = 2*sigma*sigma + alpha*dx*dx ;  
  func = exp(-dx*dx/f); 

  if (par[4] > 0.) {
    func *= par[4];
  }

  return func;
}

// effective sigma
double effectiveSigma(TH1F *histo_) {
  
  TAxis *xaxis = histo_->GetXaxis();
  Int_t nb = xaxis->GetNbins();
  if(nb < 10) {
    cout << "effsigma: Not a valid histo. nbins = " << nb << endl;
    return 0.;
  }

  Double_t bwid = xaxis->GetBinWidth(1);
  if(bwid == 0) {
    cout << "effsigma: Not a valid histo. bwid = " << bwid << endl;
    return 0.;
  }
  
  Double_t xmin = xaxis->GetXmin();
  Double_t ave = histo_->GetMean();
  Double_t rms = histo_->GetRMS();
  Double_t total=0.;
  for(Int_t i=0; i<nb+2; i++) {
    total+=histo_->GetBinContent(i);
  }
  if(total < 100.) {
    cout << "effsigma: Too few entries " << total << endl;
    return 0.;
  }

  Int_t ierr=0;
  Int_t ismin=999;
  Double_t rlim=0.683*total;
  Int_t nrms=rms/(bwid); // Set scan size to +/- rms
  if(nrms > nb/10) nrms=nb/10; // Could be tuned...
  Double_t widmin=9999999.;
  for(Int_t iscan=-nrms;iscan<nrms+1;iscan++) { // Scan window centre
    Int_t ibm=(ave-xmin)/bwid+1+iscan;
    Double_t x=(ibm-0.5)*bwid+xmin;
    Double_t xj=x;
    Double_t xk=x;
    Int_t jbm=ibm;
    Int_t kbm=ibm;
    Double_t bin=histo_->GetBinContent(ibm);
    total=bin;
    for(Int_t j=1;j<nb;j++){
      if(jbm < nb) {
	jbm++;
	xj+=bwid;
	bin=histo_->GetBinContent(jbm);
	total+=bin;
	if(total > rlim) break;
      }
      else ierr=1;
      if(kbm > 0) {
	kbm--;
	xk-=bwid;
	bin=histo_->GetBinContent(kbm);
	total+=bin;
	if(total > rlim) break;
      }
      else ierr=1;
    }
    Double_t dxf=(total-rlim)*bwid/bin;
    Double_t wid=(xj-xk+bwid-dxf)*0.5;
    if(wid < widmin) {
      widmin=wid;
      ismin=iscan;
    }
  }
  if(ismin == nrms || ismin == -nrms) ierr=3;
  if(ierr != 0) cout << "effsigma: Error of type " << ierr << endl;
  return widmin;
}
