#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, json, glob
from pprint import pprint
import array

from getpass import getuser

from math import sqrt, log

## ------------------------------------------------------------------------------------------------------------
class TriggerRatesApp(PlotApp):
    """
    """
    
    ## ------------------------------------------------------------------------------------------------------------
    def __init__(self,option_list=[],option_groups=[]):
        """ 
        Constructor
        """
        super(TriggerRatesApp,self).__init__(option_groups=[
                ("TriggerRatesApp",[                        
                        make_option("--bits",dest="bits",action="callback",type="string",
                                    callback=optpars_utils.ScratchAppend(),
                                    help="list of bits to be studied",default=["HLT_DoublePhoton85_v2","HLT_DoublePhoton50_v2","HLT_Photon100_HE10_v2","HLT_Photon165_HE10_v2"]),
                        make_option("--cross-sections",dest="cross_sections",action="callback",type="string",
                                    callback=optpars_utils.Load(),default={},
                                    help="cross sections database"),
                        make_option("--lumi",dest="lumi",action="store",type="float",
                                    default=5.e-3,
                                    help="luminosity"),
                        ]
                 )
            ]+option_groups,option_list=option_list)
        
        ## load ROOT (and libraries)
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils
        
        ROOT.gStyle.SetOptStat(111111)

    ## ------------------------------------------------------------------------------------------------------------
    def __call__(self,options,args):
        """ 
        Main method. Called automatically by PyRoot class.
        """
        
        self.loadRootStyle()
        
        ## print self.options.cross_sections
        summary = { bit : [] for bit in options.bits }
        lumi = options.lumi

        folders = glob.glob(os.path.join(options.input_dir,"*"))
        for folder in folders:
            dataset = os.path.basename(folder)
            ## print folder, dataset
            xsec = options.cross_sections[dataset]["xs"]*options.cross_sections[dataset].get("br",1.)*options.cross_sections[dataset].get("kf",1.)
            chain = ROOT.TChain("myTriggerSummary/triggerBits","myTriggerSummary")
            for fil in glob.glob(folder+"/*/*/*/*.root"):
                chain.AddFile(fil)
            
            processed = ROOT.TH1D("%s_processed" % dataset,"processed",2,-0.5,1.5)
            chain.Draw("1>>%s" % processed.GetName(),"weight","goff")
            self.keep(processed)
            
            hbits = []
            for bit in options.bits:
                triggered = ROOT.TH1D("%s_triggered_%s" % (dataset, bit),bit,2,-0.5,1.5)
                chain.Draw("1>>%s" % triggered.GetName(),"%s*weight" % bit,"goff")
                hbits.append(triggered)
            
            self.keep( hbits )
            
            print "--------------------------------------------------------------------------"
            if processed.GetEntries() != 0:
                scl = xsec*lumi/processed.GetEntries()
            else:
                print "Warning: no processed events"
                scl = 1
            print dataset, processed.GetEntries(), scl
            for h in hbits:
                bit = h.GetName().replace("%s_triggered_" % dataset,"")
                nev = h.GetEntries()
                if nev == 0:
                    edown=0.
                    eup=-log(0.683*0.5)
                else:
                    eup=sqrt(nev)
                    edown=eup
                
                summary[bit].append( (dataset, nev*scl, eup*scl, edown*scl) )
                ## print bit, nev*scl, eup*scl, edown*scl
            print "--------------------------------------------------------------------------"


        ressstr = ""
        for bit,results in summary.iteritems():
            sresults = sorted( results, key=lambda x: x[1] )
            ressstr += "--------------------------------------------------------------------------\n"
            ressstr += bit+"\n"
            total = [0.,0.,0.]
            for s in sresults:
                total[0] += s[1]
                total[1] += s[2]**2
                total[2] += s[3]**2
                ressstr += s[0][:20].ljust(22) + " %1.2g +%1.1g -%1.1g\n" %  s[1:4]
            ressstr += "--------------------------------------------------------------------------\n"
            total[1],total[2] = sqrt(total[1]),sqrt(total[2])
            ressstr += "total".ljust(22) + "%1.2g +%1.1g -%1.1g\n" % tuple(total)
            ressstr += "--------------------------------------------------------------------------\n"
            
        with open("summary.txt","w+") as fout:
            fout.write(ressstr)
            fout.close()
        print ressstr
        
        

if __name__ == "__main__":
    app = TriggerRatesApp()
    app.run()
