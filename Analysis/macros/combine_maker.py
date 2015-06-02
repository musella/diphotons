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
                        make_option("--fit-background",dest="fit_backround",action="store_true",default=False,
                                    help="Fit background",
                                    ),
                        make_option("--components",dest="components",action="callback",type="string",
                                    callback=optpars_utils.ScratchAppend(),
                                    default=["bkg"],
                                    help="Background models to use default : [%default]",
                                    ),
                        make_option("--models",dest="models",action="callback",type="string",
                                    callback=optpars_utils.ScratchAppend(),
                                    default=["dijet"],
                                    help="Background models to use default : [%default]",
                                    ),
                        make_option("--sources",dest="sources",action="callback",type="string",
                                    callback=optpars_utils.ScratchAppend(),
                                    default=["data"],
                                    help="Source dataset to use for the bkg fit default : [%default]",
                                    ),
                        make_option("--data-source",dest="data_source",action="store",type="string",
                                    default="data",
                                    help="Dataset to be used as 'data' default : [%default]",
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
        
        printLevel = ROOT.RooMsgService.instance().globalKillBelow()
        ROOT.RooMsgService.instance().setGlobalKillBelow(RooFit.FATAL)

        options.only_subset = [options.fit_name]
        
        options.store_new_only=True
        self.setup(options,args)

        if options.fit_backround:
            self.fitBackground(options,args)
            

        
    ## ------------------------------------------------------------------------------------------------------------
    def fitBackground(self,options,args):
        
        fitname = options.fit_name
        fit = options.fits[fitname]
        
        roobs = self.buildRooVar(*(self.getVar(options.observable)), recycle=False, importToWs=True)
        roobs.setBins(5000,"cache")
        roobs.setRange("fullRange",roobs.getMin(),roobs.getMax())
        roowe = self.buildRooVar("weight",[])        
        rooset = ROOT.RooArgSet(roobs,roowe)

        ## build and import data dataset
        for cat in fit["categories"]:
            treename = "%s_%s_%s" % (options.data_source,options.fit_name,cat)
            
            print treename
            dset = self.rooData(treename)
            dset.Print()
            
            reduced = dset.reduce(RooFit.SelectVars(rooset),RooFit.Range("fullRange")) ## FIXME: roobs range
            reduced.SetName("data_%s"% (cat))
            self.workspace_.rooImport(reduced)
        
        ## prepare background fit components
        for comp,model,source in zip(options.components,options.models,options.sources):
            
            ## can have one model per background component
            if comp != "":
                comp = "%s_" % comp
            
            ## loop over categories to fit background
            for cat in fit["categories"]:
                                
                treename = "%s_%s_%s" % (source,options.fit_name,cat)
                
                print treename
                dset = self.rooData(treename)
                dset.Print()
                
                dset.Print()
                reduced = dset.reduce(RooFit.SelectVars(rooset),RooFit.Range("fullRange"))
                reduced.SetName("source_dataset_%s%s"% (comp,cat))
                binned = reduced.binnedClone()
                reduced.Print()
                binned.Print()
                
                ## build pdf
                pdf = self.buildPdf(model,"model_%s%s" % (comp,cat), roobs )
                pdf.SetName("model_%s%s" % (comp,cat))
                norm = self.buildRooVar("%s_norm" %  (pdf.GetName()), [], importToWs=False ) ## normalization has to be called <pdfname>_norm or combine won't find it
                norm.setVal(reduced.sumEntries())
                extpdf = ROOT.RooExtendPdf("ext_%s" % pdf.GetName(),"ext_%s" %  pdf.GetName(),pdf,norm)
                extpdf.fitTo(binned,ROOT.RooFit.Strategy(2))
                extpdf.fitTo(reduced,ROOT.RooFit.Strategy(1))
                
                ## FIXME: set normalization to expected number of events in signal region
                ## ok as long as we data as source

                ## plot the fit result
                frame = roobs.frame()
                binned.plotOn(frame)
                extpdf.plotOn(frame)
                ## self.keep( [binned,extpdf] )

                hist   = frame.getObject(int(frame.numItems()-2))
                fitc   = frame.getObject(int(frame.numItems()-1))
                hresid = frame.residHist(hist.GetName(),fitc.GetName(),True)
                ## self.keep( [hist, fitc, hresid] )
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
                self.workspace_.rooImport(pdf)
                self.workspace_.rooImport(norm)
                self.workspace_.rooImport(reduced)
                
        # done
        self.saveWs(options)
        
    ## ------------------------------------------------------------------------------------------------------------
    def buildPdf(self,model,name,xvar,order=0,label=None):
        
        pdf = None
        if not label:
            label = model
        if model == "dijet":
            pname = "dijet_%s" % name
            linc = self.buildRooVar("%s_lin" % pname,[], importToWs=False)
            logc = self.buildRooVar("%s_log" % pname,[], importToWs=False)
            linc.setVal(5.)
            logc.setVal(-1.)
            
            self.pdfPars_.add(linc)
            self.pdfPars_.add(logc)
            
            roolist = ROOT.RooArgList( xvar, linc, logc )
            pdf = ROOT.RooGenericPdf( pname, pname, "pow(@0,@1+@2*log(@0))", roolist )
            
            self.keep( [pdf,linc,logc] )
        elif model == "moddijet":
            pname = "moddijet_%s" % name
            lina = self.buildRooVar("%s_lina" % pname,[], importToWs=False)
            loga = self.buildRooVar("%s_loga" % pname,[], importToWs=False)
            linb = self.buildRooVar("%s_linb" % pname,[], importToWs=False)
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
