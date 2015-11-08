#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, json

from pprint import pprint

import array

from getpass import getuser

from combine_maker import CombineApp

import random

from math import sqrt

## ----------------------------------------------------------------------------------------------------------------------------------------
class BiasApp(CombineApp):
    """
    Class to handle template fitting.
    Takes care of preparing templates starting from TTrees.
    Inherith from PyRapp and PlotApp classes.
    """
    
    ## ------------------------------------------------------------------------------------------------------------
    def __init__(self):
        
        super(BiasApp,self).__init__(
            option_groups=[
                ("Bias study options", [
                        make_option("--throw-toys",dest="throw_toys",action="store_true",default=False,
                                    help="Throw toy MC",
                                    ),
                        make_option("--binned-toys",dest="binned_toys",action="store_true",default=False,
                                    help="Use binned toys",
                                    ),
                        make_option("--throw-from-model",dest="throw_from_model",action="store_true",default=False,
                                    help="Throw toys from fit to full dataset",
                                    ),
                        make_option("--lumi-factor",dest="lumi_factor",action="store",default=1.,type="float",
                                    help="Luminosity normalization factor",
                                    ),
                        make_option("--fit-toys",dest="fit_toys",action="store_true",default=False,
                                    help="Fit toy MC",
                                    ),
                        make_option("--approx-minos",dest="approx_minos",action="store_true",default=False,
                                    help="Use approximate minos errors",
                                    ),
                        make_option("--plot-toys-fits",dest="plot_toys_fits",action="store_true",default=False,
                                    help="Make plots with fit results",
                                    ),
                        make_option("--n-toys",dest="n_toys",action="store",type="int",default=False,
                                    help="Number of toys",
                                    ),
                        make_option("--first-toy",dest="first_toy",action="store",type="int",default=False,
                                    help="First toy to fit",
                                    ),
                        make_option("--fit-range",dest="fit_range",action="callback",type="string",callback=optpars_utils.ScratchAppend(float),
                                    default=[300,500],
                                    help="Observable range for the fit region : [%default]",
                                    ),
                        make_option("--test-categories",dest="test_categories",action="callback",type="string",callback=optpars_utils.ScratchAppend(str),
                                    default=[],
                                    help="Categories to test : [%default]",
                                    ),
                        make_option("--test-range",dest="test_ranges",action="callback",type="string",callback=optpars_utils.ScratchAppend(float),
                                    default=[1000.,5000.],
                                    help="Observable range for the test region : [%default]",
                                    ),
                        make_option("--exclude-test-range",dest="exclude_test_range",action="store_true",default=False,
                                    help="Exclude test range from fit",
                                    ),
                        make_option("--components",dest="components",action="callback",type="string",callback=optpars_utils.ScratchAppend(str),
                                    help="Background components",default=[],
                                    ),
                        make_option("--models",dest="models",action="callback",type="string",callback=optpars_utils.ScratchAppend(str),
                                    help="Backround models",default=[],
                                    ),
                        make_option("--analyze-bias",dest="analyze_bias",action="store_true",default=False),
                        make_option("--bias-files",dest="bias_files",action="callback",type="string",callback=optpars_utils.ScratchAppend(str),
                                    default=[]
                                    ),
                        make_option("--bias-labels",dest="bias_labels",action="callback",type="string",callback=optpars_utils.ScratchAppend(str),
                                    default=[]
                                    ),                    
                        make_option("--bias-param",dest="bias_param",action="callback",type="string",callback=optpars_utils.Load(scratch=True),
                                    default={
                                "EBEB_dijet_230_7000" : "(x>500.)*((0.06*((x/600.)^-4))+1e-6)",
                                "EBEE_dijet_320_7000" : "(x>500.)*((0.1*((x/600.)^-5)))",
                                "EBEB_dijet_300_6000" : "(x>500.)*((0.22*((x/600.)^-5))+1e-6)",
                                "EBEB_dijet_400_6000" : "(x>500.)*((0.2*((x/600.)^-5))+2e-6)",
                                "EBEB_dijet_500_6000" : "(x>500.)*((0.18*((x/600.)^-5))+5e-6)",
                                "EBEE_dijet_300_6000" : "(x>500.)*((0.06*((x/600.)^-4))+1e-7)",
                                "EBEE_dijet_400_6000" : "(x>500.)*((0.04*((x/600.)^-4))+1e-7)",
                                "EBEE_dijet_500_6000" : "(x>500.)*((0.04*((x/600.)^-4))+1e-7)",
                                ### "EBEB_dijet_300_6000" : "(0.110705*((x/600.000000)^-6.04594))+7.28617e-05",
                                ### "EBEB_dijet_400_6000" : "(0.103261*((x/600.000000)^-6.14835))+7.29511e-05",
                                ### "EBEB_dijet_500_6000" : "(0.125619*((x/600.000000)^-6.23181))+7.29431e-05",
                                ### "EBEE_dijet_300_6000" : "(0.0472049*((x/600.000000)^-5.33805))+7.25388e-05",
                                ### "EBEE_dijet_400_6000" : "(0.0397002*((x/600.000000)^-5.03543))+7.21574e-05",
                                ### "EBEE_dijet_500_6000" : "(0.0514193*((x/600.000000)^-5.33518))+7.24997e-05",
                                }
                                    ),                    
                        make_option("--scale-bias",dest="scale_bias",action="store",type="float",
                                    default=1.
                                    ),                    
                        ]
                 )
                ]
            )
        
        ## load ROOT (and libraries)
        global ROOT, style_utils
        import ROOT
        import diphotons.Utils.pyrapp.style_utils as style_utils
        ROOT.gSystem.Load("libdiphotonsUtils")
        
    def __call__(self,options,args):
        ## load ROOT style
        self.loadRootStyle()
        ROOT.TGaxis.SetMaxDigits(3)
        from ROOT import RooFit

        printLevel = ROOT.RooMsgService.instance().globalKillBelow()
        ROOT.RooMsgService.instance().setGlobalKillBelow(RooFit.FATAL)

        self.use_custom_pdfs_ = options.use_custom_pdfs

        options.only_subset = [options.fit_name]
        if options.analyze_bias:        
            options.skip_templates = True

        self.setup(options,args)
        
        if options.throw_toys:
            self.throwToys(options,args)
        
        if options.fit_toys:
            self.fitToys(options,args)

        if options.analyze_bias:
            self.analyzeBias(options,args)
            
    ## ------------------------------------------------------------------------------------------------------------
    def throwToys(self,options,args):
        
        fitname = options.fit_name
        fit = options.fits[fitname]
        
        roobs = self.buildRooVar(*(self.getVar(options.observable)))
        roowe = self.buildRooVar("weight",[])
        
        for comp,model in zip(options.components,options.models):
            if comp != "":
                comp = "%s_" % comp
            for cat in fit["categories"]:
                                
                treename = "mctruth_%s%s_%s" % (comp,fitname,cat)
                
                print treename
                dset = self.rooData(treename)
                dset.Print()

                reduced = dset.reduce(ROOT.RooArgSet(roobs),"%s > %f && %s < %f" % (roobs.GetName(),roobs.getMin(),roobs.GetName(),roobs.getMax()))
                binned = reduced.binnedClone()
                
                if options.throw_from_model:
                    print "Throwing toys from fit to full dataset"

                    pdf = self.buildPdf(model,"full_%s%s" % (comp,cat), roobs )
                    norm = self.buildRooVar("full_norm_%s_%s%s" %  (model,comp,cat), [], importToWs=False )
                    norm.setVal(dset.sumEntries())
                    extpdf = ROOT.RooExtendPdf("ext_%s_%s%s" %  (model,comp,cat),"ext_%s_%s%s" %  (model,comp,cat),pdf,norm)
                    extpdf.fitTo(binned,ROOT.RooFit.Strategy(2))
                    extpdf.fitTo(reduced,ROOT.RooFit.Strategy(1))
                    
                    ## freeze parameters before importing
                    deps = pdf.getDependents(self.pdfPars_)
                    itr = deps.createIterator()
                    var = itr.Next()
                    while var:
                        var.setConstant(True)
                        var = itr.Next()
                    
                    frame = roobs.frame()
                    binned.plotOn(frame, ROOT.RooFit.DataError(ROOT.RooAbsData.Poisson))
                    extpdf.plotOn(frame)
                    

                    resid  = roobs.frame()
                    hist   = frame.getObject(int(frame.numItems()-2))
                    fit    = frame.getObject(int(frame.numItems()-1))
                    hresid = frame.residHist(hist.GetName(),fit.GetName(),True)
                    resid.addPlotable(hresid,"PE")
                    
                    canv = ROOT.TCanvas("full_fit_%s_%s%s" % (model,comp,cat), "full_fit_%s_%s%s" % (model,comp,cat) )
                    canv.Divide(1,2)
                    
                    canv.cd(1)
                    ROOT.gPad.SetPad(0.,0.35,1.,1.)
                    ROOT.gPad.SetLogy()
                    ROOT.gPad.SetLogx()

                    canv.cd(2)
                    ROOT.gPad.SetPad(0.,0.,1.,0.35)
                    
                    canv.cd(1)
                    frame.GetXaxis().SetMoreLogLabels()
                    frame.GetYaxis().SetLabelSize( frame.GetYaxis().GetLabelSize() * canv.GetWh() / ROOT.gPad.GetWh() )
                    frame.GetYaxis().SetRangeUser( 1.e-6,500. )
                    frame.Draw()

                    canv.cd(2)
                    ROOT.gPad.SetGridy()
                    ROOT.gPad.SetLogx()
                    resid.GetXaxis().SetMoreLogLabels()
                    resid.GetYaxis().SetTitleSize( frame.GetYaxis().GetTitleSize() * 6.5/3.5 )
                    resid.GetYaxis().SetTitleOffset( frame.GetYaxis().GetTitleOffset() * 6.5/3.5 )
                    resid.GetYaxis().SetLabelSize( frame.GetYaxis().GetLabelSize() * 6.5/3.5 )
                    resid.GetXaxis().SetTitleSize( frame.GetXaxis().GetTitleSize() * 6.5/3.5 )
                    resid.GetXaxis().SetLabelSize( frame.GetXaxis().GetLabelSize() * 6.5/3.5 )
                    resid.GetYaxis().SetTitle("pull")
                    resid.GetYaxis().SetRangeUser( -5., 5. )
                    resid.Draw()
                    
                    
                    self.keep(canv)
                    self.autosave(True)
                    
                else:
                    pdf = ROOT.RooHistPdf(treename,treename,ROOT.RooArgSet(roobs),binned)
                    norm = self.buildRooVar("norm_%s" %  treename, [], importToWs=False )
                    norm.setVal(dset.sumEntries())
                    print norm.getVal()
                    extpdf = ROOT.RooExtendPdf("ext_%s" %  treename, "ext_%s" %  treename, pdf, norm)
                    print norm.getVal(), extpdf.expectedEvents(ROOT.RooArgSet())
                    
                pdf.SetName("pdf_%s" % treename)
                norm.SetName("norm_%s" % treename)
                norm.setVal(dset.sumEntries()*options.lumi_factor)
                ## extpdf.SetName("geneator_%s" % treename)
                self.workspace_.rooImport(pdf)
                self.workspace_.rooImport(norm)
                
                tnorm = dset.sumEntries()*options.lumi_factor
                print tnorm, norm.getVal()
                ntoys = options.n_toys
                
                if ntoys < 0:
                    data = pdf.generate(ROOT.RooArgSet(roobs),ROOT.gRandom.Poisson(tnorm))
                    asimov = data.binnedClone()
                    asimov = ROOT.DataSetFiller.throwAsimov(tnorm,pdf,roobs,asimov)
                    asimov.SetName("toy_%s%s_asimov" % (comp,cat))
                    self.workspace_.rooImport(asimov)
                else:
                    for toy in range(ntoys):
                        data = pdf.generate(ROOT.RooArgSet(roobs),ROOT.gRandom.Poisson(tnorm)) ## 
                        if options.binned_toys: data=data.binnedClone()
                        toyname = "toy_%s%s_%d" % (comp,cat,toy)
                        data.SetName(toyname)
                        data.SetTitle(toyname)
                        self.workspace_.rooImport(data)
                    
        self.saveWs(options)


        
    ## ------------------------------------------------------------------------------------------------------------
    def fitToys(self,options,args):
        
        fout = self.openOut(options)
        fitname = options.fit_name
        fit = options.fits[fitname]
        
        roobs = self.buildRooVar(*(self.getVar(options.observable)), recycle=True)
        roobs.setRange("fitRange",*options.fit_range)
        minx = options.fit_range[0]
        maxx = options.fit_range[1]
        minf = minx
        maxf = maxx
        testRanges = []
        for itest in xrange(len(options.test_ranges)/2):
            rname = "testRange_%1.0f_%1.0f" % ( options.test_ranges[2*itest],options.test_ranges[2*itest+1] )
            print rname, options.test_ranges[2*itest:2*itest+2]
            minx = min(minx,options.test_ranges[2*itest])
            maxx = max(maxx,options.test_ranges[2*itest+1])
            if options.test_ranges[2*itest] < minf:
                continue
            print rname, options.test_ranges[2*itest:2*itest+2]
            roobs.setRange( rname, *options.test_ranges[2*itest:2*itest+2] )
            testRanges.append( (rname,options.test_ranges[2*itest:2*itest+2]) )
        ## roobs.setRange("fullRange",roobs.getMin(),roobs.getMax())
        roobs.setRange("origRange",roobs.getMin(),roobs.getMax())
        print "fullRange", minx, maxx
        roobs.setRange("fullRange",minx,maxx)
        roobs.setMin(minx)
        roobs.setMax(maxx)

        roowe = self.buildRooVar("weight",[])
        
        fitops = [ ROOT.RooFit.PrintLevel(-1),ROOT.RooFit.Warnings(False),ROOT.RooFit.NumCPU(4),ROOT.RooFit.Minimizer("Minuit2"), ROOT.RooFit.Strategy(2), ROOT.RooFit.Offset(True) ]
        if options.verbose:
            fitops[0] = ROOT.RooFit.PrintLevel(-1)
            fitops[1] = ROOT.RooFit.Warnings(True) 
            
        for comp,model in zip(options.components,options.models):
            if comp != "":
                comp = "%s_" % comp
            print comp,model
            categories = options.test_categories if len(options.test_categories)>0 else fit["categories"].keys()
            for cat in categories:
                pdf = self.buildPdf(model,"%s%s" % (comp,cat), roobs )
                
                biases = {}
                for testRange in testRanges:
                    rname = testRange[0]
                    ntp = ROOT.TNtuple("tree_bias_%s%s_%s_%s" % (comp,cat,model,rname),"tree_bias_%s%s_%s_%s" % (comp,cat,model,rname),"toy:truth:fit:minos:errhe:errp:errm:bias:fitmin:fitmax" )
                    biases[rname] = ntp
                    self.store_[ntp.GetName()] = ntp
                    
                generator = self.rooPdf("pdf_mctruth_%s%s_%s" % (comp,fitname,cat))
                gnorm     = self.buildRooVar("norm_mctruth_%s%s_%s" % (comp,fitname,cat), [], recycle=True)
                gnorm.Print() 
                
                trueNorms = {}
                pobs  = generator.getDependents(ROOT.RooArgSet(roobs))[roobs.GetName()]
                pobs.setRange("origRange",roobs.getBinning("origRange").lowBound(),roobs.getBinning("origRange").highBound())
                renorm = generator.createIntegral(ROOT.RooArgSet(pobs),"origRange").getVal() / gnorm.getVal()
                for test in testRanges:
                    testRange,testLim = test
                    pobs.setRange(testRange,roobs.getBinning(testRange).lowBound(),roobs.getBinning(testRange).highBound())
                    trueNorms[testRange] = generator.createIntegral(ROOT.RooArgSet(pobs),testRange).getVal()/renorm

                toyslist = {}
                if options.n_toys > 0:
                    for toy in xrange(options.first_toy,options.first_toy+options.n_toys):
                        toyslist[toy]="toy_%s%s_%d" % (comp,cat,toy)
                else:
                    toyslist[-1] = "toy_%s%s_asimov" % (comp,cat)
                    
                for toy,toyname in toyslist.iteritems():
                    dset = self.rooData(toyname).reduce("%s > %f && %s < %f" % (roobs.GetName(),minx,roobs.GetName(),maxx))
                    print dset,pdf
                    
                    gnll = pdf.createNLL(dset,ROOT.RooFit.Extended())
                    gminim = ROOT.RooMinimizer(gnll)
                    gminim.setMinimizerType("Minuit2")                        
                    gminim.setEps(1000)
                    gminim.setOffsetting(True)
                    gminim.setStrategy(2)
                    gminim.setPrintLevel( -1 if not options.verbose else 2)
                    gminim.migrad()

                    if options.plot_toys_fits:
                        slabel = "%s_%s_%1.0f_%1.0f" % ( cat, model, options.fit_range[0], options.fit_range[1] )
                        self.plotBkgFit(options,dset,pdf,roobs,toyname,slabel,bias_funcs=options.bias_param)

                    for test in testRanges:
                        if options.verbose:
                            print "test range: ", test
                            
                        testRange,testLim = test
                        iname = "%s_%s_%s" % (toyname, model, testRange)

                        integral = pdf.createIntegral(ROOT.RooArgSet(roobs),ROOT.RooArgSet(roobs),testRange)
                        nomnorm = integral.getVal()*dset.sumEntries()

                        largeNum = max(0.1,nomnorm*50.)
                        roonorm = ROOT.RooRealVar("norm_%s" % iname, "norm_%s" % iname, nomnorm,-largeNum,largeNum)
                        roonorm.setConstant(False)                        
                        
                        if nomnorm == 0.:
                            continue
                        if options.verbose:
                            print "Nominal normalization : ", nomnorm, integral.getVal(), dset.sumEntries()                        

                        truenorm = trueNorms[testRange]
                        epdf = ROOT.RooExtendPdf(iname,iname,pdf,roonorm,testRange)
                        
                        if options.exclude_test_range:
                            edset = dset.reduce("%s< %f || %s >%f" % ( roobs.GetName(), testLim[0], roobs.GetName(), testLim[1] ))
                        else:
                            edset = dset
                        nll = epdf.createNLL(edset,ROOT.RooFit.Extended())

                        minim = ROOT.RooMinimizer(nll)
                        minim.setEps(1000)
                        minim.setOffsetting(True)
                        minim.setMinimizerType("Minuit2")
                        minim.setMaxIterations(15)
                        minim.setMaxFunctionCalls(100)
                        if options.verbose:
                            minim.setPrintLevel(2)
                        else:
                            minim.setPrintLevel(-1)
                            
                        if options.verbose:
                            print "Running migrad"
                            
                        for stra in range(2,3):
                            minim.setStrategy(stra)
                            migrad = minim.migrad()
                            if migrad == 0:
                                break

                        if options.verbose:
                            print "Migrad sta", migrad
                                                
                        if migrad != 0:
                            continue

                        
                        ### ## print migrad
                        ### if migrad != 0:
                        ###     minim.setStrategy(1)
                        ###     migrad = minim.migrad()
                        ###     if migrad != 0:
                        ###         continue
                        
                        nomnorm = roonorm.getVal()
                        
                        if options.verbose:
                            print "Now running hesse"

                        minim.hesse()
                        hesseerr = roonorm.getError()
                        fiterrh = roonorm.getErrorHi()
                        fiterrl = roonorm.getErrorLo()
                        
                        if not options.approx_minos:
                            if options.verbose:
                                print "Running minos"
                            minos = minim.minos(ROOT.RooArgSet(roonorm))                        
                            if minos == 0:
                                if roonorm.getErrorHi() != 0.:
                                    fiterrh = roonorm.getErrorHi()
                                if roonorm.getErrorLo() != 0.:
                                    fiterrl = roonorm.getErrorLo()
                        else:
                            if options.verbose:
                                print "Computing approximate minos errors"
                            fitval  = roonorm.getVal()
                            fiterrh = abs(roonorm.getErrorHi()/2.)
                            fiterrl = abs(roonorm.getErrorLo()/2.)
                            if options.verbose:
                                print "Creating NLL"
                            # pll = nll.createProfile(ROOT.RooArgSet(roonorm))
                            # print pll.minimizer()
                            # pll.minimizer().setEps(0.1)
                            if options.verbose:
                                print "Computing NLL at minimum"
                            # minll = pll.getVal()
                            minll  = nll.getVal()
                            if fiterrl < fitval:
                                roonorm.setVal(fitval-fiterrl)
                            else:
                                roonorm.setVal(0.1)
                                fiterrl = fitval - 0.1
                            if options.verbose:
                                print "evaluating NLL at ", roonorm.getVal()
                            # nllm =  pll.getVal()
                            roonorm.setConstant(True) 
                            ## minim.migrad()
                            minimm = ROOT.RooMinimizer(nll)
                            minimm.setPrintLevel( -1 if not options.verbose else 2)
                            minimm.setMaxIterations(15)
                            minimm.setMaxFunctionCalls(100)                            
                            minimm.setStrategy(1)
                            minimm.setEps(1000)
                            minimm.setOffsetting(True)
                            minimm.setMinimizerType("Minuit2")
                            minimm.migrad()
                            nllm =  nll.getVal()

                            roonorm.setVal(fitval+fiterrh)
                            roonorm.setConstant(True) 
                            if options.verbose:
                                print "evaluating NLL at ", roonorm.getVal()
                            ## minim.migrad()
                            minimp = ROOT.RooMinimizer(nll)
                            minimp.setPrintLevel( -1 if not options.verbose else 2)
                            minimp.setMaxIterations(15)
                            minimp.setMaxFunctionCalls(100)                            
                            minimp.setStrategy(1)
                            minimp.setEps(1000)
                            minimp.setOffsetting(True)
                            minimp.setMinimizerType("Minuit2")
                            minimp.migrad()
                            nllp =  nll.getVal()
                            ## nllp =  pll.getVal()
                            
                            if nllm-minll > 0. and nllp-minll > 0.:
                                fiterrh = max(hesseerr,fiterrh / sqrt(2.*(nllp-minll))) 
                                fiterrl = max(hesseerr,fiterrl / sqrt(2.*(nllm-minll)))
                                minos = 0
                            else:
                                minos = 1
                            
                            ## y = a x^2
                            ## a = y/x^2
                            ## 1 = a*xe^2
                            ## xe = 1/sqrt(a) = x / sqrt(y)
                        
                            ### fiterrh = roonorm.getErrorHi()
                            ### fiterrl = roonorm.getErrorLo()
                            ### ## print truenorm, nomnorm, roonorm.getVal(), fiterrl, fiterrh
                        
                        errh = fiterrh if fiterrh != 0. else hesseerr
                        errl = fiterrl if fiterrl != 0. else hesseerr
                        if nomnorm > truenorm:                            
                            bias = (nomnorm-truenorm)/abs(errl)
                        else:
                            bias = (nomnorm-truenorm)/abs(errh)

                        biases[testRange].Fill( toy, truenorm, nomnorm,  minos, hesseerr, fiterrh, fiterrl, bias, options.fit_range[0], options.fit_range[1] )
                    
                    self.autosave(True)
                        
        self.saveWs(options,fout)

    ## ------------------------------------------------------------------------------------------------------------
    def analyzeBias(self,options,args):
        
        summary = {}
        
        ROOT.gStyle.SetOptStat(1111)
        ROOT.gStyle.SetOptFit(1)
        
        profiles = {}
        bprofiles = {}
        cprofiles = {}
        
        xfirst = 1e5
        xlast  = 0.

        for fname,label in zip(options.bias_files,options.bias_labels):
            fin = self.open(fname)
            for key in ROOT.TIter(fin.GetListOfKeys()):
                name = key.GetName()
                if name.startswith("tree_bias"):                    
                    toks = name.split("_",5)[2:]
                    print toks
                    ## ['pp', 'EBEB', 'dijet', 'testRange_2500_3500']
                    comp,cat,model,rng = toks
                    tree = key.ReadObj()
                    toks.append(label)
                    
                    nlabel = "_".join(toks)
                    slabel = "_".join([cat,model,label])
                    
                    bias_func = None
                    print slabel
                    print options.bias_param.keys()
                    if slabel in options.bias_param:
                        bias_func = ROOT.TF1("err_correction",options.bias_param[slabel],0,2e+6)
                        ## bias_func.Print()
                        
                    if not slabel in profiles:
                        profile = ROOT.TGraphErrors()
                        bprofile = ROOT.TGraphErrors()
                        profiles[slabel] = profile
                        bprofiles[slabel] = bprofile
                        self.keep( [profile,bprofile] )
                        if bias_func:
                            cprofile = ROOT.TGraphErrors()
                            cprofiles[slabel] = cprofile
                            self.keep( [cprofile] )
                    else:
                        profile = profiles[slabel]
                        bprofile = bprofiles[slabel]
                        if bias_func:
                            cprofile = cprofiles[slabel]
                        
                    xmin,xmax = [float(t) for t in rng.split("_")[1:]]
                    xfirst = min(xmin,xfirst)
                    xlast = max(xmax,xlast)
                    ibin = profile.GetN()
                    
                    tree.Draw("bias>>h_bias_%s(501,-5.005,5.005)" % nlabel )
                    hb = ROOT.gDirectory.Get("h_bias_%s" % nlabel )
                    hb.Fit("gaus","L+Q")
                    
                    canv = ROOT.TCanvas(nlabel,nlabel)
                    canv.cd()
                    hb.Draw()
                    
                    self.keep( [canv,hb] )
                    self.autosave(True)
                    
                    gaus = hb.GetListOfFunctions().At(0)
                    prb = array.array('d',[0.5])
                    med = array.array('d',[0.])
                    hb.GetQuantiles(len(prb),med,prb)
                    
                    tree.Draw("abs(bias)>>h_coverage_%s(501,0,5.01)" % nlabel )
                    hc = ROOT.gDirectory.Get("h_coverage_%s" % nlabel )
                    
                    prb = array.array('d',[0.683])
                    qtl = array.array('d',[0.])
                    hc.GetQuantiles(len(prb),qtl,prb)

                    tree.Draw("fit-truth>>h_deviation_%s(501,-100.2,100.2)" % nlabel )
                    hd = ROOT.gDirectory.Get("h_deviation_%s" % ("_".join(toks)))
                    hd.Fit("gaus","L+Q")
                    
                    gausd = hd.GetListOfFunctions().At(0)
                    medd = array.array('d',[0.])
                    hd.GetQuantiles(len(prb),medd,prb)
                    profile.SetPoint(ibin,0.5*(xmax+xmin),abs(medd[0])/(xmax-xmin))
                    ## profile.SetPoint(ibin,0.5*(xmax+xmin),abs(medd[0]))
                    profile.SetPointError(ibin,0.5*(xmax-xmin),0.)
                    
                    ## bprofile.SetPoint(ibin,0.5*(xmax+xmin),med[0])
                    bprofile.SetPoint(ibin,0.5*(xmax+xmin),gaus.GetParameter(1)/(gaus.GetParameter(2)))
                    bprofile.SetPointError(ibin,0.5*(xmax-xmin),0.)

                    tree.GetEntry(0)
                    summary[nlabel] = [ gaus.GetParameter(1), gaus.GetParError(1), gaus.GetParameter(2), gaus.GetParError(2),
                                        med[0], qtl[0], gausd.GetParameter(1), gausd.GetParError(1), medd[0], medd[0]/med[0], tree.truth ]
                    if bias_func:
                        tree.SetAlias("berr","(fit-truth)/bias*%f" % max(1.,gaus.GetParameter(2)))
                        ## tree.SetAlias("berr","%f+0." % max(1.,gaus.GetParameter(2)))
                        tree.SetAlias("corr_bias","(fit-truth)/sqrt(berr^2+%f^2)" % (bias_func.Integral(xmin,xmax)*options.scale_bias) )
                        tree.Draw("corr_bias>>h_corr_bias_%s(501,-5.005,5.005)" % nlabel )
                        hc = ROOT.gDirectory.Get("h_corr_bias_%s" % nlabel )
                        hc.Fit("gaus","L+Q")
                        
                        hc.Print()
                        
                        gausc = hc.GetListOfFunctions().At(0)
                        medc = array.array('d',[0.])
                        hc.GetQuantiles(len(prb),medc,prb)
                        
                        cprofile.SetPoint(ibin,0.5*(xmax+xmin),gausc.GetParameter(1))
                        cprofile.SetPointError(ibin,0.5*(xmax-xmin),0.)

                        summary[nlabel].extend( [medc[0], gausc.GetParameter(1), gausc.GetParameter(2)] )
                        
        ### styles = [ [ (style_utils.colors,ROOT.kBlack) ],  [ (style_utils.colors,ROOT.kRed) ],  
        ###            [ (style_utils.colors,ROOT.kBlue) ],  [ (style_utils.colors,ROOT.kGreen+1) ],
        ###            [ (style_utils.colors,ROOT.kOrange) ],  [ (style_utils.colors,ROOT.kMagenta+1) ] 
        ###            ]
                    
        colors = [ ROOT.kRed, ROOT.kBlue, ROOT.kGreen+1, ROOT.kOrange, ROOT.kCyan, ROOT.kMagenta, ROOT.kYellow, ROOT.kGray ]
        markers = [ROOT.kFullCircle,ROOT.kOpenCircle,ROOT.kCyan, ROOT.kMagenta, ROOT.kYellow, ROOT.kGray ]
        styles = []
        keys = sorted(bprofiles.keys())
        nfuncs = len(options.bias_labels)
        ncat   = len(keys) / nfuncs
        for icat in range(ncat):
            for ifunc in range(nfuncs):
                styles.append( [ (style_utils.colors,colors[ifunc%len(colors)]+icat), ("SetMarkerStyle",markers[icat % len(markers)]) ] )
                
        
        ROOT.gStyle.SetOptFit(0)
        canv = ROOT.TCanvas("profile_bias","profile_bias")
        canv.SetLogx()
        canv.SetLogy()
        canv.SetGridy()
        leg  = ROOT.TLegend(0.6,0.6,0.9,0.9)
        leg.SetFillStyle(0)
        profiles[keys[0]].GetXaxis().SetRangeUser(xfirst,xlast)
        bprofiles[keys[0]].GetXaxis().SetRangeUser(xfirst,xlast)                    
        ckeys = sorted(cprofiles.keys())
        print ckeys
        if len(ckeys) > 0:
            cprofiles[ckeys[0]].GetXaxis().SetRangeUser(xfirst,xlast)                  
        first = True
        cstyles = copy(styles)
        fits = []
        ## for key,profile in profiles.iteritems():
        for key in keys:
            profile = profiles[key]
            profile.Sort()
            profile.Print()
            style = cstyles.pop(0)            
            ## ## func = ROOT.TF1("bfunc","(x>[0])*( [1]/([0]+x)+[2] )")
            ## func = ROOT.TF1("bfunc","[0]*pow(x/%f,[1])+[2]"% max(600,xfirst),max(600,xfirst),xlast)
            ## # func.SetParameters(300.,1.,1.e-3)
            ## func.SetParameters(1.e-2,-4,1.e-5)
            ## profile.Fit(func,"R+")
            ## fit = profile.GetListOfFunctions().At(0)
            ## fits.append([key,fit])
            if key in options.bias_param:
                bias_func = ROOT.TF1("err_correction_%s" % key,options.bias_param[key],xfirst,xlast)
                style_utils.apply( bias_func, style[:1] )
                bias_func.Draw("same")
                self.keep(bias_func)
                
            style_utils.apply( profile, style )
            leg.AddEntry(profile,key,"pe")
            if first:
                profile.Draw("AP")
                profile.GetXaxis().SetMoreLogLabels()
                profile.GetXaxis().SetTitle("mass")
                ## profile.GetYaxis().SetRangeUser(0.001,0.3)
                profile.GetYaxis().SetRangeUser(0.00001,0.2)
                ## profile.GetYaxis().SetRangeUser(0.,6.)
                profile.GetYaxis().SetTitle("| n_{fit} - n_{true} | / GeV")
                ## profile.GetYaxis().SetTitle("| n_{fit} - n_{true} |")
                first = False
            else:
                profile.Draw("P")
            ## fit.Draw("same")
        leg.Draw("same")
        
        bcanv = ROOT.TCanvas("profile_pull","profile_pull")
        bcanv.SetLogx()
        bcanv.SetGridy()
        bcanv.SetGridx()
        bleg  = ROOT.TLegend(0.2,0.12,0.6,0.52)
        bleg.SetFillStyle(0)
        first = True
        cstyles = copy(styles)
        frame = ROOT.TH2F("frame","frame",100,xfirst,xlast,100,-3,2);
        frame.SetStats(False)
        frame.Draw()
        frame.GetXaxis().SetTitle("mass")
        frame.GetXaxis().SetMoreLogLabels()
        frame.GetYaxis().SetTitle("( n_{fit} - n_{true} )/ \sigma_{fit}")
        box = ROOT.TBox(xfirst,-0.5,xlast,0.5)
        ## box.SetFillColorAlpha(ROOT.kGray,0.1)
        box.SetFillColor(ROOT.kGray)
        box.Draw("same")        
        self.keep([frame,box])
        for key in keys:
            profile = bprofiles[key]
            profile.Sort()
            style_utils.apply( profile, cstyles.pop(0) )
            bleg.AddEntry(profile,key,"pe")
            profile.Draw("P")
        bleg.Draw("same")
        bcanv.RedrawAxis()
        bcanv.Modified()
        bcanv.Update()
        
        self.keep( [canv,leg,bcanv,bleg#,box
                    ] )
        
        if len(ckeys) > 0:
            ccanv = ROOT.TCanvas("profile_corr_pull","profile_corr_pull")
            ccanv.SetLogx()
            ccanv.SetGridy()
            ccanv.SetGridx()
            cleg  = ROOT.TLegend(0.2,0.12,0.6,0.52)
            cleg.SetFillStyle(0)
            first = True
            cstyles = copy(styles)
            cframe = ROOT.TH2F("cframe","cframe",100,xfirst,xlast,100,-3,2);
            cframe.SetStats(False)
            cframe.Draw()
            cframe.GetXaxis().SetTitle("mass")
            cframe.GetXaxis().SetMoreLogLabels()
            cframe.GetYaxis().SetTitle("( n_{fit} - n_{true} )/ ( \sigma_{fit} \oplus bias )")
            box.Draw("same")        
            self.keep([cframe])
            for key in ckeys:
                profile = cprofiles[key]
                profile.Sort()
                style_utils.apply( profile, cstyles.pop(0) )
                cleg.AddEntry(profile,key,"pe")
                profile.Draw("P")
            cleg.Draw("same")
            ccanv.RedrawAxis()
            ccanv.Modified()
            ccanv.Update()
            self.keep( [ccanv,cleg] )
            
        self.autosave(True)
        
        keys = sorted(summary.keys())
        maxl = 0
        for key in keys:
            maxl = max(len(key),maxl)
        summarystr = ""
        for name,fit in fits:
            summarystr += "%s %s\n" % ( name, fit.GetExpFormula("p") )
        summarystr += "test region".ljust(maxl+3)
        for field in ["pmean","err","psig","err","pmedian","p68","bmean","err","bmedian","smedian","truth","corr_bmedian","corr_bmean","corr_bsigma"]:
            summarystr += field.rjust(9)
        summarystr += "\n"
        for key in keys:
            val = summary[key]
            summarystr += ("%s, " % key).ljust(maxl+3)
            for v in val: 
                summarystr += ("%1.3g," %v).rjust(9)
            summarystr += "\n"
        print summarystr
        summaryf = open("%s/README.txt" % options.outdir,"w+")
        summaryf.write(summarystr)
        summaryf.close()

        
      
    
# -----------------------------------------------------------------------------------------------------------
# actual main
if __name__ == "__main__":
    app = BiasApp()
    app.run()
