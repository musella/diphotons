#include <string>
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/libminifloat.h"

class BSVertexProducer : public edm::global::EDProducer<> {
public:
    explicit BSVertexProducer(const edm::ParameterSet&);
    ~BSVertexProducer();
    
    virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const;
private:
    edm::EDGetTokenT<reco::BeamSpot> bsToken_;
};


BSVertexProducer::BSVertexProducer(const edm::ParameterSet& iConfig):
    bsToken_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("bsTag")))
{
  produces<std::vector<reco::Vertex> >();
}

BSVertexProducer::~BSVertexProducer() {}

void BSVertexProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {

    //---gen BS
    edm::Handle<reco::BeamSpot> bsHandle;
    iEvent.getByToken(bsToken_, bsHandle);
    
    std::auto_ptr<std::vector<reco::Vertex> > outPtr(new std::vector<reco::Vertex>());

    math::XYZPoint position(bsHandle->x0(), bsHandle->y0(), bsHandle->z0());
    math::Error<3>::type error;
    outPtr->push_back(reco::Vertex(position, error));
    iEvent.put(outPtr);
}

#include "FWCore/Framework/interface/MakerMacros.h"
typedef BSVertexProducer diphotonsBSVertexProducer;
DEFINE_FWK_MODULE(diphotonsBSVertexProducer);
