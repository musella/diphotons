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

#include "flashgg/DataFormats/interface/DiPhotonCandidate.h"

using namespace edm;

class MyMHTProducer : public edm::global::EDProducer<> {
public:
	explicit MyMHTProducer(const edm::ParameterSet&);
	~MyMHTProducer();
    
	virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const;
private:
	EDGetTokenT<View<reco::Candidate> > srcToken_;
	int max_;
	bool veto_;
	EDGetTokenT<View<flashgg::DiPhotonCandidate> > vetoToken_;
};


MyMHTProducer::MyMHTProducer(const edm::ParameterSet& iConfig) : 
	srcToken_( consumes<View<reco::Candidate> >( iConfig.getParameter<InputTag> ( "src" ) ) ),
	max_(-1),
	veto_(iConfig.exists("veto")),
	vetoToken_( consumes<View<flashgg::DiPhotonCandidate> >(veto_ ? iConfig.getParameter<InputTag> ( "veto" ) : InputTag("dummy") ) )
{
	if( iConfig.exists("maxCand") ) {
		max_ = iConfig.getParameter<int>("maxCand");
	}
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

	Handle<View<flashgg::DiPhotonCandidate> > veto;
	if( veto_ ) {
		iEvent.getByToken( vetoToken_,  veto);
	}
	
	int count = ( max_ > 0 ? max_ : collection.size() );
	for( size_t iob = 0; iob<collection.size() && count > 0; ++iob ) {
		// Ptr<reco::Candidate> cand = collection.ptrAt(iob);
		auto & cand = collection.at(iob);
		bool add = true;
		if( veto_ ) {
			for( const auto & iveto: *veto ) {
				if( reco::deltaR(*(iveto.leadingPhoton()),cand) < 0.4 || reco::deltaR(*(iveto.subLeadingPhoton()),cand) < 0.4) { 
					add=false;
					break;
				}
			}
		}
		if( add ) { out.addDaughter(cand); --count; }
	}
	
	AddFourMomenta addP4;
	addP4.set(out);
	
	iEvent.put(outPtr);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MyMHTProducer);
