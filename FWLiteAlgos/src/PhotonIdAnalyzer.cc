#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "../interface/PhotonIdAnalyzer.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "FWCore/Utilities/interface/TypeWithDict.h"


#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloTopology/interface/EcalBarrelHardcodedTopology.h"
#include "Geometry/CaloTopology/interface/EcalEndcapHardcodedTopology.h"
#include "RecoCaloTools/Navigation/interface/CaloNavigator.h"

#include "flashgg/MicroAOD/interface/PhotonIdUtils.h"
#include "flashgg/MicroAOD/interface/PhotonMCUtils.h"


#include <map>


using namespace std;
using namespace edm;
using namespace flashgg;
using namespace diphotons;

using pat::PackedGenParticle;
using reco::Candidate;
using reco::Vertex;

typedef enum { kFake, kPrompt  } genMatch_t;

struct GenMatchInfo {
    GenMatchInfo() : matched( 0 ), nPhotonsInCone( 0 ),
        relPt( 0. ), deltaR( 999. ),
        extraEnergy( 0. ), match( kFake )
    {};
    const PackedGenParticle *matched;
    int nPhotonsInCone;
    float relPt, deltaR;
    float extraEnergy;
    genMatch_t match;
};

/// default constructor
PhotonIdAnalyzer::PhotonIdAnalyzer( const edm::ParameterSet &cfg, TFileDirectory &fs ):
    edm::BasicAnalyzer::BasicAnalyzer( cfg, fs ),
    photons_( cfg.getParameter<edm::InputTag>( "photons" ) ),
    packedGen_( cfg.getParameter<edm::InputTag>( "packedGenParticles" ) ),
    vertexes_( cfg.getParameter<edm::InputTag> ( "vertexes" ) ),
    rhoFixedGrid_( cfg.getParameter<edm::InputTag>( "rho" ) ),
    lumiWeight_( cfg.getParameter<double>( "lumiWeight" ) ),
    processId_( cfg.getParameter<string>( "processId" ) ),
    mvaComputer_( cfg ),
    watchdog_( cfg.getParameter<edm::ParameterSet>( "idleWatchdog" ) ),
    /// photonFunctor_(edm::TypeWithDict(edm::Wrapper<vector<Photon> >::typeInfo())),
    promptTree_( 0 ), fakesTree_( 0 ),
    topology_( 0 ), theSubdetTopologyEB_( 0 ), theSubdetTopologyEE_( 0 )
{
    hists_["photonPt"  ]	     = fs.make<TH1F>( "photonPt"		, "pt"           ,  100,  0., 300. );
    hists_["photonEta" ]	     = fs.make<TH1F>( "photonEta"	, "eta"          ,  100, -3.,   3. );
    hists_["photonPhi" ]	     = fs.make<TH1F>( "photonPhi"	, "phi"          ,  100, -5.,   5. );
    hists_["promptPhotonN"  ]  = fs.make<TH1F>( "promptPhotonN"	, "n"            ,  6,  -0.5, 5.5 );
    hists_["promptPhotonPt"  ] = fs.make<TH1F>( "promptPhotonPt"	, "pt"           ,  100,  0., 300. );
    hists_["promptPhotonEta" ] = fs.make<TH1F>( "promptPhotonEta"	, "eta"          ,  100, -3.,   3. );
    hists_["promptPhotonPhi" ] = fs.make<TH1F>( "promptPhotonPhi"	, "phi"          ,  100, -5.,   5. );
    hists_["fakePhotonN"  ]  = fs.make<TH1F>( "fakePhotonN"	, "n"            ,  6,  -0.5, 5.5 );
    hists_["fakePhotonPt"  ]   = fs.make<TH1F>( "fakePhotonPt"	, "pt"           ,  100,  0., 300. );
    hists_["fakePhotonEta" ]   = fs.make<TH1F>( "fakePhotonEta"	, "eta"          ,  100, -3.,   3. );
    hists_["fakePhotonPhi" ]   = fs.make<TH1F>( "fakePhotonPhi"	, "phi"          ,  100, -5.,   5. );
    /// hists_["genPairsMass" ]    = fs.make<TH1F>("genPairsMass"	, "genPairsMass" ,  500,  0,    1.);
    hists_["matchMinDr" ]    = fs.make<TH1F>( "matchMinDr"	, "matchMinDr" ,  100,  0,    0.1 );
    hists_["matchRelPtOnePhoton" ]    = fs.make<TH1F>( "matchRelPtOnePhoton"	, "matchRelPtOnePhoton" ,  100,  0,    1.5 );
    hists_["matchRelPtIso" ]    = fs.make<TH1F>( "matchRelPtIso"	, "matchRelPtIso" ,  100,  0,    1.5 );
    hists_["matchRelPtNonIso" ]    = fs.make<TH1F>( "matchRelPtNonIso"	, "matchRelPtNonIso" ,  100,  0,    1.5 );
    hists_["matchNPartInCone" ]   = fs.make<TH1F>( "matchNPartInCone", "matchNPartInCone" , 21,  -0.5,   20 );
    hists_["matchExtraEnergy" ]    = fs.make<TH1F>( "matchExtraEnergy"	, "matchExtraEnergy" ,  100,  0,    1.5 );


    recomputeNoZsShapes_ = cfg.getUntrackedParameter<bool>( "recomputeNoZsShapes", false );

    isSherpa_ = ( processId_.find( "sherpa" ) != string::npos );

    vector<ParameterSet> miniTreeCfg = cfg.getUntrackedParameter<vector<ParameterSet> >( "miniTreeCfg", vector<ParameterSet>() );
    dumpRecHits_ = cfg.getUntrackedParameter<bool>( "dumpRecHits", false );
    dumpAllRechisInfo_ = cfg.getUntrackedParameter<bool>( "dumpAllRechisInfo", false );
    minPt_ = cfg.getUntrackedParameter<double>( "minPt", 100. );

    if( ! miniTreeCfg.empty() ) {
        for( vector<ParameterSet>::iterator ivar = miniTreeCfg.begin(); ivar != miniTreeCfg.end(); ++ivar ) {
            string method = ivar->getUntrackedParameter<string>( "var" );
            string name = ivar->getUntrackedParameter<string>( "name", "" );
            double def = ivar->getUntrackedParameter<double>( "default", 0. );
            if( name.empty() ) { name = method; }
            miniTreeBranches_.push_back( name );
            miniTreeFunctors_.push_back( PhotonFunctor( method ) );
            miniTreeBuffers_.push_back( def );
        }
        miniTreeDefaults_ = miniTreeBuffers_;
        promptTree_ = bookTree( "promptTree", fs );
        bool doDump = dumpRecHits_;
        dumpRecHits_ = false;
        fakesTree_ = bookTree( "fakesTree", fs );
        dumpRecHits_ = doDump;
    }

    if( dumpRecHits_ || recomputeNoZsShapes_ ) {
        // FIXME: memory leak
        CaloTopology *topology = new CaloTopology();
        EcalBarrelHardcodedTopology *ebTopology = new EcalBarrelHardcodedTopology();
        EcalEndcapHardcodedTopology *eeTopology = new EcalEndcapHardcodedTopology();
        topology->setSubdetTopology( DetId::Ecal, EcalBarrel, ebTopology );
        topology->setSubdetTopology( DetId::Ecal, EcalEndcap, eeTopology );
        theSubdetTopologyEB_ = ebTopology;
        theSubdetTopologyEE_ = eeTopology;
        topology_ = topology;
        //
        ecalHitEBColl_ = cfg.getParameter<edm::InputTag>( "barrelRecHits" );
        ecalHitEEColl_ = cfg.getParameter<edm::InputTag>( "endcapRecHits" );
    }

}

TTree *PhotonIdAnalyzer::bookTree( const string &name, TFileDirectory &fs )
{
    TTree *ret = fs.make<TTree>( name.c_str(), name.c_str() );

    ret->Branch( "ipho", &ipho_, "ipho/I" );
    ret->Branch( "iprompt", &iprompt_, "iprompt/I" );
    ret->Branch( "ifake", &ifake_, "ifake/I" );
    ret->Branch( "weight", &weight_, "weight/F" );
    ret->Branch( "rho", &rho_, "rho/F" );
    ret->Branch( "nvtx", &nvtx_, "nvtx/i" );
    ret->Branch( "run", &run_, "run/i" );
    ret->Branch( "event", &event_, "event/i" );
    ret->Branch( "lumi", &lumi_, "lumi/i" );
    for( size_t ibr = 0; ibr < miniTreeBuffers_.size(); ++ibr ) {
        /// cout << "miniTree branch "  << miniTreeBranches_[ibr] << endl;
        ret->Branch( Form( "%s", miniTreeBranches_[ibr].c_str() ), &miniTreeBuffers_[ibr], Form( "%s/F", miniTreeBranches_[ibr].c_str() ) );
    }

    if( dumpRecHits_ ) {
        TString tree5x5 = "kSaturated[25]/I:kLeRecovered[25]/I:kNeighRecovered[25]/I";
        if( dumpAllRechisInfo_ ) { tree5x5 += ":amplit[25]/F:ieta[25]/I:iphi[25]/I:ix[25]/I:iy[25]/I:iz[25]/I"; }
        ret->Branch( "tree5x5", &recHitsInfo_, tree5x5 );
    }

    return ret;
}

void PhotonIdAnalyzer::fillTreeBranches( const Photon &pho,
        const EcalRecHitCollection *EcalBarrelRecHits, const EcalRecHitCollection *EcalEndcapRecHits )
{
    for( size_t ibr = 0; ibr < miniTreeFunctors_.size(); ++ibr ) {
        miniTreeBuffers_[ibr] = miniTreeFunctors_[ibr]( pho );
    }

    if( dumpRecHits_ ) {
        // extra info on rechits for xtals in the 5x5 matrix around the seed
        DetId seedDetId = ( ( pho.superCluster() )->seed() )->seed();

        if( seedDetId.subdetId() == EcalEndcap ) {

            int iNeigh = 0;

            CaloNavigator<DetId> cursorE = CaloNavigator<DetId>( seedDetId, theSubdetTopologyEE_ );

            for( int ix = -2; ix < 3; ++ix ) {
                for( int iy = -2; iy < 3; ++iy ) {
                    cursorE.home();
                    cursorE.offsetBy( ix, iy );
                    DetId cryId = cursorE.pos();

                    if( cryId.subdetId() != EcalEndcap ) {
                        recHitsInfo_.amplit[iNeigh] = -5000.;
                        recHitsInfo_.kSaturated[iNeigh] = -5000;
                        recHitsInfo_.kLeRecovered[iNeigh] = -5000;
                        recHitsInfo_.kNeighRecovered[iNeigh] = -5000;
                        recHitsInfo_.ieta[iNeigh] = -5000;
                        recHitsInfo_.iphi[iNeigh] = -5000;
                        recHitsInfo_.ix[iNeigh] = -5000;
                        recHitsInfo_.iy[iNeigh] = -5000;
                        recHitsInfo_.iz[iNeigh] = -5000;
                        recHitsInfo_.ieta[iNeigh] = -5000;
                        recHitsInfo_.iphi[iNeigh] = -5000;
                        recHitsInfo_.ix[iNeigh] = -5000;
                        recHitsInfo_.iy[iNeigh] = -5000;
                        recHitsInfo_.iz[iNeigh] = -5000;
                        iNeigh++;
                        continue;
                    }

                    EcalRecHitCollection::const_iterator itneigh = EcalEndcapRecHits->find( cryId );

                    if( itneigh != EcalEndcapRecHits->end() ) {
                        recHitsInfo_.amplit[iNeigh] = itneigh->energy();
                        recHitsInfo_.kSaturated[iNeigh] = itneigh->checkFlag( EcalRecHit::kSaturated );
                        recHitsInfo_.kLeRecovered[iNeigh] = itneigh->checkFlag( EcalRecHit::kLeadingEdgeRecovered );
                        recHitsInfo_.kNeighRecovered[iNeigh] = itneigh->checkFlag( EcalRecHit::kNeighboursRecovered );
                        recHitsInfo_.ieta[iNeigh] = -999;
                        recHitsInfo_.iphi[iNeigh] = -999;
                        recHitsInfo_.ix[iNeigh] = ( ( EEDetId )itneigh->detid() ).ix();
                        recHitsInfo_.iy[iNeigh] = ( ( EEDetId )itneigh->detid() ).iy();
                        recHitsInfo_.iz[iNeigh] = ( ( EEDetId )itneigh->detid() ).zside();
                    } else {
                        recHitsInfo_.amplit[iNeigh] = -2000.;
                        recHitsInfo_.kSaturated[iNeigh] = -2000;
                        recHitsInfo_.kLeRecovered[iNeigh] = -2000;
                        recHitsInfo_.kNeighRecovered[iNeigh] = -2000;
                        recHitsInfo_.ieta[iNeigh] = -2000;
                        recHitsInfo_.iphi[iNeigh] = -2000;
                        recHitsInfo_.ix[iNeigh] = -2000;
                        recHitsInfo_.iy[iNeigh] = -2000;
                        recHitsInfo_.iz[iNeigh] = -2000;
                        recHitsInfo_.ieta[iNeigh] = -2000;
                        recHitsInfo_.iphi[iNeigh] = -2000;
                        recHitsInfo_.ix[iNeigh] = -2000;
                        recHitsInfo_.iy[iNeigh] = -2000;
                        recHitsInfo_.iz[iNeigh] = -2000;
                    }

                    iNeigh++;
                }
            }
            if( iNeigh != 25 ) { cout << "problem: not 25 crystals!  ==> " << iNeigh << endl; }

        } else if( seedDetId.subdetId() == EcalBarrel ) {

            int iNeigh = 0;

            CaloNavigator<DetId> cursorE = CaloNavigator<DetId>( seedDetId, theSubdetTopologyEB_ );

            for( int ix = -2; ix < 3; ++ix ) {
                for( int iy = -2; iy < 3; ++iy ) {
                    cursorE.home();
                    cursorE.offsetBy( ix, iy );
                    DetId cryId = cursorE.pos();

                    if( cryId.subdetId() != EcalBarrel ) {
                        recHitsInfo_.amplit[iNeigh] = -5000.;
                        recHitsInfo_.kSaturated[iNeigh] = -5000;
                        recHitsInfo_.kLeRecovered[iNeigh] = -5000;
                        recHitsInfo_.kNeighRecovered[iNeigh] = -5000;
                        recHitsInfo_.ieta[iNeigh] = -5000;
                        recHitsInfo_.iphi[iNeigh] = -5000;
                        recHitsInfo_.ix[iNeigh] = -5000;
                        recHitsInfo_.iy[iNeigh] = -5000;
                        recHitsInfo_.iz[iNeigh] = -5000;
                        recHitsInfo_.ieta[iNeigh] = -5000;
                        recHitsInfo_.iphi[iNeigh] = -5000;
                        recHitsInfo_.ix[iNeigh] = -5000;
                        recHitsInfo_.iy[iNeigh] = -5000;
                        recHitsInfo_.iz[iNeigh] = -5000;
                        iNeigh++;
                        continue;
                    }

                    EcalRecHitCollection::const_iterator itneigh = EcalBarrelRecHits->find( cryId );

                    if( itneigh != EcalBarrelRecHits->end() ) {
                        recHitsInfo_.amplit[iNeigh] = itneigh->energy();
                        recHitsInfo_.kSaturated[iNeigh] = itneigh->checkFlag( EcalRecHit::kSaturated );
                        recHitsInfo_.kLeRecovered[iNeigh] = itneigh->checkFlag( EcalRecHit::kLeadingEdgeRecovered );
                        recHitsInfo_.kNeighRecovered[iNeigh] = itneigh->checkFlag( EcalRecHit::kNeighboursRecovered );
                        recHitsInfo_.ieta[iNeigh] = ( ( EBDetId )itneigh->detid() ).ieta();
                        recHitsInfo_.iphi[iNeigh] = ( ( EBDetId )itneigh->detid() ).iphi();
                        recHitsInfo_.ix[iNeigh] = -999;
                        recHitsInfo_.iy[iNeigh] = -999;
                        recHitsInfo_.iz[iNeigh] = -999;
                    } else {
                        recHitsInfo_.amplit[iNeigh] = -2000.;
                        recHitsInfo_.kSaturated[iNeigh] = -2000;
                        recHitsInfo_.kLeRecovered[iNeigh] = -2000;
                        recHitsInfo_.kNeighRecovered[iNeigh] = -2000;
                        recHitsInfo_.ieta[iNeigh] = -2000;
                        recHitsInfo_.iphi[iNeigh] = -2000;
                        recHitsInfo_.ix[iNeigh] = -2000;
                        recHitsInfo_.iy[iNeigh] = -2000;
                        recHitsInfo_.iz[iNeigh] = -2000;
                        recHitsInfo_.ieta[iNeigh] = -2000;
                        recHitsInfo_.iphi[iNeigh] = -2000;
                        recHitsInfo_.ix[iNeigh] = -2000;
                        recHitsInfo_.iy[iNeigh] = -2000;
                        recHitsInfo_.iz[iNeigh] = -2000;
                    }

                    iNeigh++;
                }
            }

            if( iNeigh != 25 ) { cout << "problem: not 25 crystals!  ==> " << iNeigh << endl; }
        }
        //// cout << "kSaturated ";
        //// copy(&recHitsInfo_.kSaturated[0],&recHitsInfo_.kSaturated[24],ostream_iterator<int>(cout,","));
        //// cout << endl;

    }

}


PhotonIdAnalyzer::~PhotonIdAnalyzer()
{
}

void PhotonIdAnalyzer::beginJob()
{
    watchdog_.reset();
}

/// everything that needs to be done after the event loop
void PhotonIdAnalyzer::endJob()
{
}

float PhotonIdAnalyzer::getEventWeight( const edm::EventBase &event )
{
    return lumiWeight_;
}

// MC truth
GenMatchInfo doGenMatch( const Photon &pho, const vector<PackedGenParticle> &genParts, float maxDr, float minLeadPt, float minPtRelOnePhoton,
                         float minPtRel, float maxExtraEnergy// , float weight,
                         // TH1 * matchMinDr,  TH1 * matchRelPtOnePhoton,  TH1 * matchRelPtIso, TH1 * matchRelPtNonIso, TH1 * matchNPartInCone, TH1 * matchExtraEnergy)
                       )
{
    /// genMatch_t ret = kFake;
    GenMatchInfo ret;
    // look for gen level photons in the reco photon direction
    map<float, const PackedGenParticle *> genPhotonsInCone; // use map to sort candidates in deltaR
    Candidate::LorentzVector cluster( 0, 0, 0, 0 );
    for( vector<PackedGenParticle>::const_iterator igen = genParts.begin(); igen != genParts.end(); ++igen ) {
        if( igen->pdgId() != 22 ) { continue; }
        float dR = reco::deltaR( pho, *igen );
        if( dR < maxDr ) {
            genPhotonsInCone.insert( make_pair( dR, &( *igen ) ) );
            cluster += igen->p4();
        }
    }
    ret.nPhotonsInCone = genPhotonsInCone.size();

    if( !genPhotonsInCone.empty() ) {
        // find the closeset gen photon above threshold
        for( map<float, const PackedGenParticle *>::iterator ipair = genPhotonsInCone.begin(); ipair != genPhotonsInCone.end(); ++ipair ) {
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
            for( size_t imom = 0; imom < ret.matched->numberOfMothers(); ++imom ) {
                int mstat = ret.matched->mother( imom )->status();
                int mpdgId = ret.matched->mother( imom )->pdgId();
                if( mpdgId == 22 && ( mstat == 3 || mstat == 23 || mstat == 22 ) ) {
                    ret.match = kPrompt;
                    /// matchRelPtOnePhoton->Fill(ret.matched->pt()/pho.pt(),weight);
                }

            }
        }
        ret.extraEnergy = ( cluster.energy() - ret.matched->energy() ) / ret.matched->energy();
        ret.relPt = ret.matched->pt() / pho.pt();
        ret.deltaR = reco::deltaR( pho, *ret.matched );
        // if not ME photon, check harder
        if( ret.match != kPrompt ) {
            /// matchMinDr->Fill(ret.deltaR,weight);
            /// matchNPartInCone->Fill(genPhotonsInCone.size(),weight);
            if( ( genPhotonsInCone.size() == 1 ) ) {
                // if only one photon is found in the cone apply the corresponding pt cut
                /// matchRelPtOnePhoton->Fill(re.relPt,weight);
                if( ( ret.matched->pt() > minPtRelOnePhoton * pho.pt() ) ) {
                    ret.match = kPrompt;
                }
            } else {
                // if more then one photon is found in the cone, require the extra
                //   photons contribution to be below some threshold
                /// matchExtraEnergy->Fill(ret.extraEnergy,weight);
                if( ret.extraEnergy < maxExtraEnergy ) {
                    /// matchRelPtIso->Fill(ret.relPt,weight);
                    if( ( ret.matched->pt() > minPtRel * pho.pt() ) ) {
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
PhotonIdAnalyzer::analyze( const edm::EventBase &event )
{
    watchdog_.check();
    // Handle to the photon collection
    Handle<vector<Photon> > photons;
    Handle<vector<PackedGenParticle> > packedGenParticles;
    Handle<vector<Vertex> > vertexes;
    Handle<double> rhoHandle;
    Handle< EcalRecHitCollection > EcalBarrelRecHits;
    Handle< EcalRecHitCollection > EcalEndcapRecHits;

    // Handle<vector<PrunedGenParticle> > prunedGenParticles;
    event.getByLabel( photons_, photons );
    // event.getByLabel(prunedGenParticles_, prunedGenParticles);
    event.getByLabel( packedGen_, packedGenParticles );
    event.getByLabel( vertexes_, vertexes );
    event.getByLabel( rhoFixedGrid_, rhoHandle );

    if( dumpRecHits_ || recomputeNoZsShapes_ ) {
        event.getByLabel( ecalHitEBColl_, EcalBarrelRecHits );
        event.getByLabel( ecalHitEEColl_, EcalEndcapRecHits );
    }

    weight_ = getEventWeight( event );


    // loop photon collection and fill histograms
    std::vector<GenMatchInfo> genMatch;
    ipho_ = 0;
    iprompt_ = 0;
    ifake_ = 0;
    rho_ = *rhoHandle;
    nvtx_ = vertexes->size();
    run_ = event.id().run();
    lumi_ = event.id().luminosityBlock();
    event_ = event.id().event();

    PhotonIdUtils utils;
    mvaComputer_.update( event );

    for( std::vector<Photon>::const_iterator ipho = photons->begin(); ipho != photons->end(); ++ipho ) {

        Photon *pho = ipho->clone();

        if( recomputeNoZsShapes_ ) {
            utils.recomputeNonZsClusterShapes( *pho, EcalBarrelRecHits.product(), EcalEndcapRecHits.product(), topology_ );
        }

        float scEta = pho->superCluster()->eta();
        if( fabs( scEta ) > 2.5 || ( fabs( scEta ) > 1.4442 && fabs( scEta ) < 1.556 ) ) { continue; }

        if( pho->pt() < minPt_ ) { continue; }
        DetId seedId = pho->superCluster()->seed()->seed();

        EcalRecHitCollection::const_iterator seedRh = pho->recHits()->find( seedId );
        if( seedRh != pho->recHits()->end() ) {
            pho->addUserInt( "seedRecoFlag", seedRh->recoFlag() );
        } else {
            pho->addUserInt( "seedRecoFlag", -1 );
        }

        ///// // recompute maxDRCluster
        ///// //    does not seem to be set upstream
        ///// reco::CaloClusterPtr seed = pho->superCluster()->seed();
        ///// reco::CaloCluster_iterator it = pho->superCluster()->clustersBegin();
        ///// reco::CaloCluster_iterator end = pho->superCluster()->clustersEnd();
        ///// reco::CaloClusterPtr maxDRCluster;
        ///// float maxDr = 0.;
        ///// for( ; it!=end; ++it) {
        ///// 	  float dR = deltaR(seed->eta(),seed->phi(),(*it)->eta(),(*it)->phi());
        ///// 	  if( dR > maxDr ) {
        ///// 		  maxDRCluster = *it;
        ///// 	  }
        ///// }
        ///// if( maxDRCluster.isNonnull() ) {
        ///// 	  pho->setMaxDR         (deltaR(seed->eta(),seed->phi(),maxDRCluster->eta(),maxDRCluster->phi()));
        ///// 	  pho->setMaxDRDEta     (fabs(seed->eta() - maxDRCluster->eta())				);
        ///// 	  pho->setMaxDRDPhi     (deltaPhi(seed->phi(),maxDRCluster->phi())				);
        ///// 	  pho->setMaxDRRawEnergy(maxDRCluster->energy()                                                 );
        ///// }

        ////std::map<edm::Ptr<reco::Vertex>,float> pfChgIso03 = pho->getpfChgIso03();
        //// //// for(std::map<edm::Ptr<reco::Vertex>,float>::iterator it=pfChgIso03.begin(); it!=pfChgIso03.end(); ++it) {
        //// //// 	  cout << it->first.key() << " " << it->first.id() << " " << it->first->z() << " " << it->second << endl;
        //// //// }

        for( size_t iv = 0; iv < vertexes->size(); ++iv ) {
            Ptr<Vertex> vtx( vertexes, iv );
            float iso = pho->pfChgIso03WrtVtx( vtx, true );
            pho->addUserFloat( Form( "chgIsoWrtVtx%d", ( int )iv ), iso );
        }

        mvaComputer_.fill( *pho );

        fillTreeBranches( *pho, EcalBarrelRecHits.product(), EcalEndcapRecHits.product() );

        /// if( match.match == kPrompt ) {
        if( ( isSherpa_ && pho->hasMatchedGenPhoton() && pho->userFloat( "genIso" ) < 5. ) || ( pho->genMatchType() == Photon::kPrompt ) ) {
            if( iprompt_ == 0 ) {
                hists_["promptPhotonPt" ]->Fill( pho->pt(), weight_ );
                hists_["promptPhotonEta"]->Fill( pho->eta(), weight_ );
                hists_["promptPhotonPhi"]->Fill( pho->phi(), weight_ );
            }
            if( promptTree_ ) {
                promptTree_->Fill();
            }
            ++iprompt_;
        } else {
            if( ifake_ == 0 ) {
                hists_["fakePhotonPt" ]->Fill( pho->pt(), weight_ );
                hists_["fakePhotonEta"]->Fill( pho->eta(), weight_ );
                hists_["fakePhotonPhi"]->Fill( pho->phi(), weight_ );
            }
            if( fakesTree_ ) {
                fakesTree_->Fill();
            }
            ++ifake_;
        }

        hists_["photonPt" ]->Fill( pho->pt(), weight_ );
        hists_["photonEta"]->Fill( pho->eta(), weight_ );
        hists_["photonPhi"]->Fill( pho->phi(), weight_ );

        ++ipho_;
        delete pho;
    }

    hists_["promptPhotonN" ]->Fill( iprompt_, weight_ );
    hists_["fakePhotonN" ]->Fill( ifake_, weight_ );


}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

