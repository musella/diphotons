#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "TMath.h"
#include "TTree.h"
#include "TLorentzVector.h"

using namespace std;
using namespace edm;

// diphoton tree
struct diphoTree_struc_ {

  float mggGen;
};

class GenOnlyAnalyzer : public edm::EDAnalyzer {
  
public:
  
  explicit GenOnlyAnalyzer(const edm::ParameterSet&);
  ~GenOnlyAnalyzer();
  
private:
  
  edm::Service<TFileService> fs_;
  
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  
  void initTreeStructure();

  // collections
  EDGetTokenT<View<reco::GenParticle> > genPartToken_;

  // output tree with several diphoton infos
  TTree *DiPhotonTree;
  diphoTree_struc_ treeDipho_;
};
   

GenOnlyAnalyzer::GenOnlyAnalyzer(const edm::ParameterSet& iConfig):

  // collections
  genPartToken_(consumes<View<reco::GenParticle> >(iConfig.getUntrackedParameter<InputTag> ("GenParticlesTag", InputTag("genParticles")))) 
{  };

GenOnlyAnalyzer::~GenOnlyAnalyzer() { };

void GenOnlyAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  // access edm objects
  edm::Handle<View<reco::GenParticle> > genParticles;
  iEvent.getByToken( genPartToken_, genParticles );

  // to be kept in the tree  
  float mggGen;
  initTreeStructure();

  // MC truth photons
  mggGen = -999.;
  for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
    
    if ( genParticles->ptrAt( genLoop )->pdgId()==5100039) {  

      if (genParticles->ptrAt( genLoop )->numberOfDaughters()==2) {
	
	int statusd1 = genParticles->ptrAt( genLoop )->daughter(0)->status();
	int statusd2 = genParticles->ptrAt( genLoop )->daughter(1)->status();
	int pdgidd1  = genParticles->ptrAt( genLoop )->daughter(0)->pdgId();
	int pdgidd2  = genParticles->ptrAt( genLoop )->daughter(1)->pdgId();

	if (statusd1==1 && statusd2==1 && pdgidd1==22 && pdgidd2==22) { 
	
	  float ptd1  = genParticles->ptrAt( genLoop )->daughter(0)->pt();
	  float ptd2  = genParticles->ptrAt( genLoop )->daughter(1)->pt();
	  float etad1 = genParticles->ptrAt( genLoop )->daughter(0)->eta();
	  float etad2 = genParticles->ptrAt( genLoop )->daughter(1)->eta();
	  float phid1 = genParticles->ptrAt( genLoop )->daughter(0)->phi();
	  float phid2 = genParticles->ptrAt( genLoop )->daughter(1)->phi();
	  
	  TLorentzVector *myGenD1 = new TLorentzVector(0,0,0,0);
	  TLorentzVector *myGenD2 = new TLorentzVector(0,0,0,0);
	  myGenD1->SetPtEtaPhiM(ptd1, etad1, phid1, 0.);
	  myGenD2->SetPtEtaPhiM(ptd2, etad2, phid2, 0.);
	  mggGen = (*myGenD1+*myGenD2).M();
	  
	  break;
	}
      }
    }
  }

  // Variables for the tree
  treeDipho_.mggGen  = mggGen;
  
  // Filling the trees
  DiPhotonTree->Fill();
}		

void GenOnlyAnalyzer::beginJob() {

  // Trees
  DiPhotonTree = fs_->make<TTree>("DiphotonTree","di-photon tree");
  DiPhotonTree->Branch("mggGen",&(treeDipho_.mggGen),"mggGen/F");
}

void GenOnlyAnalyzer::endJob() { }

void GenOnlyAnalyzer::initTreeStructure() {

  treeDipho_.mggGen  = -500.;
}

DEFINE_FWK_MODULE(GenOnlyAnalyzer);
