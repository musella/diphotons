#include <map>
#include <string>

#include "TH1.h"
#include "PhysicsTools/UtilAlgos/interface/BasicAnalyzer.h"

/**
   \class PhotonIdAnalyzer PhotonIdAnalyzer.h "PhysicsTools/PatExamples/interface/PhotonIdAnalyzer.h"
   \brief Example class that can be used to analyze pat::Photons both within FWLite and within the full framework

   This is an example for keeping classes that can be used both within FWLite and within the full
   framework. The class is derived from the BasicAnalyzer base class, which is an interface for
   the two wrapper classes EDAnalyzerWrapper and FWLiteAnalyzerWrapper. You can fin more information
   on this on WorkBookFWLiteExamples#ExampleFive.
*/

namespace diphotons {

	class PhotonIdAnalyzer : public edm::BasicAnalyzer {
		
	public:
		/// default constructor
		PhotonIdAnalyzer(const edm::ParameterSet& cfg, TFileDirectory& fs);
		/// default destructor
		virtual ~PhotonIdAnalyzer();
		/// everything that needs to be done before the event loop
		void beginJob();
		/// everything that needs to be done after the event loop
		void endJob();
		/// everything that needs to be done during the event loop
		void analyze(const edm::EventBase& event);
		
		
	protected:
		float getEventWeight(const edm::EventBase& event);
		/// input tag for mouns
		edm::InputTag photons_, packedGen_, prunedGen_;
		/// histograms
		std::map<std::string, TH1*> hists_;
		// event weight
		float lumi_weight_,weight_;
		
	};
	
}
