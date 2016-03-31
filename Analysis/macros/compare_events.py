#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
# from copy import deepcopy as copy
import os, sys, glob, json

import itertools

from pprint import pprint

from auto_plotter import getObjects
from copy import deepcopy

from math import sqrt              

def cmpEvId(x,y):
    
    if x[0]<y[0]: return -1
    elif x[0]==y[0]:
        if x[1]<y[1]: return -1
        elif x[1]==y[1]: return 0
    return 1



# -----------------------------------------------------------------------------------------------------------
def bookhisto(hdef):
    
    name, xvar, nxbins, xbins, yvar, nybins, yvar = hdef
    # name = "".join([prefix,name])
        
    if nxbins == 0:
        xbins=(len(xbins)-1,array.array('d',xbins))
    else:
        xbins=[nxbins,xbins[0],xbins[1]]

    pwd = ROOT.gDirectory
    if yvar:
        if nybins == 0:
            ybins=(len(ybins)-1,array.array('d',ybins))
        else:
            ybins=[nybins,ybins[0],ybins[1]]
    
        histo = ROOT.TH2D(name,name,*(xbins+ybins))
    else:
        histo = ROOT.TH1D(name,name,*xbins)
    return histo
  
# -----------------------------------------------------------------------------------------------------------
class CompareEvents(PlotApp):

    def __init__(self):
        super(CompareEvents,self).__init__(option_list=[
                make_option("--file-names",action="callback", dest="file_names", type="string", callback=optpars_utils.ScratchAppend(),
                            default=[]),
                make_option("--fit-name",action="store", dest="fit_name", type="string",
                            default="cic2"),
                make_option("--file-labels",action="callback", dest="file_labels", type="string", callback=optpars_utils.ScratchAppend(),
                            default=[]), 
                make_option("--compare-categories",action="callback", dest="compare_categories", type="string", callback=optpars_utils.ScratchAppend(),
                            default=["EBEB","EBEE"]),
                make_option("--mass-range",action="callback", dest="mass_range", type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[]),
                make_option("--histograms",action="callback", dest="histograms", type="string", callback=optpars_utils.ScratchAppend(comma=";"),
                            default=["mass>>mass(1500,0,15000)",
                                     ]),
                    ])
        
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils

    def __call__(self,options,args):
        self.loadRootStyle()

        self.evlists = {}

        ### from flashgg.Taggers.dumperConfigTools import parseHistoDef
        ### histos = map( parseHistoDef, options.histograms )

        files = map( self.open, options.file_names)
        for cat in self.options.compare_categories:
            trees = map(lambda x: x.Get("tree_data_%s_%s" % (options.fit_name,cat)), files )
            
            self.evlists[cat] = []
            for tree in trees:
                evList = self.readEventList(tree)
                self.evlists[cat].append( (evList,tree) )
            
            self.indexes = self.makeIndexes(self.evlists[cat])

            overlap = self.analyzeIndexes(cat,self.indexes,options.file_labels)
            ## pprint( overlap )
            
            if len(options.file_labels) == 2:
                ### allvars = list(set(filter( lambda x: x != None, reduce( lambda x,y: x+y,  map( lambda x: [x[0],x[1]], histos )))))
                ### formulas = []
                ### th1s = map(bookhisto, histos)
                ### for v in allvars:
                ###     formulas.append(map(lambda x: ROOT.TTreeFormula(v,v,x), trees))
                
                ROOT.gStyle.SetOptStat(1)
                ROOT.gStyle.SetOptFit(1)
                deltaMass = ROOT.TH1D("relDeltaMass_%s" % cat,"relDeltaMass_%s" % cat,50,-0.1,0.1)
                deltaMass.GetXaxis().SetTitle("2 ( m_{\gamma \gamma}^{%s} - m_{\gamma \gamma}^{%s} ) /  ( m_{\gamma \gamma}^{%s} + m_{\gamma \gamma}^{%s} ) " % tuple(options.file_labels+options.file_labels) )

                deltaMassDiffVtx = ROOT.TH1D("relDeltaMassDiffVtx_%s" % cat,"relDeltaMassDiffVtx_%s" % cat,50,-0.1,0.1)
                deltaMassDiffVtx.GetXaxis().SetTitle("2 ( m_{\gamma \gamma}^{%s} - m_{\gamma \gamma}^{%s} ) /  ( m_{\gamma \gamma}^{%s} + m_{\gamma \gamma}^{%s} ) " % tuple(options.file_labels+options.file_labels) )
                
                for event in overlap:
                    map(lambda x: x[0][1].GetEntry(x[1]), zip(self.evlists[cat],event[1]))
                    deltaMass.Fill( 2.*(trees[0].mgg-trees[1].mgg) / (trees[0].mgg+trees[1].mgg) )
                    if trees[0].leadEta != trees[1].leadEta: 
                        deltaMassDiffVtx.Fill( 2.*(trees[0].mgg-trees[1].mgg) / (trees[0].mgg+trees[1].mgg) )
                    
                deltaMass.Fit("gaus")
                canv = ROOT.TCanvas("%s_%s_%s" % tuple( [deltaMass.GetName()]+options.file_labels),deltaMass.GetName())
                canv.cd()
                deltaMass.Draw()
                self.keep( [canv,deltaMass] )

                deltaMassDiffVtx.Fit("gaus")
                canv = ROOT.TCanvas("%s_%s_%s" % tuple( [deltaMassDiffVtx.GetName()]+options.file_labels),deltaMassDiffVtx.GetName())
                canv.cd()
                deltaMassDiffVtx.Draw()
                self.keep( [canv,deltaMassDiffVtx] )
                
                self.autosave(True)
        
    def readEventList(self,tree):
        lst = []
        nevt = tree.GetEntries()
        for iev in range(nevt):
            tree.GetEntry(iev)
            if len(self.options.mass_range) == 2 and (tree.mgg<=self.options.mass_range[0] or tree.mgg>=self.options.mass_range[1]):  continue
            lst.append( (int(tree.run), int(tree.event), iev) )
            
        return sorted(lst,cmp=cmpEvId)
        

    def analyzeIndexes(self,cat,indexes,labels):
        overlap = filter(lambda x: reduce(lambda z,w: z and w, map(lambda y: y != -1, x[1])), indexes.iteritems())
        print cat, len(indexes), len(overlap)
        
        for ilab,lab in enumerate(labels):
            here = filter(lambda x: x[1][ilab] != -1, indexes.iteritems() )
            print lab, len(here)
            
            for jlab in range(ilab+1,len(labels)):
                here = filter(lambda x: x[1][ilab] != -1 and x[1][jlab] != -1, indexes.iteritems() )
                print lab, labels[jlab], len(here)

        return overlap

    def makeIndexes(self,info):

        info = map(lambda x: x[0], info)
        indexes = [0]*len(info)
        lengths = map(lambda x: len(x), info)
        flags   = [True]*len(info)
        
        fullindexes = {}
        
        while reduce(lambda x,y: x or y, flags):
            evids = map(lambda x: x[0][x[1]] if x[1]<x[2] else (None,None), zip(info,indexes,lengths))
            srt = sorted(filter(lambda x: x[0] != None,evids),cmp=cmpEvId)
            minid = tuple(srt[0][:2])
            
            fullindexes[minid] = []
            for ilst,evid in enumerate(evids):
                if evid[0] == minid[0] and evid[1] == minid[1]:
                    fullindexes[minid].append(evid[2])
                    indexes[ilst] += 1
                else:
                    fullindexes[minid].append(-1)
            
            flags = map(lambda x: x[0] < x[1], zip(indexes,lengths))

        return fullindexes
        
# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app = CompareEvents()
    app.run()
