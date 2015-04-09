#!/bin/env python

import sys, types, os
import numpy
from math import sqrt, log
import json
json.encoder.FLOAT_REPR = lambda o: format(o, '.3f')

## from plotVbfVar import *

from optparse import OptionParser, make_option
from  pprint import pprint

objs = []

def main(options,args):


    mass = None
    frames = {}
    colors = [ROOT.kRed,ROOT.kBlue,ROOT.kGreen,ROOT.kMagenta]
    
    allcats = []
    for icat in range(options.ncat):
        if options.nbins == 0:
            allcats.append( "cat%d" % icat  )
        else:
            for ibin in range(options.nbins):
                allcats.append( "bin%d_cat%d" % (ibin,icat) )

    for ifile,file in enumerate(options.files):
        fin = ROOT.TFile.Open(file)
        ws = fin.Get("cms_hgg")

        fname = os.path.basename(file).replace(".root","")
        
        ws.exportToCint(fname)

        objs.extend([fin,ws])
        
        if not mass:
            mass = ws.var("CMS_hgg_mass")

        for cat in allcats:
            if not cat in frames:
                fframe = mass.frame()
                dframe = mass.frame()
                txt = []
                frames[cat] = fframe,dframe,txt
            else:
                fframe,dframe,txt = frames[cat]
                
            model = ws.pdf("model_bkg_%s" % cat)
            
            deriv = model.derivative(mass,1)
            objs.append(deriv)
                        
            bkg  = ws.data("bkg_%s" % cat)
            sig  = ws.data("sig_%s" % cat)
            rms  = sqrt(sig.covariance( mass, mass ))
            
            bkg.plotOn(fframe,ROOT.RooFit.LineColor(colors[ifile]),ROOT.RooFit.Binning(80))
            model.plotOn(fframe,ROOT.RooFit.LineColor(colors[ifile]))
            bkgPerGeV = fframe.getObject( int(fframe.numItems() - 1) ).Eval(125.);
            
            deriv.plotOn(dframe,ROOT.RooFit.LineColor(colors[ifile]),ROOT.RooFit.Range(101,179))

            txt.append( "-----------------------------------------------------------------------------" )
            txt.append( "%s" % fname )
            txt.append( "-----------------------------------------------------------------------------" )
            txt.append( "%s               : %g" % ( sig.GetName(), sig.sumEntries() ) )
            txt.append( "%s RMS           : %g" % ( sig.GetName(), rms ) )
            txt.append( "%s/GeV           : %g" % ( bkg.GetName(), bkgPerGeV ) )
            txt.append( "%s eff S/sqrt(B)     : %g" % ( sig.GetName().replace("sig_",""), sig.sumEntries()/sqrt(bkgPerGeV*rms) ) )
            txt.append( "%s               : %g" % ( bkg.GetName(), bkg.sumEntries() ) )
            txt.append( "" )
            
    if not os.path.exists(options.out):
        os.mkdir(options.out)
    os.chdir(options.out)
        
    for cat,fms in frames.iteritems():
        fframe,dframe,txt = fms
        
        fcanv = ROOT.TCanvas(cat,cat)
        fframe.Draw()

        dcanv = ROOT.TCanvas("deriv_%s" %  cat,"deriv_%s" % cat)
        dframe.Draw()

        objs.extend( [fcanv,dcanv] )

        fframe.Draw()
        dframe.Draw()

        for fmt in ["png"]:
            for c in fcanv,dcanv:
                c.SaveAs( "%s.%s" % ( c.GetName(), fmt )  )
                
        tout = open("%s.txt" % fcanv.GetName(),"w+")
        for li in txt:
            tout.write("%s\n" % li)
        tout.close()
        
    objs.append(frames)
    
if __name__ == "__main__":

    parser = OptionParser(option_list=[
            make_option("-i", "--infile",
                        action="append", type="string", dest="files",
                        default=[],
                        help="input file",
                        ),
            make_option("-o", "--out",
                        action="store", type="string", dest="out",
                        default="./",
                        help="",
                        ),
            make_option("-s", "--settings",
                        action="append", type="string", dest="settings",
                        default=[],
                        help="json file with additional settings",
                        ),
            make_option("-N", "--nbins",
                        action="store", type="int", dest="nbins",
                        default=0,
                        help="number of categories",
                        ),
            make_option("-n", "--ncat",
                        action="store", type="int", dest="ncat",
                        default=4,
                        help="number of categories",
                        ),
            ]
                          )

    (options, args) = parser.parse_args()
    sys.argv.append("-b")
    
    pprint(options.__dict__)

    import ROOT
    
    main(options,args)
