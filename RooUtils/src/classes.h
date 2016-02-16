#include "diphotons/RooUtils/interface/RooPowLogPdf.h"
#include "diphotons/RooUtils/interface/RooSlicePdf.h"
#include "diphotons/RooUtils/interface/RooStarMomentMorph.h"
#include "diphotons/RooUtils/interface/RooDoubleCBShape.h"

namespace  {
    struct dictionary {
	    RooPowLogPdf pl;
	    RooSlicePdf  sl;
	    RooStarMomentMorph  sm;
	    RooDoubleCBShape  dcb;
    };
}

