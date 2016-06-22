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
    
    xvals = graph.GetX()
    yvals = graph.GetY()
    yerrl = graph.GetEYlow()
    yerrh = graph.GetEYhigh()
    for ip in xrange(graph.GetN()):
        ## print scl
        graph.SetPoint( ip, xvals[ip], yvals[ip]*scale )
        try:
            graph.SetPointEYlow( ip, yerrl[ip]*scale )
            graph.SetPointEYhigh( ip, yerrh[ip]*scale )
        except:
            pass
    
    ## graph.Print()
    
    return graph

  
# -----------------------------------------------------------------------------------------------------------
class CompareShapes(PlotApp):

    def __init__(self):
        super(CompareShapes,self).__init__(option_list=[
                make_option("--file-names",action="callback", dest="file_names", type="string", callback=optpars_utils.ScratchAppend(),
                            default=[]),
                make_option("--file-labels",action="callback", dest="file_labels", type="string", callback=optpars_utils.ScratchAppend(),
                            default=[]), 
                make_option("--label",action="store", dest="label", type="string",
                            default=""),
                make_option("--legend",dest="legend",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[0.56,0.51,0.86,0.76]),
                    ])
        
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils

    def __call__(self,options,args):
        self.loadRootStyle()

        self.evlists = {}

        files = map( self.open, options.file_names)
        graphs = map(lambda x: x.Get("onesigma"), files )
        map(lambda x: scaleGraph(x,1./reduce(lambda z,w: z+w, [ x.GetY()[i] for i in xrange(x.GetN())] )), graphs )
        
        styles = [ [["colors",ROOT.kBlue],["SetFillColorAlpha",[ROOT.kBlue,0.5]]], [["colors",ROOT.kRed],["SetFillColorAlpha",[ROOT.kRed,0.5]]] ]
        common = [["xtitle","m_{#gamma#gamma} (GeV)"],["ytitle","1 / N #times  #Delta N / #Delta m_{#gamma#gamma} (1 / 20 GeV)"]]
        
        map(lambda x: style_utils.apply(x[0],x[1]+common), zip(graphs,styles) )
        map(lambda x: x[0].SetTitle(x[1]), zip(graphs,self.options.file_labels) )
        

        canv = ROOT.TCanvas("compareshape_%s" % (options.label),"compareshape_%s"  % (options.label) )
        legend = ROOT.TLegend(*options.legend)
        
        canv.cd()
        canv.SetLogy()
        print graphs
        graphs[0].Draw("ape3")
        for g in graphs[1:]:
            g.Draw("pe3")
            
        ptCMS=ROOT.TLatex(0.28,0.25,self.options.label)
        ptCMS.SetNDC()
        ## ptCMS.SetTextFont(61)
        ptCMS.SetTextSize(0.04)
        ptCMS.Draw("same")

        for g in graphs:
            legend.AddEntry(g)
        legend.Draw("same")
        
        self.keep(graphs)
        self.keep([canv,legend])
        self.format(canv,options.postproc)
        self.autosave(True)
        
# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app = CompareShapes()
    app.run()
