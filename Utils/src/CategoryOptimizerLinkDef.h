#include "diphotons/Utils/interface/CategoryOptimizer.h"
#include "diphotons/Utils/interface/FunctionHelpers.h"
#include "diphotons/Utils/interface/NaiveCategoryOptimization.h"
#include "diphotons/Utils/interface/SimpleShapeCategoryOptimization.h"
#include "diphotons/Utils/interface/DataSetFiller.h"
#include "diphotons/Utils/interface/DataSetMixer.h"

#include "RVersion.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "diphotons/RooUtils/interface/RooPowLogPdf.h"
#include "diphotons/RooUtils/interface/RooSlicePdf.h"

#pragma link C++ defined_in "diphotons/RooUtils/interface/RooPowLogPdf.h";
#pragma link C++ defined_in "diphotons/RooUtils/interface/RooSlicePdf.h";
#endif

#pragma link C++ defined_in "diphotons/Utils/interface/DataSetFiller.h";
#pragma link C++ defined_in "diphotons/Utils/interface/DataSetMixer.h";
#pragma link C++ defined_in "diphotons/Utils/interface/CategoryOptimizer.h";
#pragma link C++ defined_in "diphotons/Utils/interface/FunctionHelpers.h";
#pragma link C++ defined_in "diphotons/Utils/interface/NaiveCategoryOptimization.h";
#pragma link C++ defined_in "diphotons/Utils/interface/SimpleShapeCategoryOptimization.h";

#pragma link C++ class std::list<RooAbsData*>::iterator;

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

