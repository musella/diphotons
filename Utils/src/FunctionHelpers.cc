#include "../interface/FunctionHelpers.h"
#include "TTreeFormula.h"
#include "TFitResult.h"
#include "TCanvas.h"

using namespace std;


HistoConverter::~HistoConverter()
{
    if( sp_ )   { delete sp_; }
    if( hist_ ) { delete hist_; }
    if( g_ )    { delete g_; }
}

// ------------------------------------------------------------------------------------------------
TH1 *integrate1D( TH1 *h, bool normalize )
{
    TH1 *ret = ( TH1 * )h->Clone( Form( "%s_cdf", h->GetName() ) );
    ret->SetDirectory( 0 );
    for( int xx = ret->GetNbinsX() - 1; xx >= 0; --xx ) {
        ret->SetBinContent( xx, ret->GetBinContent( xx ) + ret->GetBinContent( xx + 1 ) );
    }
    if( normalize ) { ret->Scale( 1. / h->Integral() ); }
    return ret;
}

// ------------------------------------------------------------------------------------------------
TH2 *integrate2D( TH2 *h, bool normalize )
{
    TH2 *ret = ( TH2 * )h->Clone( Form( "%s_cdf", h->GetName() ) );
    ret->SetDirectory( 0 );
    for( int xx = ret->GetNbinsX(); xx >= 0; --xx ) {
        for( int yy = ret->GetNbinsY() - 1; yy >= 0; --yy ) {
            ret->SetBinContent( xx, yy, ret->GetBinContent( xx, yy ) + ret->GetBinContent( xx, yy + 1 ) );
        }
    }
    for( int yy = ret->GetNbinsY(); yy >= 0; --yy ) {
        for( int xx = ret->GetNbinsX() - 1; xx >= 0; --xx ) {
            ret->SetBinContent( xx, yy, ret->GetBinContent( xx, yy ) + ret->GetBinContent( xx + 1, yy ) );
        }
    }
    if( normalize ) { ret->Scale( 1. / h->Integral() ); }
    return ret;
}

HistoConverter *mkCdfInv( TH1 *h, double min, double max )
{
    return cdfInv<GraphToTF1>( h, min, max );
}

HistoConverter *mkCdf( TH1 *h, double min, double max )
{
    return cdf<GraphToTF1>( h, min, max );
}


// ------------------------------------------------------------------------------------------------
TF1 *GraphToTF1::asTF1( TString name )
{
    return new TF1( name, this, xmin_, xmax_, 0 );
}

// ------------------------------------------------------------------------------------------------
DecorrTransform::DecorrTransform( TH2 *histo, float ref, bool doRatio, bool invert ) : doRatio_( doRatio )
{
    refbin_ = histo->GetXaxis()->FindBin( ref );
    hist_ = histo;
    double miny = histo->GetYaxis()->GetXmin();
    double maxy = histo->GetYaxis()->GetXmax();
    for( int ii = 0; ii < histo->GetNbinsX() + 1; ++ii ) {
        TH1 *proj = histo->ProjectionY( Form( "%s_%d", histo->GetName(), ii ), ii, ii );
        /// dirtr_.push_back(cdf<GraphToTF1>(proj,miny,maxy));
        if( invert ) {
            dirtr_.push_back( cdfInv( proj, miny, maxy ) );
        } else {
            dirtr_.push_back( cdf( proj, miny, maxy ) );
        }
        if( ii == refbin_ ) {
            /// invtr_ = cdfInv<GraphToTF1>(proj,miny,maxy);
            if( invert ) {
                invtr_ = cdf( proj, miny, maxy );
            } else {
                invtr_ = cdfInv( proj, miny, maxy );
            }
            cout << invtr_->eval( 0. ) << " " << invtr_->eval( 0.5 )  << " " << invtr_->eval( 1. ) << endl;

        }
        delete proj;
    }
}

// ------------------------------------------------------------------------------------------------
double DecorrTransform::operator()( double *x, double *p )
{
    /// if( x[1] < -2.7 ) { return x[1]; }
    double ret = x[1];
    ret = invtr_->eval( getConverter( x[0] )->eval( x[1] ) );
    return ( doRatio_ ? ret / x[1] : ret );
}

// ------------------------------------------------------------------------------------------------
HistoConverter *DecorrTransform::clone() const
{
    return new DecorrTransform( *this );
}

// ------------------------------------------------------------------------------------------------
WrapDecorr::WrapDecorr( DecorrTransform *tr ) : tr_( tr )
{
}

// ------------------------------------------------------------------------------------------------
double WrapDecorr::operator()( double *x, double *p )
{
    std::vector<double> xp( 2 );
    xp[0] = p[0];
    xp[1] = x[0];
    return ( *tr_ )( &xp[0], 0 ) - x[0];
}

// ------------------------------------------------------------------------------------------------
HistoConverter *WrapDecorr::clone() const
{
    return new WrapDecorr( *this );
}

// ------------------------------------------------------------------------------------------------
WrapDecorr::~WrapDecorr()
{
}

// ------------------------------------------------------------------------------------------------
SliceFitter::SliceFitter( TH2 *histo, TString formula, float ymin, float ymax, float fmin, float fmax, bool normalize, bool yonly, int sliding )
{
    plots_ = new TList();
    yonly_ = yonly;
    hist_ = ( TH1 * )histo->ProjectionX()->Clone();
    hist_->SetDirectory( 0 );
    ymin_ = ymin;
    ymax_ = ymax;
    xmin_ = histo->GetXaxis()->GetXmin();
    xmax_ = histo->GetXaxis()->GetXmax();
    sliding_ = sliding;
    if( normalize ) {
        hist_->Scale( 1. / hist_->Integral() );
    }
    // for( int jj = 0; jj < hist_->GetNbinsX() + 1; ++jj ) {
    //     float w = hist_->GetBinWidth( jj );
    //     hist_->SetBinContent( jj, hist_->GetBinContent( jj ) / w );
    //     hist_->SetBinError( jj, hist_->GetBinError( jj ) / w );
    // }
    for( int ii = 1; ii < histo->GetNbinsX() + 1; ++ii ) {
        int imin = ii, imax = ii;
        if( sliding_ > 0 ) {
            imin = ii - sliding_;
            imax = ii + sliding_;
            while( imin <  1 ) { 
                ++imin; ++imax;
            }
            while( imax > histo->GetNbinsX() ) {
                --imin, --imax;
            }
            imin = max(1,imin);
            imax = min(histo->GetNbinsX(), imax);
            /// cout << "sliding " << imin << " " << ii << " " << imax << endl;
        }
        TH1 *proj = histo->ProjectionY( Form( "%s_%d", histo->GetName(), ii ), imin, imax );

        proj->Scale( 1. / proj->Integral() );

        // for( int jj = 0; jj < proj->GetNbinsX() + 1; ++jj ) {
        //     float w = proj->GetBinWidth( jj );
        //     proj->SetBinContent( jj, proj->GetBinContent( jj ) / w );
        //     proj->SetBinError( jj, proj->GetBinError( jj ) / w );
        // }

        sliceFits_.push_back( TF1( Form( "%s_%d", histo->GetName(), ii ), formula, ymin, ymax ) );
        proj->Fit( &sliceFits_.back(), "Q+", "", fmin, fmax );
        /// cout << sliceFits_.back().GetExpFormula("P") << " " << sliceFits_.back().Integral(ymin,ymax) << endl;
        integrals_.push_back( sliceFits_.back().Integral(ymin_,ymax_) );

        TCanvas * canv = new TCanvas(proj->GetName(),proj->GetName());
        canv->cd();
        proj->DrawCopy();
        //// canv->SaveAs(Form("%s.png",proj->GetName()));
        //// canv->SaveAs(Form("%s.root",proj->GetName()));
        plots_->Add(canv);
        
        delete proj;
        /// delete canv;
    }
    cout << this << " " << plots_ << endl;
}

// ------------------------------------------------------------------------------------------------
const TF1 &SliceFitter::getSlice( double x )
{
    int bin = hist_->FindBin( x )-1;
    return sliceFits_[bin];
}

// ------------------------------------------------------------------------------------------------
double SliceFitter::operator()( double *x, double *p )
{
    if( x[0] < xmin_ || x[0] > xmax_ || x[1] < ymin_ || x[1] > ymax_ ) { return 0.; }
    int bin = hist_->FindBin( x[0] ) -1;
    if( bin < ( int )sliceFits_.size() ) {
        return ( yonly_ ? 1. : hist_->GetBinContent( bin ) ) * std::max( 0., sliceFits_[bin].Eval( x[1] )/integrals_[bin] );
    }
    return 0.;
}


// ------------------------------------------------------------------------------------------------
TList * SliceFitter::getPlots()
{
    TList * ret = plots_;
    plots_ = 0;
    cout << this << " " << ret << " " << plots_ << endl;
    return ret;
}

// ------------------------------------------------------------------------------------------------
SliceFitter::~SliceFitter()
{
    cout << this << " being destroyed...." << endl;
    if( plots_ ) { delete plots_; }
}

// ------------------------------------------------------------------------------------------------
TF2 *SliceFitter::asTF2( TString name )
{
    return new TF2( name, this, xmin_, xmax_, ymin_, ymax_, 0, "SliceFitter" );
}


// ------------------------------------------------------------------------------------------------
void fillReweight( TString xvar, TString yvar, TString sel, TF2 &wei, TTree &in, TTree &out )
{
    float rewei, xr, yr, sr;
    TTreeFormula x( "xv", xvar, &in );
    TTreeFormula y( "xv", yvar, &in );
    TTreeFormula w( "w", sel, &in );

    out.Branch( "rewei", &rewei, "rewei/F" );
    out.Branch( "xrewei", &xr, "xrewei/F" );
    out.Branch( "yrewei", &yr, "yrewei/F" );
    out.Branch( "srewei", &sr, "srewei/F" );
    for( int ie = 0; ie < in.GetEntries(); ++ie ) {
        in.GetEntry( ie );
        rewei = 0.;
        xr = x.EvalInstance();
        yr = y.EvalInstance();
        sr = w.EvalInstance();
        if( sr > 0. ) {
            rewei = wei.Eval( xr, yr );
        }
        out.Fill();
    }
}

// ------------------------------------------------------------------------------------------------
double FlatReweight::operator()( double *x, double *p )
{
    double sum = 0.;
    for( size_t ii = 0; ii < components_.size(); ++ii ) {
        sum += ( *components_[ii] )( x, p );
    }
    if( sum != 0. ) { sum = 1. / sum; }
    return sum;
}

// ------------------------------------------------------------------------------------------------
FlatReweight::~FlatReweight()
{

}

// ------------------------------------------------------------------------------------------------
TF2 *FlatReweight::asTF2( TString name )
{
    return new TF2( name, this, xmin_, xmax_, ymin_, ymax_, 0, "FlatReweight" );
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

