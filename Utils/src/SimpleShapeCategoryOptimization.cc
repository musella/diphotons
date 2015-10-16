#include "TMath.h"
#include "TSpline.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TF2.h"
#include "TF3.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooExtendPdf.h"
#include "RooArgSet.h"
#include "RooProduct.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooMinimizer.h"
#include "RooAddition.h"
#include "RooConstraintSum.h"
#include "Math/IFunction.h"
#include "TCanvas.h"
#include "Math/AdaptiveIntegratorMultiDim.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TMatrix.h"

#include <algorithm>
#include <cmath>

#include "../interface/CategoryOptimizer.h"
#include "../interface/SimpleShapeCategoryOptimization.h"

using namespace std;
// ------------------------------------------------------------------------------------------------
void makeSecondOrder( THnSparse *in, THnSparse *norm, THnSparse *sumX, THnSparse *sumX2 )
{
    std::vector<int> idx( norm->GetNdimensions() );
    std::vector<double> stats( 4 ), qtiles( 2 ), probs( 2 );
    double effrms;
    in->Print( "all" );
    norm->Print( "all" );
    sumX->Print( "all" );
    sumX2->Print( "all" );
    probs[0] = 0.8415, probs[1] = 0.1585;
    for( int ii = 0; ii < norm->GetNbins(); ++ii ) {
        norm->GetBinContent( ii, &idx[0] );
        for( size_t idim = 0; idim < idx.size(); ++idim ) {
            TAxis *iaxis = in->GetAxis( idim );
            iaxis->SetRange( idx[idim], idx[idim] );
        }
        TH1 *hx = in->Projection( in->GetNdimensions() - 1 );
        hx->GetStats( &stats[0] );
        norm->SetBinContent( ii, stats[0] );
        sumX->SetBinContent( ii, stats[2] );
        hx->GetQuantiles( 2, &qtiles[0], &probs[0] );
        effrms = 0.5 * ( qtiles[1] - qtiles[0] );
        sumX2->SetBinContent( ii, stats[2]*stats[2] / stats[0] - stats[0]*effrms * effrms );
        delete hx;
    }

}

// ------------------------------------------------------------------------------------------------
void makeSecondOrder( THnSparse *in, THnSparse *red, SparseIntegrator *norm, SparseIntegrator *sumX, SparseIntegrator *sumX2 )
{
    std::vector<int> idx( in->GetNdimensions() - 1 );
    std::vector<double> coord( in->GetNdimensions() - 1 );
    std::vector<double> stats( 4 ), qtiles( 2 ), probs( 2 );
    double effrms;
    in->Print( "all" );
    /// norm->Print("all");
    /// sumX->Print("all");
    /// sumX2->Print("all");
    probs[0] = 0.8415, probs[1] = 0.1585;
    /// std::cout << norm->GetNbins() << std::endl;
    for( int ii = 0; ii < red->GetNbins(); ++ii ) {
        red->GetBinContent( ii, &idx[0] );
        for( size_t idim = 0; idim < idx.size(); ++idim ) {
            TAxis *iaxis = in->GetAxis( idim );
            iaxis->SetRange( idx[idim], idx[idim] );
            coord[idim] = iaxis->GetBinCenter( idx[idim] );
        }
        TH1 *hx = in->Projection( in->GetNdimensions() - 1 );
        hx->GetStats( &stats[0] );
        norm->fill( &coord[0], stats[0] );
        sumX->fill( &coord[0], stats[2] );
        hx->GetQuantiles( 2, &qtiles[0], &probs[0] );
        effrms = 0.5 * ( qtiles[1] - qtiles[0] );
        sumX2->fill( &coord[0], stats[2]*stats[2] / stats[0] - stats[0]*effrms * effrms );
        delete hx;
    }

    norm->link();
    sumX->link();
    sumX2->link();
}


// ------------------------------------------------------------------------------------------------
void makeSecondOrder( std::vector<TH1 *> &histos, SparseIntegrator *norm, SparseIntegrator *sumX, SparseIntegrator *sumX2 )
{
    std::vector<double> stats( 4 ), qtiles( 2 ), probs( 2 );
    double effrms;
    /// norm->Print("all");
    /// sumX->Print("all");
    /// sumX2->Print("all");
    probs[0] = 0.8415, probs[1] = 0.1585;
    /// std::cout << norm->GetNbins() << std::endl;
    for( IntegrationWeb::iterator ibin = norm->begin(); ibin != norm->end(); ++ibin ) {

        TH1 *hx = histos[( *ibin )->id() ];
        hx->GetStats( &stats[0] );
        IntegrationNode *nodeX = new IntegrationNode( ( *ibin )->id(), ( *ibin )->coord(), stats[2] );
        sumX->insert( nodeX );
        hx->GetQuantiles( 2, &qtiles[0], &probs[0] );
        effrms = 0.5 * ( qtiles[1] - qtiles[0] );
        IntegrationNode *nodeX2 = new IntegrationNode( ( *ibin )->id(), ( *ibin )->coord(),
                stats[2]*stats[2] / stats[0] - stats[0]*effrms * effrms );
        sumX2->insert( nodeX2 );
        delete hx;
    }
    histos.clear();

    norm->link();
    sumX->link();
    sumX2->link();
}

// ------------------------------------------------------------------------------------------------
THnSparse *integrate( THnSparse *h, float norm )
{
    THnSparse *ret = ( THnSparse * )h->Clone( Form( "%s_cdf", h->GetName() ) );
    std::vector<int> idx( h->GetNdimensions() );
    std::vector<int> alldims( h->GetNdimensions() );
    for( size_t ii = 0; ii < alldims.size(); ++ii ) {
        alldims[ii] = ii;
    }
    //// std::cout << "integrate " << norm << std::endl;
    for( int ii = 0; ii < h->GetNbins(); ++ii ) {
        h->GetBinContent( ii, &idx[0] );
        for( size_t idim = 0; idim < idx.size(); ++idim ) {
            TAxis *iaxis = h->GetAxis( idim );
            iaxis->SetRange( idx[idim], -1 );
        }
        THnSparse *tmp = h->Projection( alldims.size(), &alldims[0] );
        float integral = 0.;
        for( int jj = 0; jj < tmp->GetNbins(); ++ii ) {
            integral += tmp->GetBinContent( jj );
        }
        /// tmp->Print("V");
        //// std::cout << tmp->GetWeightSum()<< std::endl;
        ret->SetBinContent( ii, tmp->GetWeightSum() / norm );
        delete tmp;
    }
    return ret;
}

// ------------------------------------------------------------------------------------------------
TTree *toTree( const THnSparse *h, const THnSparse *hX, const THnSparse *hX2 )
{
    // Creates a TTree and fills it with the coordinates of all
    // filled bins. The tree will have one branch for each dimension,
    // and one for the bin content.

    Int_t dim = h->GetNdimensions();
    TString name( h->GetName() );
    name += "_tree";
    TString title( h->GetTitle() );
    title += " tree";

    TTree *tree = new TTree( name, title );
    Double_t *x = new Double_t[dim + 3];
    memset( x, 0, sizeof( Double_t ) * ( dim + 3 ) );

    TString branchname;
    for( Int_t d = 0; d < dim; ++d ) {
        TAxis *axis = h->GetAxis( d );
        tree->Branch( axis->GetTitle(), &x[d] );
    }
    tree->Branch( "sumw",   &x[dim] );
    if( hX ) { tree->Branch( "sumwx",  &x[dim + 1] ); }
    if( hX2 ) { tree->Branch( "sumwx2", &x[dim + 2] ); }

    Int_t *bins = new Int_t[dim];
    for( Long64_t i = 0; i < h->GetNbins(); ++i ) {
        x[dim] = h->GetBinContent( i, bins );
        if( hX ) { x[dim + 1] = hX->GetBinContent( i, bins ); }
        if( hX2 ) { x[dim + 2] = hX2->GetBinContent( i, bins ); }
        for( Int_t d = 0; d < dim; ++d ) {
            x[d] = h->GetAxis( d )->GetBinCenter( bins[d] );
        }

        tree->Fill();
    }

    delete [] bins;
    //delete [] x;
    return tree;
}


// ------------------------------------------------------------------------------------------------
SecondOrderModelBuilder::SecondOrderModelBuilder( AbsModel::type_t type,
        std::string name, RooRealVar *x,
        TTree *tree,
        const RooArgList *varlist,
        const RooArgList *sellist,
        const char *weightBr )
    :  model_( name, x, type ), ndim_( varlist->getSize() )
{
    RooArgList exvarlist( *varlist );
    exvarlist.add( *sellist );
    exvarlist.add( *x );
    size_t nvar = exvarlist.getSize();
    std::vector<TString> names( nvar );
    std::vector<int> nbins( nvar );
    std::vector<double> xmin( nvar );
    std::vector<double> xmax( nvar );
    std::vector<TTreeFormula *> formulas( nvar );
    std::vector<int> nm1( nvar - 1 );
    TTreeFormula *weight = ( weightBr != 0 ? new TTreeFormula( weightBr, weightBr, tree ) : 0 );
    selectionCuts_.resize( sellist->getSize(), -999. );
    selectionCutsBegin_.resize( sellist->getSize(), 999. );

    for( size_t ivar = 0; ivar < nvar; ++ivar ) {
        RooRealVar &var = dynamic_cast<RooRealVar &>( exvarlist[ivar] );
        names[ivar] = var.GetName();
        xmin[ivar] = var.getMin();
        xmax[ivar] = var.getMax();
        nbins[ivar] = var.getBins();
        formulas[ivar] = new TTreeFormula( names[ivar], names[ivar], tree );
        if( ivar < nm1.size() ) {
            nm1[ivar] = ivar;
            ranges_.push_back( std::make_pair( xmin[ivar], xmax[ivar] ) );
            if( ivar >= ( size_t )varlist->getSize() ) {
                selectionCuts_[ivar - varlist->getSize()] = xmin[ivar];
                selectionCutsBegin_[ivar - varlist->getSize()] = xmax[ivar];
            }
        }
    }


    hsparse_ = new THnSparseT<TArrayF>( Form( "hsparse_%s", name.c_str() ),
                                        Form( "hsparse_%s", name.c_str() ),
                                        nvar, &nbins[0], &xmin[0], &xmax[0] );
    hsparse_->Sumw2();
    for( size_t ivar = 0; ivar < nvar; ++ivar ) {
        hsparse_->GetAxis( ivar )->SetTitle( names[ivar] );
        /// FIXME: handle variable binning
    }

    norm_ = 0.;
    std::vector<double> vals( nvar );
    std::vector<TH1 *> histos;
    double eweight = 1.;
    THnSparse *hsparseRed = hsparse_->Projection( nm1.size(), &nm1[0], "A" );
    SparseIntegrator *integN  = new SparseIntegrator( hsparseRed );
    SparseIntegrator *integX  = new SparseIntegrator( hsparseRed );
    SparseIntegrator *integX2 = new SparseIntegrator( hsparseRed );
    for( int ii = 0; ii < tree->GetEntries(); ++ii ) {
        bool skip = false;
        tree->GetEntry( ii );
        for( size_t ivar = 0; ivar < nvar; ++ivar ) {
            vals[ivar] = formulas[ivar]->EvalInstance();
            if( vals[ivar] < xmin[ivar] || vals[ivar] > xmax[ivar] ) {
                skip = true;
                break;
            }
        }
        if( skip ) { continue; }
        if( weight ) { eweight = weight->EvalInstance(); }
        hsparse_->Fill( &vals[0], eweight );
        int ibin = integN->fill( &vals[0], eweight );
        if( ibin >= ( int )histos.size() ) {
            histos.resize( ibin + 1 );
            histos[ibin] = new TH1F( Form( "histo_%d", ibin ), Form( "histo_%d", ibin ), x->getBins(), x->getMin(), x->getMax() );
            histos[ibin]->SetDirectory( 0 );
        }
        histos[ibin]->Fill( vals[nvar - 1], eweight );
        norm_ += eweight;
    }
    integN->scale( 1. / norm_ );

    makeSecondOrder( histos, integN, integX, integX2 );
    delete hsparseRed;

    converterN_  = integN;
    converterX_  = integX;
    converterX2_ = integX2;
    assert( norm_ == hsparse_->GetWeightSum() );
    std::cout << "SecondOrderModelBuilder " << name
              << " normalization: " << norm_
              << " sumW: " << integN->getIntegral( &xmin[0] )
              << " sumWX: " << integX->getIntegral( &xmin[0] )
              << " sumWX2: " << integX2->getIntegral( &xmin[0] )
              << " pdf normalization: " << integN->getIntegral( &xmin[0] )
              << " number of (non-empty) bins: " << integN->size()
              << std::endl;

}

// ------------------------------------------------------------------------------------------------
TTree *SecondOrderModelBuilder::getTree()
{
    if( hsparse_ == 0 ) { return 0; }
    return toTree( ( ( SparseIntegrator * )converterN_ ) ->getIntegrand(),
                   ( ( SparseIntegrator * )converterX_ ) ->getIntegrand(),
                   ( ( SparseIntegrator * )converterX2_ )->getIntegrand() );
}

// ------------------------------------------------------------------------------------------------
SecondOrderModel::SecondOrderModel( std::string name,
                                    RooRealVar *x, AbsModel::type_t type, RooRealVar *mu, shape_t shape ) :
    name_( name ),
    x_( x ), mu_( mu ),
    shape_( shape ),
    likeg_( 0 ), minEvents_( 0. )
{
    type_ = type;
    setShape( shape );
};

// ------------------------------------------------------------------------------------------------
void SecondOrderModel::setShape( int x )
{
    if( likeg_ != 0 ) { delete likeg_; }
    shape_ = (shape_t)x;
    if( shape_ == automatic ) {
        shape_ = ( type_ == AbsModel::sig ? gaus : expo );
    }
    if( shape_ == expo ) {
        likeg_ = new TF1( Form( "likeg_%s", name_.c_str() ), "[0]-1./x+[1]*exp(-[1]*x)/(1.-exp(-[1]*x))", 0., 100. );
    }
}

// ------------------------------------------------------------------------------------------------
SecondOrderModel::~SecondOrderModel()
{
    if( likeg_ ) { delete likeg_; }
}

// ------------------------------------------------------------------------------------------------
TH1 *SecondOrderModelBuilder::getPdf( int idim )
{
    if( hsparse_ != 0 ) {
        TH1 *h = hsparse_->Projection( idim, "A" );
        h->Scale( norm_ / h->Integral() );
        return h;
    }
    if( ranges_.size() == 1 ) {
        return ( TH1 * )pdf_->Clone();
    } else if( ranges_.size() == 2 ) {
        return ( idim == 0 ? ( ( TH2 * )pdf_ )->ProjectionX() : ( ( TH2 * )pdf_ )->ProjectionY() );
    } else if( ranges_.size() == 3 ) {
        return ( idim == 0 ? ( ( TH3 * )pdf_ )->ProjectionX() :
                 ( idim == 1 ? ( ( TH3 * )pdf_ )->ProjectionY() : ( ( TH3 * )pdf_ )->ProjectionZ() )
               );
    }
    assert( 0 );
}

// ------------------------------------------------------------------------------------------------
RooAbsPdf *SecondOrderModel::getCategoryPdf( int icat )
{
    return categoryPdfs_[icat];
}

// ------------------------------------------------------------------------------------------------
void SecondOrderModel::buildPdfs()
{
    std::vector<int> catToFix;
    double smallestYield = 1e+30;
    double largestSigma = 0.;

    for( size_t icat = 0; icat < categoryYields_.size(); ++icat ) {
        /// if( minEvents_> 0 && categoryYields_[icat] < minEvents_ ) { categoryYields_[icat] = minEvents_; }
        if( icat >= categoryPdfs_.size() ) {
            bookShape( icat );
        } else {
            /// RooAbsPdf * pdf = categoryPdfs_[icat];
            setShapeParams( icat );
        }
        if( categoryYields_[icat] <= minEvents_ || ! isfinite( categoryYields_[icat] )
                || ( ( shape_ == gaus )
                     && ( categoryRMSs_[icat] == 0 || ! isfinite( categoryRMSs_[icat] ) || ! isfinite( categoryMeans_[icat] ) ) )
          ) {
            catToFix.push_back( icat );
        } else {
            smallestYield = std::min( smallestYield, categoryYields_[icat] );
            largestSigma = std::max( largestSigma, categoryRMSs_[icat] );
        }
    }

    if( categoryYields_.size() > 1 ) {
        for( size_t ifix = 0; ifix < catToFix.size(); ++ifix ) {
            int icat = catToFix[ifix];
            categoryYields_[icat] = smallestYield / 10.;
            categoryRMSs_[icat] = largestSigma * 2.;
            setShapeParams( icat );
        }
    } else {
        for( size_t ifix = 0; ifix < catToFix.size(); ++ifix ) {
            int icat = catToFix[ifix];
            categoryYields_[icat] = 1.e-6;
            categoryRMSs_[icat] = 100.;
            categoryMeans_[icat] = 0.;
            setShapeParams( icat );
        }
    }
    //// dump();
}

// ------------------------------------------------------------------------------------------------
void SecondOrderModel::bookShape( int icat )
{
    assert( icat == ( int )categoryPdfs_.size() );
    RooRealVar *norm = new RooRealVar( Form( "%s_cat_model_norm0_%d", name_.c_str(), icat ),
                                       Form( "%s_cat_model_norm0_%d", name_.c_str(), icat ), categoryYields_[icat],
                                       0., 1e+6 );
    owned_.addOwned( *norm );

    RooAbsPdf *pdf = 0;
    if( shape_ == gaus ) {
        RooRealVar *mean = new RooRealVar( Form( "%s_cat_model_gaus_mean_%d", name_.c_str(), icat ),
                                           Form( "%s_cat_model_gaus_mean_%d", name_.c_str(), icat ),
                                           categoryMeans_[icat] );
        mean->setConstant( true );
        RooRealVar *sigma = new RooRealVar( Form( "%s_cat_model_gaus_sigma_%d", name_.c_str(), icat ),
                                            Form( "%s_cat_model_gaus_sigma_%d", name_.c_str(), icat ),
                                            categoryRMSs_[icat] );
        sigma->setConstant( true );
        pdf = new RooGaussian( Form( "%s_cat_model_gaus_%d", name_.c_str(), icat ), Form( "%s_cat_model_gaus_%d", name_.c_str(), icat ),
                               *x_, *mean, *sigma );

        owned_.addOwned( * mean ), owned_.addOwned( * sigma );
    } else if( shape_ == expo ) {
        likeg_->SetParameters( categoryMeans_[icat] - x_->getMin(), x_->getMax() - x_->getMin() );
        double lambdaval = -likeg_->GetX( 0., 1. / likeg_->GetParameter( 0 ), 0.1 / likeg_->GetParameter( 0 ) );
        RooRealVar *lambda = new RooRealVar( Form( "%s_cat_model_expo_lambda_%d", name_.c_str(), icat ),
                                             Form( "%s_cat_model_expo_lambda_%d", name_.c_str(), icat ),
                                             lambdaval, -1e+30, 0. ); /// -1./categoryMeans_[icat]
        if( type_ == AbsModel::bkg ) {
            lambda->setConstant( false );
            /// lambda->setConstant(true);
        } else {
            lambda->setConstant( true );
        }
        pdf = new RooExponential( Form( "%s_cat_model_expo_%d", name_.c_str(), icat ), Form( "%s_cat_model_expo_%d", name_.c_str(), icat ),
                                  *x_, *lambda );

        owned_.addOwned( * lambda );
    }

    RooExtendPdf *expdf = 0;
    if( mu_ != 0 ) {
        RooProduct *renorm = new RooProduct( Form( "%s_cat_model_yield_%d", name_.c_str(), icat ),
                                             Form( "%s_cat_model_yield_%d", name_.c_str(), icat ),
                                             RooArgList( *norm, *mu_ ) );
        expdf = new RooExtendPdf( Form( "%s_cat_model_ext_pdf_%d", name_.c_str(), icat ),
                                  Form( "%s_cat_model_ext_pdf_%d", name_.c_str(), icat ),
                                  *pdf, *renorm );
        norm->setConstant( true );
        owned_.addOwned( *renorm );
    } else {
        norm->setConstant( false );
        expdf = new RooExtendPdf( Form( "%s_cat_model_ext_pdf_%d", name_.c_str(), icat ),
                                  Form( "%s_cat_model_ext_pdf_%d", name_.c_str(), icat ),
                                  *pdf, *norm );
    }
    categoryNorms_.push_back( norm );
    categoryPdfs_.push_back( expdf );
    owned_.addOwned( *pdf );
    owned_.addOwned( *expdf );
}

// ------------------------------------------------------------------------------------------------
void SecondOrderModel::setShapeParams( int icat )
{
    categoryNorms_[icat]->setVal( categoryYields_[icat] );
    RooArgSet *params = categoryPdfs_[icat]->getParameters( ( RooArgSet * )0 );
    if( shape_ == gaus ) {
        params->setRealValue( Form( "%s_cat_model_gaus_mean_%d", name_.c_str(), icat ), categoryMeans_[icat] );
        params->setRealValue( Form( "%s_cat_model_gaus_sigma_%d", name_.c_str(), icat ), categoryRMSs_[icat] );
    } else if( shape_ == expo ) {
        likeg_->SetParameters( categoryMeans_[icat] - x_->getMin(), x_->getMax() - x_->getMin() );
        double lambdaval = -likeg_->GetX( 0., 1. / likeg_->GetParameter( 0 ), 0.1 / likeg_->GetParameter( 0 ) );
        params->setRealValue( Form( "%s_cat_model_expo_lambda_%d", name_.c_str(), icat ), lambdaval );
    }
}


// ------------------------------------------------------------------------------------------------
void SecondOrderModel::dump()
{
    for( size_t ii = 0; ii < categoryYields_.size(); ++ii ) {
        std::cout << ii << " " << categoryYields_[ii] << " " << categoryMeans_[ii]
                  << " " << categoryRMSs_[ii] << std::endl;
        categoryPdfs_[ii]->Print();
        categoryPdfs_[ii]->getParameters( ( RooArgSet * )0 )->Print( "v" );
    }
}

// ------------------------------------------------------------------------------------------------
void throwAsimov( double nexp, RooDataHist *asimov, RooAbsPdf *pdf, RooRealVar *x )
{
    asimov->reset();
    RooArgSet mset( *x );
    pdf->fillDataHist( asimov, &mset, 1, false );

    for( int i = 0 ; i < asimov->numEntries() ; i++ ) {
        asimov->get( i ) ;

        // Expected data, multiply p.d.f by nEvents
        Double_t w = asimov->weight() * nexp;
        asimov->set( w, sqrt( w ) );
    }

    Double_t corr = nexp / asimov->sumEntries() ;
    for( int i = 0 ; i < asimov->numEntries() ; i++ ) {
        RooArgSet theSet = *( asimov->get( i ) );
        asimov->set( asimov->weight()*corr, sqrt( asimov->weight()*corr ) );
    }

}

// ------------------------------------------------------------------------------------------------
void appendData( RooDataHist &dest, RooDataHist &src )
{
    for( int i = 0 ; i < src.numEntries() ; i++ ) {
        RooArgSet theSet = *( src.get( i ) );
        dest.add( theSet, src.weight(), src.weightError() );
    }
}

// ------------------------------------------------------------------------------------------------
double SimpleShapeFomProvider::operator()( std::vector<AbsModel *> sig, std::vector<AbsModel *> bkg ) const
{

    /// std::cout << "SimpleShapeFomProvider::operator() useRooSimultaneous_=" << useRooSimultaneous_ << std::endl;

    float ret;

    assert( sig.size() % nSubcats_ == 0 );
    assert( bkg.size() % nSubcats_ == 0 );

    static std::vector<RooDataHist> asimovs;
    static std::vector<RooAddPdf> pdfs;

    for( size_t ipoi = 0; ipoi < pois_.size(); ++ipoi ) {
        pois_[ipoi]->setVal( 1. );
    }
    for( size_t ireset = 0; ireset < resets_.size(); ++ireset ) {
        resets_[ireset]->setVal( 1. );
    }

    /// double fom = 0.;
    cout << "AAAAAAAAAAAAA "<< sig.size() << endl;
    cout << "AAAAAAAAAAAAA "<< sig[0] << endl;
    size_t ncat = sig[0]->getNcat();
    size_t totcat = ncat * nSubcats_;

    RooCategory roocat( "SimpleShapeFomCat", "" );
    std::vector<std::pair<std::string, RooAbsData *> >catData;
    std::vector<RooRealVar *> normsToFix;

    //// bool CheckEndcap = false;
    //// bool* dropTheEndcap = new bool[ncat];
    //// for (size_t i=0; i<ncat; i++) dropTheEndcap[i] = false;

    if( debug_ ) {
        std::cout << "\n---------------------------------------------" << std::endl;
        std::cout << "Num. of categories: " << ncat << " num. of subcategories: " << nSubcats_ << std::endl;
    }
    /// WARNING: assuming that signal and background pdfs objects don't change for a given category
    for( size_t icat = 0; icat < ncat; ++icat ) {
        if( debug_ ) {
            std::cout << "\nCategory: " << icat << " event yields\n";
        }
        if( nSubcats_ * icat >= asimovs.size() ) {
            for( size_t iSubcat = 0; iSubcat < nSubcats_; iSubcat++ ) {
                asimovs.push_back( RooDataHist( Form( "asimovhist_%d_%d", ( int )icat, ( int )iSubcat ), "", *( sig[0]->getX() ) ) );
            }
        }

        bool buildPdf = ( nSubcats_ * icat >= pdfs.size() );
        std::vector<RooArgList> lpdfs( nSubcats_ ), bpdfs( nSubcats_ );

        std::vector<double> ntot( nSubcats_, 0. );
        //// std::cout << "cat " << icat << std::endl;
        for( size_t iSig = 0; iSig < sig.size(); iSig++ ) {
            size_t iSubcat = iSig % nSubcats_;
            ntot[iSubcat] += sig[iSig]->getCategoryYield( icat );

            //If very low stat in the EE, do not include in the lilelihood
            //// if (CheckEndcap){
            ////   if ((sig[iSig]->name()=="sigWv_EEHighR9Model" || sig[iSig]->name()=="sigWv_EELowR9Model" || sig[iSig]->name()=="sigRv_EEHighR9Model" || sig[iSig]->name()=="sigRv_EELowR9Model") && (sig[iSig]->getCategoryYield(icat)<0.05)){
            ////     dropTheEndcap[icat] = true;
            ////   }
            /// }

            if( sig[iSig]->getCategoryPdf( icat )->expectedEvents( 0 ) <= 0 ) {
                return 1e+5;
            }
            if( debug_ ) {
                std::cout << "    " << sig[iSig]->name() << " (subcat " << iSubcat << "): " << sig[iSig]->getCategoryYield( icat ) << "\n" ;
            }
            /// std::cout << sig[iSig]->getCategoryPdf(icat)->expectedEvents(0)
            /// 	  << " " << sig[iSig]->getCategoryYield(icat) << std::endl;
            if( buildPdf ) {
                lpdfs[iSubcat].add( *( sig[iSig]->getCategoryPdf( icat ) ) );
            }
        }
        for( size_t iBkg = 0; iBkg < bkg.size(); iBkg++ ) {
            size_t iSubcat = iBkg % nSubcats_;
            ntot[iSubcat] += bkg[iBkg]->getCategoryYield( icat );
            if( debug_ ) {
                std::cout << "    " << bkg[iBkg]->name() << " (subcat " << iSubcat << "): " << bkg[iBkg]->getCategoryYield( icat ) << "\n";
            }
            /// std::cout << bkg[iBkg]->getCategoryPdf(icat)->expectedEvents(0)
            /// 	  << " " << bkg[iBkg]->getCategoryYield(icat) << std::endl;
            if( buildPdf ) {
                lpdfs[iSubcat].add( *( bkg[iBkg]->getCategoryPdf( icat ) ) );
            }
        }
        if( debug_ ) {
            std::cout << std::endl;
        }

        if( buildPdf ) {
            for( size_t iSubcat = 0; iSubcat < nSubcats_; iSubcat++ ) {
                pdfs.push_back( RooAddPdf( Form( "sbpdf_%d_%d", ( int )icat, ( int )iSubcat ), "", lpdfs[iSubcat] ) );
            }
        }

        for( size_t iSubcat = 0; iSubcat < nSubcats_; iSubcat++ ) {            
            //// std::cout << "Throwing Asymov " << iSubcat << " " << nSubcats_*icat + iSubcat << " " << ntot[iSubcat] << " " <<  &asimovs[nSubcats_ * icat + iSubcat] << " " <<
            ////           &pdfs[nSubcats_ * icat + iSubcat] << std::endl;

            throwAsimov( ntot[iSubcat], &asimovs[nSubcats_ * icat + iSubcat], &pdfs[nSubcats_ * icat + iSubcat], sig[0]->getX() );
            roocat.defineType( Form( "cat_%d_%d", ( int )icat, ( int )iSubcat ) );
            //// roosim.addPdf( pdfs[nSubCats_*icat+iSubcat], Form("cat_%d_%d",icat,iSubcat) );

            catData.push_back( std::make_pair( Form( "cat_%d_%d", ( int )icat, ( int )iSubcat ), &asimovs[nSubcats_ * icat + iSubcat] ) );
        }
    }

    RooAbsReal *nll;
    RooAbsReal **nlli = new RooAbsReal*[ncat];
    std::vector<RooAbsReal *> garbageColl;
    std::vector<RooAbsData *> garbageData;

    if( useRooSimultaneous_ ) {

        RooSimultaneous *roosim = new RooSimultaneous( "SimpleShapeFomFit", "", roocat );
        garbageColl.push_back( roosim );
        //// RooRealVar * weight = new RooRealVar("weight","weight",1.);
        //// garbageColl.push_back(weight);
        /// RooDataSet * combData = new RooDataSet("combData","combData",RooArgList(*(sig[0]->getX()),roocat,*weight),"weight");
        RooDataHist *combData = new RooDataHist( "combData", "combData", RooArgList( *( sig[0]->getX() ), roocat ) );
        garbageData.push_back( combData );
        for( size_t icat = 0; icat < catData.size(); ++icat ) {
            roocat.setLabel( catData[icat].first.c_str() );
            appendData( *combData, *( ( RooDataHist * )catData[icat].second ) );
            roosim->addPdf( pdfs[icat], catData[icat].first.c_str() );
            nll = roosim->createNLL( *combData, RooFit::Extended(), RooFit::NumCPU( ncpu_ ) );
            garbageColl.push_back( nll );
        }

        nll = ( constraints_.getSize() > 0 ?
                roosim->createNLL( *combData, RooFit::Extended(), RooFit::NumCPU( ncpu_ ),
                                   RooFit::ExternalConstraints( constraints_ ) ) :
                roosim->createNLL( *combData, RooFit::Extended(), RooFit::NumCPU( ncpu_ ) )
              );
        garbageColl.push_back( nll );

    } else {
        if( !doDeltaMuBinOptim_ ) {
            RooArgSet nlls;
            for( size_t icat = 0; icat < totcat; ++icat ) {
                //// RooAbsReal *inll = pdfs[icat].createNLL( asimovs[icat], RooFit::Extended() );
                RooAbsReal *inll = ( constraints_.getSize() > 0 ?
                                     pdfs[icat].createNLL( asimovs[icat], RooFit::Extended(),
                                             RooFit::ExternalConstraints( constraints_ ) ) :
                                     pdfs[icat].createNLL( asimovs[icat], RooFit::Extended() )
                                   );
                nlls.add( *inll );
                garbageColl.push_back( inll );
            }
            //// if( constraints_.getSize() > 0 ) {
            //// 	RooAbsReal* nllCons = new RooConstraintSum("constr","constr",constraints_,constrained_);
            //// 	nlls.add(*nllCons);
            //// 	garbageColl.push_back(nllCons);
            //// 	//// constraints_.Print("V");
            //// 	//// constrained_.Print("V");
            //// }
            nll = new RooAddition( "nll", "nll", nlls );
            //// nll->Print("V");
            garbageColl.push_back( nll );
        } else if( doDeltaMuBinOptim_ ) {

            RooArgSet *nlls = new RooArgSet[ncat];
            for( size_t ibin = 0; ibin < ncat; ibin++ ) {
                /// std::cout << "Bin "<<ibin << " dropTheEndcap[icat]="<<dropTheEndcap[ibin] << std::endl;
                for( size_t isubcat = 0; isubcat < nSubcats_ ; isubcat++ ) {
                    //size_t iSubcat = iCat % nSubcats_;
                    size_t icat = ibin * nSubcats_ + isubcat;
                    /// if ( !(dropTheEndcap[ibin]==true && (isubcat==2 || isubcat==3))) {
                    std::cout << "Include icat=" << icat << " isubcat=" << isubcat << std::endl;
                    RooAbsReal *inll = ( constraints_.getSize() > 0 ?
                                         pdfs[icat].createNLL( asimovs[icat], RooFit::Extended(),
                                                 RooFit::ExternalConstraints( constraints_ ) ) :
                                         pdfs[icat].createNLL( asimovs[icat], RooFit::Extended() )
                                       );
                    nlls[ibin].add( *inll );
                    garbageColl.push_back( inll );
                    // }
                }
                nlli[ibin] = new RooAddition( "nll", "nll", nlls[ibin] );
                //else (doDeltaMuBinOptim) nll  = new RooAddition("nll","nll",nlls[ibin]);
            }
        }
    }

    /// std::cout << "poi number " << pois_.size() << endl;

    double Mes = 0;
    bool *sconverged = new bool[ncat];
    bool *s08converged = new bool[ncat];
    bool *s12converged = new bool[ncat];
    bool *bconverged = new bool[ncat];
    bool bconvergedSignif = false;
    bool sconvergedSignif = false;

    if( doDeltaMuBinOptim_ ) {
        //Minimization for each bin
        double *MuError = new double[ncat];

        for( size_t ibin = 0; ibin < ncat; ibin++ ) {

            /// std::cout << "Bin " << ibin << std::endl;

            // S+B fit
            for( size_t ipoi = 0; ipoi < pois_.size(); ++ipoi ) {
                pois_[ipoi]->setVal( 1. );
                pois_[ipoi]->setConstant( false );
            }
            RooMinimizer minimsb( *( nlli[ibin] ) );
            minimsb.setMinimizerType( minimizer_.c_str() );
            minimsb.setPrintLevel( -1 ); //-1
            sconverged[ibin] = false;
            for( int ii = minStrategy_; ii < 3; ++ii ) {
                minimsb.setStrategy( ii );
                if( ! minimsb.migrad() ) {
                    sconverged[ibin] = true;
                    break;
                }
            }
            double minNllsb = nlli[ibin]->getVal();
            /// std::cout << "mu=1 minNll=" << minNllsb << " poi0=" << pois_[0]->getVal() << " sconverged=" << sconverged[ibin] << std::endl;

            // 0.8*S+B fit
            for( size_t ipoi = 0; ipoi < pois_.size(); ++ipoi ) {
                pois_[ipoi]->setVal( 0.8 );
                pois_[ipoi]->setConstant( true );
            }
            RooMinimizer minimsb08( *( nlli[ibin] ) );
            minimsb08.setMinimizerType( minimizer_.c_str() );
            minimsb08.setPrintLevel( -1 );
            s08converged[ibin] = false;
            for( int ii = minStrategy_; ii < 3; ++ii ) {
                minimsb08.setStrategy( ii );
                if( ! minimsb08.migrad() ) {
                    s08converged[ibin] = true;
                    break;
                }
            }
            double minNllsb08 = nlli[ibin]->getVal();
            /// std::cout << "mu=0.8 minNll=" << minNllsb08 << " poi0=" << pois_[0]->getVal() << " s08converged=" << s08converged[ibin] << std::endl;

            // 1.2*S+B fit
            for( size_t ipoi = 0; ipoi < pois_.size(); ++ipoi ) {
                pois_[ipoi]->setVal( 1.2 );
                pois_[ipoi]->setConstant( true );
            }
            RooMinimizer minimsb12( *( nlli[ibin] ) );
            minimsb12.setMinimizerType( minimizer_.c_str() );
            minimsb12.setPrintLevel( -1 );
            s12converged[ibin] = false;
            for( int ii = minStrategy_; ii < 3; ++ii ) {
                minimsb12.setStrategy( ii );
                if( ! minimsb12.migrad() ) {
                    s12converged[ibin] = true;
                    break;
                }
            }
            double minNllsb12 = nlli[ibin]->getVal();
            /// std::cout << "mu=1.2 minNll=" << minNllsb12 << " poi0=" << pois_[0]->getVal() << " s12converged=" << s12converged[ibin] << std::endl;

            // B-only fit
            for( size_t ipoi = 0; ipoi < pois_.size(); ++ipoi ) {
                pois_[ipoi]->setVal( 0. );
                pois_[ipoi]->setConstant( true );
            }
            RooMinimizer minimb( *( nlli[ibin] ) );
            minimb.setMinimizerType( minimizer_.c_str() );
            minimb.setPrintLevel( -1 ); //-1
            bconverged[ibin] = false;
            for( int ii = minStrategy_; ii < 3; ++ii ) {
                minimb.setStrategy( ii );
                if( ! minimb.migrad() ) {
                    bconverged[ibin] = true;
                    break;
                }
            }
            double minNllb = nlli[ibin]->getVal();
            /// std::cout << "mu=0 minNll=" << minNllb << " poi0=" << pois_[0]->getVal() << " bconverged=" << bconverged[ibin] << std::endl;


            double qA = -2.*( minNllb - minNllsb );
            // std::cout << "qA=" << qA << std::endl;

            double qA08 = -2.*( minNllb - minNllsb08 );
            // std::cout << "qA08=" << qA08 << std::endl;

            double qA12 = -2.*( minNllb - minNllsb12 );
            // std::cout << "qA12=" << qA12 << std::endl;

            // std::cout << "Parabola computation with matrix inversion" << std::endl;

            TMatrixD nllval( 3, 1 );
            nllval[0][0] = qA08;
            nllval[1][0] = qA;
            nllval[2][0] = qA12;

            TMatrixD matrix( 3, 3 );
            matrix[0][0] = 0.8 * 0.8;
            matrix[0][1] = 0.8;
            matrix[0][2] = 1;
            matrix[1][0] = 1;
            matrix[1][1] = 1;
            matrix[1][2] = 1;
            matrix[2][0] = 1.2 * 1.2;
            matrix[2][1] = 1.2;
            matrix[2][2] = 1;
            matrix.Invert();

            TMatrixD Result( 3, 1 );
            Result = matrix * nllval;
            /// std::cout << "a=" << Result[0][0] << " b=" << Result[1][0] << " c=" << Result[2][0] << std::endl;

            double val_old = 100;
            double x1 = 0, x2 = 0;
            for( int i = 0; i < 1000; i++ ) {
                double x = -5 + ( ( double )i ) * 0.01;
                //double val = func.Eval(x);
                double val = Result[0][0] * x * x + Result[1][0] * x + Result[2][0];
                //std::cout << "Matrix mu="<<x<<" val="<<Result[0][0]*x*x+Result[1][0]*x+Result[2][0]<<std::endl;
                //std::cout << "Fit    mu="<<x<<" val="<<val<<std::endl;
                if( val_old > qA + 1 && val < qA + 1 ) {
                    x1 = x;
                    /// std::cout << "x1=" << x << std::endl;
                }
                if( val_old < qA + 1 && val > qA + 1 ) {
                    x2 = x;
                    /// std::cout << "x2=" << x << std::endl;
                }
                val_old = val;
            }
            double DeltaMu = ( x2 - x1 ) / 2.;
            /// std::cout << "Bin " << ibin << " MuError=" << DeltaMu << std::endl;
            MuError[ibin] = DeltaMu;
        }

        //Compute the measure
        /// double Sum = 0;
        // std::cout << "This iteration summary:" << std::endl;
        for( size_t i = 0; i < ncat; i++ ) {
            /// std::cout << "Bin " << i << " MuError=" << MuError[i] << std::endl;
            for( size_t j = 0; j < i; j++ ) {
                //if (abs(MuError[i] - MuError[j])>Mes) Mes = abs(MuError[i] - MuError[j]);
                Mes += ( MuError[i] - MuError[j] ) * ( MuError[i] - MuError[j] );
            }
            //Sum += MuError[i]*MuError[i];
        }
        //Mes /= Sum;
        //Mes *= 1000.;
        Mes = -1. / Mes;
        /// std::cout << "Mesure = " << Mes << std::endl;

        ret = Mes;
    }

    if( !doDeltaMuBinOptim_ ) {


        // S+B fit
        for( size_t ipoi = 0; ipoi < pois_.size(); ++ipoi ) {
            pois_[ipoi]->setVal( 1. );
            pois_[ipoi]->setConstant( false );
        }
        RooMinimizer minimsb( *nll );
        minimsb.setMinimizerType( minimizer_.c_str() );
        minimsb.setPrintLevel( -1 ); //-1
        for( int ii = minStrategy_; ii < 3; ++ii ) {
            minimsb.setStrategy( ii );
            if( ! minimsb.migrad() ) {
                sconvergedSignif = true;
                break;
            }
        }
        double minNllsb = nll->getVal();
        /// std::cout << "mu=1 minNll=" << minNllsb << " poi0=" << pois_[0]->getVal() << " sconverged=" << sconvergedSignif << std::endl;


        std::vector<RooPlot *> frames;
        if( debug_ ) {
            for( size_t icat = 0; icat < ncat; ++icat ) {
                for( size_t iSubcat = 0; iSubcat < nSubcats_; iSubcat++ ) {
                    RooPlot *frame = sig[0]->getX()->frame( RooFit::Title( Form( "Category %d/%d (subcat %d)", ( int )icat, ( int )ncat, ( int )iSubcat ) ) );
                    /// combData.plotOn(frame,RooFit::Cut(Form("SimpleShapeFomCat==SimpleShapeFomCat::cat_%d",icat)));
                    asimovs[nSubcats_ * icat + iSubcat].plotOn( frame );
                    // roosim.plotOn(frame,RooFit::Slice(roocat,Form("cat_%d",icat)),RooFit::ProjWData(roocat,combData));
                    pdfs[nSubcats_ * icat + iSubcat].plotOn( frame );
                    frames.push_back( frame );
                }
            }
        }

        // B-only fit
        for( size_t ipoi = 0; ipoi < pois_.size(); ++ipoi ) {
            pois_[ipoi]->setVal( 0. );
            pois_[ipoi]->setConstant( true );
        }
        RooMinimizer minimb( *nll );
        minimb.setMinimizerType( minimizer_.c_str() );
        minimb.setPrintLevel( -1 ); //-1
        for( int ii = minStrategy_; ii < 3; ++ii ) {
            minimb.setStrategy( ii );
            if( ! minimb.migrad() ) {
                bconvergedSignif = true;
                break;
            }
        }
        double minNllb = nll->getVal();
        /// std::cout << "mu=0 minNll=" << minNllb << " poi0=" << pois_[0]->getVal() << " bconvergedSignif=" << bconvergedSignif << std::endl;


        double qA = -2.*( minNllb - minNllsb );
        //// std::cout << "qA=" << qA << std::endl;
        //// std::cout << "S = " << -sqrt( -qA ) << std::endl;
        Mes = qA;



        for( size_t ii = 0; ii < garbageColl.size(); ++ii ) {
            delete garbageColl[ii];
        }
        for( size_t ii = 0; ii < garbageData.size(); ++ii ) {
            delete garbageData[ii];
        }




        if( debug_ ) {
            for( size_t icat = 0; icat < ncat; ++icat ) {
                for( size_t iSubcat = 0; iSubcat < nSubcats_; iSubcat++ ) {
                    RooPlot *frame = frames[nSubcats_ * icat + iSubcat];
                    pdfs[nSubcats_ * icat + iSubcat].plotOn( frame, RooFit::LineColor( kRed ) );
                    TCanvas *canvas = new TCanvas( Form( "cat_%d_%d_%d", ( int )ncat, ( int )icat, ( int )iSubcat ), Form( "cat_%d_%d_%d", ( int )ncat, ( int )icat,
                                                   ( int )iSubcat ) );
                    canvas->cd();
                    frame->Draw();
                    if( nSubcats_ > 0 ) {
                        canvas->SaveAs( Form( "cat_%d_%d_%d.png", ( int )ncat, ( int )icat, ( int )iSubcat ) );
                    } else {
                        canvas->SaveAs( Form( "cat_%d_%d.png", ( int )ncat, ( int )icat ) );
                    }
                }
            }
        }


        ret = -sqrt( -qA );
    }


    /// std::cout << "ret=" << ret << std::endl;


    bool Sconverged = 1;
    bool Sconverged08 = 1;
    bool Sconverged12 = 1;
    bool Bconverged = 1 ;
    for( size_t i = 0; i < ncat; i++ ) {
        Sconverged *= sconverged[i];
        Sconverged08 *= s08converged[i];
        Sconverged12 *= s12converged[i];
        Bconverged *= bconverged[i];
    }

    if( doDeltaMuBinOptim_ ) {
        if( ! isfinite( ret ) || ! Sconverged || ! Sconverged08 || ! Sconverged12 || ! Bconverged ) { ret = 0.; }
    } else if( !doDeltaMuBinOptim_ ) {
        if( ! isfinite( ret ) || ! sconvergedSignif || ! bconvergedSignif ) { ret = 0.; }
    }

    return ret;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

