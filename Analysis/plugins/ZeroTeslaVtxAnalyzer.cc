#include <vector>
#include <map>

#include "TTree.h"
#include "TRandom.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Handle.h"

#include "flashgg/DataFormats/interface/DiPhotonCandidate.h"

using namespace std;
using namespace edm;

namespace diphotons
{
    class ZeroTeslaVtxAnalyzer : public EDAnalyzer {
    public:
        //---constructor from parameter set
        ZeroTeslaVtxAnalyzer(const ParameterSet&);
        //---destructor
        ~ZeroTeslaVtxAnalyzer(){};
        
    protected:
        //---process an event
        virtual void analyze(const Event&, const EventSetup&) override;

    private:
        TTree* tree_;
        float gen_z=0;
        float m=0;
        float m_sm=0;
        int best_trk=0;
        int best_trk_away=0;
        vector<int> original_id;
        vector<float> reco_z;
        vector<float> sum_et2;
        vector<int>   cone_dipho;
        vector<int>   cone_middle;
        vector<int>   cone_back;
        Service<TFileService> fs_;
        EDGetTokenT<View<flashgg::DiPhotonCandidate> > diphoToken_;
    };

    ZeroTeslaVtxAnalyzer::ZeroTeslaVtxAnalyzer(const ParameterSet& config):
        diphoToken_(consumes<View<flashgg::DiPhotonCandidate> >(config.getParameter<InputTag>("src")))
    {
        tree_ = fs_->make<TTree>("vtxs_tree", "vtxs");
        tree_->Branch("gen_z", &gen_z, "gen_z/F");
        tree_->Branch("m", &m, "m/F");
        tree_->Branch("m_sm", &m_sm, "m_sm/F");
        tree_->Branch("best_trk", &best_trk, "best_trk/I");
        tree_->Branch("best_trk_away", &best_trk_away, "best_trk_away/I");
        tree_->Branch("original_id", "std::vector<int>", &original_id);
        tree_->Branch("reco_z", "std::vector<float>", &reco_z);
        tree_->Branch("sum_et2", "std::vector<float>", &sum_et2);
        tree_->Branch("cone_dipho", "std::vector<int>", &cone_dipho);
        tree_->Branch("cone_middle", "std::vector<int>", &cone_middle);
        tree_->Branch("cone_back", "std::vector<int>", &cone_back);
    }

    void ZeroTeslaVtxAnalyzer::analyze(const Event& event, const EventSetup& setup)
    {
        int most_trk=0;
        int most_trk_away=0;
        best_trk=0;
        best_trk_away=0;
        original_id.clear();
        reco_z.clear();
        sum_et2.clear();
        cone_dipho.clear();
        cone_middle.clear();
        cone_back.clear();
        
        
        Handle<View<flashgg::DiPhotonCandidate> > diphoHandle;
        event.getByToken(diphoToken_, diphoHandle);

        if(diphoHandle->size() != 0)
        {
            flashgg::DiPhotonCandidate cand0 = diphoHandle->at(0);
            m = cand0.mass();
            gen_z = cand0.genPV().z();
            int current_vtx=-1;

            if(fabs(cand0.leadingPhoton()->eta()) < 1)
                m_sm = m * sqrt(gRandom->Gaus(1, 0.008));
            else if(fabs(cand0.leadingPhoton()->eta()) < 1.5)
                m_sm = m * sqrt(gRandom->Gaus(1, 0.0115));
            else if(fabs(cand0.leadingPhoton()->eta()) < 2)
                m_sm = m * sqrt(gRandom->Gaus(1, 0.0201));
            else
                m_sm = m * sqrt(gRandom->Gaus(1, 0.023));

            if(fabs(cand0.subLeadingPhoton()->eta()) < 1)
                m_sm = m * sqrt(gRandom->Gaus(1, 0.008));
            else if(fabs(cand0.subLeadingPhoton()->eta()) < 1.5)
                m_sm = m * sqrt(gRandom->Gaus(1, 0.0115));
            else if(fabs(cand0.subLeadingPhoton()->eta()) < 2)
                m_sm = m * sqrt(gRandom->Gaus(1, 0.0201));
            else
                m_sm = m * sqrt(gRandom->Gaus(1, 0.023));
            
            for(auto& vtx : cand0.getVtxs())
            {
                ++current_vtx;
                float vtx_z = vtx->z();
                float vtx_sum_et2 = cand0.getVtxSumEt2(vtx);
                vector<int> cones = cand0.getVtxCones(vtx);
                int tmp_trk_away = cones[1] + cones[2];
                int tmp_trk = cones[0] + tmp_trk_away;
                int insert_pos=-1;
                for(unsigned int iVtx=0; iVtx<sum_et2.size(); ++iVtx)
                {
                    if(vtx_sum_et2 > sum_et2[iVtx])
                    {
                        original_id.emplace(original_id.begin()+iVtx, current_vtx);
                        sum_et2.emplace(sum_et2.begin()+iVtx, vtx_sum_et2);
                        reco_z.emplace(reco_z.begin()+iVtx, vtx_z);
                        cone_dipho.emplace(cone_dipho.begin()+iVtx, cones[0]);
                        cone_middle.emplace(cone_middle.begin()+iVtx, cones[1]);
                        cone_back.emplace(cone_back.begin()+iVtx, cones[2]);
                        insert_pos=iVtx;
                        break;
                    }
                }
                if(insert_pos == -1)
                {
                    original_id.push_back(current_vtx);
                    sum_et2.push_back(vtx_sum_et2);
                    reco_z.push_back(vtx_z);
                    cone_dipho.push_back(cones[0]);
                    cone_middle.push_back(cones[1]);
                    cone_back.push_back(cones[2]);
                    insert_pos = sum_et2.size()-1;
                }
                if(tmp_trk > most_trk)
                {
                    most_trk = tmp_trk;
                    best_trk = insert_pos;
                }
                if(tmp_trk_away > most_trk_away)
                {
                    most_trk_away = tmp_trk_away;
                    best_trk_away = insert_pos;
                }
            }
            tree_->Fill();
        }
    }
}

typedef diphotons::ZeroTeslaVtxAnalyzer diphotonsZeroTeslaVtxAnalyzer;
DEFINE_FWK_MODULE( diphotonsZeroTeslaVtxAnalyzer );


