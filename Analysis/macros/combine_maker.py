#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, json

from pprint import pprint

import array

from getpass import getuser

from templates_maker import TemplatesApp

import random

from math import sqrt

## ----------------------------------------------------------------------------------------------------------------------------------------
class CombineApp(TemplatesApp):
    """
    Class to handle template fitting.
    Takes care of preparing templates starting from TTrees.
    Inherith from PyRapp and PlotApp classes.
    """
    
    ## ------------------------------------------------------------------------------------------------------------
    def __init__(self,option_list=[],option_groups=[]):
        
        super(CombineApp,self).__init__(
            option_groups=[
                ("Combine workspace options", [
                        make_option("--fit-name",dest="fit_name",action="store",type="string",
                                    default="cic",
                                    help="Fit to consider"),
                        make_option("--observable",dest="observable",action="store",type="string",
                                    default="mgg[5000,500,6000]",
                                    help="Observable used in the fit default : [%default]",
                                    ),
                        make_option("--fit-background",dest="fit_background",action="store_true",default=False,
                                    help="Fit background",
                                    ),                        
                        make_option("--norm-as-fractions",dest="norm_as_fractions",action="store_true",default=False,
                                    help="Parametrize background components normalization as fractions",
                                    ),
                        make_option("--nuisance-fractions",dest="nuisance_fractions",action="store_true",default=False,
                                    help="Add nuisance parameters for component fractions"),
                        make_option("--bkg-shapes",dest="bkg_shapes",action="callback",callback=optpars_utils.Load(scratch=True),
                                    type="string",
                                    default={ "bkg" : {
                                    "shape" : "data", "norm" : "data"
                                    }  },
                                    help="Background shapes",
                                    ),
                        make_option("--default-model",dest="default_model",action="store",type="string",
                                    default="dijet",
                                    help="Default background mode : [%default]",
                                    ),
                        make_option("--data-source",dest="data_source",action="store",type="string",
                                    default="data",
                                    help="Dataset to be used as 'data' default : [%default]",
                                    ),
                        make_option("--generate-signal-dataset",dest="generate_signal_dataset",action="store_true",default=False,
                                    help="Generate signal dataset",
                                    ),
                        make_option("--signal-name",dest="signal_name",action="store",type="string",
                                    default=None,
                                    help="Signal name to generate the dataset and/or datacard"),            
                        make_option("--generate-datacard",dest="generate_datacard",action="store_true",default=False,
                                    help="Generate datacard",
                                    ),
                        make_option("--background-root-file",dest="background_root_file",action="store",type="string",
                                    default="full_analysis_anv1_v14_bkg_ws.root",
                                    help="Output file from the background fit",
                                    ),
                        make_option("--signal-root-file",dest="signal_root_file",action="store",type="string",
                                    default=None,
                                    help="Output file from the signal model",
                                    ),
                        make_option("--cardname",dest="cardname",action="store",type="string",
                                    default=None,
                                    help="Name of generated card",
                                    ),
                        make_option("--compute-FWHM",dest="compute_FWHM",action="store_true",default=False,
                                    help="Compute the Full Width Half Maximum",
                                    ),
                        
                        ]
                 )
                ]+option_groups,option_list=option_list
            )
        
        ## load ROOT (and libraries)
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        
        import diphotons.Utils.pyrapp.style_utils as style_utils
        ROOT.gSystem.Load("libdiphotonsUtils")
        
        self.pdfPars_ = ROOT.RooArgSet()

    def __call__(self,options,args):
        

        ## load ROOT style
        self.loadRootStyle()
        ROOT.TGaxis.SetMaxDigits(3)
        from ROOT import RooFit
        from ROOT import TH1F, TCanvas, TAxis
        
        printLevel = ROOT.RooMsgService.instance().globalKillBelow()
        ROOT.RooMsgService.instance().setGlobalKillBelow(RooFit.FATAL)

        options.only_subset = [options.fit_name]
        
        options.store_new_only=True
        self.setup(options,args)

        options.components = options.bkg_shapes.keys()
 
        self.includeBiasPdf(options,args)
        if options.fit_background:
            self.fitBackground(options,args)
            
        if options.generate_signal_dataset:
            self.generateSignalDataset(options,args)
            
        if options.generate_datacard:
            self.generateDatacard(options,args)
                

    ## ------------------------------------------------------------------------------------------------------------
    def generateDatacard(self,options,args):
        """Generate a datacard with name: signal_name.txt if signal_root_file not provided.
        
        Generates datacard for signal_name or loop over the list of signals if signal_name not provided.
        If read_ws then bkg.root file = read_ws file.
        
        """
        
        
        
        fitname = options.fit_name
        fit = options.fits[fitname]
        sidebands = list(fit.get("sidebands",{}).keys())
        categories = list(fit["categories"].keys())
        if (options.read_ws):
            options.background_root_file = options.read_ws
        isNameProvided = False
                     
        for signame,trees in options.signals.iteritems():
        
            if (options.signal_name != None):
                isNameProvided = True
                signame = options.signal_name
                 
            if (not isNameProvided or (isNameProvided and options.signal_root_file == None)):
                options.signal_root_file = signame+".root" 
            datacard = open("dataCard_"+signame+".txt","w+")
            if(options.cardname != None):    
                datacard = open(options.cardname,"w+")
            
                
            datacard.write("""
## Signal: %s - 13TeV
##
----------------------------------------------------------------------------------------------------------------------------------
imax * number of channels
jmax * number of backgrounds
kmax * number of nuisance parameters (source of systematic uncertainties)
----------------------------------------------------------------------------------------------------------------------------------\n""" % signame)
                        
            for cat in categories:
                datacard.write("shapes sig".ljust(20))
                datacard.write((" %s  %s" % (cat,options.signal_root_file)).ljust(50))
                datacard.write(" wtemplates:signal_%s_%s\n"% (signame,cat))
                
                for comp in options.components:
                    datacard.write(("shapes %s" % comp).ljust(20))
                    datacard.write((" %s  %s" % (cat,options.background_root_file)).ljust(50))
                    datacard.write(" wtemplates:model_%s_%s\n" % (comp,cat) )  
                
                datacard.write("shapes data_obs".ljust(20))
                datacard.write((" %s  %s" % (cat,options.background_root_file)).ljust(50))
                datacard.write(" wtemplates:data_%s\n" % cat) 
            
            for cat in sidebands:                
                for comp in  fit["sidebands"][cat]:
                    datacard.write(("shapes %s" % comp).ljust(20))
                    datacard.write((" %s  %s" % (cat,options.background_root_file)).ljust(50))
                    datacard.write(" wtemplates:model_%s_%s\n" % (comp,cat) )  
                datacard.write("shapes data_obs".ljust(20))
                datacard.write((" %s  %s" % (cat,options.background_root_file)).ljust(50))
                datacard.write(" wtemplates:data_%s\n" % cat)                 

            datacard.write("----------------------------------------------------------------------------------------------------------------------------------\n")
            datacard.write("bin".ljust(20))
            for cat in categories+sidebands:
                datacard.write((" %s".ljust(15) % cat))
            datacard.write("\n")

            datacard.write("observation".ljust(20))
            for cat in categories+sidebands:
                  datacard.write(" -1".ljust(15) )
            datacard.write("\n")
            
            datacard.write("----------------------------------------------------------------------------------------------------------------------------------\n")
            
            datacard.write("bin".ljust(20))
            for cat in categories:
                datacard.write((" %s" % cat).ljust(15) )
                for comp in options.components:
                    datacard.write((" %s" % cat).ljust(15) )
            for cat in sidebands:                
                for comp in  fit["sidebands"][cat]:
                    datacard.write((" %s" % cat).ljust(15) )                    
            datacard.write("\n")


            datacard.write("process".ljust(20))
            for cat in categories:
                datacard.write(" sig".ljust(15) )
                for comp in options.components:
                    datacard.write((" %s" % comp).ljust(15) )
            for cat in sidebands:                
                for comp in  fit["sidebands"][cat]:
                    datacard.write((" %s" % comp).ljust(15) )                    
            datacard.write("\n")
            
            datacard.write("process".ljust(20))
            icomp = {}
            i = 0
            for cat in categories:
                datacard.write(" 0".ljust(15) )
                
                for comp in options.components:
                    if comp in icomp:
                        j = icomp[comp]
                    else:
                        i+=1
                        j = i
                        icomp[comp] = i 
                    datacard.write((" %d" % j).ljust(15) )
            for cat in sidebands:                
                for comp in  fit["sidebands"][cat]:
                    if comp in icomp:
                        j = icomp[comp]
                    else:
                        i+=1
                        j = i
                        icomp[comp] = i 
                    datacard.write((" %d" % j).ljust(15) )
            datacard.write("\n")
            
            datacard.write("rate".ljust(20))
            for cat in categories:
                datacard.write(" -1".ljust(15) )
                for comp in options.components:
                    datacard.write(" 10".ljust(15) )
            for cat in sidebands:                
                for comp in  fit["sidebands"][cat]:                    
                    datacard.write(" 1".ljust(15) )
            datacard.write("\n")
            
            datacard.write("----------------------------------------------------------------------------------------------------------------------------------\n")
            
            # normalization nuisances
            datacard.write("lumi  lnN".ljust(20))
            for cat in categories:
                datacard.write(" 1.04".ljust(15) )
                for comp in options.components:
                    datacard.write(" -".ljust(15) )
            for cat in sidebands:                
                for comp in  fit["sidebands"][cat]:                    
                    datacard.write(" -".ljust(15) )
            datacard.write("\n")
            
            # other nuisance parameters
            datacard.write("\n")
            for param in fit.get("params",[]):
                datacard.write("%s param %1.3g %1.3g\n" % param )            
            
            datacard.write("----------------------------------------------------------------------------------------------------------------------------------\n\n")
            
            
            if isNameProvided:
                break
            
    def includeBiasPdf(self,options,args):

        print "--------------------------------------------------------------------------------------------------------------------------"
        print "including bias term in the background"
        print "--------------------------------------------------------------------------------------------------------------------------"
        
        options.background_root_file = options.read_ws
        if (options.signal_name == None or options.signal_root_file == None ) :
            print "Provide signal-name and signal-root-file "
            return;
            
        fitname = options.fit_name
        fit = options.fits[fitname]
        
        signame = options.signal_name
        
        print "reading signal and bkg pdfs from %s and %s" % (options.signal_root_file, options.background_root_file)

        ## reading back background_root_file
        
        # build observable variable
        roobs = self.buildRooVar(*(self.getVar(options.observable)), recycle=True, importToWs=True)
        roowe = self.buildRooVar("weight",[])        
        rooset = ROOT.RooArgSet(roobs,roowe)
        
        fit["params"] = []
        
        for cat in fit["categories"]:
            
            options.read_ws = options.background_root_file
            self.readWs(options,args)
                
            dataBinned = self.rooData("binned_data_%s" % cat)
            data = self.rooData("data_%s" % cat)
            
            
            ## reading back signal_root_file workspace
            options.read_ws = options.signal_root_file
            self.readWs(options,args)
            signalDataHist = self.rooData("signal_%s_%s" % (signame,cat))
            
            signalPdf = ROOT.RooHistPdf("signalPdf_%s_%s"% (signame,cat),"signalPdf_%s_%s"% (signame,cat),ROOT.RooArgSet(roobs),signalDataHist)

            options.read_ws = options.background_root_file
            self.readWs(options,args)
            
            for comp in options.components :
            
                bkgPdf = self.rooPdf("model_%s_%s" % (comp,cat))
                roopdflist = ROOT.RooArgList()
                roopdflist.add(bkgPdf)
                roopdflist.add(signalPdf)
                
                ## retrieve norm of pdf 
                rooNdata = self.buildRooVar("%s_norm" % (bkgPdf.GetName()),[],recycle=True,importToWs=False)
                rooNdataFormula = ROOT.RooFormulaVar("%s_norm_Formula" % (bkgPdf.GetName()), "%s_norm_Formula" % (bkgPdf.GetName()), "@0", ROOT.RooArgList(rooNdata))
                
                ## build list of coefficients 
                roolist = ROOT.RooArgList()
                nBias = self.buildRooVar("nBias_%s_%s_frac" % (comp,cat), [], importToWs=False )
                nBias.setVal(0.)
                nBias.setConstant(True)
                
                #fracsig = ROOT.RooFormulaVar("signal_%s_frac" % (cat), "signal_%s_frac" % (cat), "@0*1./@1", ROOT.RooArgList(nBias,rooNdataVar[cat]) )

                nuis = self.buildRooVar("%s_%s_frac_nuis" % (comp,cat), [], importToWs=False )
                nuis.setVal(0.)
                nuis.setConstant(True)
                fit["params"].append( (nuis.GetName(), nuis.getVal(), 0.) )
                nuisBias = ROOT.RooFormulaVar("%s_%s_nuisanced_nBias" % (comp,cat),"%s_%s_nuisanced_nBias" % (comp,cat),"@0*(1.+@1)",ROOT.RooArgList(nBias,nuis) ) #or fracsig ??
                fracsignuis = ROOT.RooFormulaVar("signal_%s_%s_nuisanced_frac" % (comp,cat),"signal_%s_%s_nuisanced_frac" % (comp,cat),"@0*1./@1",ROOT.RooArgList(nuisBias,rooNdataFormula) )
                fracbkg = ROOT.RooFormulaVar("background_%s_%s_frac" % (comp,cat), "background_%s_%s_frac" % (comp,cat), "1.-@0",ROOT.RooArgList(fracsignuis))
                roolist.add(fracsignuis)
                #self.keep( [nuis,nuisfrac] )
                roolist.add(fracbkg)
                
                ## summing pdfs
                pdfSum = ROOT.RooAddPdf("model_bkgnbias_%s_%s" % (comp,cat),"model_bkgnbias_%s_%s" % (comp,cat), roopdflist, roolist)
                pdfSum_norm = ROOT.RooFormulaVar("%s_norm" %  (pdfSum.GetName()), "%s_norm" %  (pdfSum.GetName()),"@0",ROOT.RooArgList(rooNdataFormula)) 
                self.workspace_.rooImport(pdfSum_norm)
                self.workspace_.rooImport(pdfSum,RooFit.RecycleConflictNodes())
        self.saveWs(options)

    ## ------------------------------------------------------------------------------------------------------------  

    def fitBackground(self,options,args):

        print "--------------------------------------------------------------------------------------------------------------------------"
        print "runnning background fit"
        print 
        
        fitname = options.fit_name
        fit = options.fits[fitname]
        
        roobs = self.buildRooVar(*(self.getVar(options.observable)), recycle=False, importToWs=True)
        #roobs.setBins(5000,"cache")
        roobs.setRange("fullRange",roobs.getMin(),roobs.getMax())
        roowe = self.buildRooVar("weight",[])        
        rooset = ROOT.RooArgSet(roobs,roowe)
        
        ## build and import data dataset
        ndata = {}
        rooNdata = {}
        sidebands = {}
        for cat in fit["categories"]:
            treename = "%s_%s_%s" % (options.data_source,options.fit_name,cat)
            
            print "importing %s as data for cat %s" % (treename, cat)
            
            dset = self.rooData(treename)
            
            reduced = dset.reduce(RooFit.SelectVars(rooset),RooFit.Range("fullRange")) ## FIXME: roobs range
            reduced.SetName("data_%s"% (cat))
            ndata[cat] = reduced.sumEntries()
            rooNdata[cat] = self.buildRooVar("%s_norm" % cat,[],recycle=False,importToWs=False)
            rooNdata[cat].setVal(ndata[cat])
            self.workspace_.rooImport(reduced)
            
            binned = reduced.binnedClone("binned_data_%s" % cat)
            self.workspace_.rooImport(binned)

        fitops = [ ROOT.RooFit.PrintLevel(-1),ROOT.RooFit.Warnings(False),ROOT.RooFit.NumCPU(4),ROOT.RooFit.Minimizer("Minuit2") ]
        if options.verbose:
            fitops[0] = ROOT.RooFit.PrintLevel(2)

        ## prepare background fit components
        print
        fit["params"] = []

        ## loop over categories to fit background
        for cat in fit["categories"]:
            
            print
            print "fitting category : ", cat
            print
            
            importme = []
            fractions = {}
            setme = []
            if options.norm_as_fractions:
                tot = 0.
                roolist = ROOT.RooArgList()
                rooformula = []
                for icomp,comp in enumerate(options.components[:-1]):
                    if comp != "":
                        comp = "%s_" % comp
                    frac = self.buildRooVar("%s%s_frac" % (comp,cat), [], importToWs=False )
                    setme.append(comp)
                    fractions[comp] = frac
                    rooformula.append("@%d"%icomp)
                    if options.nuisance_fractions:
                        nuis = self.buildRooVar("%s%s_frac_nuis" % (comp,cat), [], importToWs=False )
                        nuis.setVal(0.)
                        nuis.setConstant(True)
                        fit["params"].append( (nuis.GetName(), nuis.getVal(), 0.) )
                        nuisfrac = ROOT.RooFormulaVar("%s%s_nuisanced_frac" % (comp,cat),"%s%s_nuisanced_frac" % (comp,cat),"@0*(1.+@1)",ROOT.RooArgList(frac,nuis) )
                        roolist.add(nuisfrac)
                        self.keep( [nuis,nuisfrac] )
                    else:
                        roolist.add(frac)
                comp = options.components[-1]
                if comp != "":
                    comp = "%s_" % comp
                frac = ROOT.RooFormulaVar("%s%s_frac" % (comp,cat),"%s%s_frac" % (comp,cat),"1.-%s" % "-".join(rooformula),roolist )
                fractions[comp] = frac

            for comp,opts in options.bkg_shapes.iteritems():
                model = opts.get("model",options.default_model)
                source  = opts["shape"]
                nsource = opts["norm"]
                source_cats = opts.get("shape_cats",{})
                nsource_cats = opts.get("norm_cats",{})
                add_sideband = opts.get("add_sideband",False)
                weight_cut = opts.get("weight_cut",None)
                
                print "component : " , comp
                print "model :", model
                if comp != "":
                    comp = "%s_" % comp
                    
                # dataset used to determine shape
                catsource = source_cats.get(cat,cat)
                treename = "%s_%s_%s" % (source,options.fit_name,catsource)
                # and normalization
                catnsource = nsource_cats.get(cat,cat)
                ntreename = "%s_%s_%s" % (nsource,options.fit_name,catnsource)

                if add_sideband and not catsource in sidebands:
                    sidebands[catsource] = set()
                
                
                dset  = self.rooData(treename)
                ndset = self.rooData(ntreename)                
                ## cut away high weight events for the fit if needed, but keep the uncut dataset
                if weight_cut:                    
                    uncut        = dset.reduce(RooFit.SelectVars(rooset),RooFit.Range("fullRange"))
                    binned_uncut = uncut.binnedClone()
                    dset         = self.reducedRooData(treename,rooset,sel=weight_cut,redo=True,importToWs=True)                    
                
                reduced  = dset.reduce(RooFit.SelectVars(rooset),RooFit.Range("fullRange"))
                nreduced = ndset.reduce(RooFit.SelectVars(rooset),RooFit.Range("fullRange"))                
                reduced.SetName("source_dataset_%s%s"% (comp,cat))
                binned = reduced.binnedClone()                
                print "shape source :", treename, reduced.sumEntries(),
                if weight_cut:
                    print uncut.sumEntries()
                else:
                    print
                print "normalization source: ", ntreename, nreduced.sumEntries()
                
                ## build pdf
                if add_sideband: 
                    ## if we want to take background shape from sideband in data, book 
                    ##    the pdf such that coefficients are the same for the signal region and sideband shapes
                    pdf = self.buildPdf(model,"model_%s_%s_control" % (add_sideband,catsource), roobs )
                    sbpdf = self.buildPdf(model,"model_%s_%s_control" % (add_sideband,catsource), roobs )
                    sbpdf.SetName("model_%s_%s_control" % (add_sideband,catsource))
                    sidebands[catsource].add(add_sideband)
                else:
                    pdf = self.buildPdf(model,"model_%s%s" % (comp,cat), roobs )                    
                pdf.SetName("model_%s%s" % (comp,cat))

                ## normalization has to be called <pdfname>_norm or combine won't find it
                if options.norm_as_fractions:
                    norm = ROOT.RooFormulaVar("%s_norm" %  (pdf.GetName()),"%s_norm" %  (pdf.GetName()),
                                              ## "@0*@1",ROOT.RooArgList(ROOT.RooFit.RooConst(ndata[cat]),fractions[comp]))
                                              "@0*@1",ROOT.RooArgList(rooNdata[cat],fractions[comp]))
                else:
                    norm = self.buildRooVar("%s_norm" %  (pdf.GetName()), [], importToWs=False ) 
                    norm.setVal(reduced.sumEntries())
                ## extpdf = ROOT.RooExtendPdf("ext_%s" % pdf.GetName(),"ext_%s" %  pdf.GetName(),pdf,norm)
                ## extpdf.fitTo(binned,ROOT.RooFit.Strategy(2),*fitops)
                pdf.fitTo(binned,ROOT.RooFit.Strategy(2),*fitops)
                extpdf = pdf
                ## extpdf.fitTo(reduced,ROOT.RooFit.Strategy(1))
                
                ## set normalization to expected number of events in normalization region
                if options.norm_as_fractions:
                    if comp in setme:
                        fractions[comp].setVal(nreduced.sumEntries()/ndata[cat])
                        fractions[comp].setConstant(True) # set this to true so that other components fits don't change it
                else:
                    norm.setVal(nreduced.sumEntries()) 

                if add_sideband:
                    ## build normalization variable for sideband
                    sbnorm = self.buildRooVar("%s_norm" %  (sbpdf.GetName()), [], importToWs=False ) 
                    if weight_cut:
                        sbnorm.setVal(uncut.sumEntries())
                    else:
                        sbnorm.setVal(reduced.sumEntries())
                
                ## plot the fit result
                frame = roobs.frame()
                binned.plotOn(frame)
                extpdf.plotOn(frame)

                hist   = frame.getObject(int(frame.numItems()-2))
                fitc   = frame.getObject(int(frame.numItems()-1))
                hresid = frame.residHist(hist.GetName(),fitc.GetName(),True)
                resid  = roobs.frame()
                resid.addPlotable(hresid,"PE")
                
                canv = ROOT.TCanvas("bkg_fit_%s%s" % (comp,cat), "bkg_fit_%s%s" % (comp,cat) )
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
                frame.GetYaxis().SetRangeUser( 1.e-6,50. )
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
                
                # this will actually save the plots
                self.keep(canv)
                self.autosave(True)
                
                # import everything to the workspace
                self.workspace_.rooImport(pdf,RooFit.RecycleConflictNodes())
                importme.append([norm]) ## import this only after we run on all components, in case we use fractions
                ## self.workspace_.rooImport(norm)
                self.workspace_.rooImport(reduced)
                
                # import pdf and data for sidebands
                if add_sideband:
                    if weight_cut:
                        reduced = uncut
                        binned  = binned_uncut
                    reduced.SetName("data_%s_control" % catsource)
                    binned.SetName("binned_data_%s_control" % catsource)
                    self.workspace_.rooImport(reduced)
                    self.workspace_.rooImport(binned)
                    self.workspace_.rooImport(sbnorm)
                    self.workspace_.rooImport(sbpdf,RooFit.RecycleConflictNodes())
            
                print
            
            ### if options.norm_as_fractions:
            ###     for me in setme:
            ###         fractions[me].setConstant(False)
            for me in importme:
                self.workspace_.rooImport(*me)

        fit["sidebands"] = {}
        for nam,val in sidebands.iteritems():
            fit["sidebands"]["%s_control" % nam] = list(val)
        # done
        self.saveWs(options)
       
 ## ------------------------------------------------------------------------------------------------------------
    def generateSignalDataset(self,options,args):
        
        print "--------------------------------------------------------------------------------------------------------------------------"
        print "generating signal dataset"
        print 
        
        
        fitname = options.fit_name
        fit = options.fits[fitname] 
        isNameProvided = False
        for signame,trees in options.signals.iteritems():
            self.workspace_ = ROOT.RooWorkspace("wtemplates","wtemplates")
            self.workspace_.rooImport = getattr(self.workspace_,"import")
            if (options.signal_name != None):
                isNameProvided = True
                signame = options.signal_name
           
                    
            roobs = self.buildRooVar(*(self.getVar(options.observable)), recycle=False, importToWs=True)
            #roobs.setBins(5000,"cache")
            roobs.setRange("fullRange",roobs.getMin(),roobs.getMax())
            roowe = self.buildRooVar("weight",[])        
            rooset = ROOT.RooArgSet(roobs,roowe)
           

            if (not isNameProvided or (isNameProvided and options.output_file == None)):
                options.output_file = signame+".root" 

            nameFileOutput = options.output_file
            file_fwhm = open(signame+"_FWHM.txt","a")
            
            ## build and import signal dataset
            for cat in fit["categories"]:
                treename = "%s_%s_%s" % (signame,options.fit_name,cat)
                print treename
                dset = self.rooData(treename)
                dset.Print()
                
                reduced = dset.reduce(RooFit.SelectVars(rooset),RooFit.Range("fullRange")) ## FIXME: roobs range
                reduced.SetName("signal_%s_%s"% (signame,cat))
                binned = reduced.binnedClone()
                binned.SetName("signal_%s_%s"% (signame,cat))
                self.workspace_.rooImport(binned)
                
                if options.compute_FWHM:
                    # plot signal histogram and compute FWHM
                    canv = ROOT.TCanvas("signal_%s" % (cat),"signal" )
                    roobs.setBins(1200)

                    hist = binned.createHistogram("sigHist",roobs)
                    halfMaxVal = 0.5*hist.GetMaximum()
                    maxBin = hist.GetMaximumBin()
                    print ("%r %r" % (maxBin,halfMaxVal))
                    binLeft=binRight=xWidth=xLeft=xRight=0

                    for ibin in range(1,maxBin):
                        binVal = hist.GetBinContent(ibin)
                        if (binVal >= halfMaxVal):
                            binLeft = ibin
                            break;
                    for ibin in range(maxBin+1,hist.GetXaxis().GetNbins()+1):
                        binVal = hist.GetBinContent(ibin)
                        if (binVal < halfMaxVal):
                            binRight = ibin-1
                            break;
                    if (binLeft > 0 and binRight > 0 ):
                        xLeft = hist.GetXaxis().GetBinCenter(binLeft)
                        xRight = hist.GetXaxis().GetBinCenter(binRight)
                        xWidth = xRight-xLeft
                    else:
                        print("Did not succeed to get the FWHM")

                    print ("FWHM = %r" % (xWidth))
                    hist.GetXaxis().SetRangeUser(hist.GetXaxis().GetBinCenter(maxBin)-5*xWidth,hist.GetXaxis().GetBinCenter(maxBin)+5*xWidth)
                    hist.Draw("HIST")
                    
                    #canv.SaveAs(options.output_file.replace(".root","_%s_hist.png" % (cat))
                    canv.SaveAs(nameFileOutput.replace(".root",("%s_hist.png" % cat)))
                    
                    file_fwhm.write("%d %s %d %d\n" % (roobs.getMin(),cat,xWidth,xLeft))
                
            self.saveWs(options)
                        
            # if signame provided then stop
            if isNameProvided :
                break                
  
        
    ## ------------------------------------------------------------------------------------------------------------
    def buildPdf(self,model,name,xvar,order=0,label=None):
        
        pdf = None
        if not label:
            label = model
        if model == "dijet":
            pname = "dijet_%s" % name
            linc = self.buildRooVar("%s_lin" % pname,[-100.0,100.0], importToWs=False)
            logc = self.buildRooVar("%s_log" % pname,[-100.0,100.0], importToWs=False)
            linc.setVal(5.)
            logc.setVal(-1.)
            
            self.pdfPars_.add(linc)
            self.pdfPars_.add(logc)
            
            roolist = ROOT.RooArgList( xvar, linc, logc )
            pdf = ROOT.RooGenericPdf( pname, pname, "pow(@0,@1+@2*log(@0))", roolist )
            
            self.keep( [pdf,linc,logc] )
            
        if model == "maxdijet":
            pname = "maxdijet_%s" % name
            linc = self.buildRooVar("%s_lin" % pname,[-100.0,100.0], importToWs=False)
            logc = self.buildRooVar("%s_log" % pname,[-100.0,100.0], importToWs=False)
            linc.setVal(5.)
            logc.setVal(-1.)
            
            self.pdfPars_.add(linc)
            self.pdfPars_.add(logc)
            
            roolist = ROOT.RooArgList( xvar, linc, logc )
            pdf = ROOT.RooGenericPdf( pname, pname, "TMath::Max(1e-30,pow(@0,@1+@2*log(@0)))", roolist )
            
            self.keep( [pdf,linc,logc] )
            
        elif model == "moddijet":
            pname = "moddijet_%s" % name
            lina = self.buildRooVar("%s_lina" % pname,[-100.0,100.0], importToWs=False)
            loga = self.buildRooVar("%s_loga" % pname,[-100.0,100.0], importToWs=False)
            linb = self.buildRooVar("%s_linb" % pname,[-100.0,100.0], importToWs=False)
            sqrb = self.buildRooVar("%s_sqrb" % pname,[], importToWs=False)
            lina.setVal(5.)
            loga.setVal(-1.)
            linb.setVal(0.1)
            sqrb.setVal(1./13.e+3)
            sqrb.setConstant(1)
            
            
            self.pdfPars_.add(lina)
            self.pdfPars_.add(loga)
            self.pdfPars_.add(linb)
            self.pdfPars_.add(sqrb)
            
            roolist = ROOT.RooArgList( xvar, lina, loga, linb, sqrb )
            pdf = ROOT.RooGenericPdf( pname, pname, "pow(@0,@1+@2*log(@0))*pow(1.-@0*@4,@3)", roolist )
            
            self.keep( [pdf,lina,loga, linb, sqrb] )
        elif model == "expow":
            
            pname = "expow_%s" % name
            lam = self.buildRooVar("%s_lambda" % pname,[], importToWs=False)
            alp = self.buildRooVar("%s_alpha"  % pname,[], importToWs=False)
            lam.setVal(0.)
            alp.setVal(-4.)
            
            self.pdfPars_.add(alp)
            self.pdfPars_.add(lam)
            
            roolist = ROOT.RooArgList( xvar, lam, alp )
            pdf = ROOT.RooGenericPdf( pname, pname, "exp(@1*@0)*pow(@0,@2)", roolist )
            
            self.keep( [pdf,lam,alp] )

        elif model == "expow2":
            
            pname = "expow2_%s" % name
            lam0 = self.buildRooVar("%s_lambda0" % pname,[], importToWs=False)
            lam1 = self.buildRooVar("%s_lambda1" % pname,[], importToWs=False)
            alp = self.buildRooVar("%s_alpha"  % pname,[], importToWs=False)
            lam0.setVal(0.)
            lam1.setVal(0.)
            alp.setVal(2.)
            
            self.pdfPars_.add(alp)
            self.pdfPars_.add(lam0)
            self.pdfPars_.add(lam1)
            
            bla = ROOT.RooArgList(lam0,lam1)
            hmax = ROOT.RooFormulaVar("%s_hmax" %pname,"( @1 != 0. ? (-@0/(4.*@1)>300. && -@0/(4.*@1)<3500. ? @0*@0/(4.*@1+@1) : TMath::Max(@0*3500+2*@1*3500.*3500,@0*3500+2*@1*300.*300)) : @0*3500.)", bla )
            roolist = ROOT.RooArgList( xvar, lam0, lam1, alp, hmax )
            pdf = ROOT.RooGenericPdf( pname, pname, "exp( @1*@0+@2*@0*@0   )*pow(@0, -@3*@3 + @4  )", roolist )
            
            self.keep( [pdf,lam0,lam1,alp,hmax] )

        elif model == "invpow":
            
            pname = "invpow_%s" % name
            slo = self.buildRooVar("%s_slo" % pname,[], importToWs=False)
            alp = self.buildRooVar("%s_alp" % pname,[], importToWs=False)
            slo.setVal(2.e-3)
            alp.setVal(-7.)
            
            self.pdfPars_.add(slo)
            self.pdfPars_.add(alp)
            
            roolist = ROOT.RooArgList( xvar, slo, alp )
            pdf = ROOT.RooGenericPdf( pname, pname, "pow(1+@0*@1,@2)", roolist )
            
            self.keep( [pdf,slo,alp] )

        elif model == "invpowlog":
            
            pname = "invpowlog_%s" % name
            slo = self.buildRooVar("%s_slo" % pname,[], importToWs=False)
            alp = self.buildRooVar("%s_alp" % pname,[], importToWs=False)
            bet = self.buildRooVar("%s_bet" % pname,[], importToWs=False)
            slo.setVal(1.e-3)
            alp.setVal(-4.)
            bet.setVal(0.)
            
            self.pdfPars_.add(slo)
            self.pdfPars_.add(alp)
            self.pdfPars_.add(bet)
            
            roolist = ROOT.RooArgList( xvar, slo, alp, bet )
            pdf = ROOT.RooGenericPdf( pname, pname, "pow(1+@0*@1,@2+@3*log(@0))", roolist )
            
            self.keep( [pdf,slo,alp,bet] )

        elif model == "invpowlin":
            
            pname = "invpowlin_%s" % name
            slo = self.buildRooVar("%s_slo" % pname,[], importToWs=False)
            alp = self.buildRooVar("%s_alp" % pname,[], importToWs=False)
            bet = self.buildRooVar("%s_bet" % pname,[], importToWs=False)
            slo.setVal(1.e-3)
            alp.setVal(-4.)
            bet.setVal(0.)
            
            self.pdfPars_.add(slo)
            self.pdfPars_.add(alp)
            self.pdfPars_.add(bet)
            
            roolist = ROOT.RooArgList( xvar, slo, alp, bet )
            pdf = ROOT.RooGenericPdf( pname, pname, "pow(1+@0*@1,@2+@3*@0)", roolist )
            
            self.keep( [pdf,slo,alp,bet] )

        elif model == "invpow2":
            
            pname = "invpow2_%s" % name
            slo = self.buildRooVar("%s_slo" % pname,[], importToWs=False)
            qua = self.buildRooVar("%s_qua" % pname,[], importToWs=False)
            alp = self.buildRooVar("%s_alp" % pname,[], importToWs=False)
            slo.setVal(1.e-4)
            qua.setVal(1.e-6)
            alp.setVal(-4.)
            
            self.pdfPars_.add(slo)
            self.pdfPars_.add(qua)
            self.pdfPars_.add(alp)
            
            roolist = ROOT.RooArgList( xvar, slo, qua, alp )
            pdf = ROOT.RooGenericPdf( pname, pname, "pow(1+@1*@0+@2*@0*@0,@3)", roolist )
            
            self.keep( [pdf,slo,qua,alp] )

            
        return pdf
      
    
# -----------------------------------------------------------------------------------------------------------
# actual main
if __name__ == "__main__":
    app = CombineApp()
    app.run()
