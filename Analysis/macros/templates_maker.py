#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, json
from pprint import pprint
import array

from getpass import getuser

## ----------------------------------------------------------------------------------------------------------------------------------------
## TemplatesApp class
## ----------------------------------------------------------------------------------------------------------------------------------------

## ----------------------------------------------------------------------------------------------------------------------------------------
class TemplatesApp(PlotApp):
    """
    Class to handle template fitting.
    Takes care of preparing templates starting from TTrees.
    Inherith from PyRapp and PlotApp classes.
    """
    
    ## ------------------------------------------------------------------------------------------------------------
    def __init__(self,option_list=[],option_groups=[]):
        """ 
        Constructor
        """
        super(TemplatesApp,self).__init__(option_groups=[
                ( "Fit definition options. Usually specified through JSON configuration (see templates_maker.json for details)", [
                        make_option("--preselection",dest="preselection",action="store",type="string",
                                    default="",
                                    help="Preselection cuts to be applied."),
                        make_option("--selection",dest="selection",action="store",type="string",
                                    default="",
                                    help="(Di-)Photon selection to be used for analysis. In dataset definition it replaces '%(sel)s'."),                
                        make_option("--aliases",dest="aliases",action="callback",type="string",callback=optpars_utils.ScratchAppend(),
                                    default=[],
                                    help="List of aliases to be defined for each tree. They can be used for selection or variable definition"),
                        make_option("--fits",dest="fits",action="callback",callback=optpars_utils.Load(),type="string",
                                    default={},help="List of templates fits to be performed. Categories, componentd and templates can be specified."),
                        make_option("--mix",dest="mix",action="callback",callback=optpars_utils.Load(),type="string",
                                    default={},help="Configuration for event mixing."),
                        make_option("--skip-templates",dest="skip_templates",action="store_true",
                                    default=False,help="Skip templates generation (even if not reading back from ws)"),
                        make_option("--dataset-variables",dest="dataset_variables",action="callback",callback=optpars_utils.ScratchAppend(),type="string",
                                    default=[],help="List of variables to be added to dataets."),
                        make_option("--weight-expression",dest="weight_expression",action="store",type="string",
                                    default="",help="Expression used to define datasets weight."),
                        ]
                  ), ("General templates preparation options", [
                        make_option("--compare-templates",dest="compare_templates",action="store_true",default=False,
                                    help="Make templates comparison plots",
                                    ),
                        make_option("--prepare-truth-fit",dest="prepare_truth_fit",action="store_true",default=False,
                                    help="Prepare fit using MC truth templates",
                                    ),
                        make_option("--prepare-nominal-fit",dest="prepare_nominal_fit",action="store_true",default=False,
                                    help="Prepare fit using nominal templates.",
                                    ),
                        make_option("--do-reweight",dest="do_reweight",action="store_true",default=False,
                                    help="Reweight templates to data.",
                                    ),
                        make_option("--reweight-variables",dest="reweight_variables",action="callback",callback=optpars_utils.ScratchAppend(),
                                    default=[],
                                    help="List of variables to be used for reweighting.",
                                    ),
                        make_option("--mix-templates",dest="mix_templates",action="store_true",
                                    default=False,
                                    help="Mix templates.",
                                    ),
                        make_option("--read-ws","-r",dest="read_ws",action="store",type="string",
                                    default=False,
                                    help="List of variables to be used for reweighting.",
                                    ),
                        make_option("--output-file","-o",dest="output_file",action="store",type="string",
                                    default=None,
                                    help="Output file.",
                                    ),
                        make_option("--store-new-only",dest="store_new_only",action="store_true",
                                    default=False,
                                    help="Only store new objects in output file.",
                                    ),
                        make_option("--mc-file",dest="mc_file",action="store",type="string",
                                    default=None,help="default: %default"),
                        make_option("--only-subset",dest="only_subset",action="callback",type="string", callback=optpars_utils.ScratchAppend(),
                                    default=[],help="default: %default"),
                        
                        ]
                      )
            ]+option_groups,option_list=option_list)
        
        ## initialize data members
        self.trees_ = {}
        self.datasets_ = {}
        self.aliases_ = {}
        self.variables_ = {}
        self.cache_ = {}
        self.store_ = {}
        self.rename_ = False
        self.store_new_ = False

        ## load ROOT (and libraries)
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils
        ROOT.gSystem.Load("libdiphotonsUtils")
         
        ROOT.gStyle.SetOptStat(111111)

    ## ------------------------------------------------------------------------------------------------------------
    def setup(self,options,args):
        """ 
        Read input trees and generate new datasets/trees if required
        """

        if len(options.only_subset)>0:
            subset = {}
            for name,fit in options.fits.iteritems():
                if not name in options.only_subset:
                    continue
                subset[name] = fit
            options.fits = subset

        if options.store_new_only:
            self.store_new_ = True

        if not options.output_file:
            if options.read_ws: 
                options.output_file = options.read_ws
                if options.store_new_only:
                    options.output_file = options.output_file.replace(".root","_new.root")
            else : 
                options.output_file = "templates.root"
        
        if options.read_ws:
            self.readWs(options,args)
        elif not options.skip_templates:
            self.prepareTemplates(options,args)


    ## ------------------------------------------------------------------------------------------------------------
    def __call__(self,options,args):
        """ 
        Main method. Called automatically by PyRoot class.
        """
        ## load ROOT style
        self.loadRootStyle()
        from ROOT import RooFit
        ROOT.gStyle.SetOptStat(111111)
        printLevel = ROOT.RooMsgService.instance().globalKillBelow()
        ROOT.RooMsgService.instance().setGlobalKillBelow(RooFit.FATAL)
        
        self.setup(options,args)
        
        if options.mix_templates:
            self.mixTemplates(options,args)
            
        if options.compare_templates:
            self.compareTemplates(options,args)
            
        if options.prepare_truth_fit:
            self.prepareTruthFit(options,args)
        
        if options.prepare_nominal_fit:
            self.prepareNominalFit(options,args)
        

    ## ------------------------------------------------------------------------------------------------------------
    def openOut(self,options):
        if options.read_ws and options.output_file == options.read_ws:
            name    = options.output_file
            tmpname = name.replace(".root","_tmp.root")
            fout    = self.open(tmpname,"recreate")
            self.rename_  = (tmpname,name)
        else:
            fout = self.open(options.output_file,"recreate")
        return fout

    ## ------------------------------------------------------------------------------------------------------------
    def saveWs(self,options,fout=None):            
        if not fout:
            fout = self.openOut(options)
        fout.cd()
        cfg = { "fits"   : options.fits,
                "stored" : self.store_.keys() 
                }
        
        ROOT.TObjString( json.dumps( cfg,indent=4,sort_keys=True) ).Write("cfg")
        for key,val in self.store_.iteritems():
            val.CloneTree().Write(key,ROOT.TObject.kWriteDelete)
        self.workspace_.Write()
        fout.Close()
        
        if self.rename_:
            os.rename( *self.rename_ )

    ## ------------------------------------------------------------------------------------------------------------
    def readWs(self,options,args):
        print
        print "--------------------------------------------------------------------------------------------------------------------------"
        print "Reading back workspace from %s " % options.read_ws
        print 
        fin = self.open(options.read_ws)
        cfg = json.loads( str(fin.Get("cfg").GetString()) )
        options.fits = cfg["fits"]
        self.workspace_ = fin.Get("wtemplates")
        self.workspace_.rooImport = getattr(self.workspace_,"import")
        for name in cfg["stored"]:
            self.store_[name]=fin.Get(name)
                
        print "Fits :"
        print "---------------------------------------------------------"
        for key,val in options.fits.iteritems():
            print "- %s \n  ndim : %d \n  components : %s" % ( key, val["ndim"], ",".join(val["components"]) )
            print
        
        print "TTrees :"
        print "---------------------------------------------------------"
        for key,val in self.store_.iteritems():
            print key.ljust(30), ":", ("%d" % val.GetEntries()).rjust(8)
        print
        
        print "Datasets :"
        print "---------------------------------------------------------"
        alldata = self.workspace_.allData()
        ntoys = 0
        for dset in alldata:
            name = dset.GetName()
            if name.startswith("toy"):
                ntoys += 1
            else:
                print name.ljust(30), ":", ("%d" % dset.sumEntries()).rjust(8)
        print 
        print "Number of toys : %d"         % ntoys
        print    
        print "--------------------------------------------------------------------------------------------------------------------------"

        if self.store_new_:
            self.store_input_ = self.store_
            self.store_ = {}
            
            self.workspace_input_ = self.workspace_
            self.workspace_ = ROOT.RooWorkspace("wtemplates","wtemplates")
            self.workspace_.rooImport = getattr(self.workspace_,"import")
            

    ## ------------------------------------------------------------------------------------------------------------
    
    def compareTemplates(self,options,args):
 #       fout = self.openOut(options)
  #      fout.Print()
   #     fout.cd()
    #    self.doCompareTemplates(options,args)
     #   self.saveWs(options,fout)
    
    ## ------------------------------------------------------------------------------------------------------------
    #MQ compare truth templates with rcone and sideband templates
    #def doCompareTemplates(self,options,args):
        print "Compare truth templates with rcone and sideband templates"
        ROOT.TH1F.SetDefaultSumw2(True)
        for name, comparison in options.comparisons.iteritems():
            if name.startswith("_"): continue
            print "Comparison %s" % name
            prepfit=comparison["prepfit"] 
            print "prep fit? " ,prepfit
            ReDo=comparison["redo"] 
            print "ReDo? " ,ReDo
            weight_cut=comparison["weight_cut"] 
            print "weight_cut: " ,weight_cut
            fitname=comparison["fit"]
            fit=options.fits[fitname]
            components=comparison.get("components",fit["components"])
            for comp in components:
                if type(comp) == str or type(comp)==unicode:
                    compname = comp
                    templatesls= comparison["templates"]
                else:
                    compname, templatesls = comp
                for cat in fit["categories"]:
                    print cat
                    massargs=ROOT.RooArgSet("massargs")
                    isoargs=ROOT.RooArgSet("isoargs")
                    mass_var,mass_b=self.getVar(comparison.get("mass_binning"))
                    mass=self.buildRooVar(mass_var,mass_b,recycle=True)
                    massargs.add(mass)
                    massargs.Print()
                    template_binning = array.array('d',comparison.get("template_binning"))
                    templatebins=ROOT.RooBinning(len(template_binning)-1,template_binning,"templatebins" )
        ########## list to store templates for each category
                    templates = []
                    for idim in range(fit["ndim"]):
                        isoargs.add(self.buildRooVar("templateNdim%dDim%d" % ( fit["ndim"],idim),template_binning,recycle=True))
                    isoargs.Print()
                    setargs=ROOT.RooArgSet(massargs,isoargs)
                    setargs.add(self.buildRooVar("weight",[],recycle=True))
                    truthname= "mctruth_%s_%s_%s" % (compname,fitname,cat)
                    truth = self.reducedRooData(truthname,setargs,False,redo=ReDo)
                    templates.append(truth)
            ########### loop over templates
                    for template,mapping in templatesls.iteritems():
                        if "mix" in template:
                             mixname = template.split(":")[-1]
                             templatename= "template_mix_%s_%s_%s" % (compname,mixname,mapping.get(cat,cat))
                        else:
                             templatename= "template_%s_%s_%s" % (compname,template,mapping.get(cat,cat))
                        tempdata = self.reducedRooData(templatename,setargs,False,sel=weight_cut,redo=ReDo)
                        templates.append(tempdata)
                    print "templates list: ", templates
        ##########split in massbins
                    splitByBin=comparison.get("splitByBin")
                    masserror = array.array('d',[])
                    
                    if cat=="EEEB":#TODO implement in json file
                        catd="EBEE"
                    else:
                        catd=cat
                    print catd
                    dset_data = self.reducedRooData("data_2D_%s" %catd,setargs,False,redo=ReDo)
                    if splitByBin:
                        mass_split=comparison.get("mass_split")
                        print "mass splitting: ntot bins, ntot for run, startbin",mass_split, " dataset : " , "data_2D_%s" % catd
                        diphomass=self.massquantiles(dset_data,massargs,mass_b,mass_split) 
                        print "integrated over whole mass spectrum"
                    else:
                        mass_split=[1,1,0]
                        diphomass = array.array('d',[0.,13000.])
                    for mb in range(mass_split[2],mass_split[1]):
                        
                        cut=ROOT.TCut("mass>%f && mass<%f"% (diphomass[mb],diphomass[mb+1]))
                        cut_s= "%1.0f_%2.0f"% (diphomass[mb],diphomass[mb+1])
                        print cut.GetTitle()
                        print "massbin: ", cut_s
                        dset_massc = dset_data.Clone("dset_data_%s_mb_%s"%(catd,cut_s))
                        dset_massc.reduce(cut.GetTitle())
                        print dset_massc
                        templates_massc=[]
                        for temp_m in templates:
                            #temp_massc =temp_m.reduce(cut.GetTitle())
                            temp_massc =temp_m
                            temp_massc.Print()
                            temp_massc.SetNameTitle("%s_mb_%s"%(temp_m.GetName(),cut_s),"%s_mb_%s"%(temp_m.GetName(),cut_s))
                            templates_massc.append(temp_massc)
                ##############loop over 2 legs
                        for id in range(fit["ndim"]):
                            histls=[]
                            isoarg1d=ROOT.RooArgList("isoarg")
                            isoarg1d.add(self.buildRooVar("templateNdim%dDim%d" % ( fit["ndim"],id),template_binning,recycle=True))                
                            tit = "compiso_%s_%s_%s_mb_%s_templateNdim%dDim%d" % (fitname,compname,cat,cut_s,fit["ndim"],id)
                            print
                            print tit
                            for tm in templates_massc:
                                print "tm.GetName()",tm.GetName()
                                tempHisto=ROOT.TH1F("%s_dim%d_%d_H" % (tm.GetName(),fit["ndim"],id),"%s_dim%d_%d_H" % (tm.GetName(),fit["ndim"],id),len(template_binning)-1,template_binning)
                                tm.fillHistogram(tempHisto,isoarg1d)
                                print tempHisto.GetName()
                                tempHisto.Scale(1.0/tempHisto.Integral())
                                for bin in range(1,len(template_binning) ):
                                    tempHisto.SetBinContent(bin,tempHisto.GetBinContent(bin)/(tempHisto.GetBinWidth(bin)))
                                    tempHisto.SetBinError(bin,tempHisto.GetBinError(bin)/(tempHisto.GetBinWidth(bin)))
                                histls.append(tempHisto)
                            if not prepfit:
                                print "plot 1d histos"
                                self.plotHistos(histls,tit,template_binning,True)
                  ##########roll out for combine tool per category
                        diphomass[mb]=(diphomass[mb]+diphomass[mb+1])/2.
                        masserror.append((diphomass[mb+1]-diphomass[mb])/2.)
                        if fit["ndim"]>1:
                            print
                            self.histounroll(templates_massc,template_binning,isoargs,cat,prepfit)
                            if prepfit:
                                datals=[]
                                datals.append(dset_massc)
                                self.histounroll(datals,template_binning,isoargs,cat,prepfit)
                            

                ########outside category loop
            #######outside components loop
        self.saveWs(options)
    ## ------------------------------------------------------------------------------------------------------------

    def histounroll(self,templatelist, template_binning,isoargs,cat,prepfit):
        pad_it=0
        c1=ROOT.TCanvas("d2hist_%s" % cat,"2d hists per category",1000,1000) 
        c1.Divide(1,2)
        histlistunroll=[]
        print "roll out" 
        tempunroll_binning = array.array('d',[])
        histlsY=[]
        histlsX=[]
        print"len(template_binning)", len(template_binning)
        print"template_binning", template_binning

        for tempur in templatelist:
            pad_it+=1
            temp2d=ROOT.TH2F("temp2d%s" % (tempur.GetName()),"temp2d%s" % (tempur.GetName()),len(template_binning)-1,template_binning,len(template_binning)-1,template_binning)
            tempur.fillHistogram(temp2d,ROOT.RooArgList(isoargs))
            temp2dx=temp2d.ProjectionX("%s_X" %tempur.GetName())
            temp2dx.Scale(1./temp2dx.Integral())
            temp2dx.SetTitle("%s_X" %tempur.GetTitle())
            temp2dy=temp2d.ProjectionY("%s_Y" %tempur.GetName())
            temp2dy.Scale(1./temp2dy.Integral())
    ###### draw projections as a check
            histlsX.append(temp2dx)
            temp2dy.SetTitle("%s_Y" %tempur.GetTitle())
            histlsY.append(temp2dy)
            temp2d.Scale(1./temp2d.Integral())
            tempunroll_binning = array.array('d',[])
            tempunroll_binning.append(0.0)
            sum=0.
            for bin1 in range(1,len(template_binning)):
                for bin2 in range(1,len(template_binning)):
                    binCont=0.
                    binErr=0.
                    area=0.
                    binCont= temp2d.GetBinContent(bin1,bin2)
                    binErr=temp2d.GetBinError(bin1,bin2)
                    area=(temp2d.GetXaxis().GetBinWidth(bin1))*(temp2d.GetYaxis().GetBinWidth(bin2))
                    if not prepfit:
                        sum+=1
                        temp2d.SetBinContent(bin1,bin2,binCont/area)
                        temp2d.SetBinError(bin1,bin2,binErr/area)
                    else:
                        sum+=area
                        temp2d.SetBinContent(bin1,bin2,binCont)
                        temp2d.SetBinError(bin1,bin2,binErr)
                    tempunroll_binning.append(sum)
            if prepfit:
                templateNdim2d_unroll=self.buildRooVar("templateNdim2d_unroll",tempunroll_binning,recycle=True)
                unrollvar=ROOT.RooArgList(templateNdim2d_unroll) 
                print templateNdim2d_unroll
            c1.cd(pad_it)
            ROOT.gPad.SetLogz()
            temp2d.Draw("COLZ")
            temp2d.GetZaxis().SetRangeUser(1e-8,1)
            bin=0
            temp1dunroll=ROOT.TH1F("unrolled_%s" %(tempur.GetName()),"unrolled_%s" %(tempur.GetName()),len(tempunroll_binning)-1,tempunroll_binning)
            print "tempunroll_binning", tempunroll_binning
            print "len(tempunroll_binning)",len(tempunroll_binning)
            for b in range(1,len(template_binning)):
              #  to loop up to inclusively b
                for x in range(1,b+1):
                    bin+=1
                    binC= temp2d.GetBinContent(x,b)
               #     print binC, "bin width" ,temp1dunroll.GetBinWidth(bin)
                    binE= temp2d.GetBinError(x,b)
                    temp1dunroll.SetBinContent(bin,binC)
                    temp1dunroll.SetBinError(bin,binE)
                    #to count down to 1 = "> 0" (0 not taken)
                for y in range (b-1,0,-1):
                    bin+=1
                    binC= temp2d.GetBinContent(b,y)
                    binE= temp2d.GetBinError(b,y)
                #    print binC, "bin width" ,temp1dunroll.GetBinWidth(bin)
                    temp1dunroll.SetBinContent(bin,binC)
                    temp1dunroll.SetBinError(bin,binE)
            histlistunroll.append(temp1dunroll)
            if prepfit:
                roodatahist_1dunroll=ROOT.RooDataHist("H%s" % temp1dunroll.GetName(),"H%s" % temp1dunroll.GetName(),unrollvar, temp1dunroll)
                print roodatahist_1dunroll
                self.workspace_.rooImport(roodatahist_1dunroll,ROOT.RooFit.RecycleConflictNodes())
        titleunroll = "%s_unroll" % (tempur.GetTitle())
        print histlsX
        print histlsY
        print histlistunroll
        if not prepfit:
            self.plotHistos(histlsX,"%s_X" %tempur.GetTitle(),template_binning,False)
            self.plotHistos(histlsY,"%s_Y" %tempur.GetTitle(),template_binning,False)
            self.plotHistos(histlistunroll,titleunroll,tempunroll_binning,False)
            self.keep( [c1] )
            self.autosave(True)


## ------------------------------------------------------------------------------------------------------------

    def massquantiles(self,dataset,massargs,mass_binning,mass_split):
        print "splitByBin for dataset", dataset.GetName()
        massH=ROOT.TH1F("%s_massH" % dataset.GetName()[-17:],"%s_massH" % dataset.GetName()[-17:],len(mass_binning)-1,mass_binning)
        dataset.fillHistogram(massH,ROOT.RooArgList(massargs)) 
        print "define mass bins " 
        massH.Scale(1.0/massH.Integral())
        prob = array.array('d',[])
        dpmq = array.array('d',[0.0 for i in range((mass_split[1]+1))])
        for i in range(0,mass_split[1]+1):
            prob.append((i+float(mass_split[2]))/mass_split[0])
        massH.GetQuantiles(mass_split[1]+1,dpmq,prob)
        #show the original histogram in the top pad
        cq=ROOT.TCanvas("cq_%s" %dataset.GetName()[-20:],"mass quantiles",10,10,700,900)
        cq.Divide(1,2)
        cq.cd(1)
        ROOT.gPad.SetLogy()
        massH.GetXaxis().SetTitle("diphoton mass [GeV]")
        massH.Draw()
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

    def plotHistos(self,histlist,title,template_bins,dim1):
      #  ROOT.gStyle.SetOptStat(111111)
        leg = ROOT.TLegend(0.2,0.8,0.9,0.9)
        leg.SetTextSize(0.03)
        leg.SetTextFont(42);
        leg.SetFillColor(ROOT.kWhite)
        canv = ROOT.TCanvas(title,title)
        canv.Divide(1,2)
        canv.cd(1)
        ROOT.gPad.SetPad(0., 0.35, 1., 1.0)
        ROOT.gPad.SetLogy()
        canv.cd(2)
        ROOT.gPad.SetPad(0., 0., 1., 0.35)
        ROOT.gPad.SetGridy()
        canv.cd(1)
        
        histlist[0].SetFillColor(ROOT.kRed)
        histlist[0].SetFillStyle(3004)
        histlist[0].SetLineColor(ROOT.kRed)
        histlist[0].Draw("E2")
        histlist[0].GetXaxis().SetLimits(-0.1,max(template_bins))
        #histlist[0].SetStats()
        histlist[0].GetYaxis().SetLabelSize( histlist[0].GetYaxis().GetLabelSize() * canv.GetWh() / ROOT.gPad.GetWh() )
        if dim1:
            histlist[0].GetXaxis().SetTitle(title[-17:])
        else:
            histlist[0].GetXaxis().SetTitle("charged isolation")
        for i in range(0,len(histlist)):
            histlist[i].GetXaxis().SetLimits(-0.1,max(template_bins))
            if i>0:
                histlist[i].SetLineColor(ROOT.kAzure+i)
                histlist[i].SetMarkerColor(ROOT.kAzure+i)
                histlist[i].SetMarkerStyle(20)
                histlist[i].Draw("E SAME")
            histlist[0].GetXaxis().SetLimits(-0.1,max(template_bins))
            leg.AddEntry(histlist[i],histlist[i].GetTitle(),"l")  
        leg.Draw()
        canv.cd(2)
        ratio=histlist[1].Clone("ratio")
        ratio.Divide(histlist[0])
        ratio.SetLineColor(ROOT.kAzure+1)
        ratio.SetMarkerColor(ROOT.kAzure+1)
        ratio.GetYaxis().SetTitleSize( histlist[0].GetYaxis().GetTitleSize() * 6.5/3.5 )
        ratio.GetYaxis().SetLabelSize( histlist[0].GetYaxis().GetLabelSize() * 6.5/3.5 )
        ratio.GetYaxis().SetTitleOffset( histlist[0].GetYaxis().GetTitleOffset() * 6.5/3.5 )
        ratio.GetXaxis().SetTitleSize( histlist[0].GetXaxis().GetTitleSize() * 6.5/3.5 )
        ratio.GetXaxis().SetLabelSize( histlist[0].GetXaxis().GetLabelSize() * 6.5/3.5 )
        if dim1:
            ratio.GetXaxis().SetTitle(title[-17:])
        else:
            ratio.GetXaxis().SetTitle("charged isolation")
        ratio.Draw()
        ratio.GetYaxis().SetTitle("ratio")
        ratio.GetXaxis().SetLimits(-0.1,max(template_bins))
        ratio.GetYaxis().SetRangeUser(0.5,1.5)
        ROOT.gStyle.SetOptStat(0)
      #  ROOT.gStyle.SetOptTitle(0)
        self.keep( [canv] )
        self.autosave(True)
        

## ------------------------------------------------------------------------------------------------------------
    def prepareTruthFit(self,options,args): 
        self.saveWs(options)

    ## ------------------------------------------------------------------------------------------------------------
    def prepareNominalFit(self,options,args):
        fout = self.openOut(options)
        fout.Print()
        fout.cd()
        self.doNominalFit(options,args)
        self.saveWs(options,fout)
    
    ## ------------------------------------------------------------------------------------------------------------
    def doNominalFit(self,options,args):
        #add data in json file
        for name, nomFit in options.nominalFit.iteritems():
            if name.startswith("_"): continue
            fitname=nomFit["fit"]
            fit=options.fits[fitname]
            obsls=ROOT.RooArgList("obsls")
            obsls.add(self.getVar(nomFit.get("observable")))
            #add rooformula for purity estimate fsig
            jpp = ROOT.RooRealVar("jpp","jpp",0.3,0,1)
            jpf = ROOT.RooRealVar("jpf","jpf",0.3,0,1)
            fpurity = ROOT.RooFormulaVar("fpurity","fpurity","jpp+jpf ",RooFit.RooArgList(jpp,jpf))
            #automatically binning from this variable imported?
            print "nominal fit with: ", name, " observable : ", nomFit.get("observable")
            roodatahists=nomFit.get("histos",fit["components"])
            return
            print roodatahists
            hist_Eta=[]
            #components pp pf and ffshould be in histo
            for cat in fit["categories"]:
                rooHistPdfs=ROOT.RooArgList("rooPdfs")
                for histo in roodatahists:
                    print histo
                    rooHistPdf=ROOT.RooHistPdf("pdf_%s"% histo.GetName(),"pdf_%s"% histo_GetName(),obsls,rooHist_comp)
                    rooHistPdfs.add(rooHistPdf)
                fit2dpdf=ROOT.RooAddPdf("fit2dpf_%s" % cat,"fit2dpdf_%s" % cat,rooHistPdfs,RooArgList(fpurity),False)
          #save roofitresult in outputfile
                fitresult = fit2dpdf.fitTo(data, RooFit.NumCPU(8), RooFit.Extended(False),RooFit.SumW2Error(False),RooFit.Verbose(False),RooFit.Save(True))
                fitresult.Print()
                #self.plotFit()#TODO also implement 2d option 
    ## ------------------------------------------------------------------------------------------------------------
    def plotFit(self,roovar,rooaddpdf,components,data,unroll_binning,title,log):
        b=ROOT.TLatex()
        b.SetNDC()
        b.SetTextSize(0.06)
        b.SetTextColor(ROOT.kRed)
        cFit = ROOT.TCanvas("cFit","cFit",1200,800)
        leg =ROOT.TLegend(0.15,0.8,0.35,0.9)
        cFit.cd(1)
        if log:
            cFit.SetLogy()
        frame = roovar.frame(RooFit.Title(title))
        data.plotOn(frame,RooFit.Binning(unroll_binning),RooFit.Name("data"))
        rooaddpdf.plotOn(frame,RooFit.Name("fit"))
        if len(components)>2:
            rooaddpdf.plotOn(frame,RooFit.Components(components[0]),RooFit.LineStyle(kDashed),RooFit.LineColor(kRed),RooFit.Name("pp"))
            rooaddpdf.plotOn(frame,RooFit.Components(components[1]),RooFit.LineStyle(kDashed),RooFit.LineColor(kCyan+1),RooFit.Name("pf"))
            rooaddpdf.plotOn(frame,RooFit.Components(components[2]),RooFit.LineStyle(kDashed),RooFit.LineColor(kBlack),RooFit.Name("ff"))
        frame.Draw()
        leg.AddEntry("fit","fit","l")
        if len(components)>2:
            leg.AddEntry("pp","prompt-prompt ","l")
            leg.AddEntry("pf","prompt-fake ","l")
            leg.AddEntry("pp","fake-fake ","l")
        leg.SetFillColor(ROOT.kWhite) 
        leg.Draw()
        #b.DrawLatex(0.55,0.7,"PRELIMINARY")
        self.keep([cFit])

    ## ------------------------------------------------------------------------------------------------------------
    def prepareTemplates(self,options,args):
        
        fout = self.openOut(options)
        self.workspace_ = ROOT.RooWorkspace("wtemplates","wtemplates")
        tmp = fout

        ## read input trees
        self.datasets_["data"] = self.openDataset(None,options.data_file,options.infile,options.data)
        self.datasets_["mc"]   = self.openDataset(None,options.mc_file,options.infile,options.mc)
        self.datasets_["templates"]   = self.openDataset(None,options.data_file,options.infile,options.templates)
        # used by parent class PlotApp to read in objects
        self.template_ = options.treeName
        
        self.groups = options.groups
        if options.groups:
            self.categories_ = options.groups.keys()
        else:
            self.categories_ = options.categories
            
        ## create output workspace
        self.workspace_.rooImport = getattr(self.workspace_,"import")

        ## read and store list of aliases. will be defined later in all trees
        for var in options.aliases:
            self.getVar(var)
        
        ## define list of variables for the dataset
        varlist = ROOT.RooArgList()
        weight  = self.getVar(options.weight_expression)[0]
        for var in options.dataset_variables+[weight]:
            name, binning = self.getVar(var)
            rooVar = self.buildRooVar(name,binning)
            varlist.add(rooVar)
            
        ## loop over configured fits
        for name, fit in options.fits.iteritems():
            print
            print "--------------------------------------------------------------------------------------------------------------------------"
            print "Preparing fit %s" % name
            print 
            
            ndim            = fit["ndim"]
            bins            = fit["bins"]
            components      = fit["components"]
            categories      = fit["categories"]
            truth_selection = fit["truth_selection"]
            template_binning = array.array('d',fit["template_binning"])
            templates       = fit["templates"]
            storeTrees      = fit.get("store_trees",False)
            selection       = fit.get("selection",options.selection)
            preselection    = fit.get("preselection",options.preselection)
            
            variables       = fit.get("dataset_variables",[])

            fulllist = varlist.Clone()
            for var in variables:
                vname, binning = self.getVar(var)
                rooVar = self.buildRooVar(vname,binning)
                fulllist.add(rooVar)
            
            for dim in range(ndim):
                dimVar = self.buildRooVar("templateNdim%dDim%d" % (ndim,dim),template_binning)
                fulllist.add( dimVar )
            print "Will put the following variables in the dataset : "
            fulllist.Print()
            
            print "Number of dimensions : %d" % ndim
            print "Components           : %s" % ",".join(components)
            print 
            
            tmp.cd()
            
            ## prepare data
            dataTrees = self.prepareTrees("data",selection,options.verbose,"Data trees")
            self.buildRooDataSet(dataTrees,"data",name,fit,categories,fulllist,weight,preselection,storeTrees)
            
            ## prepare mc
            mcTrees =  self.prepareTrees("mc",selection,options.verbose,"MC trees")
            self.buildRooDataSet(mcTrees,"mc",name,fit,categories,fulllist,weight,preselection,storeTrees)
            
            ## prepare truth templates
            for truth,sel in truth_selection.iteritems():
                cut = ROOT.TCut(preselection)
                cut *= ROOT.TCut(sel)
                legs = [""]
                if "legs" in fit:
                    legs = fit["legs"]
                self.buildRooDataSet(mcTrees,"mctruth_%s" % truth,name,fit,categories,fulllist,weight,cut.GetTitle(),storeTrees)
            
                        
            print
            ## sanity check
            for cat in categories.keys():
                catCounts = {}
                catCounts["tot"] = self.rooData("mc_%s_%s" % (name,cat) ).sumEntries()
                
                breakDown = 0.
                for truth in truth_selection.keys():
                    count = self.rooData("mctruth_%s_%s_%s" % (truth,name,cat) ).sumEntries()
                    breakDown += count
                    catCounts[truth] = count
                print cat, " ".join( "%s : %1.4g" % (key,val) for key,val in catCounts.iteritems() ),
                if breakDown != catCounts["tot"]:
                    print "\n   Warning : total MC counts don't match sum of truths. Difference: ", catCounts["tot"]-breakDown
                else:
                    print
                
            ## prepare templates
            print 
            for component,cfg in fit["templates"].iteritems():
                dataset = cfg.get("dataset","templates")
                trees = self.prepareTrees(dataset,cfg["sel"],options.verbose,"Templates selection for %s" % component)
                cats = {}
                presel = cfg.get("presel",preselection)
                for cat,fill in cfg["fill_categories"].iteritems():
                    config = { "src" : categories[cat]["src"],
                               "fill": fill
                               }
                    cats[cat] = config
                self.buildRooDataSet(trees,"template_%s" % component,name,fit,cats,fulllist,weight,presel,storeTrees)
                for cat in categories.keys():
                    print "template %s - %s" % (component,cat), self.rooData("template_%s_%s_%s" % (component,name,cat) ).sumEntries()
            print 
            print "--------------------------------------------------------------------------------------------------------------------------"
            print 

        if options.mix_templates:
            self.doMixTemplates(options,args)

        self.saveWs(options,fout)
    
    ## ------------------------------------------------------------------------------------------------------------
    def mixTemplates(self,options,args):
        fout = self.openOut(options)
        fout.Print()
        fout.cd()
        self.doMixTemplates(options,args)
        self.saveWs(options,fout)
    
    ## ------------------------------------------------------------------------------------------------------------
    def doMixTemplates(self,options,args):
        
        for name, mix in options.mix.iteritems():
            print
            print "--------------------------------------------------------------------------------------------------------------------------"
            print "Mixing templates %s" % name
            print 

            targetName      = mix["target"]
            targetFit       = options.fits[targetName]
            ndim            = targetFit["ndim"]
            ## categories      = target["categories"]
            
            ptLeadMin       = mix["ptLeadMin"]
            ptSubleadMin    = mix["ptSubleadMin"]
            massMin         = mix["massMin"]
            mixType         = mix.get("type","simple") 

            pt              = mix["pt"]
            eta             = mix["eta"]
            phi             = mix["phi"]
            energy          = mix["energy"]
            replace         = mix["replace"]
            fill_categories = mix["fill_categories"]
            
            if mixType == "simple":
                matchVars = ROOT.RooArgList() # FIXME
                for var,thr in mix["match"].iteritems():
                    var = self.buildRooVar(var,[])
                    var.setVal(thr)
                    matchVars.add(var)

            elif mixType == "kdtree":
                pass
            else:
                sys.exit(-1,"Uknown event mixing type %s" % mixType)
  
            if ndim != 2:
                sys.exit(-1,"can only do event mixing with two objects")

            sources = {}
            print "Source templates: "            
            for comp,source in mix["sources"].iteritems():
                print comp, ":", " ".join(source)
                sources[comp] = [ s.split(":") for s in source ]
            print

            for cat, fill in fill_categories.iteritems():
                print
                print "Filling category :", cat
                for comp,source in sources.iteritems():
                    legs = []
                    legnams = []
                    print
                    print "Component :", comp
                    for leg,src in zip(fill["legs"],source):
                        sname,scomp = src
                        legname = "template_%s_%s_%s" % (scomp,sname,leg)
                        legnams.append( legname )
                        print legname
                        dset = self.rooData(legname,False)
                        print dset
                        legs.append( (self.treeData(legname),ROOT.RooArgList(self.dsetVars(legname)) ) )
                    if len(legs) != ndim:
                        sys.exit(-1,"number of legs does not match number of dimensions for dataset mixing")
                    rndswap     = fill.get("rndswap",False)
                    rndmatch     = fill.get("rndmatch",0.)
                    
                    print "legs  :", " ".join(legnams)
                    print "type  :", mixType
                    
                    (tree1, vars1), (tree2, vars2)  = legs
                    mixer = ROOT.DataSetMixer( "template_mix_%s_%s_%s" % ( comp, name, cat),"template_mix_%s_%s_%s" % ( comp, name, cat),
                                               vars1, vars2, replace, replace,
                                               ptLeadMin, ptSubleadMin, massMin,
                                               "weight", "weight", True,                                               
                                               )
                    
                    if mixType == "simple":
                        maxEvents   = fill.get("maxEvents",-1)
                        matchEffMap = fill.get("matchEff",{})
                        matchEff    = matchEffMap.get(comp,1.)
                        print "maxEvents :", maxEvents, "rndswap :", rndswap, "mathcEffMap"
                        mixer.fillFromTree(tree1,tree2,pt,eta,phi,energy,pt,eta,phi,energy,matchVars,rndswap,maxEvents,matchEff)
                        
                    elif mixType == "kdtree":
                        targetCat       = fill.get("targetCat",cat)
                        nNeigh          = fill.get("nNeigh",10)
                        nMinNeigh       = fill.get("nMinNeigh",nNeigh)
                        useCdfDistance  = fill.get("useCdfDistance",False)
                        matchWithThreshold  = fill.get("matchWithThreshold",False)
                        targetWeight    = fill.get("targetWeight","weight")
                        dataname        = "data_%s_%s" % (targetName,targetCat)                        
                        target          = self.treeData(dataname)
                    

                        matchVars1   = ROOT.RooArgList()
                        matchVars2   = ROOT.RooArgList()
                        targetMatch1 = ROOT.RooArgList()
                        targetMatch2 = ROOT.RooArgList()
                        
                        for var in fill["match1"]:
                            var = self.buildRooVar(var,[])
                            matchVars1.add(var)
                        for var in fill["match2"]:
                            var = self.buildRooVar(var,[])
                            matchVars2.add(var)
                        for var in fill["target1"]:
                            var = self.buildRooVar(*(self.getVar(var)))
                            targetMatch1.add(var)
                        for var in fill["target2"]:
                            var = self.buildRooVar(*(self.getVar(var)))
                            targetMatch2.add(var)
                            
                        print "target :", dataname
                        print "rndswap :", rndswap, " rndmatch :", rndmatch," useCdfDistance :", useCdfDistance, "matchWithThreshold :", matchWithThreshold
                        print "nNeigh :", nNeigh, "nMinNeigh :", nMinNeigh
                        print "target :", target
                        mixer.fillLikeTarget(target,targetMatch1,targetMatch2,targetWeight,tree1,tree2,
                                             pt,eta,phi,energy,pt,eta,phi,energy,
                                             matchVars1,matchVars2,rndswap,rndmatch,nNeigh,nMinNeigh,
                                             useCdfDistance,matchWithThreshold)
                    
                    dataset = mixer.get()
                    self.workspace_.rooImport(dataset,ROOT.RooFit.RecycleConflictNodes())
                    tree = mixer.getTree()
                    self.store_[tree.GetName()] = tree

            print 
            print "--------------------------------------------------------------------------------------------------------------------------"
            print 


    ## ------------------------------------------------------------------------------------------------------------
    def setAliases(self,tree):
        """ Define all aliases in tees
        """
        for var,vdef in self.aliases_.iteritems():
            tree.SetAlias(var,vdef)
    

    ## ------------------------------------------------------------------------------------------------------------

    def reducedRooData(self,name,rooset,binned,weight="weight",sel=None,redo=False):
        data = self.rooData("r_%s" % name)
        if not data or redo:
            print "create rooData"
            data = self.rooData(name,rooset=rooset,weight=weight,sel=sel)
            if binned:
                data = data.binnedClone("r_%s" % name,"r_%s" % name)
            else:
                data.SetName("r_%s" % name)
        self.workspace_.rooImport(data)
        return data
    ## ------------------------------------------------------------------------------------------------------------

    def rooPdf(self,name):
        pdf = self.workspace_.pdf(name)
        if not pdf and self.store_new_:
            pdf = self.workspace_input_.pdf(name)            
        return pdf


    ## ------------------------------------------------------------------------------------------------------------
    def rooData(self,name,autofill=True,rooset=None,weight="weight",sel=None):
        if name in self.cache_:
            return self.cache_[name]        
        dataset = self.workspace_.data(name)
        if not dataset and self.store_new_:
            dataset = self.workspace_input_.data(name)
        if not dataset:
            return dataset
        if autofill and dataset.sumEntries() == 0.:
            tree = self.treeData(name)
            if not tree: 
                return dset
            if rooset:
                dataset = dataset.reduce(RooFit.SelectVars(rooset))
            else:
                dataset = dataset.emptyClone()
            self.cache_[name] = dataset
            filler = ROOT.DataSetFiller(dataset)
            cut=ROOT.TCut(weight)
            if sel:
                cut *=sel
            filler.fillFromTree(tree,cut.GetTitle(),True)
        elif rooset:
            if sel:
                dataset = dataset.reduce(rooset,sel)
            else:
                dataset = dataset.reduce(rooset)
        return dataset

    ## ------------------------------------------------------------------------------------------------------------
    def treeData(self,name):
        if "tree_%s" % name in self.store_:
            return self.store_["tree_%s" % name]
        elif self.store_new_ and "tree_%s" % name in self.store_input_:
            return self.store_input_["tree_%s" % name]
        return None
        
    ## ------------------------------------------------------------------------------------------------------------
    def dsetVars(self,name):
        st = self.workspace_.set("variables_%s" %name)
        if not st and self.store_new_:
            st = self.workspace_input_.set("variables_%s" %name)
        return st

    ## ------------------------------------------------------------------------------------------------------------
    def getVar(self,var):
        """ 
        Parse variable definition
        General form:
        'var := expression [binning]'
        ':= expression' can be omitted if the variable already exists in trees.
        '[binning]' is also optional and can be specified as [nbins,min,max] or list of boundaires.
        """
        if "[" in var:
            name,binning = var.rstrip("-").rstrip("]").rsplit("[",1)
            
            if "," in binning:
                binning = binning.split(",")
            else:
                binning = binning.split(":")
            if len(binning) == 3:
                nbins = int(binning[0])
                xmin = float(binning[1])
                xmax = float(binning[2])
                step = ( xmax - xmin ) / float(nbins)
                xbins = array.array('d',[xmin+step*float(ib) for ib in range(nbins+1)])
            else:
                xbins = array.array('d',[float(b) for b in binning])
        else:
            name,xbins = var.rstrip("-"),[]

        if ":=" in name:
            name,vdef = [ t.lstrip(" ").rstrip(" ").lstrip("\t").rstrip("\t") for t in name.split(":=",1) ]
            self.aliases_[name] = vdef
            
        name = name.lstrip(" ").rstrip(" ").lstrip("\t").rstrip("\t")
        if len(xbins) == 0 and name in self.variables_:
            xbins = self.variables_[name]
        else:
            self.variables_[name] = xbins
        return name,xbins

    ## ------------------------------------------------------------------------------------------------------------
    def buildRooVar(self,name,binning,importToWs=True,recycle=False):
        rooVar = None
        if recycle:
            rooVar = self.workspace_.var(name)
            if not rooVar and self.store_new_:
                rooVar = self.workspace_input_.var(name)
        if not rooVar:
            if name in self.aliases_:
                title = self.aliases_[name]
            else:
                title = name
            rooVar = ROOT.RooRealVar(name,title,0.)
            rooVar.setConstant(False)

        if len(binning) > 0:
            if len(binning)==1:
                rooVar.setVal(binning[0])                
            else:
                rooVar.setMin(binning[0])
                rooVar.setMax(binning[-1])
                rooVar.setVal(0.5*(binning[0]+binning[-1]))
                rooVar.setBinning(ROOT.RooBinning(len(binning)-1,binning))
        if importToWs:
            self.workspace_.rooImport(rooVar,ROOT.RooFit.RecycleConflictNodes())
        self.keep(rooVar) ## make sure the variable is not destroyed by the garbage collector
        return rooVar

    ## ------------------------------------------------------------------------------------------------------------
    def buildRooDataSet(self,trees,name,fitname,fit,categories,fulllist,weight,preselection,storeTrees):
        """ Build per-category RooDataSet starting from trees
        """
        # define loop over legs
        legs = [""]
        redef = []        
        if "legs" in fit:
            legs = fit["legs"]
            redef = [ fulllist[ivar].GetTitle()  for ivar in range(fulllist.getSize() ) ]

        ## fill datasets
        for catname,cfg in categories.iteritems():
            filler = ROOT.DataSetFiller( "%s_%s_%s" % (name,fitname,catname), "%s_%s_%s" % (name,fitname,catname), fulllist, weight, storeTrees )
            
            ## source category
            src = trees[cfg["src"]]
            
            ## filling directives
            fill  = cfg["fill"]

            ## loop over directives 
            for cut,variables in fill.iteritems():
                # assume template vars are at the end
                firstVar = fulllist.getSize()-len(variables)
                ## loop over all legs
                for leg in legs:
                    ## adapt the definition of all variables
                    for ired, red in enumerate(redef):
                        filler.vars()[ired].SetTitle(red % {"leg" : leg})
                    ## adapt the definition of the template variables
                    for ivar,var in enumerate(variables):
                        filler.vars()[firstVar+ivar].SetTitle(var % {"leg" : leg})
                
                    ## compute weight as preselection*cut*weight
                    wei  = ROOT.TCut(preselection)
                    wei *= ROOT.TCut(cut)
                    wei *= ROOT.TCut(weight)
                    
                    ## fill dataset from source trees
                    for tree in src:
                        twei = wei.GetTitle() % {"leg" : leg}
                        ## this will actually discard all events with weight 0 
                        ##   or outside of the range of any variable in fulllist
                        filler.fillFromTree(tree,twei)
            
            # restore variables definition
            for ired, red in enumerate(redef):
                fulllist[ired].SetTitle(red)
                
            ## and we are done
            dataset = filler.get()
            self.workspace_.rooImport(dataset,ROOT.RooFit.RecycleConflictNodes())

            variables = ROOT.RooArgSet(filler.vars())
            self.workspace_.defineSet("variables_%s" % dataset.GetName(),variables)
            
            if storeTrees:
                tree = filler.getTree()
                self.store_[tree.GetName()] = tree
                

    ## ------------------------------------------------------------------------------------------------------------
    def prepareTrees(self,name,selection,doPrint=False,printHeader=""): 
        """ Read trees from input file(s) and set all aliases 
        """ 
        if doPrint:    
            print "%s :" % printHeader
        
        ## read trees for given selection
        allTrees = self.getTreesForSelection(name,selection)
        for cat,trees in allTrees.iteritems():
            treePaths = []
            ## set aliases
            for t in trees:
                self.setAliases(t)
                treePaths.append( "%s/%s" % (os.path.relpath(t.GetDirectory().GetPath()), t.GetName()) )
            if doPrint:
                print " %s : \n  %s" % (cat, "\n  ".join(treePaths) ) 

        if doPrint:    
            print 
           
        return allTrees

    ## ------------------------------------------------------------------------------------------------------------
    def getTreesForSelection(self,dataset,selection):
        """ Load trees used for datasets definition.
        """ 
        ret = {}
        
        ## keep track of already loaded datasets
        key = "%s:%s" % ( dataset, selection ) 
        if not key in self.trees_:
            infile,samplesTmpl = self.datasets_[dataset]
            
            ## replace %(sel)s keyword with choosen selection
            replacements = { "sel" : selection }
            samples = [ s % replacements for s in samplesTmpl ]
            ## initialize list of trees: one entry per category
            self.trees_[key] = {}
            
            ## loop over categories and read in trees
            for cat in self.categories_:
                if type(cat) == int: 
                    catname = "cat%d" % cat
                else:
                    catname = cat
                    group = None
                if self.groups:
                    group = self.groups[cat]
                
                # call PlotApp.readObjects to read trees
                self.trees_[key][catname] = self.readObjects(infile,"",samples=samples,cat=catname,group=group)
                
        ## done
        return self.trees_[key]

        
    ## ------------------------------------------------------------------------------------------------------------
    ## End of class definition
    ## ------------------------------------------------------------------------------------------------------------


# -----------------------------------------------------------------------------------------------------------
# actual main
if __name__ == "__main__":
    app = TemplatesApp()
    app.run()
