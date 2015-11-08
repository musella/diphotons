#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, json
from pprint import pprint
import array
from getpass import getuser
from templates_maker import TemplatesApp

from getpass import getuser
import random

from math import sqrt

# ----------------------------------------------------------------------------------------------------
def computeShapeWithUnc(histo,extraerr=None):
    histo.Scale(1./histo.Integral())
    if not extraerr:
        return
    for xb in range(histo.GetNbinsX()+1):
        for yb in range(histo.GetNbinsX()+1):
            ib = histo.GetBin(xb,yb)            
            err = histo.GetBinError(ib)
            bbyb = extraerr*histo.GetBinContent(ib)
            err = sqrt( err*err + bbyb*bbyb )
            histo.SetBinError(ib,err)

    return
    denom = histo.Clone("temp")
    denom.Reset("ICE")
    error = ROOT.Double(0.)
    entries = histo.GetEntries()
    try:
        integral = histo.IntegralAndError(-1,-1,error) 
    except:
        integral = histo.IntegralAndError(-1,-1,-1,-1,error) 
    for xb in range(denom.GetNbinsX()+1):
        for yb in range(denom.GetNbinsX()+1):
            ib = histo.GetBin(xb,yb)            
            denom.SetBinContent(ib,integral)
            denom.SetBinError(ib,error)
    histo.Divide(histo,denom,1.,1.,"B")
    del denom
    


## ----------------------------------------------------------------------------------------------------------------------------------------
## TemplatesApp class
## ----------------------------------------------------------------------------------------------------------------------------------------

## ----------------------------------------------------------------------------------------------------------------------------------------
class TemplatesFitApp(TemplatesApp):
    """
    Class to handle template fitting.
    Takes care of preparing templates starting from TTrees.
    Inherith from PyRapp and PlotApp classes and Template Maker App class.
    """
    
    ## ------------------------------------------------------------------------------------------------------------
    def __init__(self,option_list=[],option_groups=[]):
        super(TemplatesFitApp,self).__init__(option_groups=[
                ("General templates preparation options", [
                        make_option("--compare-templates",dest="compare_templates",action="store_true",default=False,
                                    help="Make templates comparison plots",
                                    ),
                        make_option("--nominal-fit",dest="nominal_fit",action="store_true",default=False,
                                    help="do fit templates",
                                    ),
                        make_option("--fit-mc",dest="fit_mc",action="store_true",default=False,
                                    help="do fit with mc ",
                                    ),
                        make_option("--build-3dtemplates",dest="build_3dtemplates",action="store_true",
                                    default=False,
                                     help="build 3d templates with unrolled variable and mass",
                                    ), 
                        make_option("--corr-singlePho",dest="corr_singlePho",action="store_true",
                                    default=False,
                                     help="correlation sieie and chiso single fake photon",
                                    ),
                        make_option("--jackknife",dest="jack_knife",action="store_true",default=False,
                                    help="Plot RMS etc from jk pseudosamples",
                                    ),
                        make_option("--purity-sigregion",dest="pu_sigregion",action="store_true",default=False,
                        
                                    help="get also purity values for sig region, can be done on top of fit over full range",
                                    ),
                        make_option("--fixed-massbins",dest="fixed_massbins",action="store_true",default=False,
                                    help="if you want to fix the massbins otherwise --fit-massbins",
                                    ),
                        
                        make_option("--extra-shape-unc",dest="extra_shape_unc",action="store",type="float",
                                    default=None,
                                    help="Add extra uncertainty to template shapes (implemented only for plotting)",
                                    ),
                        ]
                      )
            ]+option_groups,option_list=option_list)
        

        ## load ROOT (and libraries)
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        import diphotons.Utils.pyrapp.style_utils as style_utils
        ROOT.gSystem.Load("libdiphotonsUtils")
        if ROOT.gROOT.GetVersionInt() >= 60000:
            ROOT.gSystem.Load("libdiphotonsRooUtils")

        ROOT.gStyle.SetOptStat(111111)

    ## ------------------------------------------------------------------------------------------------------------
    def __call__(self,options,args):
        """ 
        Main method. Called automatically by PyRoot class.
        """
        ## load ROOT style
        self.loadRootStyle()
        from ROOT import RooFit
        from ROOT import TH1D, TCanvas, TAxis
        ROOT.gStyle.SetOptStat(111111)
        printLevel = ROOT.RooMsgService.instance().globalKillBelow()
        ROOT.RooMsgService.instance().setGlobalKillBelow(RooFit.FATAL)
        ROOT.TH1D.SetDefaultSumw2(True)
        
        self.setup(options,args)
        
        if options.compare_templates:
            self.compareTemplates(options,args)
            
        if options.nominal_fit:
            self.nominalFit(options,args)
        if options.plot_purity:
            self.plotPurity(options,args)
        if options.corr_singlePho:
            self.corrSinglePho(options,args)
        if options.build_3dtemplates:
            self.build3dTemplates(options,args)
        if options.jack_knife:
            self.Jackknife(options,args)
        

    ## ------------------------------------------------------------------------------------------------------------
    
    def compareTemplates(self,options,args):
        fout = self.openOut(options)
        fout.Print()
        fout.cd()
        self.doCompareTemplates(options,args)
        self.saveWs(options,fout)
    
    ## ------------------------------------------------------------------------------------------------------------
    #MQ compare truth templates with rcone and sideband templates
    def doCompareTemplates(self,options,args):
        print "Compare truth templates with rcone and sideband templates"
        ROOT.TH1F.SetDefaultSumw2(True)
        for name, comparison in options.comparisons.iteritems():
            if name.startswith("_"): continue
            print "Comparison %s" % name
            prepfit=comparison["prepfit"] 
            ReDo=comparison.get("redo",True)
            doDataMc=comparison.get("doDataMc",True)
            weight_cut=comparison["weight_cut"] 
            fitname=comparison["fit"]
            if fitname=="2D" : d2=True
            else: d2=False
            fit=options.fits[fitname]
            components=comparison.get("components",fit["components"])
            print components
            for comp in components:
                if type(comp) == str or type(comp)==unicode:
                    compname = comp
                    templatesls= comparison["templates"]
                else:
                    compname, templatesls = comp
                for cat in comparison.get("categories",fit["categories"]):
                    print
                    print cat, compname
                    isoargs=ROOT.RooArgSet("isoargs")
                    massargs=ROOT.RooArgSet("massargs")
                    mass_var,mass_b=self.getVar(comparison.get("mass_binning"))
                    mass=self.buildRooVar(mass_var,mass_b,recycle=True)
                    massargs.add(mass)
                    if len(options.template_binning) > 0:
                        template_binning = array.array('d',options.template_binning)
                    else:
                        template_binning = array.array('d',comparison.get("template_binning"))
                    templatebins=ROOT.RooBinning(len(template_binning)-1,template_binning,"templatebins" )
### list to store templates for each category
                    templates = []
                    for idim in range(fit["ndim"]):
                        isoargs.add(self.buildRooVar("templateNdim%dDim%d" % ( fit["ndim"],idim),template_binning,recycle=True))
                    if d2:
                        setargs=ROOT.RooArgSet(massargs,isoargs)
                        sigRegionlow2D=float(comparison.get("lowerLimitSigRegion2D"))
                        sigRegionup2D=float(comparison.get("upperLimitSigRegion2D"))
                        sigRegionup1D=float(comparison.get("upperLimitSigRegion1D"))
                    else: setargs=ROOT.RooArgSet(isoargs)
                   # setargs.add(self.buildRooVar("weight",[],recycle=True))
                    rooweight=self.buildRooVar("weight",[],recycle=True)
                    setargs.add(rooweight)
                    setargs.Print()
                    #needed to estimate true purity for alter 2dfit
                  
                    truthname= "mctruth_%s_%s_%s" % (compname,fitname,cat)
                    truth = self.reducedRooData(truthname,setargs,False,sel=weight_cut,redo=ReDo)
                    truth.Print()
                    #if not doDataMc:
                    templates.append(truth)
### loop over templates
                    tempdatals=self.buildTemplates(templatesls,setargs,weight_cut,compname,cat) 
                    for temp in tempdatals:
                        templates.append(temp)
###------------------- split in massbins
                    masserror = array.array('d',[])
                    
                    if cat=="EEEB": catd="EBEE"#TODO implement in json file
                    else: catd=cat
                    setargs.add(massargs)
                    setargs.Print()
                    dset_data = self.reducedRooData("data_%s_%s" % (fitname,catd),setargs,False,sel=weight_cut,redo=ReDo)
                    dset_mc = self.reducedRooData("mc_%s_%s" % (fitname,catd),setargs,False,sel=weight_cut,redo=ReDo)
                    print "number of entries after reduced"
                    if not options.fixed_massbins:
                        mass_split= [int(x) for x in options.fit_massbins]
                        diphomass=self.massquantiles(dset_data,massargs,mass_b,mass_split)
                        massrange=[mass_split[2],mass_split[1]]
                    elif options.fixed_massbins and cat=="EBEB":
                        diphomass=[200.0,216.187076923,230.0,253.415384615,281.651965812,295.277948718,332.332307692,408.787692308,500.0,12999.0]
                        massrange=[0,len(diphomass)-1]
                    elif options.fixed_massbins and cat=="EBEE":
                        diphomass=[299.446153846,320.0,355.459828644,443.85640967,500.0,12999.0153846]
                        massrange=[0,len(diphomass)-1]
                    truth_pp= "mctruth_%s_%s_%s" % (compname,fitname,cat)
                    if d2:
                        tp_mcpu = ROOT.TNtuple("tree_truth_purity_all_%s_%s_%s" % (compname,fitname,cat),"tree_truth_purity_%s_%s_%s" % (compname,fitname,cat),"number_pu:frac_pu:massbin:masserror" )
                        ntp_mcpu = ROOT.TNtuple("tree_truth_purity_signalregion_%s_%s_%s" % (compname,fitname,cat),"tree_truth_purity_signalrange_%s_%s_%s" % (compname,fitname,cat),"number_pu:frac_pu:massbin:masserror" )
                        self.store_[ntp_mcpu.GetName()] =ntp_mcpu
                        self.store_[tp_mcpu.GetName()] =tp_mcpu

                 
                    for mb in range(massrange[0],massrange[1]):
                        massbin=(diphomass[mb]+diphomass[mb+1])/2.
                        masserror=(diphomass[mb+1]-diphomass[mb])/2.
                        cut=ROOT.TCut("mass>%f && mass<%f"% (diphomass[mb],diphomass[mb+1]))
                        cut_s= "%1.0f_%2.0f"% (diphomass[mb],diphomass[mb+1])
                        print cut.GetTitle()
                        if d2:
                            cut_sigregion=ROOT.TCut("templateNdim2Dim0< %f && templateNdim2Dim1< %f" %(sigRegionup1D,sigRegionup1D))
                            dset_massc=self.masscutTemplates(dset_data,cut,cut_s)
                            dset_massc_mc=self.masscutTemplates(dset_mc,cut,cut_s)
                            temp_massc_truth=self.masscutTemplates(templates[0],cut,cut_s,"temp_truthinformation")
                            number_pu=temp_massc_truth.sumEntries()
                            frac_pu=number_pu/dset_massc_mc.sumEntries()
                            tempSig_massc_truth=temp_massc_truth.Clone("tempSig_truthinformation")
                            templates_massc=[]
                            for temp_m in templates:
                                temp_massc=self.masscutTemplates(temp_m,cut,cut_s)
                                if temp_massc.sumEntries() ==0:
                                    print "!!!!!!!!!!!! attention dataset ", temp_massc, " has no entries !!!!!!!!!!!!!!!!!"
                                else:templates_massc.append(temp_massc)
###---------------get truth information per massbin and in signal range
                            data_massc_truth = dset_massc_mc.Clone("data_truthinformation")
                            data_massc_truth=data_massc_truth.reduce(cut_sigregion.GetTitle())
                            tempSig_massc_truth=tempSig_massc_truth.reduce(cut_sigregion.GetTitle())
                            number_pu_sigrange=tempSig_massc_truth.sumEntries()
                            frac_pu_sigrange=number_pu_sigrange/data_massc_truth.sumEntries()
                            tp_mcpu.Fill(number_pu,frac_pu,massbin, masserror)
                            ntp_mcpu.Fill(number_pu_sigrange,frac_pu_sigrange,massbin, masserror)
                        elif not d2:
                            templates_massc=templates[:]
###----------------------- loop over 2 legs
                        for id in range(fit["ndim"]):
                            histls=[]
                            isoarg1d=ROOT.RooArgList("isoarg")
                            isoarg1d.add(self.buildRooVar("templateNdim%dDim%d" % ( fit["ndim"],id),template_binning,recycle=True))                
                            tit = "compiso_%s_%s_%s_mb_%s_templateNdim%dDim%d" % (fitname,compname,cat,cut_s,fit["ndim"],id)
                            numEntries_s=""
                            for tm in templates_massc:
                                tempHisto=ROOT.TH1F("%s_dim%d_%d" % (tm.GetName(),fit["ndim"],id),
                                                    "%s_dim%d_%d" % (tm.GetName(),fit["ndim"],id),len(template_binning)-1,template_binning)
                                tm.fillHistogram(tempHisto,isoarg1d)
                                numEntries_s+= (" %f " % tempHisto.Integral())
                                if "truth" in tempHisto.GetName():
                                    computeShapeWithUnc(tempHisto)
                                else:
                                    computeShapeWithUnc(tempHisto,options.extra_shape_unc)
                                for bin in range(1,len(template_binning) ):
                                    tempHisto.SetBinContent(bin,tempHisto.GetBinContent(bin)/(tempHisto.GetBinWidth(bin)))
                                    tempHisto.SetBinError(bin,tempHisto.GetBinError(bin)/(tempHisto.GetBinWidth(bin)))
                                histls.append(tempHisto)
                      #      if not prepfit: 
                           # print "plot 1d histos"
                            self.plotHistos(histls,tit,template_binning,True,True,numEntries_s)
                        

                        ## roll out for combine tool per category
                        if fit["ndim"]>1:
                            self.histounroll(templates_massc,template_binning,isoargs,compname,cat,cut_s,prepfit,sigRegionlow2D,sigRegionup2D,extra_shape_unc=options.extra_shape_unc)
                            self.histounroll_book(template_binning,isoargs)

    ## ------------------------------------------------------------------------------------------------------------
    def buildTemplates(self,templatesls,setargs, weight_cut=None,compname="pf",cat="EBEB"):
        templs=[]
        for template,mapping in templatesls.iteritems():
            print template, mapping
            if "mix" in template:
                mixname = template.split(":")[-1]
                print "template_mix_%s_%s_%s" % (compname,mixname,mapping.get(cat,cat))
                templatename= "template_mix_%s_%s_%s" % (compname,mixname,mapping.get(cat,cat))
            elif "template_mc" in template:
                tempname = template.split(":")[-1]
                print "template_mc_%s_%s_%s" % (compname,tempname,mapping.get(cat,cat))
                templatename= "template_mc_%s_%s_%s" % (compname,tempname,mapping.get(cat,cat))
            else:
                print "template_%s_%s_%s" % (compname,template,mapping.get(cat,cat))
                templatename= "template_%s_%s_%s" % (compname,template,mapping.get(cat,cat))
            tempdata = self.reducedRooData(templatename,setargs,False,sel=weight_cut,redo=True)

            if "mix" in template:
                mixname=mixname[11:]
                templatename=( "reduced_template_mix_%s_%s_%s" % (compname,mixname,mapping.get(cat,cat)))
                print templatename
                tempdata.SetName(templatename)
            tempdata.Print()
            if tempdata.sumEntries() ==0:
                print "!!!!!!!!!!!! attention dataset ", templatename, " has no entries !!!!!!!!!!!!!!!!!"
            else:
                templs.append(tempdata)
        return templs
    ## ------------------------------------------------------------------------------------------------------------
    def masscutTemplates(self,dset,cut,cut_s,name=None):
        if name==None:
            name=dset.GetName()[8:]
        dset_massc = dset.Clone("%s_mb_%s"%(name,cut_s))
        dset_massc=dset_massc.reduce(cut.GetTitle())
        dset_massc.Print()
        return dset_massc
    ## ------------------------------------------------------------------------------------------------------------


    def histounroll(self,templatelist, template_binning,isoargs,comp,cat,mcut_s,prepfit,sigRegionlow,sigRegionup,extra_shape_unc=None,plot=True):
        pad_it=0
        c1=ROOT.TCanvas("d2hist_%s" % cat,"2d hists per category",1000,1000) 
        c1.Divide(1,2)
        histlistunroll=[]
        print
        print "roll out" 
        tempunroll_binning = array.array('d',[])
        histlsY=[]
        histlsX=[]
    #    print"len(template_binning)", len(template_binning)
   #     print"template_binning", template_binning
        for tempur in templatelist:
            pad_it+=1
            temp2d=ROOT.TH2F("d2%s" % (tempur.GetName()),"d2%s" % (tempur.GetName()),len(template_binning)-1,template_binning,len(template_binning)-1,template_binning)
            tempur.fillHistogram(temp2d,ROOT.RooArgList(isoargs))
           # print "integral 2d  histo", temp2d.Integral()
            temp2dx=temp2d.ProjectionX("%s_X" %tempur.GetName())
            if plot:
                if "truth" in temp2dx.GetName():
                    computeShapeWithUnc(temp2dx)
                else:
                    computeShapeWithUnc(temp2dx,extra_shape_unc)
                temp2dx.SetTitle("%s_X" %tempur.GetName())
                temp2dy=temp2d.ProjectionY("%s_Y" %tempur.GetName())
                if "truth" in temp2dy.GetName():
                    computeShapeWithUnc(temp2dy)
                else:
                    computeShapeWithUnc(temp2dy,extra_shape_unc)
                ## draw projections as a check
                histlsX.append(temp2dx)
                temp2dy.SetTitle("%s_Y" %tempur.GetName())
                histlsY.append(temp2dy)
            if len(templatelist) >1 and plot:
                ## temp2d.Scale(1./temp2d.Integral())
                if "truth" in temp2d.GetName():
                    computeShapeWithUnc(temp2d)
                else:
                    computeShapeWithUnc(temp2d,extra_shape_unc)
            tempunroll_binning = array.array('d',[])
            tempunroll_binning.append(0.0)
            sum=0.
            bin=0
            binslist=[]
            #binslist=array.array('i',(0,0))
            for b in range(1,len(template_binning)):
                for x in range(1,b+1):
                    bin+=1
                    binslist.append((x,b))
                for y in range (b-1,0,-1):
                    bin+=1
                    binslist.append((b,y))
            for bin1, bin2 in binslist:
                binErr=0.
                area=0.
                binCont=0.
                binCont= temp2d.GetBinContent(bin1,bin2)
                binErr=temp2d.GetBinError(bin1,bin2)
                area=(temp2d.GetXaxis().GetBinWidth(bin1))*(temp2d.GetYaxis().GetBinWidth(bin2))
                if not prepfit:
                    sum+=1
                    temp2d.SetBinContent(bin1,bin2,binCont/area)
                    temp2d.SetBinError(bin1,bin2,binErr/area)
                else:
                   # sum+=area
                    sum+=1
                    temp2d.SetBinContent(bin1,bin2,binCont)
                    temp2d.SetBinError(bin1,bin2,binErr)
                tempunroll_binning.append(sum)
            if prepfit:
                templateNdim2d_unroll=self.buildRooVar("templateNdim2d_unroll",tempunroll_binning,importToWs=True)
                templateNdim2d_unroll.setRange("sigRegion",sigRegionup,sigRegionlow)
                rootemplate_binning=ROOT.RooBinning(len(template_binning),template_binning,"rootemplate_binning")
                unrollvar=ROOT.RooArgList(templateNdim2d_unroll) 
              #  templateNdim2d_unroll.setBinning(rootemplate_binning)
            if plot:
                c1.cd(pad_it)
                ROOT.gPad.SetLogz()
                temp2d.Draw("COLZ")
                temp2d.GetZaxis().SetRangeUser(1e-8,1)
            bin=0
            temp1dunroll=ROOT.TH1F("hist_%s" % (tempur.GetName()),"hist_%s"% (tempur.GetName()),len(tempunroll_binning)-1,tempunroll_binning)
            for bin1, bin2 in binslist:
                #  to loop up to inclu sively b
                bin+=1
                binC= temp2d.GetBinContent(bin1,bin2)
                binE= temp2d.GetBinError(bin1,bin2)
                temp1dunroll.SetBinContent(bin,binC)
                temp1dunroll.SetBinError(bin,binE)
            histlistunroll.append(temp1dunroll)
            fail=0
            if prepfit:
                for b in range(1,temp1dunroll.GetNbinsX()+1):
                    if temp1dunroll.GetBinContent(b) ==0:
                            temp1dunroll.SetBinContent(b,0.)
                            print "ui, the bin content is zero"
                            fail=fail+1
                roodatahist_1dunroll=ROOT.RooDataHist("unrolled_%s" % (tempur.GetName()),"unrolled_%s_zerobins%u" %(tempur.GetName(),fail),unrollvar, temp1dunroll)
                roodatahist_1dunroll.Print()
                self.workspace_.rooImport(roodatahist_1dunroll,ROOT.RooFit.RecycleConflictNodes())
        if len(histlistunroll) >1 and plot:
            title="histo_%s_%s_%s" %(comp,cat,mcut_s)
            self.plotHistos(histlsX,"%s_X" %title,"charged isolation_X",template_binning,False,True,False,True)
            self.plotHistos(histlsY,"%s_Y" %title,"charged isolation_Y",template_binning,False,True,False,True)
            self.plotHistos(histlistunroll,"%s_unrolled" % (title),"charged isolation",tempunroll_binning,False,True,False,True)
            self.keep( [c1] )
            self.autosave(True)
        else: return histlistunroll 


    ## ------------------------------------------------------------------------------------------------------------
    def histounroll_book(self,template_binning,args,importToWs=True,buildHistFunc=False):
        print args
        template_binning = array.array('d',template_binning)
        th2d=ROOT.TH2F("th2d","th2d",len(template_binning)-1,template_binning,len(template_binning)-1,template_binning)
        bin=0
        binslist=[]
        isoargs=ROOT.RooArgList(args)
        #booking binslist
        for b in range(1,len(template_binning)):
            for x in range(1,b+1):
                bin+=1
                binslist.append((x,b))                
            for y in range (b-1,0,-1):
                bin+=1
                binslist.append((b,y))
        unroll_widths=array.array('d',[])
        for bin1,bin2 in binslist:
            area = th2d.GetXaxis().GetBinWidth(bin1)*th2d.GetYaxis().GetBinWidth(bin2)
            unroll_widths.append(area)
        for ibin,bins in enumerate(binslist):
            bin1,bin2=bins
            th2d.SetBinContent(bin1,bin2,
                               ## (unroll_binning[ibin]+unroll_binning[ibin+1])*0.5)
                               ibin+0.5)
        hist2d_forUnrolled=ROOT.RooDataHist("hist2d_forUnrolled","hist2d_forUnrolled",ROOT.RooArgList(isoargs), th2d)
        self.keep(hist2d_forUnrolled)
        #ct=ROOT.TCanvas("ct","ct",1000,1000) 
        #ct.cd()
        #ROOT.gStyle.SetPaintTextFormat("1.1f")
        #th2d.SetMarkerSize(3.)
        #th2d.GetXaxis().SetTitle("templateNdim2Dim0")
        #th2d.GetYaxis().SetTitle("templateNdim2Dim1")
        #th2d.Draw("TEXT")
        #self.keep( [th2d,ct] )
        #self.autosave(True)
        ret=hist2d_forUnrolled
        if buildHistFunc:
            ret=ROOT.RooHistFunc(buildHistFunc,buildHistFunc,args,hist2d_forUnrolled)
            self.keep(ret)
            
        if importToWs:
            self.workspace_.rooImport(ret,ROOT.RooFit.RecycleConflictNodes())
            
        return ret,unroll_widths

    ## ------------------------------------------------------------------------------------------------------------
    def massquantiles(self,dataset,massargs,mass_binning,mass_split):
        #print "splitByBin for dataset", dataset.GetName()
        #massH=ROOT.TH1F("%s_massH" % dataset.GetName()[-17:],"%s_massH" % dataset.GetName()[-17:],len(mass_binning)-1,mass_binning)
        massargs.Print()

        massH=ROOT.TH1F("%s_massH" % dataset.GetName(),"%s_massH" % dataset.GetName(),len(mass_binning)-1,mass_binning)
        dataset.fillHistogram(massH,ROOT.RooArgList(massargs)) 
       # print "define mass bins 0 
        massH.Scale(1.0/massH.Integral())
        prob = array.array('d',[])
        dpmq = array.array('d',[0.0 for i in range((mass_split[1]+1))])
        for i in range(0,mass_split[1]+1):
            prob.append((i+float(mass_split[2]))/mass_split[0])
        massH.GetQuantiles(mass_split[1]+1,dpmq,prob)
        #show the original histogram in the top pad
        massHC=ROOT.TH1F("%s_massHC" % dataset.GetName(),"%s_massHC" % dataset.GetName(),len(dpmq)-1,dpmq)
        dataset.fillHistogram(massHC,ROOT.RooArgList(massargs)) 
        cq=ROOT.TCanvas("cq_%s" %dataset.GetName()[-20:],"mass quantiles",10,10,700,900)
        cq.Divide(1,2)
        cq.cd(1)
        ROOT.gPad.SetLogx()
        massHC.Draw()
        #show the quantiles in the bottom pad
        cq.cd(2)
        gr =ROOT.TGraph(mass_split[1]+1,prob,dpmq)
        ROOT.gPad.SetLogy()
        gr.SetMarkerStyle(21)
        gr.GetXaxis().SetTitle("quantiles")
        gr.GetYaxis().SetTitle("diphoton mass [GeV]")
        gr.Draw("alp")
        self.keep( [cq] )
        self.autosave(True)
        #
        for  k in range(0,len(dpmq)):
            print "prob " ,prob[k] ," diphomass " , dpmq[k]  
        return dpmq
 
    ## ------------------------------------------------------------------------------------------------------------
    #MQ compare truth templates with rcone and sideband templates
    def corrSinglePho(self,options,args):
        fout = self.openOut(options)
        fout.Print()
        fout.cd()
        ROOT.TH1F.SetDefaultSumw2(True)
        setargs=ROOT.RooArgSet("setargs")
        iso,isob=self.getVar("templateNdim1Dim0")
        isovar=self.buildRooVar(iso,isob,recycle=True)
        setargs.add(isovar)
        sigma_var,sigma_b=self.getVar("phoSigmaIeIe")
        template_binning=array.array('d',[])
        for i in range(0,16):
            i=i*1.
            template_binning.append(i)
        sieievar=self.buildRooVar(sigma_var,sigma_b,recycle=True)
        setargs.add(sieievar)
        rooweight=self.buildRooVar("weight",[],recycle=True)
        setargs.add(rooweight)
        setargs.Print()
        prob = array.array('d',[])
        #n=10
        #sieieb = array.array('d',[0.0 for i in range(n+1)])
       # for i in range(0,n+1):
        #    prob.append(i/float(n))
        for cat in options.corrPlot.get("categories"):
            if cat=="EB":
                sieielow=0.007
                sieieup=0.02
                sieieb = array.array('d',[sieielow,0.0105,0.012,sieieup])
                ymax=0.22e6
            elif cat =="EE":
                sieielow=0.015
                sieieup=0.045
                sieieb = array.array('d',[sieielow,0.028,0.035,sieieup])
                ymax=90e3
            sieievar.setRange(sieielow,sieieup)
            truth = self.reducedRooData("mctruth_f_singlePho_%s"% cat,setargs,False,weight="weight < 1000.",redo=True)
            truth.Print()
            tempdata = self.reducedRooData("template_f_singlePho_%s" %cat,setargs,False,weight="weight < 1000.",redo=True)
            tempdata.Print()
            tempdata.append(truth)
            tempCombined=tempdata
            tempCombined.SetName("template_allsieie_f_singlePho_%s" %cat)
            tempCombined.Print()
            histo_sieie=ROOT.TH1F("histo_sieie_%s" %cat,"histo_sieie_%s"%cat,30,sieielow,sieieup)
            tempCombined.fillHistogram(histo_sieie,ROOT.RooArgList(sieievar)) 
            histo_sieie2=histo_sieie.Clone()
            histo_sieie.Scale(1.0/histo_sieie.Integral())
        #    histo_sieie.GetQuantiles(n+1,sieieb,prob)
            sieiebins=ROOT.RooBinning(len(sieieb)-1,sieieb,"sieiebins" )
            sieievar.setBinning(sieiebins)
            histo2_sieie=ROOT.TH2F("histo2_sieie_%s" %cat,"histo2_sieie_%s"%cat,len(sieieb)-1,sieieb,len(template_binning)-1,template_binning)
            tempCombined.fillHistogram(histo2_sieie,ROOT.RooArgList(sieievar,isovar)) 
            histo2_sieie.GetXaxis().SetNdivisions(4,3,0) 
            self.workspace_.rooImport(tempCombined)
            prb = array.array('d',[0.99,0.8,0.7,0.6,0.5,0.3,0.1])
            graphs=[]
            graphs=getQuantilesGraphs(histo2_sieie,prb)
            self.keep([graphs,histo2_sieie])
            self.plotQuantileGraphs(histo2_sieie,graphs,cat)
            #TODO fix numbers x axis
            truthp = self.reducedRooData("mctruth_p_singlePho_%s"% cat,setargs,False,redo=True)
            #works only if json file modified accordingly
            tempdatap = self.reducedRooData("template_p_singlePho_%s" %cat,setargs,False,redo=True)
            tempdatap.append(truthp)
            tempCombinedp=tempdatap
            tempCombinedp.SetName("template_allsieie_p_singlePho_%s" %cat)
            tempCombinedp.Print()
            histop_sieie=ROOT.TH1F("histop_sieie_%s" %cat,"histop_sieie_%s"%cat,30,sieielow,sieieup)
            tempCombinedp.fillHistogram(histop_sieie,ROOT.RooArgList(sieievar)) 
            #histop_sieie.Scale(1.0/histop_sieie.Integral())
            cSide=ROOT.TCanvas("cSide_%s" %cat,"cSide_%s" %cat)
            cSide.cd()
            cSide.SetLogy()
        #    histo_sieie.GetQuantiles(n+1,sieieb,prob)
            sieiebins=ROOT.RooBinning(len(sieieb)-1,sieieb,"sieiebins" )
            lineSR=ROOT.TLine(sieieb[1],0.,sieieb[1],ymax)
            lineSB=ROOT.TLine(sieieb[2],0.,sieieb[2],ymax)
            histo_sieie2.SetLineColor(ROOT.kRed)
            histo_sieie2.SetLineWidth(2)
            histop_sieie.SetLineWidth(2)
            histop_sieie.Draw("HIST")
            lineSR.Draw("SAME")
            lineSB.Draw("SAME")
            histo_sieie2.Draw(" same HIST")
            leg =ROOT.TLegend(0.55,0.4,0.85,0.6)
            leg.SetTextSize(0.03)
            leg.SetTextFont(42);
            leg.SetFillColor(ROOT.kWhite)
            leg.AddEntry(histop_sieie,"prompt single photons","l")
            leg.AddEntry(histo_sieie2,"fake single photons","l")
            leg.Draw()
            histop_sieie.GetXaxis().SetTitle("#sigma_{i#etai#eta}") 
            histop_sieie.GetXaxis().SetNdivisions(4,3,0) 
            self.keep([cSide])
            self.autosave(True)
        self.saveWs(options,fout)

    ## ------------------------------------------------------------------------------------------------------------
    def plotQuantileGraphs(self,histo,graphs,cat):
        
        c=ROOT.TCanvas("cCorrelation2d_%s"%cat ,"cCorrelation2d_%s"%cat,10,10,700,900)
        c.cd()
        histo.GetXaxis().SetTitle("#sigma_{i#etai#eta}")
        histo.GetYaxis().SetTitle("Charged PF Isolation [GeV]")
        histo.Draw("colz")
        c.Update()
        ps = c.GetPrimitive("stats")
        ps.SetX2(0.99)
        histo.SetStats(0)
        c.Modified()
        c2=ROOT.TCanvas("cCorrelation1d_%s"%cat ,"cCorrelation1d_%s"%cat,10,10,700,900)
        c2.Divide(1,2)
        c2.cd(1)
        histo.ProjectionX().Draw()
        c2.cd(2)
        histo.ProjectionY().Draw()
        
        cQ=ROOT.TCanvas("cCorrelation_%s"%cat ,"corr chIso mass %s"% cat,10,10,700,900)
        cQ.cd()
        i=0
        leg =ROOT.TLegend(0.55,0.65,0.85,0.9)
        leg.SetTextSize(0.03)
        leg.SetTextFont(42);
        leg.SetFillColor(ROOT.kWhite)
        for gr in graphs:
            gr.SetMarkerStyle(21)
            gr.SetMarkerColor(ROOT.kRed-i)
            gr.SetLineColor(ROOT.kRed-i)
            if i==0:
                gr.GetXaxis().SetTitle("#sigma_{i#etai#eta}")
                gr.GetXaxis().SetNdivisions(4,3,0) 
                gr.GetYaxis().SetTitle("Charged PF Isolation [GeV]")
                gr.GetYaxis().SetRangeUser(0.,24.)
                gr.Draw("AP")
            if i>0:
                gr.Draw("P SAME")
            leg.AddEntry(gr,gr.GetName()[-14:],"ple")
            i=i+1
        leg.Draw()
        self.keep( [c,c2,cQ] )
        self.autosave(True)
        #

    ## ------------------------------------------------------------------------------------------------------------

    def plotHistos(self,histlist,title,titlex,template_bins,dim1,doDataMc,logx=False,logy=False,numEntries=None):
        leg = ROOT.TLegend(0.65,0.75,0.85,0.9  )
        leg.SetFillColor(ROOT.kWhite)
       # leg.SetHeader("#%s " % numEntries)
        canv = ROOT.TCanvas(title,title)
        if not doDataMc:
            canv.Divide(1,2)
            canv.cd(1)
            ROOT.gPad.SetPad(0., 0.35, 1., 1.0)
            canv.cd(2)
            ROOT.gPad.SetPad(0., 0., 1., 0.35)
            ROOT.gPad.SetGridy()
        ROOT.gPad.SetLogy()
        if logx:
            ROOT.gPad.SetLogx()
        canv.cd(1)
        # for dataMc plot MC as filled and data as points
        if doDataMc:
            histstart=0
        else:
            histstart=0
        ymax = 0.
        ymin = 1.e+5
        histlist[histstart].GetYaxis().SetLabelSize( histlist[histstart].GetYaxis().GetLabelSize() * canv.GetWh() / ROOT.gPad.GetWh() )
        k=0
        
        minX=-1.0
        #if min(template_bins)==0.: minX=-0.1
        #else: minX=min(template_bins)
        if dim1:histlist[histstart].GetXaxis().SetTitle(title[-17:])
        else:histlist[histstart].GetXaxis().SetTitle("charged particle flow isolation (GeV)")
        for i in range(histstart,len(histlist)):
            if "pp" in histlist[histstart].GetName():comp="#gamma #gamma"
            if "pf" in histlist[histstart].GetName():comp="#gamma j"
            if "ff" in histlist[histstart].GetName():comp="j j"
            histlist[i].GetXaxis().SetLimits(minX,15.)
            if histlist[i].GetMinimum() != 0.:   ymin = min(ymin,histlist[histstart].GetMinimum())
            mctruth_expectedStyle =  [["SetFillStyle",3004],["SetFillColorAlpha",(ROOT.kRed,0.0)],["SetLineColor",ROOT.kRed]]
            mc_expectedStyle =[["SetFillStyle",3004],["SetFillColorAlpha",(ROOT.kAzure+2,0.0)],["SetLineColor",ROOT.kAzure+2]]
            data_expectedStyle =[["SetLineWidth",3],["SetMarkerStyle",20],["SetMarkerSize",2.0],["SetMarkerColor",ROOT.kBlack],["SetLineColor",ROOT.kBlack]]
            if "mctruth" in histlist[i].GetName():
                style_utils.apply(histlist[i],mctruth_expectedStyle)
                if i==histstart:histlist[i].Draw("E2")
                else: histlist[i].Draw("E2 SAME")
                leg.AddEntry(histlist[i],"%s MC truth"%comp,"f")  
            elif ("mix" and "MC" in histlist[i].GetName()) or ("mc_pp" in histlist[i].GetName()):
                style_utils.apply(histlist[i],mc_expectedStyle)
                if i==histstart:histlist[i].Draw("E2")
                else: histlist[i].Draw("E2 SAME")
                leg.AddEntry(histlist[i],"%s MC"%comp,"f")  
            elif not ("mc" or "MC") in histlist[i].GetName():
                style_utils.apply(histlist[i],data_expectedStyle)
                if i==histstart:histlist[i].Draw("E")
                else: histlist[i].Draw("E SAME")
                leg.AddEntry(histlist[i],"%s data"%comp,"lp")  
            #leg.AddEntry(histlist[i],histlist[i].GetName(),"l")  
                ymax = max(ymax,histlist[histstart].GetMaximum())
                histlist[i].GetXaxis().SetLimits(minX,15.)
                histlist[i].GetYaxis().SetRangeUser(ymin*0.5,ymax*2.)
        if "unroll" in title:
           histlist[histstart].GetYaxis().SetRangeUser(ymin*0.5,ymax*20.)
        leg.Draw()
        #change for data mc comparison 
        #ratios = []
        #canv.cd(2)
        #for ihsit,hist in enumerate(histlist[1:]):
        #    ratios.append( hist.Clone("ratio_%d" % ihsit) )
        #    ratios[-1].Divide(histlist[0])
       # 
       # ratios[0].GetYaxis().SetTitleSize( histlist[0].GetYaxis().GetTitleSize() * 3.5/3.5 )
       # ratios[0].GetYaxis().SetLabelSize( histlist[0].GetYaxis().GetLabelSize() * 6.5/3.5 )
       # ratios[0].GetYaxis().SetTitleOffset( histlist[0].GetYaxis().GetTitleOffset() * 6.5/3.5 )
        #ratios[0].GetXaxis().SetTitleSize( histlist[0].GetXaxis().GetTitleSize() * 4.5/3.5 )
        #ratios[0].GetXaxis().SetLabelSize( histlist[0].GetXaxis().GetLabelSize() * 6.5/3.5 )
        #if dim1:
        #    ratios[0].GetXaxis().SetTitle(title[-17:])
        #else:
        #    ratios[0].GetXaxis().SetTitle("charged particle flow isolation (GeV)")
        #ratios[0].Draw()        
        #ratios[0].GetYaxis().SetTitle("ratio")
        #ratios[0].GetXaxis().SetLimits(-1.0,15.)
        #ratios[0].GetYaxis().SetRangeUser(-2,2)
       # for r in ratios[1:]:
        #    r.Draw("same")
        ROOT.gStyle.SetOptStat(0)
            #  ROOT.gStyle.SetOptTitle(0)
        self.keep( [canv] )
        self.autosave(True)
            


    ## ------------------------------------------------------------------------------------------------------------
    def build3dTemplates(self,options,args):
        fout = self.openOut(options)
        fout.Print()
        fout.cd()
        ROOT.TH1F.SetDefaultSumw2(True)
        weight_cut=options.build3d.get("weight_cut") 
        var,var_b=self.getVar("templateNdim2d_unroll")
        unrolledIso=self.buildRooVar(var,var_b,recycle=True)
        unrolledIso.Print()
        isoargs=ROOT.RooArgSet("isoargs")
        for idim in range(int(options.build3d["ndim"])):
            iso,biniso=self.getVar("templateNdim2Dim%d" % (idim))
            isoargs.add(self.buildRooVar(iso,biniso,recycle=True))
        template_binning=array.array('d',[0.0,0.1,5.,15.])
         
        self.histounroll_book(template_binning,isoargs)
        return
        components=options.build3d.get("components")
        dim=options.build3d.get("dimensions")
        mass_var,mass_b=self.getVar("mass")
        mass=self.buildRooVar(mass_var,mass_b,recycle=True)
        mass.Print()
        setargs=ROOT.RooArgSet(isoargs)
        setargs.add(mass)
        categories = options.build3d.get("categories")
        components = options.build3d.get("components")
        for catd in categories:
            print "-----------------------------------------------------------------"
            if catd=="EEEB": cat="EBEE" 
            else:cat=catd
            data_book=self.rooData("hist2d_forUnrolled")
            data_book.Print()
            #get dataset and add column (actually filling values in) 
            unrolledVar=ROOT.RooHistFunc(unrolledIso.GetName(),unrolledIso.GetName(),isoargs,data_book)
            data = self.reducedRooData("data_2D_%s" %cat,setargs,False,sel=weight_cut, redo=False)
            data.addColumn(unrolledVar)
            dataCombine=data.reduce(ROOT.RooArgSet(mass,unrolledIso))
            dataCombine.SetName("data_3D_%s" %cat)
            dataCombine.Print()
            self.workspace_.rooImport(dataCombine,ROOT.RooFit.RenameVariable("mass","mgg"))
          #  for temp in tempname=options.build3d.get("tempname"):
          #TODO grab template names from json file
            for comp in components:
                print cat, comp 
                histo_book=self.rooData("hist2d_forUnrolled")
                if comp=="pp":
                    histo_temp = self.reducedRooData("template_%s_2D_%s" %(comp,cat),setargs,False,sel=weight_cut,redo=False)
                else:
                    histo_temp = self.reducedRooData("template_mix_%s_kDSinglePho2D_%s" %(comp,cat),setargs,False,sel=weight_cut,redo=False)
                
                histo_temp.addColumn(unrolledVar)
                histoCombine_temp=histo_temp.reduce(ROOT.RooArgSet(mass,unrolledIso))
                histoCombine_temp.SetNameTitle("template_%s_3D_%s" %(comp,cat),"template_%s_3D_%s" %(comp,cat))
                histoCombine_temp.Print()
                self.workspace_.rooImport(histoCombine_temp,ROOT.RooFit.RenameVariable("mass","mgg"))
                histo_mctruth = self.reducedRooData("mctruth_%s_2D_%s" %(comp,cat),setargs,False,sel=weight_cut,redo=False)
                histo_mctruth.addColumn(unrolledVar)
                histoCombine_mctruth=histo_mctruth.reduce(ROOT.RooArgSet(mass,unrolledIso))
                histoCombine_mctruth.SetNameTitle("mctruth_%s_3D_%s" %(comp,cat),"mctruth_%s_3D_%s" %(comp,cat))
                histoCombine_mctruth.Print()
                self.workspace_.rooImport(histoCombine_mctruth,ROOT.RooFit.RenameVariable("mass","mgg"))
        self.saveWs(options,fout)
    ## ------------------------------------------------------------------------------------------------------------
    def nominalFit(self,options,args):
        fout = self.openOut(options)
        fout.Print()
        fout.cd()
        self.doNominalFit(options,args)
        self.saveWs(options,fout)

## ------------------------------------------------------------------------------------------------------------
    def doNominalFit(self,options,args):
        ROOT.TH1F.SetDefaultSumw2(True)
        for name, nomFit in options.nominalFit.iteritems():
            if name.startswith("_"): continue
            isoargs=ROOT.RooArgSet("isoargs")
            iso1,biniso1=self.getVar("templateNdim2Dim0")
            iso2,biniso2=self.getVar("templateNdim2Dim1")
            if len(options.template_binning) > 0:
                biniso = array.array('d',options.template_binning)
            else:
                biniso = array.array('d',options.comparisons.get("template_binning"))
            isoargs.add(self.buildRooVar(iso1,biniso,recycle=True))
            isoargs.add(self.buildRooVar(iso2,biniso,recycle=True))
            obsls=ROOT.RooArgList("obsls")
            weight_cut="weight < 5." 
            var,var_b=self.getVar(nomFit.get("observable"))
            lowsigRegion=float(nomFit.get("lowerLimitSigRegion"))
            upsigRegion=float(nomFit.get("upperLimitSigRegion"))
            extended_fit=nomFit.get("extended_fit",False)
            observable=self.buildRooVar(var,var_b,recycle=True)
            observable.setRange("sigRegion",lowsigRegion,upsigRegion)
            obsls.add(observable)
             
            #you want to keep bins from 0 to 3
            components=nomFit.get("components")
            print "nominal fit with: ", name, " observable : ", nomFit.get("observable")
            tempname=options.fit_templates[0]
            dim=nomFit.get("dimensions")
            mass_var,mass_b=self.getVar(nomFit.get("mass_binning"))
            mass=self.buildRooVar(mass_var,mass_b,recycle=True)
            setargs=ROOT.RooArgSet(isoargs)
            setargs.add(mass)
            hist_Eta=[]
            categories = options.fit_categories
            mass_split= [int(x) for x in options.fit_massbins]
            jkpf=nomFit.get("jackknife_pf",False)
            jkpp=nomFit.get("jackknife_pp",False)
            jkID="non existing"

            if jkpf:
                jkID="jkpf"
                jks=int(options.jackknife.get("jk_source"))
                jkt=int(options.jackknife.get("jk_target"))
                num=jks+jkt
            elif jkpp:
                jkID="jkpp"
                num=int(options.jackknife.get("jk_pp"))
            else:num=1
            for cat in categories:
                print "-----------------------------------------------------------------"
                if cat=="EEEB": catd="EBEE" 
                else:catd=cat
                data_book=self.rooData("hist2d_forUnrolled")
                data_book.Print()
                unrolledVar=ROOT.RooHistFunc(observable.GetName(),observable.GetName(),isoargs,data_book)
                if not options.fit_mc: dodata=True
                else: dodata=False
                if dodata:
                    dset="_"
                else:
                    dset="_mc_"
              #get dataset and add column (actually filling values in)
                if dodata:
                    data = self.reducedRooData("data_2D_%s" % (catd),setargs,False,sel=weight_cut, redo=False)
                else:
                    data = self.reducedRooData("mc_2D_%s" % (catd),setargs,False,sel=weight_cut, redo=False)
               # data = self.reducedRooData("data_2D_%s" % (catd),setargs,False,sel=weight_cut, redo=False)
                print data.sumEntries()
                data.addColumn(unrolledVar)
                data=data.reduce(ROOT.RooArgSet(mass,observable))
                data.Print()
                tree_mass=self.treeData("%s_pp_2D_%s"%(options.plotPurity["treetruth"], cat))
                if not dodata:
                    tp = ROOT.TNtuple("tree_fitresult_fraction%s%s_%s_%s" % (dset,tempname,dim,cat),"tree_fitresult_fraction_%s_%s_%s" % (tempname,dim,cat),"purity_pp:error_pp_sumw2on:error_pp:purity_pf:error_pf_sumw2on:error_pf:purity_ff:error_ff:massbin:masserror" )
                    self.store_[tp.GetName()] = tp
                if dodata:
                    tps=[]
                    for i in range(num):
                        if not (jkpf or jkpp):tpi = ROOT.TNtuple("tree_fitresult_fraction%s%s_%s_%s" % (dset,tempname,dim,cat),"tree_fitresult_fraction_%s_%s_%s" % (tempname,dim,cat),"purity_pp:error_pp:purity_pf:error_pf:purity_ff:error_ff:massbin:masserror" )
                        elif jkpf: tpi = ROOT.TNtuple("tree_fitresult_fraction%s%s_jkpf%i_%s_%s" % (dset,tempname,i,dim,cat),"tree_fitresult_fraction_%s_jk%i_%s_%s" % (tempname,i,dim,cat),"purity_pp:error_pp:purity_pf:error_pf:massbin:masserror" )

                        elif jkpp: tpi = ROOT.TNtuple("tree_fitresult_fraction%s%s_jkpp%i_%s_%s" % (dset,tempname,i,dim,cat),"tree_fitresult_fraction_%s_jk%i_%s_%s" % (tempname,i,dim,cat),"purity_pp:error_pp:purity_pf:error_pf:massbin:masserror" )
                        self.store_[tpi.GetName()] = tpi
                        tps.append(tpi)
                    if extended_fit:
                        ntp = ROOT.TNtuple("tree_fitresult_events%s%s_%s_%s" % (dset,tempname,dim,cat),"tree_fitresult_events_%s_%s_%s" % (tempname,dim,cat),"norm:purity_pp:error_pp_sumw2off:error_pp_sumw2on:purity_pf:error_pf_sumw2off:error_pf_sumw2on:massbin:masserror" )
                        self.store_[ntp.GetName()] = ntp
                if options.pu_sigregion:
                    tpSig = ROOT.TNtuple("tree_fitresult_fraction_sigRegion%s%s_%s_%s" % (dset,tempname,dim,cat),"tree_fitresult_fraction_sigRegion_%s_%s_%s" % (tempname,dim,cat),"purity_pp:error_pp:purity_pf:error_pf:purity_ff:error_ff:massbin:masserror" )
                    tpRatSig = ROOT.TNtuple("tree_fitresult_fraction_RatsigRegion%s%s_%s_%s" % (dset,tempname,dim,cat),"tree_fitresult_fraction_RatsigRegion_%s_%s_%s" % (tempname,dim,cat),"ratSig_pp:ratSig_pf:ratSig_ff:massbin:masserror" )
                    self.store_[tpSig.GetName()] = tpSig
                    self.store_[tpRatSig.GetName()] = tpRatSig
                massrange= range(0,tree_mass.GetEntries())
                if not options.fixed_massbins and len(mass_split)== 3:
                    massrange=range(mass_split[2],mass_split[1])
                for mb in massrange:
                    print "---------------------------------------------------" 
                    tree_mass.GetEntry(mb)
                    cut=ROOT.TCut("mass>%f && mass<%f"% (tree_mass.massbin-tree_mass.masserror,tree_mass.massbin+tree_mass.masserror))
                    cut_s= "%1.0f_%1.0f"%  (tree_mass.massbin-tree_mass.masserror,tree_mass.massbin+tree_mass.masserror)
                    print cut.GetTitle()
                    data_massc=data.reduce(cut.GetTitle())
                    data_massc.Print()
                    #define fit parameters
                    jpp = ROOT.RooRealVar("jpp","jpp",0.6,0.,1.)
                    jpf = ROOT.RooRealVar("jpf","jpf",0.4,0.,1.)
                    if extended_fit:
                        entries= data_massc.sumEntries("templateNdim2d_unroll <= 4.")
                        jnorm = ROOT.RooRealVar("jnorm","jnorm",entries,0.,2.*entries)
                        fpp= ROOT.RooFormulaVar("fpp","fpp","@0*@1 ",ROOT.RooArgList(jnorm,jpp))
                    else:
                        fpp= ROOT.RooFormulaVar("fpp","fpp","jpp ",ROOT.RooArgList(jpp))
                    pu_estimates=ROOT.RooArgList(fpp)
                    pu_estimates_roopdf=ROOT.RooArgList(fpp)
                    if len(components)>2 and not extended_fit: 
                        fpf= ROOT.RooFormulaVar("fpf","fpf","jpf ",ROOT.RooArgList(jpf))
                        pu_estimates.add(fpf)
                    elif extended_fit:
                        if len(components)>2:
                            fpf= ROOT.RooFormulaVar("fpf","fpf","@0*@1 ",ROOT.RooArgList(jnorm,jpf))
                            fff= ROOT.RooFormulaVar("fff","fff","(@0*TMath::Max(0.,(1-@1-@2))) ",ROOT.RooArgList(jnorm,jpp,jpf))
                            pu_estimates_roopdf.add(fpf)
                            pu_estimates_roopdf.add(fff)
                        elif len(components)<3:
                            fpf= ROOT.RooFormulaVar("fpf","fpf","(@0*(1-@1)) ",ROOT.RooArgList(jnorm,jpp))
                            pu_estimates_roopdf.add(fpf)
                    pdf_collections=[ ]
                    i=0
                    if not (jkpf or jkpp):
                        pdf_set=ROOT.RooArgList() 
                        for comp in nomFit["components"]:
                            if tempname=="unrolled_template_mix" and not dodata:
                                dim_new="2DMC"
                                dset="_"
                            else:dim_new=dim
                            if tempname=="unrolled_mctruth": dset="_"
                            if i==0 and  tempname=="unrolled_template_mix":
                                tempname_new="unrolled_template"
                                if dodata:
                                    dset="_"
                                else: 
                                    dset="_mc_"
                                    dim_new="2D"
                            else: tempname_new=tempname
                            
                            print "%s%s%s_%s_%s_mb_%s"%(tempname_new,dset,comp, dim_new,cat,cut_s)
                            histo = self.rooData("%s%s%s_%s_%s_mb_%s"%(tempname_new,dset,comp, dim_new,cat,cut_s))
                            rooHistPdf=ROOT.RooHistPdf("pdf_%s"% histo.GetName(),"pdf_%s"% histo.GetTitle(),ROOT.RooArgSet(obsls),histo)
                         
                            self.keep([rooHistPdf])
                            if extended_fit:
                                rooExtPdf=ROOT.RooExtendPdf("extpdf_%s"% histo.GetName(),"extpdf_%s"% histo.GetTitle(),rooHistPdf,pu_estimates_roopdf[i],"sigRegion")
                                pdf_set.add(rooExtPdf)
                            else:
                                pdf_set.add(rooHistPdf)
                            i=i+1

                        pdf_collections.append(pdf_set)
                    else:
                        for i in range(num):
                            pdf_set=ROOT.RooArgList()
                            for comp in nomFit["components"]:
                                if jkpf:
                                    if comp=="pp":
                                        histo = self.rooData("unrolled_template_pp_2D_%s_mb_%s"%(cat,cut_s))
                                        rooHistPdf=ROOT.RooHistPdf("pdf_%s"% histo.GetName(),"pdf_%s"% histo.GetName(),ROOT.RooArgSet(obsls),histo)
                                    elif comp=="pf":
                                        if i < jks: name= "unrolled_template_mix_pf_2D_%i_%s_mb_%s"%(i,cat,cut_s)
                                        elif i>= jks:name= "unrolled_template_mix_pf_%i_2D_%s_mb_%s"%(i-jks,cat,cut_s)
                                        histo = self.rooData(name)
                                        print name
                                        rooHistPdf=ROOT.RooHistPdf("pdf_%s"% histo.GetName(),"pdf_%s"% histo.GetName(),ROOT.RooArgSet(obsls),histo)
                                else:
                                    if comp=="pp":
                                        histo = self.rooData("unrolled_template_pp_%i_2D_%s_mb_%s"%(i,cat,cut_s))
                                        rooHistPdf=ROOT.RooHistPdf("pdf_%s"% histo.GetName(),"pdf_%s"% histo.GetName(),ROOT.RooArgSet(obsls),histo)
                                    elif comp=="pf":
                                        histo = self.rooData("unrolled_template_mix_pf_2D_%s_mb_%s"%(cat,cut_s))
                                        rooHistPdf=ROOT.RooHistPdf("pdf_%s"% histo.GetName(),"pdf_%s"% histo.GetName(),ROOT.RooArgSet(obsls),histo)
                                self.keep([rooHistPdf])
                                pdf_set.add(rooHistPdf)
                            pdf_collections.append(pdf_set)
                    for k in range(num):
                        ArgListPdf=None
                        jpp.setVal(0.6)
                        jpf.setVal(0.4)
                        ArgListPdf=pdf_collections[k]
                        ArgListPdf.Print()
                        if extended_fit:
                            fitUnrolledPdf=ROOT.RooAddPdf("fitPdfs_%s%s%s_%s_mb_%s" % (tempnam,dset,cat,dim,cut_s),"fitPdfs_%s_%s_%s_mb_%s" % (tempname,cat,dim,cut_s),ArgListPdf  )
                        else:
                            fitUnrolledPdf=ROOT.RooAddPdf("fitPdfs_%s%s%s_%s_mb_%s" % (tempname,dset,cat,dim,cut_s),"fitPdfs_%s_%s_%s_mb_%s" % (tempname,cat,dim,cut_s),ArgListPdf,pu_estimates,True )
                        self.workspace_.rooImport(fitUnrolledPdf)
              #save roofitresult in outputfile
                        data_massc.Print()
                        print data_massc.sumEntries()
                        fit_studies = fitUnrolledPdf.fitTo(data_massc, RooFit.NumCPU(8),RooFit.Strategy(2),RooFit.Extended(extended_fit),RooFit.SumW2Error(False),RooFit.Save(True))
                        pu_pp=fpp.getParameter("jpp").getVal()
                        err_pp=fpp.getParameter("jpp").getError()
                        if extended_fit:
                            norm=fpp.getParameter("jnorm").getVal()
                            norm_err=fpp.getParameter("jnorm").getError()
                            pu_npp=fpp.getVal()
                            err_npp=fpp.getPropagatedError(fit_mcstudies)
                        if len(components)>2: 
                            if not extended_fit:
                                fpu_pf= ROOT.RooFormulaVar("fpu_pf","fpu_pf","(1-@0)*@1",ROOT.RooArgList(fpp.getParameter("jpp"),fpf.getParameter("jpf")))
                                pu_pf=fpu_pf.getVal()
                                err_pf=fpu_pf.getPropagatedError(fit_studies)
                                fpu_ff=ROOT.RooFormulaVar("fpu_ff","fpu_ff","(1-@0)*(1-@1)",ROOT.RooArgList(fpp.getParameter("jpp"),fpf.getParameter("jpf")))
                                pu_ff=fpu_ff.getVal()
                                err_ff=fpu_ff.getPropagatedError(fit_studies)
                            else:
                                pu_npf=fpf.getVal()
                                err_npf=fpf.getPropagatedError(fit_studies)
                            covariance_studies=fit_studies.covarianceMatrix()
                            correlation_studies=fit_studies.correlationMatrix()
                            self.workspace_.rooImport(covariance_studies, "covariance_studies_%i"%k)
                            self.workspace_.rooImport(correlation_studies,"correlation_studies_%i"%k)
                            self.workspace_.rooImport(fit_studies,"fit_studies_%i" %k)
                        else:
                            pu_pf=1-pu_pp
                            pu_ff=0.
                            err_ff=0.
                            if extended_fit:
                                pu_npf=1-pu_npp
                                err_npf=0.
                            err_pf=err_pp
                        print
                        if dodata and not extended_fit:
                            tps[k].Fill(pu_pp,err_pp,pu_pf,err_pf,pu_ff,err_ff,tree_mass.massbin,tree_mass.masserror)
                        if dodata and extended_fit:
                            tps[k].Fill(pu_pp,err_pp,pu_pf,err_pf,tree_mass.massbin,tree_mass.masserror)
                        
                        if dodata:
                                self.plotFit(observable,fitUnrolledPdf,ArgListPdf,data_massc,components,cat,log=True,i=k) 
                                self.plotFit(observable,fitUnrolledPdf,ArgListPdf,data_massc,components,cat,log=False,i=k)
                        else:
                                self.plotFit(observable,fitUnrolledPdf,ArgListPdf,data_massc,components,cat,log=True,i=k) 
                                self.plotFit(observable,fitUnrolledPdf,ArgListPdf,data_massc,components,cat,log=False,i=k)
                        if options.pu_sigregion:
                            fpuSig_pp= ROOT.RooFormulaVar("fpuSig_pp","fpuSig_pp","(@0*@1)/@2",ROOT.RooArgList(fpp,fitUnrolledPdf.pdfList()[0].createIntegral(ROOT.RooArgSet(observable),"sigRegion"),fitUnrolledPdf.createIntegral(ROOT.RooArgSet(observable),"sigRegion")))
                            puSig_pp=fpuSig_pp.getVal()
                            errSig_pp=fpuSig_pp.getPropagatedError(fit_studies)
                            if err_pp !=0:ratSig_pp=errSig_pp/err_pp
                            else:ratSig_pp=0.
                            print pu_pp, fitUnrolledPdf.pdfList()[0].createIntegral(ROOT.RooArgSet(observable),"sigRegion").getVal(),fitUnrolledPdf.createIntegral(ROOT.RooArgSet(observable),"sigRegion").getVal(), puSig_pp
                            if len(components)==2:
                                puSig_pf=1-puSig_pp
                                errSig_pf=errSig_pp
                                if err_pf !=0: ratSig_pf=errSig_pf/err_pf
                                else: ratSig_pf=0.
                                puSig_ff=0.
                                errSig_ff=0.
                                ratSig_ff=0.

                            if len(components)>2:
                                fpuSig_pf= ROOT.RooFormulaVar("fpuSig_pf","fpuSig_pf","(@0*@1)/@2",ROOT.RooArgList(fpu_pf,fitUnrolledPdf.pdfList()[1].createIntegral(ROOT.RooArgSet(observable),"sigRegion"),fitUnrolledPdf.createIntegral(ROOT.RooArgSet(observable),"sigRegion")))
                                puSig_pf=fpuSig_pf.getVal()
                                errSig_pf=fpuSig_pf.getPropagatedError(fit_studies)
                                ratSig_pf=errSig_pf/err_pf
                                fpuSig_ff= ROOT.RooFormulaVar("fpuSig_ff","fpuSig_ff","(@0*@1)/@2",ROOT.RooArgList(fpu_ff,fitUnrolledPdf.pdfList()[2].createIntegral(ROOT.RooArgSet(observable),"sigRegion"),fitUnrolledPdf.createIntegral(ROOT.RooArgSet(observable),"sigRegion")))
                                puSig_ff=fpuSig_ff.getVal()
                                errSig_ff=fpuSig_ff.getPropagatedError(fit_studies)
                                ratSig_ff=errSig_ff/err_ff
                            tpSig.Fill(puSig_pp,errSig_pp,puSig_pf,errSig_pf,puSig_ff,errSig_ff,tree_mass.massbin,tree_mass.masserror)
                            tpRatSig.Fill(ratSig_pp,ratSig_pf,ratSig_ff,tree_mass.massbin,tree_mass.masserror)
    #ML fit to weighted dataset: SumW2Error takes statistics of dataset into account, scales with number of events in datasetif ON good for MC comparison, takes limited statistics of MC dataset into account
  #  if OUT treated as if it would be data- for data MC comparison
                    print "-------------------------------------------------------------------------"
                    if not dodata:
                        jpp.setVal(0.8)
                        jpf.setVal(0.2)
                        fit_mcstudies = fitUnrolledPdf.fitTo(data_massc, RooFit.NumCPU(8),RooFit.Strategy(2),RooFit.Extended(extended_fit),RooFit.SumW2Error(True),RooFit.Save(True))
                        if extended_fit: 
                            jnorm.setVal(entries)
                            puerr_npp=fpp.getPropagatedError(fit_mcstudies)
                        puerr_pp=fpp.getParameter("jpp").getError()
                        if len(components)>2:
                            if extended_fit:
                               puerr_npf=fpf.getPropagatedError(fit_mcstudies)
                            puerr_pf=fpf.getParameter("jpf").getError()
                            covariance_mcstudies=fit_mcstudies.covarianceMatrix()
                            correlation_mcstudies=fit_mcstudies.correlationMatrix()
                            self.workspace_.rooImport(covariance_mcstudies,"covariance_mcstudies")
                            self.workspace_.rooImport(correlation_mcstudies,"correlation_mcstudies")
                            self.workspace_.rooImport(fit_mcstudies,"fit_mcstudies")
                        else: 
                            if extended_fit:
                                puerr_npf=0.
                            puerr_pf=0.
                        print "puerr_pp " ,puerr_pp, " puerr_pf " ,puerr_pf
                    if extended_fit and not dodata:
                        ntp.Fill(norm,pu_npp,puerr_npp,err_npp,pu_npf,puerr_npf,err_npf,tree_mass.massbin,tree_mass.masserror )
                    if not dodata:
                        tp.Fill(pu_pp,puerr_pp,err_pp,pu_pf,puerr_pf,err_pf,pu_ff,err_ff,tree_mass.massbin,tree_mass.masserror )
                if jkpf or jkpp:
                    self.plotJKpurity(options,cat,dim,tps,jkID)
                print "done fit ...."
                print
    ## ---------------#--------------------------------------------------------------------------------------------
    
    
    
    
    
    ## ---------------#--------------------------------------------------------------------------------------------
    def plotFit(self,roovar,rooaddpdf,roopdfs,data,components,cat,log,i=None):
        ROOT.TH1F.SetDefaultSumw2(True)
        b=ROOT.TLatex()
        b.SetNDC()
        b.SetTextSize(0.06)
        b.SetTextColor(ROOT.kRed)
        if "mc" in data.GetName():
            cFit = ROOT.TCanvas("c%s_%u_%s_mc_%i" %(rooaddpdf.GetName(),len(components),log,i),"cFit",1200,800)
        else:cFit = ROOT.TCanvas("c%s_%u_%s_%i" %(rooaddpdf.GetName(),len(components),log,i),"cFit",1200,800)
        leg =ROOT.TLegend(0.5,0.8,0.35,0.9)
        leg.SetTextSize(0.03)
        leg.SetTextFont(42);
        leg.SetFillColor(ROOT.kWhite)
        cFit.cd(1)
        if log:
            cFit.SetLogy()
        frame = roovar.frame(RooFit.Title("1d fit for category %s and %u components"% (cat,len(components))))
        data.plotOn(frame,RooFit.Name("data"))
    #    print "data has sigRegion ? ", data.get()[roovar.GetName()].hasRange("sigRegion")
    #    dataVar = data.get()[roovar.GetName()]
     #   dataVar.setRange("sigRegion",roovar.getBinning("sigRegion").lowBound(),roovar.getBinning("sigRegion").highBound())
    #     data.plotOn(frame,RooFit.Name("datasigRegion"),RooFit.Range("sigRegion"),RooFit.LineColor(ROOT.kCyan+1))
        rooaddpdf.plotOn(frame,RooFit.Name("fit"))
        rooaddpdf.plotOn(frame,RooFit.Components(roopdfs[0].GetName()),RooFit.LineStyle(ROOT.kDashed),RooFit.LineColor(ROOT.kRed),RooFit.Name("pp"))
        rooaddpdf.plotOn(frame,RooFit.Components(roopdfs[1].GetName()),RooFit.LineStyle(ROOT.kDashed),RooFit.LineColor(ROOT.kCyan+1),RooFit.Name("pf"))
        if len(components)>2:
            rooaddpdf.plotOn(frame,RooFit.Components(roopdfs[2].GetName()),RooFit.LineStyle(ROOT.kDashed),RooFit.LineColor(ROOT.kBlack),RooFit.Name("ff"))
        frame.Draw()
        leg.AddEntry("fit","fit","l")
        leg.AddEntry("data","data","l")
        leg.AddEntry("pp","prompt-prompt ","l")
        leg.AddEntry("pf","prompt-fake ","l")
        if len(components)>2:
            leg.AddEntry("ff","fake-fake ","l")
        leg.Draw()
        #b.DrawLatex(0.55,0.7,"PRELIMINARY")
        self.keep([cFit])
        self.autosave(True)

    ## ------------------------------------------------------------------------------------------------------------
    def plotPurity(self,options,args):
        
        comp=3
        treetruthname=options.plotPurity["treetruth"]
        dim=options.plotPurity["dimensions"]
        categories = options.plotPurity["categories"]
        closure = options.plot_closure
        if options.fit_mc: data=False
        else:data=True
        purity_values = options.plot_purityvalue
        for opt,pu_val in zip(closure,purity_values):
            for cat in categories:
                print cat
                if data:
                    if options.pu_sigregion:
                        tree_template=self.treeData("fitresult_fraction_sigRegion_unrolled_%s_%s_%s"%(opt,dim, cat))
                        tree_templateRat=self.treeData("fitresult_fraction_RatsigRegion_unrolled_%s_%s_%s"%(opt,dim, cat))
                    else:
                        tree_template=self.treeData("fitresult_fraction_unrolled_%s_%s_%s"%(opt,dim, cat))
                    g_templatepp=ROOT.TGraphErrors(tree_template.GetEntries())
                    g_templatepf=ROOT.TGraphErrors(tree_template.GetEntries())
                    g_templateff=ROOT.TGraphErrors(tree_template.GetEntries())
                    g_ratiopp=ROOT.TGraphErrors(tree_template.GetEntries())
                    g_syspp=ROOT.TGraphErrors(tree_template.GetEntries())
                    g_syspf=ROOT.TGraphErrors(tree_template.GetEntries())
                    g_sysff=ROOT.TGraphErrors(tree_template.GetEntries())
                    nentries=tree_template.GetEntries()
                else:
                    if options.pu_sigregion:
                        tree_templatemc=self.treeData("fitresult_fraction_sigRegion_mc_unrolled_%s_%s_%s"%(opt,dim, cat))
                        tree_mctruth=self.treeData("fitresult_fraction_sigRegion_mc_unrolled_mctruth_%s_%s"%( dim, cat))
                        tree_templatemcInt=self.treeData("fitresult_fraction_RatsigRegion_mc_unrolled_%s_%s_%s"%(opt,dim, cat))
                        tree_mctruthInt=self.treeData("fitresult_fraction_RatsigRegion_mc_unrolled_mctruth_%s_%s"%( dim, cat))
                    
                    else:
                        tree_templatemc=self.treeData("fitresult_fraction_mc_unrolled_%s_%s_%s"%(opt,dim, cat))
                        tree_mctruth=self.treeData("fitresult_fraction_mc_unrolled_mctruth_%s_%s"%( dim, cat))
                    g_templateppmc=ROOT.TGraphErrors(tree_templatemc.GetEntries())
                    g_templatepfmc=ROOT.TGraphErrors(tree_templatemc.GetEntries())
                    g_templateffmc=ROOT.TGraphErrors(tree_templatemc.GetEntries())
                    nentries=tree_templatemc.GetEntries()
                if options.pu_sigregion:
                        treetruthSigname="truth_purity_signalregion"
                        tree_truthpp=self.treeData("%s_pp_%s_%s"%(treetruthSigname, dim, cat))
                        tree_truthpf=self.treeData("%s_pf_%s_%s"%(treetruthSigname,dim, cat))
                        tree_truthff=self.treeData("%s_ff_%s_%s"%(treetruthSigname,dim, cat))
                else:
                    tree_truthpp=self.treeData("%s_pp_%s_%s"%(treetruthname, dim, cat))
                    tree_truthpf=self.treeData("%s_pf_%s_%s"%(treetruthname,dim, cat))
                    tree_truthff=self.treeData("%s_ff_%s_%s"%(treetruthname,dim, cat))
                if tree_truthff!=None:
                    g_truthff=ROOT.TGraphErrors(tree_truthff.GetEntries())
                else:
                    g_truthff=ROOT.TGraphErrors()
                    print "no truth ff component"
                g_truthpp=ROOT.TGraphErrors(tree_truthpp.GetEntries())
                g_truthpf=ROOT.TGraphErrors(tree_truthpf.GetEntries())
                if not data:
                    g_mctruthpp=ROOT.TGraphErrors(tree_mctruth.GetEntries())
                    g_mctruthpp=ROOT.TGraphErrors(tree_mctruth.GetEntries())
                    g_mctruthpf=ROOT.TGraphErrors(tree_mctruth.GetEntries())
                    g_mctruthff=ROOT.TGraphErrors(tree_mctruth.GetEntries())
                    g_mctruthpf=ROOT.TGraphErrors(tree_mctruth.GetEntries())
                    g_mctruthff=ROOT.TGraphErrors(tree_mctruth.GetEntries())
                    g_pullpp=ROOT.TGraphErrors(nentries)
                    h_pullpp=ROOT.TH1F("h_pullpp_%s" % cat,"h_pullpp_%s"% cat,10*tree_truthpp.GetEntries(),-2.,2.)
                    if ((tree_truthpp.GetEntries()!=nentries)):
                        print "number of entries in trees dont agree"
                #todo as option
                tot_err=True
                if data and tot_err:
                    if cat=="EBEB": JK=[0.0139109553524065,0.00959013747087344,0.0116646894784206, 0.0117471711709682, 0.019751798623848,0.0177599723287563, 0.0147565968015614, 0.0248929144203025 ,0.0119405285994062]
                    else: JK=[0.0619663846038683,0.0321605557968228, 0.0505903441500092, 0.0664021481494921  ,0.0391118445030965]
                    
                for mb in range(0,nentries):
                    if data:
                        tree_template.GetEntry(mb)
                        if mb==nentries-1:
                            massbin=5500/2.
                            masserror=4500/2.
                        else:
                            massbin=tree_template.massbin
                            masserror=tree_template.masserror
                        pf_p=tree_template.purity_pf
                        pp_p=tree_template.purity_pp
                        ff_p=tree_template.purity_ff
                        if not tot_err:
                            pf_err=tree_template.error_pf
                            pp_err=tree_template.error_pp
                            ff_err=tree_template.error_ff
                        elif tot_err and cat=="EBEB":
                            #sys=0.0723
                            sys=0.10088
                        elif tot_err and cat=="EBEE":
                           # sys=0.14852
                            sys=0.20185
                        if tot_err:
                            if not options.pu_sigregion:
                                stat_pp=sqrt(JK[mb]*JK[mb]+tree_template.error_pp*tree_template.error_pp)
                                pp_sys=sys*sys*pp_p*pp_p
                                pp_err=sqrt(pp_sys+stat_pp*stat_pp)
                                stat_pf=sqrt(JK[mb]*JK[mb]+tree_template.error_pf*tree_template.error_pf)
                                pf_sys=sys*sys*pf_p*pf_p
                                pf_err=sqrt(pf_sys+stat_pf*stat_pf)
                                stat_ff=sqrt(JK[mb]*JK[mb]+tree_template.error_ff*tree_template.error_ff)
                                ff_sys=sys*sys*ff_p*ff_p
                                ff_err=sqrt(sys*sys*ff_p*ff_p+stat_ff*stat_ff)
                            else:
                                tree_templateRat.GetEntry(mb)
                                stat_pp=sqrt(JK[mb]*JK[mb]*tree_templateRat.ratSig_pp*tree_templateRat.ratSig_pp+tree_template.error_pp*tree_template.error_pp)
                                pp_sys=sys*sys*tree_templateRat.ratSig_pp*tree_templateRat.ratSig_pp*pp_p*pp_p     
                                stat_pf=sqrt(JK[mb]*JK[mb]*tree_templateRat.ratSig_pf*tree_templateRat.ratSig_pf+tree_template.error_pf*tree_template.error_pf)
                                pf_sys=sys*sys*tree_templateRat.ratSig_pf*tree_templateRat.ratSig_pf*pf_p*pf_p     
                                stat_ff=sqrt(JK[mb]*JK[mb]*tree_templateRat.ratSig_ff*tree_templateRat.ratSig_ff+tree_template.error_ff*tree_template.error_ff)
                                ff_sys=sys*sys*tree_templateRat.ratSig_ff*tree_templateRat.ratSig_ff*ff_p*ff_p     
                                pp_err=sqrt(pp_sys+stat_pp*stat_pp)
                                pf_err=sqrt(pf_sys+stat_pf*stat_pf)
                                ff_err=sqrt(ff_sys+stat_ff*stat_ff)
                            print "sys",sys,"pp_err", pp_err,"pp_sys", pp_sys,"pf_err", pf_err,"pf_sys", pf_sys,"ff_err",ff_err,"ff_sys",ff_sys 
                            g_syspf.SetPoint(mb,massbin,pf_p)
                            g_syspf.SetPointError(mb,masserror,sqrt(pf_sys))
                            g_syspp.SetPoint(mb,massbin,pp_p)
                            g_syspp.SetPointError(mb,masserror,sqrt(pp_sys))
                            g_sysff.SetPoint(mb,massbin,ff_p)
                            g_sysff.SetPointError(mb,masserror,sqrt(ff_sys))
                            print "sys",sys,"pp_err", pp_err,"pp_sys", pp_sys,"pf_err", pf_err,"pf_sys", pf_sys,"ff_err",ff_err,"ff_sys",ff_sys 
                        g_templatepf.SetPoint(mb,massbin,pf_p)
                        g_templatepf.SetPointError(mb,masserror,pf_err)
                        g_templatepp.SetPoint(mb,massbin,pp_p)
                        g_templatepp.SetPointError(mb,masserror,pp_err)
                        g_templateff.SetPoint(mb,massbin,ff_p)
                        g_templateff.SetPointError(mb,masserror,ff_err)
                        print mb, "pp_p", pp_p,"pf_p", pf_p,"ff_p",ff_p,"ff_err",ff_err
                    else:
                        tree_templatemc.GetEntry(mb)
                        if mb==nentries-1:
                            massbin=5500/2.
                            masserror=4500/2.
                        else:
                            massbin=tree_templatemc.massbin
                            masserror=tree_templatemc.masserror
                        g_templatepfmc.SetPoint(mb,massbin,tree_templatemc.purity_pf)
                        g_templatepfmc.SetPointError(mb,masserror,tree_templatemc.error_pf)
                        g_templateffmc.SetPoint(mb,massbin,tree_templatemc.purity_ff)
                        g_templateffmc.SetPointError(mb,masserror,tree_templatemc.error_ff)
                        g_templateppmc.SetPoint(mb,massbin,tree_templatemc.purity_pp)
                        g_templateppmc.SetPointError(mb,masserror,tree_templatemc.error_pp)
                        tree_mctruth.GetEntry(mb)
                        g_mctruthpp.SetPoint(mb,massbin,tree_mctruth.purity_pp)
                        g_mctruthpp.SetPointError(mb,masserror,tree_mctruth.error_pp)
                    tree_truthpp.GetEntry(mb)
                    tree_truthpf.GetEntry(mb)
                    if tree_truthff!=None:
                        tree_truthff.GetEntry(mb)
                    g_truthpp.SetPoint(mb,massbin,tree_truthpp.frac_pu)
                    g_truthpp.SetPointError(mb,masserror,0.)
                    g_truthpf.SetPoint(mb,massbin,tree_truthpf.frac_pu)
                    g_truthff.SetPoint(mb,massbin,tree_truthff.frac_pu)
                    g_truthpf.SetPointError(mb,masserror,0.)
                    g_truthff.SetPointError(mb,masserror,0.)
                    if options.pu_sigregion:
                        g_ratiopp.SetPoint(mb,massbin,pp_p-tree_truthpp.frac_pu)
                        g_ratiopp.SetPointError(mb,0.,pp_err)
                    else:
                        g_ratiopp.SetPoint(mb,massbin,pp_p-tree_truthpp.frac_pu)
                        g_ratiopp.SetPointError(mb,0.,pp_err)
                    
                    if not data:
                        if opt=="template_mix":
                            pullpp=(tree_templatemc.purity_pp-tree_mctruth.purity_pp)/tree_templatemc.error_pp
                        elif opt=="mctruth":
                            pullpp=(tree_mctruth.purity_pp-tree_truthpp.frac_pu)/tree_mctruth.error_pp
                        else:print "dont know what to compare to truth"
                        g_pullpp.SetPoint(mb,massbin,pullpp)
                        h_pullpp.Fill(pullpp)
                        g_mctruthpf.SetPoint(mb,massbin,tree_mctruth.purity_pf)
                        g_mctruthff.SetPoint(mb,massbin,tree_mctruth.purity_ff)
                        g_mctruthpf.SetPointError(mb,masserror,tree_mctruth.error_pf)
                        g_mctruthff.SetPointError(mb,masserror,tree_mctruth.error_ff)
                if not data:
                    if options.pu_sigregion:  self.plotClosure(cat,pu_val,opt,"sigRegionMC",g_templateppmc,g_templatepfmc,g_templateffmc,g_pullpp,g_mctruthpp,g_mctruthpf,g_mctruthff)
                    else: self.plotClosure(cat,pu_val,opt,"fullRegionMC",g_templateppmc,g_templatepfmc,g_templateffmc,g_pullpp,g_mctruthpp,g_mctruthpf,g_mctruthff)
                else:
                    if options.pu_sigregion:
                        self.plotPurityMassbins(cat,pu_val,opt,"sigRegionData",g_templatepp,g_templatepf,g_templateff,g_syspp,g_syspf,g_sysff)
                        self.plotPurityMassbins(cat,pu_val,opt,"sigRegionData+MC",g_templatepp,g_templatepf,g_templateff,g_mctruthpp=g_truthpp,g_mctruthpf=g_truthpf,g_mctruthff=g_truthff,g_ratiopp=g_ratiopp)
                        self.plotPurityMassbins(cat,pu_val,opt,"sigRegionData+MC+sys",g_templatepp,g_templatepf,g_templateff,g_syspp,g_syspf,g_sysff,g_truthpp,g_truthpf,g_truthff,g_ratiopp)
                    else: 
                        self.plotPurityMassbins(cat,pu_val,opt,"data+sys",g_templatepp,g_templatepf,g_templateff,g_syspp,g_syspf,g_sysff)
                        self.plotPurityMassbins(cat,pu_val,opt,"data+MC",g_templatepp,g_templatepf,g_templateff,g_mctruthpp=g_truthpp,g_mctruthpf=g_truthpf,g_mctruthff=g_truthff,g_ratiopp=g_ratiopp)
                        self.plotPurityMassbins(cat,pu_val,opt,"data+MC+sys",g_templatepp,g_templatepf,g_templateff,g_syspp,g_syspf,g_sysff,g_truthpp,g_truthpf,g_truthff,g_ratiopp)
            ## ------------------------------------------------------------------------------------------------------------
    def pullFunction(self,g_pull,h_pull,cat,comp,opt,pu_val):
        leg = ROOT.TLegend(0.5,0.8,0.9,0.9)
        print "cpull_%s_%s" % (comp,cat)
        cpull = ROOT.TCanvas("cpull_for%s_%s_%s_%s" % (opt,comp,cat,pu_val),"cpull_for%s_%s_%s_%s" % (opt,comp,cat,pu_val))
        cpull.Divide(1,2)
        cpull.cd(1)
        ROOT.gPad.SetPad(0., 0.5, 1., 1.0)
        ROOT.gStyle.SetOptFit(1)
        cpull.cd(2)
        ROOT.gPad.SetPad(0.,  0., 1., 0.5)
        ROOT.gPad.SetGridx()
        ROOT.gPad.SetGridy()
        ROOT.gPad.SetLogx()
        cpull.cd(1)
        fitgauss=ROOT.TF1("fitgauss","gaus",-5.,5.)
        h_pull.GetXaxis().SetTitle("(pu_tp-pu_mctruth)/pu_tperr")
        h_pull.Fit("fitgauss","L ");
        h_pull.Draw("HIST") 
        fitgauss.Draw("SAME")
        cpull.cd(2)
        g_pull.SetMarkerStyle(20)
        g_pull.GetYaxis().SetRangeUser(-5.,5.)
        g_pull.GetXaxis().SetRangeUser(0.,13000.)
        g_pull.GetXaxis().SetTitle("Diphoton mass [GeV]")
        g_pull.GetYaxis().SetTitle("(pu_tp-pu_mctruth)/pu_tperr")
        g_pull.Draw("AP")
        self.keep( [cpull] )
        self.autosave(True)
    ## ------------------------------------------------------------------------------------------------------------
    def addCmsLumi(canv,period,pos,extraText=None):
        if extraText:
                ROOT.writeExtraText = True
                if type(extraText) == str and extraText != "":
                        ROOT.extraText = extraText
                ROOT.CMS_lumi(canv,period,pos)

    ## ------------------------------------------------------------------------------------------------------------
    def plotClosure(self,cat,pu_val,opt,opt2,g_templatepp=None,g_templatepf=None,g_templateff=None,g_ratiopp=None,g_mctruthpp=None,g_mctruthpf=None,g_mctruthff=None):
        leg = ROOT.TLegend(0.6,0.6,0.8,0.9)
        basicStyle = [["SetMarkerSize",1.3],["SetLineWidth",2],
        ["SetTitle",";m_{#gamma #gamma} (GeV);purity"]]
        mc_expectedStyle = basicStyle+ [["SetMarkerStyle",ROOT.kFullTriangleUp]]
        mctruth_expectedStyle = basicStyle+ [["SetMarkerStyle",ROOT.kOpenCircle],["SetLineStyle",ROOT.kDashed]]
        cpu = ROOT.TCanvas("cpu_%s_%s_%s_%s" % (opt,cat,pu_val,opt2),"cpu_%s_%s_%s_%s" %(opt,cat,pu_val,opt2))
        cpu.Divide(1,2)
        cpu.cd(1)
        ROOT.gPad.SetPad(0., 0.3, 1., 1.0)
        ROOT.gPad.SetLogx()
        ROOT.gPad.SetGridx()
        ROOT.gPad.SetTicky()
        ROOT.gPad.SetGridy()
        cpu.cd(2)
        ROOT.gPad.SetPad(0., 0., 1., 0.3)
        ROOT.gPad.SetTicky()
        ROOT.gPad.SetGridx()
        ROOT.gPad.SetGridy()
        ROOT.gPad.SetLogx()
        cpu.cd(1)
        style_utils.apply(g_templatepp, [["colors",ROOT.kRed]]+mc_expectedStyle)
        style_utils.apply(g_mctruthpp, [["colors",ROOT.kRed]]+mctruth_expectedStyle )
        style_utils.apply(g_templatepf, [["colors",ROOT.kBlue]]+mc_expectedStyle )
        style_utils.apply(g_mctruthpf, [["colors",ROOT.kBlue]]+mctruth_expectedStyle )
        style_utils.apply(g_templateff, [["colors",ROOT.kBlack]]+mc_expectedStyle )
        style_utils.apply(g_mctruthff, [["colors",ROOT.kBlack]]+mctruth_expectedStyle )
        g_templatepp.GetYaxis().SetRangeUser(-0.1,1.7)
        g_templatepp.GetXaxis().SetLimits(200.,5000.)
        g_templatepp.Draw("AP")
        g_templatepp.GetXaxis().SetMoreLogLabels()
        g_mctruthpf.Draw("P SAME")
        g_mctruthpp.Draw("P SAME")
        g_mctruthff.Draw("P SAME")
        g_templatepf.Draw("P SAME")
        g_templateff.Draw("P SAME")
        leg.AddEntry(g_mctruthpp,"#gamma #gamma MC truth","lp")  
        leg.AddEntry(g_mctruthpf,"#gamma j MC truth","lp")
        leg.AddEntry(g_mctruthff,"j j MC truth","lp")
        leg.AddEntry(g_templatepp,"#gamma #gamma  MC","lp")  
        leg.AddEntry(g_templatepf,"#gamma j  MC","lp")
        leg.AddEntry(g_templateff,"j j  MC","lp")

        leg.Draw()
        g_ratiopp.SetMarkerStyle(20)
        cpu.cd(2)
        style_utils.apply(g_ratiopp, [["colors",ROOT.kRed]]+mc_expectedStyle )
        g_ratiopp.GetYaxis().SetTitle("pull fct")
        g_ratiopp.GetYaxis().SetRangeUser(-3.,3.)
        g_ratiopp.GetYaxis().SetTitleSize( g_mctruthpp.GetYaxis().GetTitleSize() *7./3. )
        g_ratiopp.GetYaxis().SetLabelSize( g_mctruthpp.GetYaxis().GetLabelSize()*7./3.  )
        g_ratiopp.GetYaxis().SetTitleOffset(g_mctruthpp.GetYaxis().GetTitleOffset()*7/3. )
        g_ratiopp.GetXaxis().SetTitleSize( g_mctruthpp.GetXaxis().GetTitleSize() *7./3. )
        g_ratiopp.GetXaxis().SetLabelSize( g_mctruthpp.GetXaxis().GetLabelSize()*7./4. )
        g_ratiopp.GetXaxis().SetLimits(200.,5000.)
        g_ratiopp.GetXaxis().SetMoreLogLabels()
        g_ratiopp.Draw("AP")
        self.keep( [g_mctruthpp,g_ratiopp] )
        self.keep( [g_mctruthpp,g_mctruthpf, g_mctruthff] )
        self.keep( [cpu,g_templatepp,g_templatepf,g_templateff] )
        self.autosave(True)

    ## ------------------------------------------------------------------------------------------------------------
    def plotPurityMassbins(self,cat,pu_val,opt,opt2,g_templatepp=None,g_templatepf=None,g_templateff=None,g_syspp=None,g_syspf=None,g_sysff=None,g_mctruthpp=None,g_mctruthpf=None,g_mctruthff=None,g_ratiopp=None):
        leg = ROOT.TLegend(0.6,0.6,0.8,0.9)
        basicStyle = [["SetMarkerSize",1.3],["SetLineWidth",4],
        ["SetTitle",";m_{#gamma #gamma} (GeV);purity"]]
        data_expectedStyle = basicStyle+ [["SetMarkerStyle",ROOT.kFullTriangleUp],["SetLineStyle",1]]
        mctruth_expectedStyle = basicStyle+ [["SetMarkerStyle",ROOT.kOpenCircle],["SetLineStyle",ROOT.kDashed]]
        cpu = ROOT.TCanvas("cpu_%s_%s_%s_%s" % (opt,cat,pu_val,opt2),"cpu_%s_%s_%s_%s" %(opt,cat,pu_val,opt2))
        if g_mctruthpp:
                cpu.Divide(1,2)
                cpu.cd(1)
                ROOT.gPad.SetPad(0., 0.3, 1., 1.0)
                cpu.cd(2)
                ROOT.gPad.SetPad(0., 0., 1., 0.3)
                ROOT.gPad.SetTicky()
                ROOT.gPad.SetGridx()
                ROOT.gPad.SetGridy()
                ROOT.gPad.SetLogx()
        cpu.cd(1)
        ROOT.gPad.SetTicky()
        ROOT.gPad.SetGridx()
        ROOT.gPad.SetGridy()
        ROOT.gPad.SetLogx()
        style_utils.apply(g_templatepp, [["colors",ROOT.kRed]]+data_expectedStyle)
        style_utils.apply(g_templatepf, [["colors",ROOT.kBlue]]+data_expectedStyle )
        style_utils.apply(g_templateff, [["colors",ROOT.kBlack]]+data_expectedStyle )
        if g_mctruthpp:
                style_utils.apply(g_mctruthpp, [["colors",ROOT.kRed+2]]+mctruth_expectedStyle)
                style_utils.apply(g_mctruthpf, [["colors",ROOT.kBlue+2]]+mctruth_expectedStyle )
                style_utils.apply(g_mctruthff, [["colors",ROOT.kBlack]]+mctruth_expectedStyle )
        if g_syspp:
            style_utils.apply(g_syspp, [["SetFillStyle",3002],["colors",ROOT.kRed]]+data_expectedStyle )
            style_utils.apply(g_syspf, [["SetFillStyle",3002],["colors",ROOT.kBlue]]+data_expectedStyle )
            style_utils.apply(g_sysff, [["SetFillStyle",3002],["colors",ROOT.kBlack]]+data_expectedStyle )
        g_templatepp.GetYaxis().SetRangeUser(0.,1.75)
        g_templatepp.GetXaxis().SetLimits(200.,5000.)
        g_templatepp.Draw("AP")
        g_templatepf.Draw("P SAME")
        g_templateff.Draw("P SAME")
        if g_syspp:
            g_syspp.Draw("E2 SAME")
            g_syspf.Draw("E2 SAME")
            g_sysff.Draw("E2 SAME")
        if g_mctruthpp:
                g_mctruthpp.Draw("p SAME")
                g_mctruthpf.Draw("p SAME")
                g_mctruthff.Draw("p SAME")
                leg.AddEntry(g_mctruthpp,"#gamma #gamma MC truth","lp")  
                leg.AddEntry(g_mctruthpf,"#gamma j MC truth","lp")
                leg.AddEntry(g_mctruthff,"j j MC truth","lp")
        g_templatepp.GetXaxis().SetMoreLogLabels()
        leg.AddEntry(g_templatepp,"#gamma #gamma data","lp")  
        leg.AddEntry(g_templatepf,"#gamma j data","lp")
        leg.AddEntry(g_templateff,"j j data","lp")
        leg.Draw()
        if g_ratiopp:
                cpu.cd(2)
                style_utils.apply(g_ratiopp, [["colors",ROOT.kRed]]+data_expectedStyle )
                g_ratiopp.GetYaxis().SetTitle("difference")
                g_ratiopp.GetYaxis().SetRangeUser(-1,1)
                g_ratiopp.GetYaxis().SetTitleSize( g_mctruthpp.GetYaxis().GetTitleSize() *6./3. )
                g_ratiopp.GetYaxis().SetLabelSize( g_mctruthpp.GetYaxis().GetLabelSize()*6./3.  )
                g_ratiopp.GetYaxis().SetTitleOffset(g_mctruthpp.GetYaxis().GetTitleOffset()*6/3. )
                g_ratiopp.GetXaxis().SetTitleSize( g_mctruthpp.GetXaxis().GetTitleSize() *6./3. )
                g_ratiopp.GetXaxis().SetLabelSize( g_mctruthpp.GetXaxis().GetLabelSize()*6./3. )
                g_ratiopp.GetXaxis().SetLimits(200.,5000.)
                g_ratiopp.GetXaxis().SetMoreLogLabels()
                g_ratiopp.Draw("AP")
                self.keep( [g_ratiopp] )
        self.keep( [cpu,g_templatepp,g_templatepf,g_templateff] )
        self.autosave(True)

    ## ------------------------------------------------------------------------------------------------------------
    
    def Jackknife(self,options,args):
        fout = self.openOut(options)
        fout.Print()
        fout.cd()
    
    def Jackknife(self,options,args):
        fout = self.openOut(options)
        fout.Print()
        fout.cd()
        fit=options.fits["2D"]
        for cat in options.jackknife.get("categories", fit["categories"]):
            isoargs=ROOT.RooArgSet("isoargs")
            setargs=ROOT.RooArgSet("setargs")
            massargs=ROOT.RooArgSet("massargs")
            mass_var,mass_b=self.getVar(options.jackknife.get("mass_binning"))
            mass=self.buildRooVar(mass_var,mass_b,recycle=True)
            massargs.add(mass)
            if len(options.template_binning) > 0:
                template_binning = array.array('d',options.template_binning)
            else:
                template_binning = array.array('d',options.jackknife.get("template_binning"))
            templatebins=ROOT.RooBinning(len(template_binning)-1,template_binning,"templatebins" )
            for idim in range(fit["ndim"]):
                isoargs.add(self.buildRooVar("templateNdim%dDim%d" % ( fit["ndim"],idim),template_binning,recycle=True))
            setargs.add(isoargs)
            setargs.add(massargs)
            dset_data = self.reducedRooData("data_2D_%s" % (cat),massargs)
            dset_data.Print()
            if not options.fixed_massbins:
                mass_split= [int(x) for x in options.fit_massbins]
                diphomass=self.massquantiles(dset_data,massargs,mass_b,mass_split)
                massrange=[mass_split[2],mass_split[1]]
            elif options.fixed_massbins and cat=="EBEB":
                diphomass=[200.0,216.187076923,230.0,253.415384615,281.651965812,295.277948718,332.332307692,408.787692308,500.0,12999.0]
                massrange=[0,len(diphomass)-1]
            elif options.fixed_massbins and cat=="EBEE":
                diphomass=[299.446153846,320.0,355.459828644,443.85640967,500.0,12999.0153846]
                massrange=[0,len(diphomass)-1]
            for mb in range(massrange[0],massrange[1]):
                massbin=(diphomass[mb]+diphomass[mb+1])/2.
                masserror=(diphomass[mb+1]-diphomass[mb])/2. 
                cut=ROOT.TCut("mass>%f && mass<%f"% (diphomass[mb],diphomass[mb+1]))
                cut_s= "%1.0f_%2.0f"% (diphomass[mb],diphomass[mb+1])
                print cut.GetTitle()
                for comp in options.jackknife.get("components",fit["components"]) :
                    name="%s_%s_%s" %(comp,cat,cut_s)
                    print name
                    #if comp=="pf"
                    #    full_temp = self.reducedRooData( "template_mix_%s_kDSinglePho2D_%s" % (comp,cat),setargs,redo=True)
                    #    full_temp.SetName("template_mix_%s_2D_%s" % (comp,cat))
                    #    full_template =self.masscutTemplates(full_temp,cut,cut_s,"%s"% (full_temp.GetName()))
                    
                   # print full_template
                   # full_hist=self.histounroll([full_template],template_binning,isoargs,comp,cat,cut_s,True,min(template_binning),max(template_binning),extra_shape_unc=options.extra_shape_unc,plot=False)

                    #TODO get number of pseudosamples more elegant
                    #if options.verbose:
                    #    c1=ROOT.TCanvas("c1_%s"%name,"c1_%s"%name)
                    #    full_hist[0].Draw()
                    #    self.keep(c1)

                    temps_all = []
                    temps = []
                    if not comp=="pp":
                        jks=int(options.jackknife.get("jk_source"))
                        jkt=int(options.jackknife.get("jk_target"))
                        print "jks ",jks, " jkt ", jkt
                        for s in range(jks):
                            temp = self.reducedRooData( "template_mix_%s_%i_kDSinglePho2D_%s" % (comp,s,cat),setargs,redo=True)
                            temp.SetName("template_mix_%s_%i_2D_%s" % (comp,s,cat))
                            temps_all.append(temp)
                        for t in range(jkt):
                            temp = self.reducedRooData( "template_mix_%s_kDSinglePho2D_%i_%s" % (comp,t,cat),setargs,redo=True)
                            temp.SetName("template_mix_%s_2D_%i_%s" % (comp,t,cat))
                            temps_all.append(temp)
                        print temp
                    else:
                        jkp=int(options.jackknife.get("jk_pp"))
                        for s in range(jkp):
                            temp = self.reducedRooData( "template_%s_%i_2D_%s" % (comp,s,cat),setargs,redo=True)
                            temp.SetName("template_%s_%i_2D_%s" % (comp,s,cat))
                            temps_all.append(temp)
                    print temps_all
                    for template in temps_all:
                        template_massc =self.masscutTemplates(template,cut,cut_s,"%s"% (template.GetName()))
                        temps.append(template_massc)
                    print "number of pseudo samples", len(temps)
                    print temps
                    hists=self.histounroll(temps,template_binning,isoargs,comp,cat,cut_s,True,min(template_binning),max(template_binning),extra_shape_unc=options.extra_shape_unc,plot=False)
                    if options.verbose:
                        c12=ROOT.TCanvas("c12_1%s"%name,"c12_1%s"%name)
                        hists[1].Draw()
                        self.keep(c12)
                        self.autosave(True)
                    #self.varJK(self.options,full_hist,hists,name)
        self.saveWs(options,fout)
    
    ## ------------------------------------------------------------------------------------------------------------
    def plotJKpurity(self,options,cat,dim,tps,jkID="jk"):
        jks=int(options.jackknife.get("jk_source"))
        jkt=int(options.jackknife.get("jk_target"))
      #   tree_fitresult_fraction_unrolled_template_mix_2D_EBEB
        nom_tree =self.treeData("fitresult_fraction_unrolled_template_mix_2D_%s" % (cat))
        nentries= nom_tree.GetEntries()
        g_purity=ROOT.TGraphErrors(nentries*len(tps))
        g_ratio=ROOT.TGraphErrors(nentries)
        g_puerr=ROOT.TGraphErrors(nentries)
        g_puratio=ROOT.TGraphErrors(nentries)
        histos=[]
        for mb in range(nentries):
            h_p=ROOT.TH1D("h_p%s_%s_%i"%(jkID,cat,mb),"h_p%s_%s_%i"%(jkID,cat,mb),50*len(tps),.3,1.)
            histos.append(h_p)
        g_purity.GetXaxis().SetTitle("Diphoton mass [GeV]")
        g_purity.GetYaxis().SetTitle("purity")
        i=1
        for tree_template in tps:
            for mb in range(nentries):
                tree_template.GetEntry(mb)
                histos[mb].Fill(tree_template.purity_pp)
                massbin=tree_template.massbin
                masserror=tree_template.masserror
                if mb==nentries-1:
                    massbin=5500/2.
                    masserror=4500/2.
                else:
                    massbin=tree_template.massbin
                    masserror=tree_template.masserror
                g_purity.SetPoint(mb+i,massbin,tree_template.purity_pp)
                g_purity.SetPointError(mb+i,masserror,tree_template.error_pp)
            i=i+nentries
        
        for mb in range(nentries):
            cmb = ROOT.TCanvas("cJK%s_%s_%i" % (jkID,cat,mb),"cJK%s_%s_%i" % (jkID,cat,mb))
            cmb.cd()
            histos[mb].Draw("HIST E2")
            rms=histos[mb].GetRMS()*(len(tps)-1)/sqrt(len(tps))
            histos[mb].GetXaxis().SetTitle("Diphoton mass %i"%mb)
            histos[mb].GetXaxis().SetLimits( histos[mb].GetMean()-3*histos[mb].GetRMS(),histos[mb].GetMean()+3*histos[mb].GetRMS())
            self.keep( [cmb,histos[mb]] )
            nom_tree.GetEntry(mb)
            if mb==nentries-1:
                massbin=5500/2.
                masserror=4500/2.
            else:
                massbin=nom_tree.massbin
                masserror=nom_tree.masserror
            g_ratio.SetPoint(mb+1,massbin,nom_tree.purity_pp)
            g_ratio.SetPointError(mb+1,masserror,rms)
            g_puratio.SetPoint(mb+1,massbin,0.)
            g_puerr.SetPoint(mb+1,massbin,nom_tree.purity_pp)
            err=sqrt(pow(rms,2)+pow(nom_tree.error_pp,2) )
            print mb, rms, err
            g_puerr.SetPointError(mb+1,masserror,err)
            g_puratio.SetPointError(mb+1,masserror,err/nom_tree.purity_pp)
        cpurity = ROOT.TCanvas("cpurity%s_%s" % (jkID,cat),"cpurity%s_%s" % (jkID,cat))
        cpurity.Divide(1,2)
        cpurity.cd(1)
        ROOT.gPad.SetGridx()
        ROOT.gPad.SetGridy()
        ROOT.gPad.SetPad(0., 0.5, 1., 1.0)
        ROOT.gPad.SetLogx()
        g_purity.SetMarkerSize(1.3)
        g_purity.SetMarkerStyle(20)
        g_purity.GetYaxis().SetTitle("JK purities")
        g_purity.Draw("AP")
        g_purity.GetXaxis().SetLimits(200.,5000.)
        cpurity.cd(2)
        ROOT.gPad.SetPad(0., 0., 1., 0.5)
        ROOT.gPad.SetLogx()
        ROOT.gPad.SetGridy()
        g_ratio.SetMarkerSize(1.0)
        g_ratio.SetMarkerStyle(20)
        g_ratio.Draw("AP" )
        g_ratio.GetXaxis().SetTitle("Diphoton mass [GeV]")
        g_ratio.GetYaxis().SetTitleSize( g_purity.GetYaxis().GetTitleSize() )
        g_ratio.GetYaxis().SetLabelSize( g_purity.GetYaxis().GetLabelSize()  )
        g_ratio.GetYaxis().SetTitleOffset(g_purity.GetYaxis().GetTitleOffset() )
        g_ratio.GetXaxis().SetTitleSize( g_purity.GetXaxis().GetTitleSize()  )
        g_ratio.GetXaxis().SetLabelSize( g_purity.GetXaxis().GetLabelSize() )
        g_ratio.GetYaxis().SetTitle("purity + JK stat error")
        g_ratio.GetXaxis().SetLimits(200.,5000.)
        g_ratio.GetYaxis().SetRangeUser(0.4,1.)
        
        #draw whole nominal purity with JK
        cpu = ROOT.TCanvas("cpurity_%s" % (cat),"cpurity_%s" % (cat))
        cpu.Divide(1,2)
        cpu.cd(1)
        ROOT.gPad.SetGridx()
        ROOT.gPad.SetGridy()
        leg =ROOT.TLegend(0.4,0.2,0.7,0.5)
        leg.SetFillColor(ROOT.kWhite)
        leg.AddEntry(g_puerr,"full stat. error","l")
        leg.AddEntry(g_ratio,"JK error","l")
        ROOT.gPad.SetPad(0., 0.4, 1., 1.0)
        ROOT.gPad.SetLogx()
        g_puerr.SetMarkerSize(1.3)
        g_puerr.SetMarkerStyle(20)
        g_puerr.SetLineWidth(2)
        g_ratio.SetLineWidth(2)
        g_ratio.SetMarkerColor(ROOT.kRed+1)
        g_ratio.SetLineColor(g_ratio.GetMarkerColor())
        g_puerr.Draw("AP")
        g_puerr.GetXaxis().SetTitle("Diphoton mass [GeV]")
        g_ratio.Draw("P SAME")
        leg.Draw()
        g_puerr.GetXaxis().SetLimits(200.,5000.)
        cpu.cd(2)
        ROOT.gPad.SetGridy()
        ROOT.gPad.SetPad(0., 0., 1., 0.4)
        ROOT.gPad.SetLogx()
        g_puratio.SetMarkerSize(1.3)
        g_puratio.SetMarkerStyle(20)
        g_puratio.Draw("AP")
        g_puratio.GetXaxis().SetTitle("Diphoton mass [GeV]")
        g_puratio.GetYaxis().SetTitle("fullerr/purity")
        g_puratio.GetXaxis().SetLimits(200.,5000.)
        g_puratio.GetYaxis().SetRangeUser(-0.3,0.3)
        g_puratio.GetYaxis().SetTitleSize( g_puratio.GetYaxis().GetTitleSize() *6./4. )
        g_puratio.GetYaxis().SetLabelSize( g_puratio.GetYaxis().GetLabelSize()*6./4.  )
        g_puratio.GetYaxis().SetTitleOffset(g_puratio.GetYaxis().GetTitleOffset()*4./6. )
        g_puratio.GetXaxis().SetTitleSize( g_puratio.GetXaxis().GetTitleSize() *6./4. )
        g_puratio.GetXaxis().SetLabelSize( g_puratio.GetXaxis().GetLabelSize()*6./4. )
        
       #TODO new function for last plot to add JKpp and JKpf error 
        self.keep( [cpu,cpurity,g_purity,g_ratio] )
        self.autosave(True)
        
    ## ------------------------------------------------------------------------------------------------------------
    def varJK(self,options,full_hist,hists,name):
        ROOT.TH1D.SetDefaultSumw2(True)
        num_bins=full_hist[0].GetNbinsX()
        ntuple_rms = ROOT.TNtuple("tree_rms_%s" % (name),"tree_rms_%s" % (name),"rms_bin" )
        self.store_[ntuple_rms.GetName()] =ntuple_rms
        hist_diffHigh=ROOT.TH2D("hdiffHigh_%s"% (name),"Variance for %s of the difference of # entries between two bins, bin_{X-(X+1)}"%name,num_bins,0.,num_bins,num_bins,0.,num_bins)
        hist_var=ROOT.TH1D("hvar_%s"% (name),"hvar_%s"% (name),num_bins,0.0,num_bins)
        var_bins=[] #goal to have all differences in an array
        bincont_bins=[]
        #2d array with:y columns,x rows
        diffHigh = [ [  [] for col in range(num_bins)] for row in range(num_bins)]
        mean_dHigh = [ [ 0. for col in range(num_bins)] for row in range(num_bins)]
        for bin in range(1,num_bins+1):
           #get #of entries for current bin
            mean_bin=0
            for hist in hists:
                if full_hist[0].GetBinContent(bin)!=0:
                    bincont=hist.GetBinContent(bin)/full_hist[0].GetBinContent(bin)
                    mean_bin=mean_bin+bincont
                    bincont_bins.append(bincont)
                else:
                    bincont_bins.append()
                #get difference of # entries between current and next bin to calculate RMS for this value (diffHigh)
                for i in range (1,num_bins+1):
                    if full_hist[0].GetBinContent(i) >0.:
                        diffHigh[bin-1][i-1].append(bincont - hist.GetBinContent(i)/full_hist[0].GetBinContent(i))
            #get variance for #of entries for each bin
            #get rms for #of entries for current bin
            mean_bin=mean_bin/len(hists)
            n=0
            for j in range(len(hists)):
                n=n+pow((mean_bin-bincont_bins[j]),2)
            var_rmsbin=(len(hists)-1)/float(len(hists))*n
            hist_var.SetBinContent(bin,var_rmsbin)
            ntuple_rms.Fill(var_rmsbin)
        #draw variance of #of entries for each bin
        canv = ROOT.TCanvas("cvar_%s" % (name),"cvar_%s"% (name) )
        canv.cd()
        hist_var.Draw("HIST E2")
        ROOT.gStyle.SetOptStat(111111)
        hist_var.GetXaxis().SetTitle("bin_{JK}/bin_{full_dataset}") 
        hist_var.GetYaxis().SetTitle("variance of # entries per bin") 
        hist_var.SetTitle("Var for %s ChIso" %(name) )
        self.keep( [canv] )



       #get mean for diffHigh of each bin difference
        for row in range(num_bins):
            for col in range(num_bins):
                add_hists=0
                for j in diffHigh[row][col]:
                    add_hists=j+add_hists
                mean_dHigh[row][col]=add_hists/len(hists)

        #calculate variance for this diffHigh~ (RMS-currentdiffHigh)
        for row in range(num_bins):
            for col in range(num_bins):
                var_diff=0
                for j in range(len(hists)):
                   var_diff=var_diff+pow((mean_dHigh[row][col]-diffHigh[row][col][j]),2)
                var_rmsbin=(len(hists)-1)/float(len(hists))*var_diff
                print "mean ", mean_dHigh[row][col], "var_diff", var_diff, "row",row+1 ,"col", col+1
                hist_diffHigh.SetBinContent(row+1,col+1,var_diff)
       #plot variance for each bin difference from bin1 - binX
        cHigh=ROOT.TCanvas("chigh_%s"% name,"chigh_%s"% name)
        hist_diffHigh.GetXaxis().SetTitle("bin X") 
        hist_diffHigh.GetYaxis().SetTitle("bin X+1") 
        cHigh.cd()
        hist_diffHigh.Draw("colz")
        hist_diffHigh.SetTitle("Variance for %s difference of # entries between two bins, bin_{X-(X+1)}" %(name) )
        self.keep([cHigh])
        self.autosave(True)


    ## ------------------------------------------------------------------------------------------------------------
    

# -----------------------------------------------------------------------------------------------------------
# actual main
if __name__ == "__main__":
    app = TemplatesFitApp()
    app.run()



