/// #ifndef _diphotons_MyTriggerResults_cc_
/// #define _diphotons_MyTriggerResults_cc_

#include <map>
#include <string>

#include "TH1.h"
#include "TTree.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "PhysicsTools/UtilAlgos/interface/BasicAnalyzer.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
    
using namespace std;
using namespace edm;

namespace diphotons {

    class  MyTriggerResults : public edm::BasicAnalyzer
    {
    public:

        MyTriggerResults( const edm::ParameterSet &cfg, TFileDirectory &fs );
        MyTriggerResults( const edm::ParameterSet &cfg, TFileDirectory &fs, const edm::ConsumesCollector &cc ) : MyTriggerResults( cfg, fs ) {};
        
        /// everything that needs to be done before the event loop
        void beginJob() {};
        /// everything that needs to be done after the event loop
        void endJob() {};
        /// everything that needs to be done during the event loop
        void analyze( const edm::EventBase &event );
        

    protected:
        double eventWeight( const edm::EventBase &event );

        edm::InputTag triggerTag_, genInfo_;
        double lumiWeight_;
        double weight_;
        
        TTree *tree_;
        
        std::vector<std::pair<std::string,bool> > bits_;
        std::string ref_;
    };

    MyTriggerResults::MyTriggerResults( const ParameterSet &cfg, TFileDirectory &fs ) : BasicAnalyzer(cfg,fs), tree_(0)
    {
        genInfo_ = cfg.getParameter<InputTag>("genInfo");
        lumiWeight_ = cfg.getParameter<double>("lumiWeight");
        triggerTag_ = cfg.getParameter<InputTag>("tag");
        auto process = cfg.getParameter<string>("processId");
        if( ! process.empty() ) { process += "_"; }
        auto bitNames   = cfg.getParameter<vector<string> >("bits");
        for(auto & bit : bitNames) {
            bits_.push_back(make_pair(bit,false));
        }
        ref_ = cfg.getParameter<string>("ref");
        
        tree_ = fs.make<TTree>( Form("%striggerBits",process.c_str()), "triggerBits" );
        tree_->Branch("weight",&weight_,"weight/D");
        for(auto & bit : bits_ ) {
            tree_->Branch(bit.first.c_str(),&bit.second, (bit.first+"/O").c_str());
        }
    }
    

    void MyTriggerResults::analyze( const EventBase &event )
    {
        if( ! bits_.empty() ) {
            Handle<TriggerResults> trigResults; //our trigger result object
            event.getByLabel(triggerTag_,trigResults);
            
            for(auto & bit : bits_ ) { bit.second = false; }
            auto & trigNames = event.triggerNames(*trigResults);
            bool fill_ = true;
            for(size_t itrg=0; itrg<trigNames.size(); ++itrg) {
                auto pathName = trigNames.triggerName(itrg);
                if( pathName == ref_ && ! trigResults->accept(itrg) ) {
                    fill_ = false;
                    break; 
                }
                if( ! trigResults->accept(itrg) ) { continue; }
                for(auto & bit : bits_ ) {
                    if( pathName.find(bit.first) != string::npos ) {
                        bit.second = true;
                        break;
                    }
                }
            }
            
            if( fill_ ) { 
                weight_ = eventWeight( event );
                tree_->Fill(); 
            }
        }
    }
    
    double MyTriggerResults::eventWeight( const EventBase &event ) 
    {
        double weight = 1.;
        if( ! event.isRealData() ) {
            Handle<GenEventInfoProduct> genInfo;
                event.getByLabel( genInfo_, genInfo );
                
                weight = lumiWeight_;
                
                if( genInfo.isValid() ) {
                    const auto &weights = genInfo->weights();
                    if( ! weights.empty() ) {
                        weight *= weights[0];
                    }
                }
        }
        return weight;
    }

}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "PhysicsTools/UtilAlgos/interface/EDAnalyzerWrapper.h"

typedef AnalyzerWrapper<diphotons::MyTriggerResults> MyTriggerResults;
DEFINE_FWK_MODULE( MyTriggerResults );

// #endif  // _diphotons_MyTriggerResults_cc_

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

