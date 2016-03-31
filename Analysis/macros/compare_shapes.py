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
  
def scaleGraph(graph,scale):
    graph = graph.Clone()
    graph.GetListOfFunctions().Clear()
    ## graph.Print()
    
    
    xvals = graph.GetX()
    yvals = graph.GetY()
    yerrl = graph.GetEYlow()
    yerrh = graph.GetEYhigh()
    for ip in xrange(graph.GetN()):
        scl = scale
        ## print scl
        graph.SetPoint( ip, xvals[ip], yvals[ip]*scl )
        try:
            graph.SetPointEYlow( ip, yerrl[ip]*scl )
            graph.SetPointEYhigh( ip, yerrh[ip]*scl )
        except:
            pass
    
    ## graph.Print()
    
    return graph

# -----------------------------------------------------------------------------------------------------------
class ShapePlot(PlotApp):

    def __init__(self):
        super(ShapePlot,self).__init__(option_list=[
                make_option("--compare-file","--compare-files",dest="compare_files",action="callback",type="string", callback=optpars_utils.ScratchAppend(str),
                            default=[]),
                make_option("--compare-label","--compare-labels",dest="compare_labels",action="callback",type="string", callback=optpars_utils.ScratchAppend(str),
                            default=[]),
                make_option("--rescale","--rescale",dest="rescale",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[]),
                make_option("--label",dest="label",type="string",action="store",default="")
            ])
        
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils

        
    def __call__(self,options,args):
        self.loadRootStyle()
        
        graphs = map(lambda x: ( scaleGraph(self.open(x[0]).Get("onesigma"),x[2]),x[1]),  zip(options.compare_files,options.compare_labels,options.rescale) )
        
        canv = ROOT.TCanvas("shape_cmp_%s" % options.label,options.label)
        canv.cd()
        canv.SetLogy()

        legend = ROOT.TLegend(0.56,0.51,0.86,0.76)

        legend.AddEntry(None,options.label,"")

        g0 = graphs[0][0]
        g0.Draw("al")
        legend.AddEntry(g0,graphs[0][1],"le")
        
        style_utils.apply(g0, [["colors",ROOT.kBlue]] )
        for g in graphs[1:]:
            g[0].Draw("l")
            legend.AddEntry(g[0],g[1],"le")
            style_utils.apply(g[0], [["colors",ROOT.kRed]] )
            
        legend.Draw()
        self.keep( graphs )
        self.keep( [canv,legend] )
        
        self.autosave(True)
        
        
# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app = ShapePlot()
    app.run()
