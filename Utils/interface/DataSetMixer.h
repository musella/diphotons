#ifndef _DataSetMixer_h_
#define _DataSetMixer_h_

#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooArgList.h"

#include "TTree.h"
#include "TNtuple.h"

#include <string>

class DataSetMixer 
{
public:
    DataSetMixer(const char * name, const char * title, 
                 const RooArgList & variables1, const RooArgList & variables2,                  
                 const RooArgList & variablesT,
                 const char * replace1, const char * replace2, 
                 float ptLeadMin, float ptSubleadMin, float massMin,
                 const char *weightVarName1=0, const char *weightVarName2=0, bool fillTree=false);
    
    void fillFromTree(TTree * tree1, TTree * tree2,
                      const char *pT1, const char *eta1, const char *phi1, const char *energy1, 
                      const char *pT2, const char *eta2, const char *phi2, const char *energy2,
                      const RooArgList & matchVars, bool rndSwap, int maxEvents=-1, float matchEff=1.
                      );

    void fillLikeTarget(TTree * target,
                        const RooArgList & targetVars1, const RooArgList & targetVars2,  
                        std::string  targetWeight,
                        TTree * tree1, TTree * tree2,
                        const char *pT1, const char *eta1, const char *phi1, const char *energy1, 
                        const char *pT2, const char *eta2, const char *phi2, const char *energy2,
                        const RooArgList & matchVars1, const RooArgList & matchVars2,
                        bool rndSwap,float rndMatch, int nNeigh=10, int nMinNeigh=10, float targetFraction=0.,
                        bool useCdfDistance=false, bool matchWithThreshold=false,
                        float maxWeightTarget=0.,
                        float maxWeightCache=0.,
                        Double_t * axesWeights=0
        );
    
    RooArgList & vars() { return vars_; };
    
    RooDataSet * get() { return dataset_; }
    TTree * getTree() { return tree_; }
    
private:
    float ptLeadMin_, ptSubleadMin_, massMin_;
    std::string weight1_,weight2_;
    RooArgList vars_, vars1_, vars2_, varsT_;
    RooDataSet * dataset_;
    TNtuple * tree_;
    std::vector<float> treeBuf_;

};

#endif // _DataSetMixer_h_

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

