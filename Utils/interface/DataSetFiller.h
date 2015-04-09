#ifndef _DataSetFiller_h_
#define _DataSetFiller_h_

#include "RooDataSet.h"
#include "RooArgList.h"

#include "TTree.h"

class DataSetFiller 
{
public:
    DataSetFiller(const char * name, const char * title, const RooArgList & variables, const char *weightVarName=0);
    
    void fillFromTree(TTree * tree, const char * weightExpr=0);
    RooArgList & vars() { return vars_; };
    
    RooDataSet * get() { return dataset_; }
    
private:
    RooArgList vars_;
    RooDataSet * dataset_;


};



#endif // _DataSetFiller_h_

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

