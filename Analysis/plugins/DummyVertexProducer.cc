#include <string>
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/libminifloat.h"

class DummyVertexProducer : public edm::global::EDProducer<> {
public:
	explicit DummyVertexProducer(const edm::ParameterSet&);
	~DummyVertexProducer();
    
	virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const;
private:
};


DummyVertexProducer::DummyVertexProducer(const edm::ParameterSet& iConfig)
{
  produces<std::vector<reco::Vertex> >();
}

DummyVertexProducer::~DummyVertexProducer() {}

void DummyVertexProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
    std::auto_ptr<std::vector<reco::Vertex> > outPtr(new std::vector<reco::Vertex>());

    outPtr->push_back(reco::Vertex());
    iEvent.put(outPtr);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(DummyVertexProducer);
