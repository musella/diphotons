#include <string>
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/libminifloat.h"

#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CompositePtrCandidate.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"

using namespace edm;

class MyMHTProducer : public edm::global::EDProducer<> {
public:
	explicit MyMHTProducer(const edm::ParameterSet&);
	~MyMHTProducer();
    
	virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const;
private:
	EDGetTokenT<View<reco::Candidate> > srcToken_;
};


MyMHTProducer::MyMHTProducer(const edm::ParameterSet& iConfig) : 
	srcToken_( consumes<View<reco::Candidate> >( iConfig.getParameter<InputTag> ( "src" ) ) )
{
	//produces<std::vector<reco::CompositePtrCandidate> >();
	produces<std::vector<reco::CompositeCandidate> >();
}

MyMHTProducer::~MyMHTProducer() {}

void MyMHTProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
	//std::auto_ptr<std::vector<reco::CompositePtrCandidate> > outPtr(new std::vector<reco::CompositePtrCandidate>(1));
        std::auto_ptr<std::vector<reco::CompositeCandidate> > outPtr(new std::vector<reco::CompositeCandidate>(1));
	
	auto & out = outPtr->at(0);
	
	Handle<View<reco::Candidate> > src;
	iEvent.getByToken( srcToken_,  src);
	auto & collection = *src;
	
	for( size_t iob = 0; iob<collection.size(); ++iob ) {
		// Ptr<reco::Candidate> cand = collection.ptrAt(iob);
		auto & cand = collection.at(iob);
		out.addDaughter(cand);
	}
	
	AddFourMomenta addP4;
	addP4.set(out);
	
	iEvent.put(outPtr);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MyMHTProducer);
