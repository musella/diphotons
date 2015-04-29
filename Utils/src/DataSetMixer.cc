#include "../interface/DataSetMixer.h"
#include "RooRealVar.h"

#include "TTreeFormula.h"
#include "TLorentzVector.h"

#include "TRandom3.h"

#include <iostream>
#include <algorithm>
#include <random>
using namespace std;

// --------------------------------------------------------------------------------------------------------------------------------
std::string reword(const std::string & name, const char * prep, const char * strip)
{
    std::string ret(prep), nam(name), str(strip);
    size_t capitalize = ret.size();
    if( nam.find(str) == 0 ) {
        nam.erase(0,str.size());
    }
    ret += nam;
    if( ret[capitalize] >= 'a' && ret[capitalize] <= 'z' ) {
        ret[capitalize] -= 0x20;
    }
    return ret;
}

// --------------------------------------------------------------------------------------------------------------------------------
DataSetMixer::DataSetMixer(const char * name, const char * title, 
                           const RooArgList & variables1, const RooArgList & variables2,                  
                           const char * replace1, const char * replace2, 
                           float ptLeadMin, float ptSubleadMin, float massMin,                           
                           const char *weightVarName1, const char *weightVarName2, bool fillTree) :
    ptLeadMin_(ptLeadMin), ptSubleadMin_(ptSubleadMin), massMin_(massMin),
    vars1_(variables1),    vars2_(variables2), 
    tree_(0)
{
    // make sure that the two legs have the same variables
    assert( vars1_.getSize() == vars2_.getSize() || vars2_.getSize() == 0  );
    if( vars2_.getSize() == 0  ) {
        vars2_ = vars1_;
    }
    weight1_ = weightVarName1;
    weight2_ = weightVarName2;
    
    // allocate RooRealVar for leg1
    for(int ivar=0; ivar<vars1_.getSize(); ++ivar) {
        RooRealVar &var = dynamic_cast<RooRealVar &>( vars1_[ivar] );
        RooRealVar replaceVar = var;
        std::string name = var.GetName();
        replaceVar.SetTitle(name.c_str());
        if( name.find("templateNdim") == 0 ) { 
            replaceVar.SetName( "templateNdim2Dim0" ); // FIXME do not hardcode this 
        } else {
            replaceVar.SetName( reword(name,"leg1",replace1).c_str() ); // FIXME do not hardcode this 
        }
        vars_.addClone(replaceVar);
    }
    // allocate RooRealVar for leg2
    for(int ivar=0; ivar<vars2_.getSize(); ++ivar) {
        RooRealVar &var = dynamic_cast<RooRealVar &>( vars2_[ivar] );
        RooRealVar replaceVar = var;
        std::string name = var.GetName();
        replaceVar.SetTitle(name.c_str());
        if( name.find("templateNdim") == 0 ) { 
            replaceVar.SetName( "templateNdim2Dim1" ); // FIXME do not hardcode this 
        } else {
            replaceVar.SetName( reword(name,"leg2",replace2).c_str() ); // FIXME do not hardcode this 
        }
        vars_.addClone(replaceVar);
    }
    // add mass, pt and weight. FIXME: do not hard-code?
    RooRealVar mass("mass","mass",0.); vars_.addClone(mass);
    RooRealVar pt("pt","pt",0.); vars_.addClone(pt);
    RooRealVar weight("weight","weight",1.); vars_.addClone(weight);
    // done booking vars
    // vars_.Print();
    
    // book dataset
    dataset_ = new RooDataSet(name,title,RooArgSet(vars_),"weight");
    // and tree
    if( fillTree ) {
        std::string vars;
        size_t nvar = vars_.getSize();
        for(size_t ivar=0; ivar<nvar; ++ivar) {
            RooRealVar &var = dynamic_cast<RooRealVar &>( vars_[ivar] );
            if( ! vars.empty() ) { vars += ":"; }
            vars += var.GetName();
        }
        tree_ = new TNtuple(Form("tree_%s",name),Form("tree_%s",name),vars.c_str());
        // cout << vars << endl;
        treeBuf_.resize(nvar);
    }
}

// helper cache
struct Cache {
    Cache(size_t n, size_t m) : vars(n), match(m) {};
    Cache() {};
    
    TLorentzVector p4;
    std::vector<float> vars, match;
    int index;
    float weight;
    bool lead;
};

// --------------------------------------------------------------------------------------------------------------------------------
void DataSetMixer::fillFromTree(TTree * tree1, TTree * tree2,
                                const char *pT1, const char *eta1, const char *phi1, const char *energy1, 
                                const char *pT2, const char *eta2, const char *phi2, const char *energy2,
                                RooArgList matchVars, bool rndSwap, int maxEvents, float matchEff
                      )
{
    // book TTres formula for dataset variables
    size_t nvar = ( vars_.getSize() - 3 )/ 2;
    std::vector<TTreeFormula *> formulas1(nvar), formulas2(nvar);
    TTreeFormula * weight1 = (weight1_.empty() != 0 ? new TTreeFormula("weight1",weight1_.c_str(),tree1) : 0);
    TTreeFormula * weight2 = (weight2_.empty() != 0 ? new TTreeFormula("weight2",weight2_.c_str(),tree2) : 0);
    for(size_t ivar=0; ivar<nvar; ++ivar){
        RooRealVar &var1 = dynamic_cast<RooRealVar &>( vars_[ivar] );
        RooRealVar &var2 = dynamic_cast<RooRealVar &>( vars_[ivar+nvar] );
        formulas1[ivar] = new TTreeFormula( var1.GetName(), var1.GetTitle(), tree1 );
        formulas2[ivar] = new TTreeFormula( var2.GetName(), var2.GetTitle(), tree2 );
    }
    // book TTres formula for variables to be matched
    std::vector<TTreeFormula *> match1, match2;
    std::vector<float> thr;
    for(size_t ivar=0; ivar<(size_t)matchVars.getSize(); ++ivar){
        RooRealVar &var = dynamic_cast<RooRealVar &>( matchVars[ivar] );
        match1.push_back( new TTreeFormula( var.GetName(), var.GetTitle(), tree1 ) );
        match2.push_back( new TTreeFormula( var.GetName(), var.GetTitle(), tree2 ) );
        thr.push_back(var.getVal()); // interpret variable value as threshold
        // cout << var.GetTitle() << " " << thr.back() << endl;
    }
    // 4-vectors
    std::vector<TTreeFormula *> fourVec1, fourVec2;
    fourVec1.push_back(new TTreeFormula(pT1,pT1,tree1));
    fourVec1.push_back(new TTreeFormula(eta1,eta1,tree1));
    fourVec1.push_back(new TTreeFormula(phi1,phi1,tree1));
    fourVec1.push_back(new TTreeFormula(energy1,energy1,tree1));
    fourVec2.push_back(new TTreeFormula(pT2,pT2,tree2));
    fourVec2.push_back(new TTreeFormula(eta2,eta2,tree2));
    fourVec2.push_back(new TTreeFormula(phi2,phi2,tree2));
    fourVec2.push_back(new TTreeFormula(energy2,energy2,tree2));
    
    
    // actual mixing
    // Cache cache(nvar,thr.size());
    Cache cache(0,0);
    std::vector<Cache> cache1, cache2; // for caching 

    if( maxEvents < 0 ) {
        maxEvents = std::max(tree1->GetEntries(),tree2->GetEntries());
    }
    double frac1 = ((double)maxEvents) / (((double)tree1->GetEntries())*matchEff);
    double frac2 = ((double)maxEvents) / (((double)tree2->GetEntries())*matchEff);
    /// cout << tree1->GetEntries() << " " << tree2->GetEntries() << " " << frac1 << " " << frac2 << endl;
    
    // loop over 1st tree and store kinematics and variables
    cout << "DataSetMixer: loop over 1st leg ...";
    for(int iev1=0; iev1<tree1->GetEntries(); ++iev1) {
        tree1->GetEntry(iev1);
        if( gRandom->Uniform() > frac1 ) { continue; }
        float pt1 = fourVec1[0]->EvalInstance();
        if( pt1 < ptSubleadMin_ ) { continue; }
        cache.p4.SetPtEtaPhiE(pt1,fourVec1[1]->EvalInstance(),fourVec1[2]->EvalInstance(),fourVec1[3]->EvalInstance());
        cache.index = iev1;

 	cache.vars.resize(nvar);
	cache.match.resize(thr.size());
	for(size_t ivar=0; ivar<nvar; ++ivar){
                cache.vars[ivar] = formulas1[ivar]->EvalInstance();
	}
	for(size_t ivar=0; ivar<thr.size(); ++ivar){
                cache.match[ivar] = match1[ivar]->EvalInstance();
	}
        
        cache.weight = ( weight1 ? weight1->EvalInstance() : 1. );
        if( pt1 >= ptLeadMin_ ) {
            cache.lead = true;
        } else {
            cache.lead = false;
        }
        cache1.push_back(cache);
    }
    cout << "done. Selected " << cache1.size() << " entries "<< endl;
    
    // loop over 2nd tree and store kinematics and variables
    cout << "DataSetMixer: loop over 2nd leg ...";
    for(int iev2=0; iev2<tree2->GetEntries(); ++iev2) {
        tree2->GetEntry(iev2);
        if( gRandom->Uniform() > frac2 ) { continue; }
        float pt2 = fourVec2[0]->EvalInstance();
        if( pt2 < ptSubleadMin_ ) { continue; }
        cache.p4.SetPtEtaPhiE(pt2,fourVec2[2]->EvalInstance(),fourVec2[2]->EvalInstance(),fourVec2[3]->EvalInstance());
        cache.index = iev2;
        
	cache.vars.resize(nvar);
	cache.match.resize(thr.size());
	for(size_t ivar=0; ivar<nvar; ++ivar){
		cache.vars[ivar] = formulas2[ivar]->EvalInstance();
	}
	for(size_t ivar=0; ivar<thr.size(); ++ivar){
		cache.match[ivar] = match2[ivar]->EvalInstance();
	}
	
        cache.weight = ( weight2 ? weight2->EvalInstance() : 1. );
        if( pt2 >= ptLeadMin_ ) {
            cache.lead = true;
        } else {
            cache.lead = false;
        }
        cache2.push_back(cache);
    }
    cout << "done. Selected " << cache2.size() << " entries "<< endl;
    
    // make combinations
    cout << "DataSetMixer: preparing combinations ... ";
    maxEvents = std::round(((double)maxEvents)/matchEff);
    size_t npure = std::min(cache1.size(), cache2.size());
    auto engine = std::default_random_engine{};
    
    std::vector<std::pair<Cache *,Cache *>> combinations;
    while( (int)combinations.size() < maxEvents ) {
        // randomize
        std::shuffle(std::begin(cache2), std::end(cache2), engine);
        std::shuffle(std::begin(cache1), std::end(cache1), engine);
        size_t add = std::min( maxEvents - combinations.size(), npure );
        for(size_t ii=0; ii<add; ++ii) { combinations.push_back(std::make_pair(&cache1[ii],&cache2[ii]) ); }
    }
    
    // select combinations
    size_t ientry = 0;
    size_t prnt = maxEvents / 100;
    std::vector<size_t> nentries(10,0);
    cout << "  done. Prepared " << combinations.size() << " combinations " << endl;
    if( npure < combinations.size() ) { 
	    cout << "                WARNING: only " << npure << " statistically independent combinations are available " << endl; 
    }

    for(auto & cmb : combinations ) {
        auto & obj1 = *(cmb.first);
        auto & obj2 = *(cmb.second);
        if( nentries[0] % prnt == 0 ) { 
		cerr << "DataSetMixer: selecting combinations ... " << nentries[0] << "\r";
	}
        ++nentries[0];
    
        // at least one object should be above the pt threshold for the leading leg
        if( ! (obj1.lead || obj2.lead) ) { continue; }
        ++nentries[1];
        // mas cut
        TLorentzVector sum = obj1.p4 + obj2.p4;
        if( sum.M() < massMin_ ) { continue; }
        ++nentries[2];
        
        // match variables
        bool keep = true;
        for(size_t ivar=0; ivar<thr.size(); ++ivar){
            if( abs(obj1.match[ivar] - obj2.match[ivar]) > thr[ivar] ) {
                keep = false;
                break;
            }
        }
        if( ! keep ) { continue; }
        ++nentries[3];
        
        // done selecting: fill the mixed entry
        bool swap = rndSwap && ientry % 2 == 0;
        /// bool leadIs1 = obj1.p4.Pt() > obj2.p4.Pt(); 
        auto & leg1 = ( swap ? obj1 : obj2 );
        auto & leg2 = ( swap ? obj2 : obj1 );
        
        // set variables
        float weight = sqrt(obj1.weight*obj2.weight);
        for(size_t ivar=0; ivar<nvar; ++ivar) {
            if( tree_ ) {
                treeBuf_[ivar] = leg1.vars[ivar];
                treeBuf_[ivar+nvar] = leg2.vars[ivar];
            } else {
                RooRealVar & varLeg1 = dynamic_cast<RooRealVar &>( vars_[ivar] );
                RooRealVar & varLeg2 = dynamic_cast<RooRealVar &>( vars_[ivar+nvar] );
                varLeg1.setVal( leg1.vars[ivar] );
                    varLeg2.setVal( leg2.vars[ivar] );                     
            }
        }
        // fill tree or dataset
        if( tree_ ) { 
            treeBuf_[2*nvar] = sum.M();
            treeBuf_[2*nvar+1] = sum.Pt();
            treeBuf_[2*nvar+2] = weight;
            tree_->Fill( &treeBuf_[0] );
        } else { 
            dynamic_cast<RooRealVar &>(vars_[2*nvar]  ).setVal(sum.M());
            dynamic_cast<RooRealVar &>(vars_[2*nvar+1]).setVal(sum.Pt());
            dynamic_cast<RooRealVar &>(vars_[2*nvar+2]).setVal(weight);
            dataset_->add( RooArgSet(vars_), weight );
        }
        ++ientry;
    }
    cerr << endl;
    
    cout << "DataSetMixer:  Matching summary: " << ientry << " " << nentries[0] << " " << nentries[1] << " " << nentries[2] << " "  << " efficiency: "  << (float)ientry / (float)nentries[0] << endl;

    // Done. Cleanup
    for(auto & formula : formulas1 ) {
        delete formula;
    }
    for(auto & formula : formulas2 ) {
        delete formula;
    }
    for(auto & formula : match1 ) {
        delete formula;
    }
    for(auto & formula : match2 ) {
        delete formula;
    }
    for(auto & formula : fourVec1 ) {
        delete formula;
    }
    for(auto & formula : fourVec2 ) {
        delete formula;
    }
    if( weight1 ) { delete weight1; }
    if( weight2 ) { delete weight2; }
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

