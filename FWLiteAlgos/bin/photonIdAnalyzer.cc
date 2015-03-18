
#include "../interface/PhotonIdAnalyzer.h"
#include "PhysicsTools/UtilAlgos/interface/FWLiteAnalyzerWrapper.h"

using namespace diphotons;

typedef fwlite::AnalyzerWrapper<PhotonIdAnalyzer> WrappedFWLiteAnalyzer;

int main(int argc, char* argv[]) 
{
  // load framework libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();
  gSystem->Load("libDataFormatsFWLite.so");
  gSystem->Load("libDataFormatsPatCandidates.so");
  gSystem->Load("libflashggDataFormats.so"); 
  
  // only allow one argument for this simple example which should be the
  // the python cfg file
  if ( argc < 2 ) {
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  // get the python configuration
  PythonProcessDesc builder(argv[1],argc,argv);
  WrappedFWLiteAnalyzer ana(*(builder.processDesc()->getProcessPSet()), std::string("photonIdAnalyzer"));
  ana.beginJob();
  ana.analyze();
  ana.endJob();
  return 0;
}
