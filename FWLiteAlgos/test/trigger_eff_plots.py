#!/bin/env python

from pyrapp import *
from optparse import OptionParser, make_option

from copy import copy
from pprint import pprint
import csv,os

from math import sqrt

import array

# ------------------------------------------------------------------------------------------
class TrgEffPlots(PlotApp):

    def __init__(self):
        super(TrgEffPlots,self).__init__(default_cats=["EBEB","notEBEB"],
                                         option_list=[make_option("--variables",dest="variables",action="callback",callback=optpars_utils.ScratchAppend(),
                                                                  default=["pt","superCluster_eta","phi","hadronicOverEm"],
                                                                  help="default: %default"),

                ]
                                         )
    
        global ROOT, style_utils
        import ROOT
        import pyrapp.style_utils as style_utils
        

    def __call__(self,options,args):
        
        self.loadRootStyle()
    
        ROOT.TH1.SetDefaultSumw2(True)

        fin = self.open(options.infile)
        for cat in options.categories:
            for photon in "leading", "subLeading":
                for var in options.variables:
                    name = "dipho_1_%sPhoton_%s" % (photon,var)
                    num = fin.Get("num_%s/%s" % (cat,name))
                    den = fin.Get("den_%s/%s" % (cat,name))
                    
                    print name
                    
                    pname = "%s_%s_%s" % ( cat, photon, var )
                    
                    ratio = num.Clone(pname)
                    ratio.Sumw2()
                    num.Sumw2()
                    den.Sumw2()
                    ratio.Divide(num,den,1.,1.,"B")
                    
                    print name, num.Integral()/den.Integral()

                    canv = ROOT.TCanvas(pname,pname)
                    ratio.GetYaxis().SetTitle("#varepsilon")
                    ratio.GetYaxis().SetRangeUser(0.,1.1)
                    ratio.GetXaxis().SetTitle(pname)
                    ratio.SetLineColor(ROOT.myColorA1)
                    den.SetLineColor(ROOT.myColorB1)
                    ratio.Draw("e")
                    den.Scale(0.6/den.GetMaximum())
                    den.Draw("hist same")
                    self.keep( [canv,ratio,den], True )
                    

                    ## canv = ROOT.TCanvas("dist_%s" % pname,"dist_%s" % pname)
                    ## den.GetXaxis().SetTitle(pname)
                    ## self.keep( [den,canv], True )
                    
        

if __name__ == "__main__":
    app = TrgEffPlots()
    app.run()

        
