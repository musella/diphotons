#include "DataFormats/Common/interface/Handle.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "flashgg/MicroAODFormats/interface/Photon.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "../interface/PhotonIdAnalyzer.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

#include <map>


using namespace std;
using namespace edm;
using namespace flashgg;
using namespace diphotons;

using pat::PackedGenParticle;
using reco::Candidate;

typedef enum { kFake, kPrompt  } genMatch_t;

struct GenMatchInfo {
	GenMatchInfo() : matched(0), nPhotonsInCone(0),
			 relPt(0.), deltaR(999.), 
			 extraEnergy(0.), match(kFake) 
		{};
	const PackedGenParticle * matched;
	int nPhotonsInCone;
	float relPt, deltaR;
	float extraEnergy;
	genMatch_t match;
};

/// default constructor
PhotonIdAnalyzer::PhotonIdAnalyzer(const edm::ParameterSet& cfg, TFileDirectory& fs): 
  edm::BasicAnalyzer::BasicAnalyzer(cfg, fs),
  photons_(cfg.getParameter<edm::InputTag>("photons")),
  packedGen_(cfg.getParameter<edm::InputTag>("packedGenParticles")),
  lumi_weight_(cfg.getParameter<double>("lumi_weight"))
{
  hists_["photonPt"  ]	     = fs.make<TH1F>("photonPt"		, "pt"           ,  100,  0., 300.);
  hists_["photonEta" ]	     = fs.make<TH1F>("photonEta"	, "eta"          ,  100, -3.,   3.);
  hists_["photonPhi" ]	     = fs.make<TH1F>("photonPhi"	, "phi"          ,  100, -5.,   5.); 
  hists_["promptPhotonN"  ]  = fs.make<TH1F>("promptPhotonN"	, "n"            ,  6,  -0.5, 5.5);
  hists_["promptPhotonPt"  ] = fs.make<TH1F>("promptPhotonPt"	, "pt"           ,  100,  0., 300.);
  hists_["promptPhotonEta" ] = fs.make<TH1F>("promptPhotonEta"	, "eta"          ,  100, -3.,   3.);
  hists_["promptPhotonPhi" ] = fs.make<TH1F>("promptPhotonPhi"	, "phi"          ,  100, -5.,   5.); 
  hists_["fakePhotonN"  ]  = fs.make<TH1F>("fakePhotonN"	, "n"            ,  6,  -0.5, 5.5);
  hists_["fakePhotonPt"  ]   = fs.make<TH1F>("fakePhotonPt"	, "pt"           ,  100,  0., 300.);
  hists_["fakePhotonEta" ]   = fs.make<TH1F>("fakePhotonEta"	, "eta"          ,  100, -3.,   3.);
  hists_["fakePhotonPhi" ]   = fs.make<TH1F>("fakePhotonPhi"	, "phi"          ,  100, -5.,   5.); 
  /// hists_["genPairsMass" ]    = fs.make<TH1F>("genPairsMass"	, "genPairsMass" ,  500,  0,    1.);
  hists_["matchMinDr" ]    = fs.make<TH1F>("matchMinDr"	, "matchMinDr" ,  100,  0,    0.1);
  hists_["matchRelPtOnePhoton" ]    = fs.make<TH1F>("matchRelPtOnePhoton"	, "matchRelPtOnePhoton" ,  100,  0,    1.5);
  hists_["matchRelPtIso" ]    = fs.make<TH1F>("matchRelPtIso"	, "matchRelPtIso" ,  100,  0,    1.5);
  hists_["matchRelPtNonIso" ]    = fs.make<TH1F>("matchRelPtNonIso"	, "matchRelPtNonIso" ,  100,  0,    1.5);
  hists_["matchNPartInCone" ]   = fs.make<TH1F>("matchNPartInCone", "matchNPartInCone" , 21,  -0.5,   20);
  hists_["matchExtraEnergy" ]    = fs.make<TH1F>("matchExtraEnergy"	, "matchExtraEnergy" ,  100,  0,    1.5);
}


PhotonIdAnalyzer::~PhotonIdAnalyzer()
{
}

void PhotonIdAnalyzer::beginJob()
{

}
/// everything that needs to be done after the event loop
void PhotonIdAnalyzer::endJob()
{
}

float PhotonIdAnalyzer::getEventWeight(const edm::EventBase& event)
{
	return lumi_weight_;
}

// MC truth
GenMatchInfo doGenMatch(const Photon & pho, const vector<PackedGenParticle> & genParts, float maxDr, float minLeadPt, float minPtRelOnePhoton,
		      float minPtRel, float maxExtraEnergy// , float weight,
		      // TH1 * matchMinDr,  TH1 * matchRelPtOnePhoton,  TH1 * matchRelPtIso, TH1 * matchRelPtNonIso, TH1 * matchNPartInCone, TH1 * matchExtraEnergy)
	)
{
	/// genMatch_t ret = kFake;
	GenMatchInfo ret;
	// look for gen level photons in the reco photon direction
	map<float,const PackedGenParticle *> genPhotonsInCone; // use map to sort candidates in deltaR
	Candidate::LorentzVector cluster( 0, 0, 0, 0 );
	for(vector<PackedGenParticle>::const_iterator igen=genParts.begin(); igen!=genParts.end(); ++igen) {
		if( igen->pdgId() != 22 ) { continue; }
		float dR = reco::deltaR(pho,*igen);
		if(dR < maxDr) {
			genPhotonsInCone.insert(make_pair(dR,&(*igen)));
			cluster += igen->p4();
		}
	}
	ret.nPhotonsInCone = genPhotonsInCone.size();
	
	if(!genPhotonsInCone.empty()){
		// find the closeset gen photon above threshold
		for(map<float,const PackedGenParticle *>::iterator ipair=genPhotonsInCone.begin(); ipair!=genPhotonsInCone.end(); ++ipair ) {
			if( ipair->second->pt() > minLeadPt ) {
				ret.matched = ipair->second;
				break;
			}
		}
		if( ret.matched == 0 ) { 
			/// matchMinDr->Fill(maxDr,weight);
			return ret; 
		}
		if( ret.matched->numberOfMothers() > 0 ) {
			for(size_t imom=0; imom<ret.matched->numberOfMothers(); ++imom) {
				int mstat = ret.matched->mother(imom)->status();
				int mpdgId = ret.matched->mother(imom)->pdgId();
				if( mpdgId == 22 && (mstat == 3 || mstat == 23 || mstat == 22) ) {
					ret.match = kPrompt;
					/// matchRelPtOnePhoton->Fill(ret.matched->pt()/pho.pt(),weight);
				}
				
			}
		}
		ret.extraEnergy = (cluster.energy() - ret.matched->energy())/ret.matched->energy();
		ret.relPt = ret.matched->pt()/pho.pt();
		ret.deltaR = reco::deltaR(pho,*ret.matched);
		// if not ME photon, check harder
		if( ret.match != kPrompt ) {
			/// matchMinDr->Fill(ret.deltaR,weight);
			/// matchNPartInCone->Fill(genPhotonsInCone.size(),weight);
			if( ( genPhotonsInCone.size() == 1 ) ) {
				// if only one photon is found in the cone apply the corresponding pt cut
				/// matchRelPtOnePhoton->Fill(re.relPt,weight);
				if( ( ret.matched->pt() > minPtRelOnePhoton*pho.pt() ) ) {
					ret.match = kPrompt;
				}
			} else { 
				// if more then one photon is found in the cone, require the extra 
				//   photons contribution to be below some threshold
				/// matchExtraEnergy->Fill(ret.extraEnergy,weight);
				if( ret.extraEnergy < maxExtraEnergy ) {
					/// matchRelPtIso->Fill(ret.relPt,weight);
					if( ( ret.matched->pt() > minPtRel*pho.pt() ) ) { 
						ret.match = kPrompt; 
					}
				} else {
					/// matchRelPtNonIso->Fill(ret.relPt,weight);
				}
			}
		}
	}
	//// if( matched != 0 && matched->numberOfMothers() > 0 ) {
	//// 	for(size_t imom=0; imom<matched->numberOfMothers(); ++imom) {
	//// 		int mstat = matched->mother(imom)->status();
	//// 		int mpdgId = matched->mother(imom)->pdgId();
	//// 		cout << "match " << ret << " mstat " << mstat << " mpdgId " << mpdgId << endl;
	//// 	}
	//// }
	return ret;
}


/// everything that needs to be done during the event loop
void 
PhotonIdAnalyzer::analyze(const edm::EventBase& event)
{
  // Handle to the photon collection
  Handle<vector<Photon> > photons;
  Handle<vector<PackedGenParticle> > packedGenParticles;
  // Handle<vector<PrunedGenParticle> > prunedGenParticles;
  event.getByLabel(photons_, photons);
  // event.getByLabel(prunedGenParticles_, prunedGenParticles);
  event.getByLabel(packedGen_, packedGenParticles);
  
  weight_ = getEventWeight(event);
  
  // loop photon collection and fill histograms
  std::vector<GenMatchInfo> genMatch;
  int nPrompt=false, nFakes = false;
  for(std::vector<Photon>::const_iterator pho1=photons->begin(); pho1!=photons->end(); ++pho1){
	  
	  GenMatchInfo match = doGenMatch(*pho1,*packedGenParticles,0.1,15.,0.,0.,0.05
					  /// ,weight_,
					  /// hists_["matchMinDr"],hists_["matchRelPtOnePhoton"],
					  /// hists_["matchRelPtIso"],hists_["matchRelPtNonIso"],
					  /// hists_["matchNPartInCone"],hists_["matchExtraEnergy"]
		  );
	  /// cout << "match " << match << endl;
	  genMatch.push_back( match );
	  
	  if( match.match == kPrompt ) {
		  if( nPrompt ==0 ) { 
			  hists_["promptPhotonPt" ]->Fill( pho1->pt (), weight_ );
			  hists_["promptPhotonEta"]->Fill( pho1->eta(), weight_ );
			  hists_["promptPhotonPhi"]->Fill( pho1->phi(), weight_ );
		  }
		  ++nPrompt;
	  } else {
		  if(  nFakes == 0 ) {
			  hists_["fakePhotonPt" ]->Fill( pho1->pt (), weight_ );
			  hists_["fakePhotonEta"]->Fill( pho1->eta(), weight_ );
			  hists_["fakePhotonPhi"]->Fill( pho1->phi(), weight_ );
		  }
		  ++nFakes;
	  }
	  
	  hists_["photonPt" ]->Fill( pho1->pt (), weight_ );
	  hists_["photonEta"]->Fill( pho1->eta(), weight_ );
	  hists_["photonPhi"]->Fill( pho1->phi(), weight_ );
  }

  hists_["promptPhotonN" ]->Fill(nPrompt, weight_);
  hists_["fakePhotonN" ]->Fill(nFakes, weight_);
  
  
}

