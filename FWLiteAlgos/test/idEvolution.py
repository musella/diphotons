#!/bin/env python

from pyrapp import *
from optparse import OptionParser, make_option

from copy import copy
from pprint import pprint
import csv,os

from math import sqrt

import array

# ------------------------------------------------------------------------------------------
class IdEvolution(PlotApp):

    def __init__(self):
        super(IdEvolution,self).__init__(
            ### default_cats=["isEB := abs(scEta)<1.5"],
            default_cats=["isEB := abs(scEta)<1.5","isEE := abs(scEta)>1.5"],
            option_list=[make_option("--variables",dest="variables",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",
                                     default=[
                        "absScEta := scEta*((scEta>=0.)-(scEta<0.))[100,0.,2.5]",
                        "pt[100000,100,2000]",
                        ### "passPixelVeto := !hasPixelSeed[2,0,2]-","passElectronVeto[2,0,2]-",
                        ### "sigmaIetaIeta[5000,0.,5.e-2]",
                        ### "r9[5000,0.,1.1]-","r1x5[5000,0.,1.1]-","r2x5[5000,0.,1.1]-",
                        ### "r1x5[5000,0.,1.1]-",
                        ### "r1 := e1x5/e5x5[5000,0.,1.]-",
                        ### "s4[5000,0.,1.]-",
                        ### 
                        ### ## "maxDR[1000,0.,0.2]",
                        ### ## "maxDRERatio := maxDRRawEnergy/energy[5000,0.,1.]",
                        ### 
                        ### ## "chgIsoWrtVtx0[1000,0,50.]",
                        ### ## "chgIsoWrtVtx1[1000,0,50.]",
                        ## "chgIsoWrtWorstVtx[1000,0,50.]",
                        ## ### 
                        "phoIsoBlock[1000,0,50]",
                        "phoIsoVeto007[1000,0,50]",
                        "phoIsoVeto015[1000,0,50]",
                        "phoIsoBlockVeto015[1000,0,50]",
                        
                        ## "relPhoIsoBlock := phoIsoBlock/pt[1000,0,2]",
                        ## "relPhoIsoVeto007 := phoIsoVeto007/pt[1000,0,2]",
                        ## "relPhoIsoVeto015 := phoIsoVeto015/pt[1000,0,2]",
                        
                        ### "relNeuIsoBlock := neuIsoBlock/pt[1000,0,2]",
                        ### "relNeuIsoRing005 := neuIsoRing005/pt[1000,0,2]",
                        "neuIsoBlock[1000,0,50]",
                        "neuIsoBlockVeto005 := neuIsoBlock-neuIsoRing005[1000,0,50]",
                        ### "relNeuIsoBlockVeto005 := neuIsoBlockVeto005/pt[1000,0,2]",
                        "neuIsoBlockVeto010 := neuIsoBlock-neuIsoRing005-neuIsoRing010[1000,0,50]",
                        ### "relNeuIsoBlockVeto010 := neuIsoBlockVeto010/pt[1000,0,2]",
                        "neuIsoBlockVeto020 := neuIsoBlock-neuIsoRing005-neuIsoRing010-neuIsoRing020[1000,0,50]",
                        ### "relNeuIsoBlockVeto020 := neuIsoBlockVeto020/pt[1000,0,2]",
                        
                        ### "hadronicOverEm[1000,0,0.2]","hadronicDepth1OverEm[1000,0,0.2]","hadronicDepth2OverEm[1000,0,0.1]",
                        ### "hadTowOverEm[1000,0,0.2]",
                                              ],
                                     ### default=["sigmaIetaIeta","r1x5","r2x5","r9",
                                     ###          "hadronicOverEm","hadronicDepth1OverEm","hadronicDepth2OverEm",
                                     ###          "r1 := e1x5/e5x5","maxDR","maxDRERatio := maxDRRawEnergy/energy",
                                     ###          "s4"
                                     ###          ],
                                     help="default: %default"),
                         make_option("--xvar",dest="xvar",action="store",type="string",
                                     default="pt[100000,100,2000]",
                                     help="default: %default"),
                         make_option("-w","--weight",dest="weight",action="store",type="string",
                                     default="weight*(pt>100)", help="default: %default"
                                     ),
                         make_option("--trees",dest="trees",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",
                                     default=["promptTree","fakesTree"],
                                     help="default: %default"),
                         make_option("--store",dest="store",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",
                                     default="absScEta",help="default: %default"),
                         make_option("--reweight",action="store",default=None,type="string"
                                     ),
                         make_option("--wread",action="store_true",default=False
                                     ),
                         make_option("--wvars",dest="wvars",action="callback",callback=optpars_utils.ScratchAppend(),
                                     type="string",default=["pt","absScEta"]),
                         make_option("--wrng",dest="wrng",action="callback",callback=optpars_utils.Load(),
                                     type="string",default={"isEB" : [0.,1.5], "isEE" : [1.5,2.5] }),
                         ]
            )

        self.aliases_ = {}
        self.signs_ = {}
        self.store_ = []
        
        global ROOT, style_utils
        import ROOT
        import pyrapp.style_utils as style_utils
        ROOT.gSystem.Load("libdiphotonsCategoryOptimizer")
        
        self.setStyle("*quantile*", [ ("SetLineWidth",2), ("SetMarkerSize",2) ] )
        

    def __call__(self,options,args):
        
        self.loadRootStyle()
    
        ROOT.TH1.SetDefaultSumw2(True)

        fin = self.open(options.infile)
        sigTree = fin.Get(options.trees[0])
        
        catvars = {}

        variables = {}
        for var in options.variables:
            var,bins = self.getVar(var)
            variables[var] = bins
        self.getVar(options.xvar)
        
        self.setAliases(sigTree)
        
        win = False
        wname = None
        wfuncs = {}
        if options.reweight:
            fnam,wname = options.reweight.split("::")
            win = self.open(fnam)
            
        for cat in options.categories:
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
                    win.ls()
                    hist = win.Get( hname )
                    sf = ROOT.SliceFitter(hist,"pol2",*options.wrng[name])
                    wfuncs[tree].add(sf)
                    self.keep(sf)
                    xbins = [ hist.GetXaxis().GetXbins()[ib] for ib in range(hist.GetNbinsX()+1) ]
                    xvar = self.getVar(options.xvar)[0]
            else:
                xbins,xvar = self.recursiveBinning(xvar,sigTree,weight)
            
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
        sigQuantiles = [0.99,0.95,0.9,0.85,0.5]
        bkgQuantiles = [0.7,0.5,0.4,0.3]
        
        sigColors    = [ROOT.kBlue-2,ROOT.kBlue-1,ROOT.kBlue,ROOT.kBlue+1,ROOT.kBlue+2]
        bkgColors    = [ROOT.kRed-2,ROOT.kRed-1,ROOT.kRed,ROOT.kRed+1,ROOT.kRed+2]
        
        trees = {}
        for tnam in sig,bkg:
            tree = fin.Get(tnam)
            trees[tnam] = tree
            self.setAliases(tree)
            
        if win:
            if not options.wread:
                fwei = self.open("%s/wei.root" % options.outdir, "recreate")
                wd = ROOT.gDirectory
                fwei.cd()
                for tnam in sig,bkg:
                    friend = ROOT.TTree("%s_rewei" % tnam,"%s_rewei" % tnam)
                    func = wfuncs[tnam].asTF2("%s_rewei" % tnam)
                    tree = trees[tnam]
                    print "Reweighting %s" % tnam
                    ROOT.fillReweight(options.wvars[0],options.wvars[1],options.weight,func,tree,friend)
                    canv = ROOT.TCanvas("%s_rewei" % tnam,"%s_rewei" % tnam)
                    canv.cd()
                    func.Draw("colz")
                    self.keep( [canv, func ] )
                    self.autosave(True)
                    friend.Write()
                fwei.Close()
                wd.cd()
                
            options.weight = "(%s) * rewei" % ( options.weight )
            fwei = self.open("%s/wei.root" % options.outdir)
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
                    
                    ## plots[cat][name][var] = pdf2D
                    if name == sig:
                        canv = ROOT.TCanvas("pdfs_%s_%s" % (var,cat))
                        canvs[var] = canv
                        
                        plots[cat][name][var] = getQuantilesGraphs(pdf2D,sigQuantiles,errors=False,sign=self.signs_[var])
                        frames[cat][var] = pdf2D.Clone("%s_%s" % ( var, cat ) )
                        style_utils.colors(pdf1D,sigColors[0])
                        
                        canv.cd()
                        pdfs.append( pdf1D.DrawNormalized("hist") )
                    else:
                        plots[cat][name][var] = getQuantilesGraphs(pdf2D,bkgQuantiles,errors=False,sign=self.signs_[var])
                        style_utils.colors(pdf1D,bkgColors[0])
                        pdf1D.SetFillStyle(0)
                        canvs[var].cd()
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

                canv = ROOT.TCanvas("qtiles_%s"%name,name)
                legSig = ROOT.TLegend(0.7,0.7,0.9,0.9)
                legBkg = ROOT.TLegend(0.5,0.7,0.7,0.9)

                for gr,col,qt in zip(sigGraphs,sigColors,sigQuantiles):
                    legSig.AddEntry(gr,"#varepsilon(sig)=%1.2g" % qt, "lp")
                    style_utils.colors(gr,col)
                    
                for gr,col,qt in zip(bkgGraphs,bkgColors,bkgQuantiles):
                    legBkg.AddEntry(gr,"#varepsilon(bkg)=%1.2g" % qt, "lp")
                    style_utils.colors(gr,col)
                
                ## frame = sig2D.Clone()
                graphs = sigGraphs+bkgGraphs
                self.keep( graphs+[frame,canv,legSig,legBkg], True )
                
                canv.cd()
                ypoints = [ gr.GetY()[i] for gr in graphs for i in range(gr.GetN()) ]
                ymin = min( ypoints )*0.9
                ymax = max( ypoints )*1.2
                
                frame.Reset("ICE")
                frame.GetYaxis().SetRangeUser(ymin,ymax)
                frame.Draw()
                for g in graphs:
                    g.Draw("lp")
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
        print var
        if var.endswith("-"):
            sign = -1
        else:
            sign = +1
        name,binning = var.rstrip("-").rstrip("]").rsplit("[",1)
        
        binning = binning.split(",")
        if len(binning) == 3:
            nbins = int(binning[0])
            xmin = float(binning[1])
            xmax = float(binning[2])
            step = ( xmax - xmin ) / float(nbins)
            xbins = array.array('d',[xmin+step*float(ib) for ib in range(nbins+1)])
        else:
            xbins = array.array('d',[float(b) for b in binning])
        
        if ":=" in name:
            name,vdef = [ t.lstrip(" ").rstrip(" ") for t in name.split(":=",1) ]
            self.aliases_[name] = vdef
            
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
        ### pdf = ROOT.TH1F("%s_pdf" % label, "%s_pdf" % label, len(xbins)-1, xbins)
        ### tree.Draw("%s>>%s_pdf" % ( name, label), "weight", "goff" )
        
        ## return ROOT.integrate1D(pdf)
        return name, xbins[0],xbins[-1], ROOT.mkCdfInv(pdf,xbins[0],xbins[-1])
        

    def recursiveBinning(self,var,tree,weight,binsPerStep=12,nsteps=4,plot=False):
    ### def recursiveBinning(self,var,tree,weight,binsPerStep=15,nsteps=1,plot=False):
        
        
        istep = 0
        print "recursiveBinning step %d " % istep
        firstStep = self.makeCdf(var,tree,weight)
        name, xmin, xmax, cdf = firstStep
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

            print xbins
            
            istep += 1 
            
            nextmin = cdf.eval(1-width)
            width = (xmax - nextmin)/float(100*binsPerStep)
            nextbins = [ nextmin + ibin*width for ibin in range(100*binsPerStep) ]
            nextbins.append(xmax)
            nextbins = array.array('d', nextbins)
            curstep = self.makeCdf(name,tree,weight,nextbins,"%s_step%d" % ( name, istep ) )
            
            name, xmin, xmax, cdf = curstep
            
        xbins.append(xmax)

        return xbins,name
    
if __name__ == "__main__":
    app = IdEvolution()
    app.run()

        
