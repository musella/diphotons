#ifndef flashgg_DiPhotonsRechiFlagProducer_h
#define flashgg_DiPhotonsRechiFlagProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "flashgg/DataFormats/interface/DiPhotonCandidate.h"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "flashgg/MicroAOD/interface/PhotonIdUtils.h"

#include <tuple>
#include <vector>
#include <map>

using namespace std;
using namespace edm;

namespace flashgg {
    class DiPhotonsRechiFlagProducer : public EDProducer
    {

    public:
        DiPhotonsRechiFlagProducer( const edm::ParameterSet &config ) :
            srcToken_( consumes<View<flashgg::DiPhotonCandidate> >( config.getParameter<InputTag>( "src" ) ) ),
            ecalHitEBToken_( consumes<EcalRecHitCollection>( config.getParameter<edm::InputTag>( "reducedBarrelRecHitCollection" ) ) ),
            ecalHitEEToken_( consumes<EcalRecHitCollection>( config.getParameter<edm::InputTag>( "reducedEndcapRecHitCollection" ) ) ),
            ecalHitESToken_( consumes<EcalRecHitCollection>( config.getParameter<edm::InputTag>( "reducedPreshowerRecHitCollection" ) ) )
        {
            produces<vector<flashgg::DiPhotonCandidate> >();
        };

        void produce( Event &evt, const EventSetup & iSetup)
        {
            Handle<View<flashgg::DiPhotonCandidate> > input;
            evt.getByToken( srcToken_, input );
            auto_ptr<vector<DiPhotonCandidate> > output( new vector<DiPhotonCandidate> );
            
            noZS::EcalClusterLazyTools noZsLazyTool( evt, iSetup, ecalHitEBToken_, ecalHitEEToken_, ecalHitESToken_ );

            for( auto dipho : *input ) {
                dipho.makePhotonsPersistent();
                PhotonIdUtils::fillRechHitFlags( dipho.getLeadingPhoton(), noZsLazyTool );
                PhotonIdUtils::fillRechHitFlags( dipho.getSubLeadingPhoton(), noZsLazyTool );

                output->push_back( dipho );
            }
            evt.put( output );
        };

    private:
        EDGetTokenT<View<DiPhotonCandidate> > srcToken_;
        edm::EDGetTokenT<EcalRecHitCollection> ecalHitEBToken_;
        edm::EDGetTokenT<EcalRecHitCollection> ecalHitEEToken_;
        edm::EDGetTokenT<EcalRecHitCollection> ecalHitESToken_;
    };
}

typedef flashgg::DiPhotonsRechiFlagProducer DiphotonsDiPhotonsRechiFlagProducer;
DEFINE_FWK_MODULE( DiphotonsDiPhotonsRechiFlagProducer );

#endif // flashgg_PerPhotonMVADiPhotonComputer_h
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

