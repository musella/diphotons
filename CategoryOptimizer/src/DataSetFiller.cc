#include "../interface/DataSetFiller.h"
#include "RooRealVar.h"

#include "TTreeFormula.h"


DataSetFiller::DataSetFiller(const char * name, const char * title, const RooArgList & variables, const char *weightVarName) :
    vars_(variables),
    dataset_(new RooDataSet(name,title,RooArgSet(variables),weightVarName))
{
    

}

void DataSetFiller::fillFromTree(TTree * tree, const char * weightExpr)
{
    size_t nvar = vars_.getSize();
    std::vector<TTreeFormula *> formulas(nvar);
    for(size_t ivar=0; ivar<nvar; ++ivar){
        RooRealVar &var = dynamic_cast<RooRealVar &>( vars_[ivar] );
        formulas[ivar] = new TTreeFormula( var.GetName(), var.GetTitle(), tree );
    }
    
    TTreeFormula * weight = (weightExpr != 0 ? new TTreeFormula("weight",weightExpr,tree) : 0);

    for(int iev=0; iev<tree->GetEntries(); ++iev) {
        tree->GetEntry(iev);
        float wei = 1.;
        if( weight ) {
            wei = weight->EvalInstance();
        }
        if( wei == 0. ) { continue; }
        bool keep = true;
        for(size_t ivar=0; ivar<nvar; ++ivar){
            double val = formulas[ivar]->EvalInstance();
            RooRealVar & var = dynamic_cast<RooRealVar &>( vars_[ivar] );
            if( (var.hasMin() && val < var.getMin()) || (var.hasMax() && val > var.getMax()) ) { keep = false; break; }
            var.setVal( val  );
        }
        if( keep ) {
            dataset_->add( RooArgSet(vars_), wei );
        }
    }
    
    for(size_t ivar=0; ivar<nvar; ++ivar){
        delete formulas[ivar];
    }
    if( weight ) { delete weight; }
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

