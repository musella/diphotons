#include "../interface/DataSetMixer.h"

#include "../interface/FunctionHelpers.h"
#include "RooRealVar.h"

#include "TTreeFormula.h"
#include "TLorentzVector.h"

#include "TRandom3.h"
#include "TKDTree.h"
#include "TH1F.h"

#include <iostream>
#include <algorithm>
#include <random>
#include <set>

using namespace std;

// helper functions and classes
// --------------------------------------------------------------------------------------------------------------------------------
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
void fillFormulas(const RooArgList & vars, TTree * tree, std::vector<TTreeFormula *> & formulas, std::vector<float> * vals=0)
{
    size_t nvar;
    if(formulas.empty() ) {
        nvar = (size_t)vars.getSize();
        formulas.resize(nvar);
    } else {
        nvar = formulas.size();
        assert( nvar <= (size_t)vars.getSize() );
    }
    if( vals ) { vals->resize(nvar); }
    
    for(size_t ivar=0; ivar<nvar; ++ivar){
        RooRealVar &var = dynamic_cast<RooRealVar &>( vars[ivar] );
        formulas[ivar] = new TTreeFormula( var.GetName(), var.GetTitle(), tree );
        if( vals ) { (*vals)[ivar] = var.getVal(); }
    }

}

// --------------------------------------------------------------------------------------------------------------------------------
void fillFormulas(const char * A, const char * B, const char * C, const char * D, TTree * tree, 
                  std::vector<TTreeFormula *> & formulas)
{
    formulas.push_back(new TTreeFormula(A,A,tree));
    formulas.push_back(new TTreeFormula(B,B,tree));
    formulas.push_back(new TTreeFormula(C,C,tree));
    formulas.push_back(new TTreeFormula(D,D,tree));
}

// --------------------------------------------------------------------------------------------------------------------------------
void addEntry(Cache & leg1,Cache & leg2, TLorentzVector & sum, size_t nvar, RooDataSet * dataset, TNtuple * tree, RooArgList & vars, 
              std::vector<float> & treeBuf, float wei=1.)
{        
    // set variables
    //float weight = sqrt(leg1.weight*leg2.weight)*wei;
    float weight = (leg1.weight*leg2.weight)*wei;
    for(size_t ivar=0; ivar<nvar; ++ivar) {
        if( tree ) {
            treeBuf[ivar] = leg1.vars[ivar];
            treeBuf[ivar+nvar] = leg2.vars[ivar];
        } else {
            RooRealVar & varLeg1 = dynamic_cast<RooRealVar &>( vars[ivar] );
            RooRealVar & varLeg2 = dynamic_cast<RooRealVar &>( vars[ivar+nvar] );
            varLeg1.setVal( leg1.vars[ivar] );
            varLeg2.setVal( leg2.vars[ivar] );                     
        }
    }
    // fill tree or dataset
    if( tree ) { 
        treeBuf[2*nvar] = sum.M();
        treeBuf[2*nvar+1] = sum.Pt();
        treeBuf[2*nvar+2] = weight;
        tree->Fill( &treeBuf[0] );
    } else { 
        dynamic_cast<RooRealVar &>(vars[2*nvar]  ).setVal(sum.M());
        dynamic_cast<RooRealVar &>(vars[2*nvar+1]).setVal(sum.Pt());
        dynamic_cast<RooRealVar &>(vars[2*nvar+2]).setVal(weight);
        dataset->add( RooArgSet(vars), weight );
    }
}

// --------------------------------------------------------------------------------------------------------------------------------
void eval(std::vector<float> & target, std::vector<TTreeFormula *> & src)
{
    target.resize(src.size());
    for(size_t ivar=0; ivar<target.size(); ++ivar){
        target[ivar] = src[ivar]->EvalInstance();
    }
    
}

// --------------------------------------------------------------------------------------------------------------------------------
class ComparePairSecond
{
public:
    bool operator()(const std::pair<float,float> & a, const std::pair<float,float> & b) const { return a.second < b.second; }
};

typedef std::multiset<std::pair<float,float>,ComparePairSecond> sorted_dataset_type;

// --------------------------------------------------------------------------------------------------------------------------------
void fillCache(std::vector<Cache> & target, TTree *source, float frac, float ptSubleadMin, float ptLeadMin,
               std::vector<TTreeFormula *> & fourVec, 
               std::vector<TTreeFormula *> & formulas, 
               std::vector<TTreeFormula *> & match,
               TTreeFormula * weight,
               std::vector<std::vector<float>> * matchTarget=0,
               std::vector<TH1 *> * matchHisto=0
    )
{
    Cache cache(formulas.size(),(matchTarget ? 0 : match.size()));
    std::vector<float> matchCache(matchTarget ? match.size() : 0);
    std::vector<float> & matchFill( matchTarget ? matchCache : cache.match);
    std::vector<sorted_dataset_type> pdfs(matchHisto ? match.size() : 0);
    
    double totwei = 0.;
    for(int iev=0; iev<source->GetEntries(); ++iev) {
        source->GetEntry(iev);
        if( gRandom->Uniform() > frac ) { continue; }
        float pt = fourVec[0]->EvalInstance();
        if( pt < ptSubleadMin ) { continue; }
        cache.p4.SetPtEtaPhiE(pt,fourVec[1]->EvalInstance(),fourVec[2]->EvalInstance(),fourVec[3]->EvalInstance());
        cache.index = iev;

        ///// cache.vars.resize(nvar);
        ///// cache.match.resize(thr.size());
        for(size_t ivar=0; ivar<formulas.size(); ++ivar){
            cache.vars[ivar] = formulas[ivar]->EvalInstance();
        }
        for(size_t ivar=0; ivar<match.size(); ++ivar) {
            /// cache.match[ivar] = match[ivar]->EvalInstance();
            matchFill[ivar] = match[ivar]->EvalInstance();
        }
        
        cache.weight = ( weight ? weight->EvalInstance() : 1. );
        totwei += cache.weight;
        if( pt >= ptLeadMin ) {
            cache.lead = true;
        } else {
            cache.lead = false;
        }
        target.push_back(cache);
        if( matchTarget ) {
            for(size_t im=0; im<matchFill.size(); ++im) {
                (*matchTarget)[im].push_back(matchFill[im]);
            }
        }
        if( ! pdfs.empty() ) {
            for(size_t im=0; im<matchFill.size(); ++im) {
                pdfs[im].insert(make_pair(cache.weight,matchFill[im]));
            }
        }
    }
    
    if( ! pdfs.empty() ) {
        std::vector<std::vector<float>> bins(pdfs.size());
        std::vector<sorted_dataset_type::iterator> idx(pdfs.size());
        std::vector<float> count(pdfs.size(),0.);
        float res = 1.e-2;
        float step = totwei * res;        
        for(size_t ip=0; ip<pdfs.size(); ++ip) {
            idx[ip] = pdfs[ip].begin();
            bins[ip].push_back(idx[ip]->second);
        }
        for(float prob=step; prob<totwei; prob+=step) {
            for(size_t ip=0; ip<pdfs.size(); ++ip) {
                while(count[ip] < prob && idx[ip] != pdfs[ip].end()) {
                    count[ip] += idx[ip]->first;                    
                    ++idx[ip];
                }
                if( idx[ip] != pdfs[ip].end()) {
                    bins[ip].push_back(idx[ip]->second);
                }
            }
        }
        for(size_t ip=0; ip<pdfs.size(); ++ip) {
            bins[ip].push_back(pdfs[ip].rbegin()->second);
            TH1 * h = new TH1F(Form("h_pdf_%s_%lx",match[ip]->GetTitle(),(long unsigned int)matchHisto),Form("h_pdf_%s",match[ip]->GetTitle()),bins[ip].size()-1,&bins[ip][0]);
            (*matchHisto)[ip] = h;
            // h->SetDirectory(0);
            for(auto bin : bins[ip]) {
                h->Fill(bin,res);
            }
        }
    }
}

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


// Actual DataSetMixer implementation
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

// --------------------------------------------------------------------------------------------------------------------------------
void DataSetMixer::fillLikeTarget(TTree * target,
                                  const RooArgList & targetMatchVars1, const RooArgList & targetMatchVars2, 
                                  std::string  targetWeight,
                                  TTree * tree1, TTree * tree2,
                                  const char *pT1, const char *eta1, const char *phi1, const char *energy1, 
                                  const char *pT2, const char *eta2, const char *phi2, const char *energy2,
                                  const RooArgList & matchVars1, const RooArgList & matchVars2,
                                  bool rndSwap,float rndMatch, int nNeigh, bool useCdfDistance
        )
{
    
    size_t nvar = ( vars_.getSize() - 3 )/ 2;
    std::vector<TTreeFormula *> formulas1(nvar), formulas2(nvar);
    TTreeFormula * weight1      = (!weight1_.empty() ? new TTreeFormula("weight1",weight1_.c_str(),tree1) : 0);
    TTreeFormula * weight2      = (!weight2_.empty()? new TTreeFormula("weight2",weight2_.c_str(),tree2) : 0);
    TTreeFormula * weightTarget = (!targetWeight.empty() ? new TTreeFormula("weightTarget",targetWeight.c_str(),target) : 0);
    /// cout << targetWeight << " " << weightTarget << endl;
    fillFormulas(vars_,tree1,formulas1);
    fillFormulas(vars_,tree2,formulas2);

    // book TTres formula for variables to be matched
    std::vector<TTreeFormula *> match1, match2, targetMatch1, targetMatch2;
    fillFormulas(matchVars1,tree1,match1);
    fillFormulas(matchVars2,tree2,match2);
    fillFormulas(targetMatchVars1,target,targetMatch1);
    fillFormulas(targetMatchVars2,target,targetMatch2);
    
    // 4-vectors
    std::vector<TTreeFormula *> fourVec1, fourVec2;
    fillFormulas(pT1,eta1,phi1,energy1,tree1,fourVec1);
    fillFormulas(pT2,eta2,phi2,energy2,tree2,fourVec2);
    
    // actual mixing
    std::vector<Cache> cache1, cache2; // for caching 
    std::vector<std::vector<float>> cacheMatch1(matchVars1.getSize()), cacheMatch2(matchVars2.getSize());
    std::vector<TH1 *> matchHisto1(matchVars1.getSize(),0), matchHisto2(matchVars2.getSize(),0);
    
    // loop over 1st tree and store kinematics and variables
    cout << "DataSetMixer: loop over 1st tree/leg ...";
    fillCache(cache1,tree1,1.,ptSubleadMin_,ptLeadMin_,fourVec1,formulas1,match1,weight1,&cacheMatch1,(useCdfDistance?&matchHisto1:0));
    cout << "done. Selected " << cache1.size() << " entries "<< endl;
    
    // loop over 2nd tree and store kinematics and variables
    cout << "DataSetMixer: loop over 2nd tree/leg ...";
    fillCache(cache2,tree2,1.,ptSubleadMin_,ptLeadMin_,fourVec2,formulas2,match2,weight2,&cacheMatch2,(useCdfDistance?&matchHisto2:0));
    cout << "done. Selected " << cache2.size() << " entries "<< endl;
	
    std::vector<HistoConverter *> cdfs1, cdfs2;
    if( useCdfDistance ) { 
        for(size_t idim=0; idim<matchHisto1.size(); ++idim) {
            cdfs1.push_back( cdf(matchHisto1[idim],matchHisto1[idim]->GetXaxis()->GetXmin(),matchHisto1[idim]->GetXaxis()->GetXmax()) );
        }
        for(size_t idim=0; idim<matchHisto2.size(); ++idim) {
            cdfs2.push_back( cdf(matchHisto2[idim],matchHisto2[idim]->GetXaxis()->GetXmin(),matchHisto2[idim]->GetXaxis()->GetXmax()) );
        }
    }

    TKDTreeIF *kdtree1 = new TKDTreeIF(cache1.size(),cacheMatch1.size(),1), *kdtree2 = new TKDTreeIF(cache2.size(),cacheMatch2.size(),1);
    cout << "DataSetMixer: building kD-trees ...";
    for(size_t idim=0; idim<cacheMatch1.size(); ++idim) { 
        if( useCdfDistance ) {
            for(auto & val : cacheMatch1[idim] ) {
                val = cdfs1[idim]->eval(val);
            }
        }
        kdtree1->SetData(idim,&cacheMatch1[idim][0]); 
    }
    for(size_t idim=0; idim<cacheMatch2.size(); ++idim) {
        if( useCdfDistance ) {
            for(auto & val : cacheMatch2[idim] ) {
                val = cdfs2[idim]->eval(val);
            }
        }
        kdtree2->SetData(idim,&cacheMatch2[idim][0]); 
    }
    kdtree1->Build();
    kdtree2->Build();
    cout << "done" << endl;
    
    // loop over target to fill template
    std::vector<float> target1, target2;
    std::vector<int>   neigh1(nNeigh), neigh2(nNeigh);
    std::vector<float> dist1(nNeigh), dist2(nNeigh);
    size_t ientry = 0;
    
    int maxwarn = 10, nwarn = 0;
    auto engine = std::default_random_engine{};
    for(int iev=0; iev<target->GetEntries(); ++iev) {
        target->GetEntry(iev);
        eval(target1,targetMatch1);
        eval(target2,targetMatch2);

        // FIXME decouple swap in matching and template filling
        //bool swap = rndSwap && ientry % 2 == 0;
        //bool rndmatch = rndMatch && ientry % 2 == 0;
        TKDTreeIF *mtree1=kdtree1, *mtree2=kdtree2;
        std::vector<Cache> *mcache1=&cache1, *mcache2=&cache2; 
        //bool swap = (rndMatch ) && ientry % 2 == 0;
        bool swap = (rndMatch!=0.0? gRandom->Uniform()> rndMatch: false );
        if( swap ) {
            std::swap(mtree1,mtree2);
            std::swap(mcache1,mcache2);
        }
        
        float wei = ( weightTarget != 0 ? weightTarget->EvalInstance() : 1. );
        if( useCdfDistance ) {
            for(size_t idim=0; idim<target1.size(); ++idim) {
                target1[idim] = cdfs1[idim]->eval(target1[idim]);
                target2[idim] = cdfs2[idim]->eval(target2[idim]);
            }
        }
        
        mtree1->FindNearestNeighbors(&target1[0],nNeigh,&neigh1[0],&dist1[0]);
        mtree2->FindNearestNeighbors(&target2[0],nNeigh,&neigh2[0],&dist2[0]);
        
        std::shuffle(std::begin(neigh1), std::end(neigh1), engine);
        std::shuffle(std::begin(neigh2), std::end(neigh2), engine);
        
        for(int ip=0; ip<nNeigh; ++ip) {
            if( neigh1[ip] < 0 || neigh2[ip] < 0 ) {
                if( nwarn++ < maxwarn ) {
                    cout << "Warning negative index for neighbour " << neigh1[ip] << " " << neigh2[ip] << endl;
                }
                continue;
            }
            auto & obj1 = (*mcache1)[neigh1[ip]];
            auto & obj2 = (*mcache2)[neigh2[ip]];
            
            bool reswap = rndSwap && gRandom->Uniform()>=0.5;
            auto & leg1 = ( reswap ? obj1 : obj2 );
            auto & leg2 = ( reswap ? obj2 : obj1 );

            TLorentzVector sum = obj1.p4 + obj2.p4;
            addEntry(leg1,leg2,sum,nvar,dataset_,tree_,vars_,treeBuf_,wei);        
            ++ientry;
        }
    }
    cout << "DataSetMixer:  Matching summary: target " << target->GetEntries() << " accepted entries " << ientry << " invalid neighbours " << nwarn
         << endl;
    
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
    for(auto & formula : targetMatch1 ) {
        delete formula;
    }
    for(auto & formula : targetMatch2 ) {
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
    if( weightTarget ) { delete weightTarget; }
    for( auto & h : matchHisto1 ) { 
        if( h ) delete h;
    }
    for( auto & h : matchHisto2 ) { 
        if( h ) delete h;
    }
    delete kdtree1;
    delete kdtree2,
    cout << "here" << endl;
}

// --------------------------------------------------------------------------------------------------------------------------------
void DataSetMixer::fillFromTree(TTree * tree1, TTree * tree2,
                                const char *pT1, const char *eta1, const char *phi1, const char *energy1, 
                                const char *pT2, const char *eta2, const char *phi2, const char *energy2,
                                const RooArgList & matchVars, bool rndSwap, int maxEvents, float matchEff
                      )
{
    // book TTres formula for dataset variables
    size_t nvar = ( vars_.getSize() - 3 )/ 2;
    std::vector<TTreeFormula *> formulas1(nvar), formulas2(nvar);
    TTreeFormula * weight1 = (! weight1_.empty() ? new TTreeFormula("weight1",weight1_.c_str(),tree1) : 0);
    TTreeFormula * weight2 = (! weight2_.empty() ? new TTreeFormula("weight2",weight2_.c_str(),tree2) : 0);
    fillFormulas(vars_,tree1,formulas1);
    fillFormulas(vars_,tree2,formulas2);

    // book TTres formula for variables to be matched
    std::vector<TTreeFormula *> match1, match2;
    std::vector<float> thr;
    fillFormulas(matchVars,tree1,match1,&thr);
    fillFormulas(matchVars,tree2,match2);

    // 4-vectors
    std::vector<TTreeFormula *> fourVec1, fourVec2;
    fillFormulas(pT1,eta1,phi1,energy1,tree1,fourVec1);
    fillFormulas(pT2,eta2,phi2,energy2,tree2,fourVec2);
        
    // actual mixing
    std::vector<Cache> cache1, cache2; // for caching 

    if( maxEvents < 0 ) {
        maxEvents = std::max(tree1->GetEntries(),tree2->GetEntries());
    }
    double frac1 = ((double)maxEvents) / (((double)tree1->GetEntries())*matchEff);
    double frac2 = ((double)maxEvents) / (((double)tree2->GetEntries())*matchEff);
    /// cout << tree1->GetEntries() << " " << tree2->GetEntries() << " " << frac1 << " " << frac2 << endl;
    
    // loop over 1st tree and store kinematics and variables
    cout << "DataSetMixer: loop over 1st leg ...";
    fillCache(cache1,tree1,frac1,ptSubleadMin_,ptLeadMin_,fourVec1,formulas1,match1,weight1);
    cout << "done. Selected " << cache1.size() << " entries "<< endl;
    
    // loop over 2nd tree and store kinematics and variables
    cout << "DataSetMixer: loop over 2nd leg ...";
    fillCache(cache2,tree2,frac2,ptSubleadMin_,ptLeadMin_,fourVec2,formulas2,match2,weight2);
    cout << "done. Selected " << cache2.size() << " entries "<< endl;
    
    // make combinations
    cout << "DataSetMixer: preparing combinations ... ";
    maxEvents = std::round(((double)maxEvents)/matchEff);
    size_t nindep = std::min(cache1.size(), cache2.size());
    std::vector<std::pair<Cache *,Cache *>> combinations;
    
    // randomize
    auto engine = std::default_random_engine{};
    while( (int)combinations.size() < maxEvents ) {
        std::shuffle(std::begin(cache2), std::end(cache2), engine);
        std::shuffle(std::begin(cache1), std::end(cache1), engine);
        size_t add = std::min( maxEvents - combinations.size(), nindep );
        for(size_t ii=0; ii<add; ++ii) { combinations.push_back(std::make_pair(&cache1[ii],&cache2[ii]) ); }
    }
    
    // select combinations
    size_t ientry = 0;
    size_t prnt = maxEvents / 100;
    std::vector<size_t> nentries(10,0);
    cout << "  done. Prepared " << combinations.size() << " combinations " << endl;
    if( nindep < combinations.size() ) { 
	    cout << "                WARNING: only " << nindep << " statistically independent combinations are available " << endl; 
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
        
        addEntry(leg1,leg2,sum,nvar,dataset_,tree_,vars_,treeBuf_);        
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

