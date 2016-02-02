#include <vector>
#include <map>

#include "TTree.h"

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
        float gen_z;
        vector<float> reco_z;
        vector<int> cone_dipho;
        vector<int> cone_middle;
        vector<int> cone_back;
        vector<int> trk_sum;
        Service<TFileService> fs_;
        EDGetTokenT<View<flashgg::DiPhotonCandidate> > diphoToken_;
    };

    ZeroTeslaVtxAnalyzer::ZeroTeslaVtxAnalyzer(const ParameterSet& config):
        diphoToken_(consumes<View<flashgg::DiPhotonCandidate> >(config.getParameter<InputTag>("src")))
    {
        tree_ = fs_->make<TTree>("vtxs_tree", "vtxs");
        tree_->Branch("gen_z", &gen_z, "gen_z/F");
        tree_->Branch("reco_z", "std::vector<float>", &reco_z);
        tree_->Branch("cone_dipho", "std::vector<int>", &cone_dipho);
        tree_->Branch("cone_middle", "std::vector<int>", &cone_middle);
        tree_->Branch("cone_back", "std::vector<int>", &cone_back);
        tree_->Branch("trk_sum", "std::vector<int>", &trk_sum);
    }

    void ZeroTeslaVtxAnalyzer::analyze(const Event& event, const EventSetup& setup)
    {
        reco_z.clear();
        cone_dipho.clear();
        cone_middle.clear();
        cone_back.clear();
        trk_sum.clear();
        
        Handle<View<flashgg::DiPhotonCandidate> > diphoHandle;
        event.getByToken(diphoToken_, diphoHandle);

        if(diphoHandle->size() != 0)
        {
            flashgg::DiPhotonCandidate cand0 = diphoHandle->at(0);
            for(auto& vtx : cand0.getVtxsCones())
            {                
                float vtx_z = vtx.second.first;
                vector<int> cones = vtx.second.second;
                int vtx_trk_sum = cones[0]+cones[1]+cones[2];
                cout << cones[0] << "  " << cones[1] << "  " << cones[2] << endl;
                for(unsigned int iVtx=0; iVtx<trk_sum.size(); ++iVtx)
                {
                    if(vtx_trk_sum > trk_sum[iVtx])
                    {
                        trk_sum.emplace(trk_sum.begin()+iVtx, vtx_trk_sum);
                        cone_dipho.emplace(cone_dipho.begin()+iVtx, cones[0]);
                        cone_middle.emplace(cone_middle.begin()+iVtx, cones[1]);
                        cone_back.emplace(cone_back.begin()+iVtx, cones[2]);
                        reco_z.emplace(reco_z.begin()+iVtx, vtx_z);
                        vtx_trk_sum=-1;
                        break;
                    }
                }
                if(vtx_trk_sum != -1)
                {
                    trk_sum.push_back(vtx_trk_sum);
                    cone_dipho.push_back(cones[0]);
                    cone_middle.push_back(cones[1]);
                    cone_back.push_back(cones[2]);
                    reco_z.push_back(vtx_z);
                }
            }
            tree_->Fill();
        }
    }
}

typedef diphotons::ZeroTeslaVtxAnalyzer diphotonsZeroTeslaVtxAnalyzer;
DEFINE_FWK_MODULE( diphotonsZeroTeslaVtxAnalyzer );


