#ifndef _DataSetFiller_h_
#define _DataSetFiller_h_

#include "RooDataSet.h"
#include "RooArgList.h"

#include "TTree.h"
#include "TNtuple.h"

class DataSetFiller 
{
public:
    DataSetFiller(const char * name, const char * title, const RooArgList & variables, const char *weightVarName=0, bool fillTree=false);
    DataSetFiller(RooAbsData * dset);
    
    void fillFromTree(TTree * tree, const char * weightExpr=0, bool ignoreExpr=false );
    RooArgList & vars() { return vars_; };
    
    RooAbsData * get() { return dataset_; }
    TTree * getTree() { return tree_; }

    static RooDataHist * throwAsimov( double nexp, RooAbsPdf *pdf, RooRealVar *x, RooDataHist *asimov=0);
    
    
private:
    RooArgList vars_;
    RooAbsData * dataset_;
    TNtuple * tree_;
    std::vector<float> treeBuf_;

};

#include <list>
typedef std::list<RooAbsData *> RooDataSetList;

#endif // _DataSetFiller_h_

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

