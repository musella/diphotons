#include <map>
#include <string>

#include "TH1.h"
#include "TTree.h"
#include "PhysicsTools/UtilAlgos/interface/BasicAnalyzer.h"
#include "flashgg/DataFormats/interface/Photon.h"
#include "flashgg/MicroAOD/interface/PhotonMVAComputer.h"
/// #include "PhysicsTools/FWLite/interface/ScannerHelpers.h"

#include "flashgg/MetaData/interface/IdleWatchdog.h"

#include "CommonTools/Utils/interface/StringObjectFunction.h"

/**
   \class PhotonIdAnalyzer PhotonIdAnalyzer.h "PhysicsTools/PatExamples/interface/PhotonIdAnalyzer.h"
   \brief Example class that can be used to analyze pat::Photons both within FWLite and within the full framework

   This is an example for keeping classes that can be used both within FWLite and within the full
   framework. The class is derived from the BasicAnalyzer base class, which is an interface for
   the two wrapper classes EDAnalyzerWrapper and FWLiteAnalyzerWrapper. You can fin more information
   on this on WorkBookFWLiteExamples#ExampleFive.
*/

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
/// class EcalRecHitCollection;
class CaloTopology;
class CaloSubdetectorTopology;

namespace diphotons {

    typedef StringObjectFunction<flashgg::Photon, false> PhotonFunctor;
    /// typedef fwlite::helper::ScannerBase PhotonFunctor;

    class PhotonIdAnalyzer : public edm::BasicAnalyzer
    {

    public:
        /// default constructor
        PhotonIdAnalyzer( const edm::ParameterSet &cfg, TFileDirectory &fs );
        /// default destructor
        virtual ~PhotonIdAnalyzer();
        /// everything that needs to be done before the event loop
        void beginJob();
        /// everything that needs to be done after the event loop
        void endJob();
        /// everything that needs to be done during the event loop
        void analyze( const edm::EventBase &event );


    protected:
        struct RecHitsInfo {

            int kSaturated[25];
            int kLeRecovered[25];
            int kNeighRecovered[25];
            float amplit[25];
            int ieta[25];
            int iphi[25];
            int ix[25];
            int iy[25];
            int iz[25];
        };

        float getEventWeight( const edm::EventBase &event );
        /// input tag for mouns
        edm::InputTag photons_, packedGen_, prunedGen_, vertexes_, rhoFixedGrid_, ecalHitEBColl_, ecalHitEEColl_;
        /// histograms
        std::map<std::string, TH1 *> hists_;
        // event weight
        float lumiWeight_, weight_;
        double minPt_;
        std::string processId_;
        bool isSherpa_;

        flashgg::PhotonMVAComputer mvaComputer_;
        flashgg::IdleWatchdog watchdog_;

        TTree *bookTree( const std::string &name, TFileDirectory &fs );
        void fillTreeBranches( const flashgg::Photon &pho,
                               const EcalRecHitCollection *EcalBarrelRecHits, const EcalRecHitCollection *EcalEndcapRecHits );

        TTree *promptTree_, *fakesTree_;
        std::vector<std::string> miniTreeBranches_;
        /// PhotonFunctor photonFunctor_;
        std::vector<PhotonFunctor> miniTreeFunctors_;
        std::vector<float> miniTreeBuffers_, miniTreeDefaults_;
        int ipho_, iprompt_, ifake_;
        float rho_;
        unsigned int nvtx_, run_, event_, lumi_;

        bool recomputeNoZsShapes_;

        bool dumpRecHits_, dumpAllRechisInfo_;
        const CaloTopology *topology_;
        const CaloSubdetectorTopology *theSubdetTopologyEB_;
        const CaloSubdetectorTopology *theSubdetTopologyEE_;

        RecHitsInfo recHitsInfo_;
    };

}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

