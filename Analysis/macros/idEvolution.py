#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option

from copy import copy
from pprint import pprint
import csv,os,json
json.encoder.FLOAT_REPR = lambda o: format(o, '.3f')

from math import sqrt,fabs

import array

# ------------------------------------------------------------------------------------------
class IdEvolution(PlotApp):

    def __init__(self):
        super(IdEvolution,self).__init__(
            ### default_cats=["isEB := abs(scEta)<1.5"],
            default_cats=["isEB := abs(scEta)<1.5","isEE := abs(scEta)>1.5"],
            ### default_cats=["innerEB  := abs(scEta)<0.9",
            ###               "outerEB  := abs(scEta)>=0.9 && abs(scEta)<1.5",
            ###               "innerEE  := abs(scEta)>=1.5 && abs(scEta)<2.",
            ###               ## "middleEE := abs(scEta)>=2. && abs(scEta)<2.2",
            ###               ## "outerEE  := abs(scEta)>=2.2 && abs(scEta)<2.5"
            ###               "outerEE  := abs(scEta)>=2. && abs(scEta)<2.5"
            ###               ],
            option_list=[make_option("--aliases",dest="aliases",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",
                                     default=[
                        ### "phoIsoVeto005EA      := 0.31*(abs(scEta)>=2.2 && abs(scEta)<2.5)+0.21*(abs(scEta)<0.9)+0.22*(abs(scEta)>=2. && abs(scEta)<2.2)+0.14*(abs(scEta)>=1.5 && abs(scEta)<2.)+0.2*(abs(scEta)>=0.9 && abs(scEta)<1.5)", 
                        ### "phoIsoBlockEA        := 0.3*(abs(scEta)>=2.2 && abs(scEta)<2.5)+0.21*(abs(scEta)<0.9)+0.21*(abs(scEta)>=2. && abs(scEta)<2.2)+0.12*(abs(scEta)>=1.5 && abs(scEta)<2.)+0.2*(abs(scEta)>=0.9 && abs(scEta)<1.5)", 
                        ### "neuIsoVeto015EA      := 0.2*(abs(scEta)>=2.2 && abs(scEta)<2.5)+0.04*(abs(scEta)<0.9)+0.021*(abs(scEta)>=2. && abs(scEta)<2.2)+0.063*(abs(scEta)>=1.5 && abs(scEta)<2.)+0.019*(abs(scEta)>=0.9 && abs(scEta)<1.5)", 
                        ### "egNeutralHadronIsoEA := 0.15*(abs(scEta)>=2.2 && abs(scEta)<2.5)+0.04*(abs(scEta)<0.9)+0.037*(abs(scEta)>=2. && abs(scEta)<2.2)+0.046*(abs(scEta)>=1.5 && abs(scEta)<2.)+0.059*(abs(scEta)>=0.9 && abs(scEta)<1.5)"
                        
                        
                        ## "phoIsoVeto005EA          := 0.13*(abs(scEta)>=2.2 && abs(scEta)<2.5)+0.095*(abs(scEta)<0.9)+0.07*(abs(scEta)>=2. && abs(scEta)<2.2)+0.038*(abs(scEta)>=1.5 && abs(scEta)<2.)+0.086*(abs(scEta)>=0.9 && abs(scEta)<1.5)", 
                        ## "phoIsoBlockEA            := 0.13*(abs(scEta)>=2.2 && abs(scEta)<2.5)+0.095*(abs(scEta)<0.9)+0.065*(abs(scEta)>=2. && abs(scEta)<2.2)+0.034*(abs(scEta)>=1.5 && abs(scEta)<2.)+0.086*(abs(scEta)>=0.9 && abs(scEta)<1.5)", 
                        ## "neuIsoVeto015EA          := 0.00031*(abs(scEta)>=2.2 && abs(scEta)<2.5)+1.6e-05*(abs(scEta)<0.9)+-0.00074*(abs(scEta)>=2. && abs(scEta)<2.2)+-0.00044*(abs(scEta)>=1.5 && abs(scEta)<2.)+8.4e-05*(abs(scEta)>=0.9 && abs(scEta)<1.5)", 
                        ## "egNeutralHadronIsoEA     := 0.00016*(abs(scEta)>=2.2 && abs(scEta)<2.5)+3.6e-05*(abs(scEta)<0.9)+-0.00037*(abs(scEta)>=2. && abs(scEta)<2.2)+-0.00022*(abs(scEta)>=1.5 && abs(scEta)<2.)+0.00011*(abs(scEta)>=0.9 && abs(scEta)<1.5)"

                        ]),
                         make_option("--efficiencies",dest="efficiencies",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",
                                     default=[]),
                         make_option("--variables",dest="variables",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",
                                     default=[
                        "absScEta := scEta*((scEta>=0.)-(scEta<0.))[100,0.,2.5]",
                        "pt[500,70,1000]",
                        ### "rho[10000,0,100]",
                        
                        ### "phoIsoDeltaMap005 := phoIsoHybrid005*(phoIsoHybrid005<49)+49.*(phoIsoHybrid005>=49)[1000,-10,50]",
                        ### "phoIsoDeltaMap007 := phoIsoHybrid007*(phoIsoHybrid007<49)+49.*(phoIsoHybrid007>=49)[1000,-10,50]",
                        ### "phoIsoDeltaMap008 := phoIsoHybrid008*(phoIsoHybrid008<49)+49.*(phoIsoHybrid008>=49)[1000,-10,50]",
                        ### "phoIsoDeltaMap010 := phoIsoHybrid010*(phoIsoHybrid010<49)+49.*(phoIsoHybrid010>=49)[1000,-10,50]",
                        ### "phoIsoDeltaMap020 := phoIsoHybrid020*(phoIsoHybrid020<49)+49.*(phoIsoHybrid020>=49)[1000,-10,50]",
                        
                        #### "relPhoIsoDeltaMapd005 := phoIsoHybrid005/pt[1000,-0.2,1.2]",
                        #### "relPhoIsoDeltaMapd007 := phoIsoHybrid007/pt[1000,-0.2,1.2]",
                        #### "relPhoIsoDeltaMapd008 := phoIsoHybrid008/pt[1000,-0.2,1.2]",
                        #### "relPhoIsoDeltaMapd010 := phoIsoHybrid010/pt[1000,-0.2,1.2]",
                        #### "relPhoIsoDeltaMapd020 := phoIsoHybrid020/pt[1000,-0.2,1.2]",
                        
                        #### "phoIsoVeto005[1000,-10,50]",
                        #### "phoIsoVeto006[1000,-10,50]",
                        #### "phoIsoVeto007[1000,-10,50]",
                        #### "phoIsoVeto008[1000,-10,50]",
                        #### "relPhoIsoVeto005 := phoIsoVeto005/pt[1000,-0.2,1.2]",
                        #### 
                        #### "neuIsoBlockVeto010[1000,-10,50]",
                        #### "neuIsoVeto010[1000,-10,50]",
                        #### 
                        #### "neuIsoBlockVeto015[1000,-10,50]",
                        #### "neuIsoVeto015[1000,-10,50]",
                        #### 
                        #### "chgIsoWrtVtx0[1000,-10,50.]",
                        #### "chgIsoWrtWorstVtx[1000,-10,50.]",
                        #### 
                        #### "egPhotonIso[1000,-10,50]",
                        #### "egChargedHadronIso[1000,-10,50]",
                        #### "egNeutralHadronIso[1000,-10,50]",
                        #### "relEgPhotonIso := egPhotonIso/pt[1000,-0.2,1.2]",
                        #### "relEgChargedHadronIso := egChargedHadronIso/pt[1000,-0.2,1.2]",
                        #### "relEgNeutralHadronIso := egNeutralHadronIso   /pt[1000,-0.2,1.2]",
                        
                        

                        ### "phoIsoVeto005[1000,-10,50]",
                        ### "phoIsoBlock[1000,-10,50]",
                        ### 
                        ### "neuIsoVeto015 := neuIsoRing020 + neuIsoRing030[1000,-10,100]",
                        ### "egNeutralHadronIso[1000,-10,50]",
                        ### 
                        ### "chgIsoWrtVtx0[1000,-10,50.]",
                        ### "chgIsoWrtWorstVtx[1000,-10,50.]",
                        
                        ### "eScale := energy/etrue[501,0.501,1.501]",
                        ### "eScale5x5 := e5x5/etrue[501,0.501,1.501]"

                        ### "egPhotonIso[1000,-10,50]",
                        ### "egChargedHadronIso[1000,-10,50]",
                        ### "egNeutralHadronIso[1000,-10,50]",
                        ### "relEgPhotonIso := egPhotonIso/pt[1000,-0.2,1.2]",
                        ### "relEgChargedHadronIso := egChargedHadronIso/pt[1000,-0.2,1.2]",
                        ### "relEgNeutralHadronIso := egNeutralHadronIso   /pt[1000,-0.2,1.2]",
                        ### 
                        ### "chgIsoWrtVtx0[1000,-10,50.]",
                        ### "chgIsoWrtWorstVtx[1000,-10,50.]",
                        ### 
                        ### "phoIsoBlock[1000,-10,50]",
                        ### "phoIsoDelta005 := phoIsoVeto005-phoIsoBlockVeto005[1000,-10,50]",
                        ### "phoIsoVeto005[1000,-10,50]",
                        ### "phoIsoVeto006[1000,-10,50]",
                        ### "phoIsoVeto007[1000,-10,50]",
                        ### "phoIsoVeto008[1000,-10,50]",
                        ### 
                        ### "relPhoIsoBlock   := phoIsoBlock/pt[1000,-0.2,1.2]",
                        ### "relPhoIsoDelta005:= phoIsoDelta005/pt[1000,-0.2,1.2]",
                        ### "relPhoIsoVeto005 := phoIsoVeto005/pt[1000,-0.2,1.2]",
                        ### "relPhoIsoVeto006 := phoIsoVeto006/pt[1000,-0.2,1.2]",
                        ### "relPhoIsoVeto007 := phoIsoVeto007/pt[1000,-0.2,1.2]",
                        ### "relPhoIsoVeto008 := phoIsoVeto008/pt[1000,-0.2,1.2]",
                        ###                     
                        ### "neuIsoBlock[1000,-10,50]",
                        ### "neuIsoBlockVeto010 := neuIsoBlock[1000,-10,50]",
                        ### "neuIsoBlockVeto012 := neuIsoBlock-neuIsoBlockRing012[1000,-10,50]",
                        ### "neuIsoBlockVeto015 := neuIsoBlock-neuIsoBlockRing012-neuIsoBlockRing015[1000,-10,50]",
                        ### "neuIsoBlockVeto020 := neuIsoBlock-neuIsoBlockRing012-neuIsoBlockRing015-neuIsoBlockRing020[1000,-10,50]",
                        
                        ### ### "neuIso := neuIsoRing012 + neuIsoRing015 + neuIsoRing020 + neuIsoRing030[1000,-10,50]",
                        ### "neuIsoVeto010 := neuIsoRing012 + neuIsoRing015 + neuIsoRing020 + neuIsoRing030[1000,-10,100]",
                        ### "neuIsoVeto012 := neuIsoRing015 + neuIsoRing020 + neuIsoRing030[1000,-10,100]",
                        ### "neuIsoVeto015 := neuIsoRing020 + neuIsoRing030[1000,-10,100]",
                        ### "neuIsoVeto020 := neuIsoRing030+0.0[1000,-10,100]",
                        ### 
                        ### ## "neuIsoBlock := neuIsoBlockRing012 + neuIsoBlockRing015 + neuIsoBlockRing020 + neuIsoBlockRing030[1000,-10,50]",
                        ### "neuIsoBlockVeto010 := neuIsoBlockRing012 + neuIsoBlockRing015 + neuIsoBlockRing020 + neuIsoBlockRing030[1000,-10,100]",
                        ### "neuIsoBlockVeto012 := neuIsoBlockRing015 + neuIsoBlockRing020 + neuIsoBlockRing030[1000,-10,100]",
                        ### "neuIsoBlockVeto015 := neuIsoBlockRing020 + neuIsoBlockRing030[1000,-10,100]",
                        ### "neuIsoBlockVeto020 := neuIsoBlockRing030[1000,-10,100]",
                        ### 
                        ### "relNeuIsoVeto010 := neuIsoVeto010/pt[1000,-0.2,1.2]",
                        ### "relNeuIsoVeto012 := neuIsoVeto012/pt[1000,-0.2,1.2]",
                        ### "relNeuIsoVeto015 := neuIsoVeto015/pt[1000,-0.2,1.2]",
                        ### "relNeuIsoVeto020 := neuIsoVeto020/pt[1000,-0.2,1.2]",
                        ### 
                        ### "relNeuIsoBlockVeto010 := neuIsoBlockVeto010/pt[1000,-0.2,1.2]",
                        ### "relNeuIsoBlockVeto012 := neuIsoBlockVeto012/pt[1000,-0.2,1.2]",
                        ### "relNeuIsoBlockVeto015 := neuIsoBlockVeto015/pt[1000,-0.2,1.2]",
                        ### "relNeuIsoBlockVeto020 := neuIsoBlockVeto020/pt[1000,-0.2,1.2]",
                        

                        ### "hadronicOverEm[1000,0,0.2]","hadronicDepth1OverEm[1000,0,0.2]","hadronicDepth2OverEm[1000,0,0.1]",
                        ### "hadTowOverEm[1000,0,0.2]",


                        ### "passPixelVeto := !hasPixelSeed[2,0,2]-","passElectronVeto[2,0,2]-",
                        ### "sigmaIetaIeta[5000,0.,5.e-2]",
                        ### "r9[5000,0.,1.1]-","r1x5[5000,0.,1.1]-","r2x5[5000,0.,1.1]-",
                        ### "r1x5[5000,0.,1.1]-",
                        ### "r1 := e1x5/e5x5[5000,0.,1.]-",
                        ### "s4[5000,0.,1.]-",

                        
                        #### ### ### ### 
                        #### ### ### ### ## "maxDR[1000,0.,0.2]",
                        #### ### ### ### ## "maxDRERatio := maxDRRawEnergy/energy[5000,0.,1.]",
                        #### ### ### ### 
                        #### ### 

                                              ],
                                     ### default=["sigmaIetaIeta","r1x5","r2x5","r9",
                                     ###          "hadronicOverEm","hadronicDepth1OverEm","hadronicDepth2OverEm",
                                     ###          "r1 := e1x5/e5x5","maxDR","maxDRERatio := maxDRRawEnergy/energy",
                                     ###          "s4"
                                     ###          ],
                                     help="default: %default"),
                         make_option("--xvar",dest="xvar",action="store",type="string",
                                     default="pt[100000,70,1000]",
                                     ## default="pt[100000,100,2000]",
                                     ## default="rho[1000,5,15]",
                                     help="default: %default"),
                         make_option("--binningMaxErr",dest="binningMaxErr",action="store",type="float",
                                     default=5.e-2,
                                     ## default="pt[100000,100,2000]",
                                     ## default="rho[1000,5,15]",
                                     help="default: %default"),
                         make_option("--binningTarget",dest="binningTarget",action="store",type="int",
                                     default=30,
                                     ## default="pt[100000,100,2000]",
                                     ## default="rho[1000,5,15]",
                                     help="default: %default"),
                         make_option("-w","--weight",dest="weight",action="store",type="string",
                                     default="weight", help="default: %default"
                                     ),
                         make_option("--weights",dest="weights",action="callback",type="string",
                                     callback=optpars_utils.ScratchAppend(),
                                     default=["genIso<10.","genIso>=-999999999999"], help="default: %default"
                                     ),
                         make_option("--trees",dest="trees",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",
                                     default=["promptTree","fakesTree"],
                                     ## default=["photonDumper/trees/promptTree","photonDumper/trees/fakesTree"],
                                     help="default: %default"),
                         make_option("--store",dest="store",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",
                                     default="absScEta",help="default: %default"),
                         make_option("--reweight",action="store",default=None,type="string"
                                     ),
                         make_option("--wread",action="store",default=False,type="string"
                                     ),
                         make_option("--wvars",dest="wvars",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",default=["pt","absScEta"]),
                         make_option("--wrng",dest="wrng",action="callback",callback=optpars_utils.Load(),
                                     type="string",default={}),
                         make_option("--fit-median",dest="fitMedian",action="store_true",
                                     default=False),
                         make_option("--fit-range",dest="fitRange",action="callback",type="string",callback=optpars_utils.ScratchAppend(float),
                                     default=[]),
                         make_option("--fit-expression",dest="fitExpression",action="store",type="string",
                                     default="pol1"),
                         make_option("--rho-corr",dest="rhoCorr",action="store_true",
                                     default=False),
                         make_option("--rho-corr-var",dest="rhoCorrVar",action="store",type="string",
                                     default="rho"),
                         make_option("--no-rho-corr",dest="rhoCorr",action="store_false",
                                     ),
                         make_option("--pt-corr",dest="ptCorr",action="store_true",
                                     default=False),
                         make_option("--no-pt-corr",dest="ptCorr",action="store_false",
                                     ),
                         make_option("--pt-corr-var",dest="ptCorrVar",action="store",type="string",
                                     default="pt"),
                         make_option("--offset-corr",dest="offsetCorr",action="store_true",
                                     default=False),
                         make_option("--no-offset-corr",dest="offsetCorr",action="store_false",
                                     ),
            
                         ]
            )

        self.aliases_ = {}
        self.signs_ = {}
        self.store_ = []
        
        global ROOT, style_utils
        import ROOT
        import diphotons.Utils.pyrapp.style_utils as style_utils
        ROOT.gSystem.Load("libdiphotonsUtils")
        if ROOT.gROOT.GetVersionInt() >= 60000:
            # ROOT.gSystem.Load("libdiphotonsRooUtils")
            ROOT.gSystem.AddIncludePath("-I$CMSSW_BASE/src")
            ROOT.gROOT.ProcessLine('#include "diphotons/Utils/interface/FunctionHelpers.h"')
            
        self.setStyle("*quantile*", [ ("SetLineWidth",2), ("SetMarkerSize",2) ] )
        

    def __call__(self,options,args):
        
        self.loadRootStyle()
    
        ROOT.TH1.SetDefaultSumw2(True)
        ROOT.TGaxis.SetMaxDigits(3)
        
        fin = self.open(options.infile)
        sigTree = fin.Get(options.trees[0])
        
        catvars = {}

        for al in options.aliases:
            self.getVar(al)
                
        variables = {}
        for var in options.variables:
            var,bins = self.getVar(var)
            variables[var] = bins
        self.getVar(options.xvar)
                
        efficiencies = []
        for var in options.efficiencies:
            var,bins = self.getVar(var)
            efficiencies.append(var)
            variables[var] = array.array('d',[-0.5,0.5,1.5])
        
        for var in variables.keys():
            print var

        self.setAliases(sigTree)
        
        win = False
        wname = None
        wfuncs = {}
        if options.reweight:
            fnam,wname = options.reweight.split("::")
            win = self.open(fnam)
            
        if options.fitMedian:
            sigQuantiles = [0.5,0.95,0.97,0.98,0.99]
            ## sigQuantiles = [0.05,0.10,0.5,0.9,0.95]
            ## sigQuantiles = [0.25,0.4,0.5,0.6,0.75]
            bkgQuantiles = []
        else:
            sigQuantiles = [0.99,0.95,0.9,0.85,0.5]
            bkgQuantiles = [0.7,0.5,0.4,0.3]

        if options.fitMedian:
            ## summary = { v : { } for v in variables.keys() }
            summary = {}
            for q in sigQuantiles:
                summary[q] = {}
                for v in variables.keys():
                    summary[q][v] = []
                    summary[q]["%sOffset"%v] = []
            # summary = { q : { v : [] for v in variables.keys() } for q in sigQuantiles }
        
        for cat in set(options.categories):
            xvar = options.xvar
            
            if ":=" in cat:
                name,cdef = [ t.lstrip(" ").rstrip(" ") for t in cat.split(":=",1) ]
            else:
                name = cat
                cdef  = cat
            
            weight = "(%s) * (%s)" % ( cdef, options.weight )
            
            if win:
                for tree in options.trees:
                    if not tree in wfuncs:
                        xrng = variables[options.wvars[0]]
                        yrng = variables[options.wvars[1]]
                        wfuncs[tree] = ROOT.FlatReweight( xrng[0],xrng[-1],yrng[0],yrng[-1] )
                    hname = "%s_%s_%s" % (tree,wname,name)
                    ## win.ls()
                    print hname 
                    win.ls()
                    hist = win.Get( str(hname) )
                    print hist
                    yrng, frng = options.wrng[name]
                    sf = ROOT.SliceFitter(hist,"[0]+[1]*x+[2]*x*x",yrng[0],yrng[1],frng[0],frng[1],False,True,1)
                    ## plots = sf.getPlots()
                    ## for p in plots: self.keep(p)
                    wfuncs[tree].add(sf)
                    self.keep(sf)
                    xbins = [ hist.GetXaxis().GetXbins()[ib] for ib in range(hist.GetNbinsX()+1) ]
                    xvar = self.getVar(options.xvar)[0]
            else:
                ## xbins,xvar = self.recursiveBinning(xvar,sigTree,weight)
                xbins,xvar = self.adaptiveBinning(xvar,sigTree,weight,plot=name,maxerr=options.binningMaxErr,target=options.binningTarget)
            
            xbins = array.array('d',xbins)
            pdf = self.draw(sigTree,xvar,"%s_%s_pdf" % (xvar,name), xbins, weight, "goff", True)
            canv = ROOT.TCanvas(pdf.GetName(),pdf.GetName())
            canv.SetLogy()
            canv.SetGridx()
            canv.SetGridy()
            canv.cd()
            pdf.Draw()
            
            catvars[name] = [cdef,xvar,xbins,pdf]
            
            self.keep( [canv,pdf] )
        
        self.autosave(True)
        
            
        print "making 2D histos"
        sig,bkg = options.trees
        
        sigColors    = [ROOT.kBlue-2,ROOT.kBlue-1,ROOT.kBlue,ROOT.kBlue+1,ROOT.kBlue+2]
        bkgColors    = [ROOT.kRed-2,ROOT.kRed-1,ROOT.kRed,ROOT.kRed+1,ROOT.kRed+2]
        
        trees = {}
        for tnam in sig,bkg:
            tree = fin.Get(tnam)
            trees[tnam] = tree
            self.setAliases(tree)
        if len(self.options.weights) == 2:
            trees[sig].SetAlias("_extra_weight_",self.options.weights[0])
            trees[bkg].SetAlias("_extra_weight_",self.options.weights[1])
            self.options.weight = "(%s) * _extra_weight_" % options.weight
            
        if win or options.wread:
            if not options.wread:
                fwei = self.open("%s/wei.root" % options.outdir, "recreate")
                wd = ROOT.gDirectory
                fwei.cd()
                for tnam in sig,bkg:
                    friend = ROOT.TTree("%s_rewei" % tnam,"%s_rewei" % tnam)
                    func = wfuncs[tnam].asTF2(str("%s_rewei" % tnam))
                    tree = trees[tnam]
                    print "Reweighting %s" % tnam
                    ROOT.fillReweight(str(options.wvars[0]),str(options.wvars[1]),str(options.weight),func,tree,friend)
                    canv = ROOT.TCanvas("%s_rewei" % tnam,"%s_rewei" % tnam)
                    canv.cd()
                    func.Draw("colz")
                    self.keep( [canv, func ] )
                    self.autosave(True)
                    friend.Write()
                fwei.Close()
                wd.cd()
                
            options.weight = "(%s) * rewei" % ( options.weight )
            fwei = self.open("%s/wei.root" % options.outdir if not options.wread else options.wread)
            for tnam in sig,bkg:
                tree = trees[tnam] 
                friend = fwei.Get("%s_rewei" % tnam)
                tree.AddFriend(friend)
                self.keep( [friend ] )
        
        plots = {}
        frames = {}
        for cat,defs in catvars.iteritems():
            print "category: %s" % cat
            plots[cat] = {}
            canvs = {}
            pdfs  = []
            frames[cat] = {}
            
            ## for name in options.trees:
            for name in sig,bkg:
                print "tree: %s" % name
                plots[cat][name] = {}
                tree = trees[name]
                self.setAliases(tree)

                for var,bins in variables.iteritems():
                    print "variable: %s" % var
                    
                    cdef = defs[0]
                    xvar = defs[1]
                    xbins = defs[2]
                    weight = "(%s) * (%s)" % (options.weight,cdef)
                    print weight
                    pdf2D = self.draw2D(tree,"%s_%s_%s" % (name,var,cat),  xvar, xbins, var, bins, weight, "goff" )
                    
                    if var in options.store:
                        self.store_.append( pdf2D )
                        

                    pdf1D = pdf2D.ProjectionY()
                    pdf1D.UseCurrentStyle()
                    
                    ## plots[cat][name][var] = pdf2D
                    if name == sig:
                        canv = ROOT.TCanvas("pdfs_%s_%s" % (var,cat))
                        logCanv = ROOT.TCanvas("pdfs_%s_%s_logy" % (var,cat))
                        logCanv.SetLogy()
                        canvs[var] = [canv,logCanv]
                        
                        if var in efficiencies:
                            plots[cat][name][var] = getEfficiencyGraphs(pdf2D,[1.])
                        else:
                            plots[cat][name][var] = getQuantilesGraphs(pdf2D,sigQuantiles,errors=False,sign=self.signs_[var])
                        frames[cat][var] = pdf2D.Clone("%s_%s" % ( var, cat ) )
                        style_utils.colors(pdf1D,sigColors[0])
                        
                        if options.fitMedian:
                            if len(self.options.fitRange)>0:
                                rngmin,rngmax = self.options.fitRange
                            else:
                                rngmin,rngmax = pdf2D.GetXaxis().GetXmin(),pdf2D.GetXaxis().GetXmax()
                            print self.options.fitRange
                            print rngmin, rngmax
                            func = ROOT.TF1("fit_%s_%s_%s" % (cat,name,var),options.fitExpression,rngmin,rngmax)
                            for i,p in enumerate(plots[cat][name][var]):
                                fp = func.Clone("%s_%d" %( func.GetName(),i))
                                ## fp.SetParameters(pdf1D.GetMean(),0.)
                                p.Fit(fp,"QWR+")
                                self.keep(fp)
                                
                                
                        canv.cd()
                        pdfs.append( pdf1D.DrawNormalized("hist") )
                        logCanv.cd()
                        pdfs.append( pdf1D.DrawNormalized("hist") )
                    else:
                        if var in efficiencies:
                            plots[cat][name][var] = getEfficiencyGraphs(pdf2D,[1.])
                        else:
                            plots[cat][name][var] = getQuantilesGraphs(pdf2D,bkgQuantiles,errors=False,sign=self.signs_[var])
                        style_utils.colors(pdf1D,bkgColors[0])
                        pdf1D.SetFillStyle(0)
                        if not options.fitMedian:
                            canvs[var][0].cd()
                            pdf1D.DrawNormalized("hist same")
                            canvs[var][1].cd()
                            pdf1D.DrawNormalized("hist same")
                        self.keep( canvs[var] )
                        self.autosave(True)
                    

        print "making plots"
        for cat,defs in catvars.iteritems():
            for var,bins in variables.iteritems():
                ### sig2D = plots[cat][sig][var]
                ### bkg2D = plots[cat][bkg][var]
                ### 
                ### ### sigQtiles = ROOT.DecorrTransform(sig2D,200.,False,True)
                ### ### bkgQtiles = ROOT.DecorrTransform(bkg2D,200.,False,True)
                ### ### 
                ### ### sigGraphs = self.fillGraphs(sigQtiles,sigQuantiles,sigColors)
                ### ### bkgGraphs = self.fillGraphs(bkgQtiles,bkgQuantiles,bkgColors)
                ### 
                ### sigGraphs = getQuantilesGraphs(sig2D,sigQuantiles,errors=False)
                ### bkgGraphs = getQuantilesGraphs(bkg2D,bkgQuantiles,errors=False)
                sigGraphs = plots[cat][sig][var]
                bkgGraphs = plots[cat][bkg][var]
                
                
                frame = frames[cat][var]
                name  = frame.GetName()
                ## name = sig2D.GetName().split("_",1)[-1]
                
                if var in efficiencies:
                    canv = ROOT.TCanvas("effs_%s"%name,name)
                else:
                    canv = ROOT.TCanvas("qtiles_%s"%name,name)
                
                legBkg = ROOT.TLegend(0.5,0.7,0.7,0.9)
                legSig = legBkg if self.options.fitMedian else ROOT.TLegend(0.7,0.7,0.9,0.9)
                
                for gr,col,qt in zip(sigGraphs,sigColors,sigQuantiles):
                    if var in efficiencies:
                        label = "#varepsilon(sig)"
                    else:
                        label = "#varepsilon(sig)=%1.2g" % qt
                    if options.fitMedian:
                        func = gr.GetListOfFunctions().At(0)
                        style_utils.colors(func,col)
                        ## summary[var][cat].append( (qt,func.GetParameter(1) ))
                        summary[qt][var].append("%1.2g*(%s)" % (func.GetParameter(1),defs[0]))
                        summary[qt]["%sOffset" % var].append("%1.2g*(%s)" % (func.GetParameter(0),defs[0]))
                        label += " %1.2g" % (func.GetParameter(1))
                    legSig.AddEntry(gr,label, "lp")
                    style_utils.colors(gr,col)
                    
                for gr,col,qt in zip(bkgGraphs,bkgColors,bkgQuantiles):
                    if var in efficiencies:
                        label = "#varepsilon(bkg)"
                    else:
                        label = "#varepsilon(bkg)=%1.2g" % qt
                    legBkg.AddEntry(gr,label, "lp")
                    style_utils.colors(gr,col)
                
                ## frame = sig2D.Clone()
                graphs = sigGraphs+bkgGraphs
                self.keep( graphs+[frame,canv,legSig,legBkg], True )
                
                canv.cd()
                ypoints = [ gr.GetY()[i] for gr in graphs for i in range(gr.GetN()) ]
                spread = max( ypoints ) - min( ypoints )
                ymin = min( ypoints )-0.2*spread
                ymax = max( ypoints )+0.4*spread
                
                frame.Reset("ICE")
                frame.GetYaxis().SetRangeUser(ymin,ymax)
                frame.Draw()
                for g in graphs:
                    g.Draw("p")
                legSig.Draw("same")
                legBkg.Draw("same")
                
                self.autosave(True)
        
        
        if len(self.store_)>0:
            out = self.open("%s/store.root" % options.outdir,"recreate")
            out.cd()
            for ob in self.store_:
                ob.Write()
            self.store_ = []
            out.Close()
        
        if options.fitMedian:
            out = open("%s/README.txt"% options.outdir,"w+")
            ### out.write(json.dumps(summary,sort_keys=True,indent=4))
            effa = { q : [] for q in summary.keys() }
            for qt,qvar in summary.iteritems():
                for var,vals in qvar.iteritems():
                    effa[qt].append("%sEA := %s" % (var,"+".join(vals)))
            out.write(json.dumps(effa,sort_keys=True,indent=4))
            out.close()
        
    def draw(self,tree,var,name,bins,weight,opt,density=False):
        hist = ROOT.TH1F(name, name, len(bins)-1, bins)
        hist.GetXaxis().SetTitle(var)
        
        tree.Draw("%s>>%s" % ( var, name), weight, opt )
        
        if density:
            for ibin in range(hist.GetNbinsX()+2):
                cont = hist.GetBinContent(ibin)
                err  = hist.GetBinError(ibin)
                width= hist.GetBinWidth(ibin)
                hist.SetBinContent(ibin,cont/width)
                hist.SetBinError(ibin,err/width)

        self.keep(hist)
        return hist

    def draw2D(self,tree,name,xvar,xbins,yvar,ybins,weight,opt,density=False):
        hist = ROOT.TH2F(name, name, len(xbins)-1, xbins, len(ybins)-1, ybins)
        
        hist.GetXaxis().SetTitle(xvar)
        hist.GetYaxis().SetTitle(yvar)
        
        yvar0 = yvar
        if self.options.rhoCorr and ("%sEA"%yvar0 in  self.aliases_):
            print "Doing rho correction for", yvar, self.aliases_["%sEA"%yvar0]
            yvar += "- (%sEA*%s)" % (yvar0,self.options.rhoCorrVar)
        if self.options.ptCorr and ("%sPtCorr"%yvar0 in  self.aliases_):
            print "Doing pt correction for", yvar, self.aliases_["%sPtCorr"%yvar0]
            yvar += "- (%sPtCorr*%s)" % (yvar0,self.options.ptCorrVar)
        if self.options.offsetCorr and ("%sOffset"%yvar0 in  self.aliases_):
            print "Doing offset correction correction for", yvar, self.aliases_["%sOffset"%yvar0]
            yvar += "- (%sOffset)" % (yvar0)

        print yvar
        tree.Draw("%s:%s>>%s" % ( yvar, xvar, name), weight, opt )
        
        if density:
            for ibin in range(hist.GetNbinsX()+2):
                cont = hist.GetBinContent(ibin)
                err  = hist.GetBinError(ibin)
                width= hist.GetBinWidth(ibin)
                hist.SetBinContent(ibin,cont/width)
                hist.SetBinError(ibin,err/width)

        self.keep(hist)
        return hist
    
    def setAliases(self,tree):
        for var,vdef in self.aliases_.iteritems():
            tree.SetAlias(var,vdef)
    
    def getVar(self,var):
        if var.endswith("-"):
            sign = -1
        else:
            sign = +1
        if "[" in var:
            name,binning = var.rstrip("-").rstrip("]").rsplit("[",1)
            
            if "," in binning:
                binning = binning.split(",")
            else:
                binning = binning.split(":")
            if len(binning) == 3:
                nbins = int(binning[0])
                xmin = float(binning[1])
                xmax = float(binning[2])
                step = ( xmax - xmin ) / float(nbins)
                xbins = array.array('d',[xmin+step*float(ib) for ib in range(nbins+1)])
            else:
                xbins = array.array('d',[float(b) for b in binning])
        else:
            name,xbins = var.rstrip("-"),[]

        if ":=" in name:
            name,vdef = [ t.lstrip(" ").rstrip(" ").lstrip("\t").rstrip("\t") for t in name.split(":=",1) ]
            self.aliases_[name] = vdef
            
        name = name.lstrip(" ").rstrip(" ").lstrip("\t").rstrip("\t")
        self.signs_[name] = sign
        return name,xbins
    
    def makeCdf(self,var,tree,weight,xbins=None,label=None):
        
        if not xbins:
            name,xbins = self.getVar(var)
        else:
            name=var
        
        if not label:
            label = name
     
        pdf = self.draw(tree,name,"%s_pdf" % label,xbins,weight,"goff")

        return name, xbins[0],xbins[-1], ROOT.mkCdfInv(pdf,xbins[0],xbins[-1]), pdf
        
    def updateBins(self,bins,vals,gr=None):

        curderiv = None
        curbin   = None
        
        for ibin in range(len(vals)-2,0,-1):
            val0  = vals[ibin]
            val1  = vals[ibin+1]
            
            bin0  = bins[ibin-1]
            bin1  = bins[ibin]
            bin2  = bins[ibin+1]
            
            deltaVal = val1/(bin2-bin1)-val0/(bin1-bin0)
            deltaCent = (bin0-bin2)*0.5
            
            ### print bin0, bin1, bin2
            deriv = fabs(deltaVal / deltaCent)
            
            if not curderiv or deriv < curderiv:
                curbin = ibin
                curderiv = deriv
                
        ## print curbin, curderiv
        if gr:
            ip = gr.GetN()
            gr.SetPoint( ip, ip, bins[curbin] )
        newbins = [bins[0]]
        newvals = []
        ibin = 1
        ival = 0.
        while ibin < (len(bins)-1):
            ival += vals[ibin-1]
            if ibin != curbin:
                newvals.append(ival)
                newbins.append(bins[ibin])
                ival = 0.
            ibin += 1
        ## newbins.append(bins[-1])
        ## print newbins
        ## print newvals
        
        return newbins,newvals

    def updateBinsErr(self,bins,vals,gr=None):

        ## print "updateBins", len(vals)
        ## print bins
        ## print vals
        
        curerr = None
        curbin = None
        
        for ibin in range(len(vals)-2,0,-1):
            sumw,sumw2  = vals[ibin]
            
            bin  = bins[ibin+1]
            
            err = sqrt(sumw2)/sumw

            
            if not curerr or err > curerr:
                curbin = ibin+1
                curerr = err
                
        ## print curbin, curderiv
        ## print curerr, curbin, bins[curbin]
        if gr:
            ip = gr.GetN()
            gr.SetPoint( ip, ip, bins[curbin] )
        newbins = [bins[0]]
        newvals = []
        ibin = 1
        ivalw,ivalw2 = 0.,0.
        while ibin < (len(bins)-1):
            ivalw += vals[ibin-1][0]
            ivalw2 += vals[ibin-1][1]
            if ibin != curbin:
                newvals.append((ivalw,ivalw2))
                newbins.append(bins[ibin])
                ivalw,ivalw2 = 0.,0.
            ibin += 1
        newbins.append(bins[-1])
        ## print newbins
        ## print newvals
        
        return newbins,newvals

    def adaptiveBinning(self,var,tree,weight,target=30,maxerr=5.e-2,direction=-1,plot=False):
        print "adaptive binning"
        firstStep = self.makeCdf(var,tree,weight)
        name, xmin, xmax, cdf, pdf = firstStep
        ## xbins = [xmax]
        
        nevents = 0        
        if direction<0:
            bin1 = pdf.GetNbinsX()
            bin0 = pdf.GetNbinsX()
            xbins = [xmax]
        else:
            bin1 = 1
            bin0 = 0
            xbins = [xmin]
        vals  = []

        ## while nevents < minevents and (bin1>0 and bin1<=pdf.GetNbinsX()):
        while (bin1>0 and bin1<=pdf.GetNbinsX()):
            err   = 1.
            sumw2 = 0.
            sumw  = 0.
            while err > maxerr and (bin1>0 and bin1<=pdf.GetNbinsX()):
                sumw += pdf.GetBinContent(bin1)
                sumw2 += pdf.GetBinError(bin1)*pdf.GetBinError(bin1)
                bin1    += direction
                if sumw > 0.:
                    err = sqrt(sumw2) / sumw
            if direction<0:
                xbins.append( pdf.GetXaxis().GetBinLowEdge(bin1) ) 
            else:
                xbins.append( pdf.GetXaxis().GetBinUpEdge(bin1) )
            ## vals.append( sumw / pdf.Integral() )
            vals.append( (sumw,sumw2) )
            ## print xbins, err
            bin1 += direction
            
        ### ## width = (cdf.eval(pdf.GetXaxis().GetBinLowEdge(bin0)) - cdf.eval(pdf.GetXaxis().GetBinLowEdge(bin1)))*direction
        ### width = sumw / pdf.Integral()
        ### print sumw, sumw2, err, bin0, bin1, width
        ### nbins = int(1./width)
        ### 
        ### xbins = [xmin]
        ### vals  = []
        ### width = 1./float(nbins)
        ### for ibin in range(1,nbins):
        ###     xbins.append(cdf.eval(float(ibin)*width))
        ###     vals.append(width)            
        ### xbins.append(xmax)
        ### 
        gr = None
        if plot:
            gr = ROOT.TGraph()
            gr.SetName("adaptive_binning_%s" % plot)
            gr.SetMarkerSize(2)
        
        while len(vals) > target:
            xbins,vals = self.updateBinsErr(xbins,vals,gr)
        
        if gr:
            canv = ROOT.TCanvas(gr.GetName(),gr.GetName())
            canv.cd()
            gr.Draw("ap")
            self.keep([canv,gr])
        self.autosave(True)
        
        sbins = set(xbins)
        xbins = [ xmin ]
        for b in sorted( list(sbins) ):
            if b < xmax and b > xmin:
                xbins.append(b)
        xbins.append( xmax )
        print xbins
        return xbins,name
    
    ## def recursiveBinning(self,var,tree,weight,binsPerStep=12,nsteps=4,plot=False):
    def recursiveBinning(self,var,tree,weight,binsPerStep=12,nsteps=1,plot=False):
    ### def recursiveBinning(self,var,tree,weight,binsPerStep=15,nsteps=1,plot=False):
                
        istep = 0
        print "recursiveBinning step %d " % istep
        firstStep = self.makeCdf(var,tree,weight)
        name, xmin, xmax, cdf, pdf = firstStep
        xbins = [xmin]
        
        while istep < nsteps:

            self.keep( [cdf] )
            
            if plot:
                cdfTF1 = cdf.asTF1("xvarCdf%d" % istep)
                canv = ROOT.TCanvas(cdfTF1.GetName(),cdfTF1.GetName())
                canv.cd()
                cdfTF1.Draw()
                                
                self.keep( [canv,cdfTF1] )
                
                
            width = 1./float(binsPerStep)
            for ibin in range(1,binsPerStep):
                xbins.append(cdf.eval(float(ibin)*width))

            ## print xbins
            
            istep += 1 
            
            nextmin = cdf.eval(1-width)
            width = (xmax - nextmin)/float(100*binsPerStep)
            nextbins = [ nextmin + ibin*width for ibin in range(100*binsPerStep) ]
            nextbins.append(xmax)
            nextbins = array.array('d', nextbins)
            curstep = self.makeCdf(name,tree,weight,nextbins,"%s_step%d" % ( name, istep ) )
            
            name, xmin, xmax, cdf, pdf = curstep
            
        xbins.append(xmax)

        return xbins,name
    
if __name__ == "__main__":
    app = IdEvolution()
    app.run()

        
