#!/bin/env python
from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, json
from pprint import pprint
import array
import random
from getpass import getuser

from math import sqrt


## ----------------------------------------------------------------------------------------------------------------------------------------
class rooImport:

    def __init__(self,target):
        self.import_ = getattr(target,"import")
        
    def __call__(self,*args):
        if len(args) == 1:
            try:
                # workaround for https://sft.its.cern.ch/jira/browse/ROOT-6785
                self.import_(args[0],ROOT.RooCmdArg())
            except:
                # for actual TObjects
                self.import_(args[0])
        else:
            self.import_(*args)
            
## ----------------------------------------------------------------------------------------------------------------------------------------
class LookUp:
    def __init__(self,target,method):
        self.method_ = method
        self.container_ = target.container_

    def __call__(self,*args):
        ## print args
        for ws in self.container_:
            obj = getattr(ws,self.method_)(*args)
            if obj: return obj
        return None

## ----------------------------------------------------------------------------------------------------------------------------------------
class WsList:
    
    def __init__(self,init=None):
        self.container_ = []
        if init:
            self.append(init)
        
    def append(self,what):
        self.container_.append(what)
    
    def Print(self,opt=""):
        for w in self.container_:
            w.Print(opt)
        
    def __getattr__(self,method):
        return LookUp(self,method)
    
    ### def data(self,what):
    ###     return self.lookup(what,"var")
    ### 
    ### def var(self,what):
    ###     return self.lookup(what,"var")
    ### 
    ### def pdf(self,what):
    ###     return self.lookup(what,"pdf")
    ### 
    ### def set(self,what):
    ###     return self.lookup(what,"set")
    ### 
    ### def func(self,what):
    ###     return self.lookup(what,"func")
    ### 
    ### def lookup(self,what,method):
    ###     for ws in self.container_:
    ###         obj = getattr(ws,method)(what)
    ###         if obj: return obj
    ###     return None



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
                                    help="(Di-)Photon selection to be used for analysis. In dataset definition it replaces '%(sel)s'."),                
                        make_option("--aliases",dest="aliases",action="callback",type="string",callback=optpars_utils.ScratchAppend(),
                                    default=[],
                                    help="List of aliases to be defined for each tree. They can be used for selection or variable definition"),
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
                        make_option("--read-ws","-r",dest="read_ws",type="string",
                                    default=[],action="callback",callback=optpars_utils.ScratchAppend(),
                                    help="workspace input file.",
                                    ),
                        make_option("--ws-dir","-w",dest="ws_dir",action="store",type="string",
                                    default=None,
                                    help="Folder to be used to read and write workspaces"
                                    ),
                        make_option("--output-file","-o",dest="output_file",action="store",type="string",
                                    default=None,
                                    help="Output file.",
                                    ),
                        make_option("--store-new-only",dest="store_new_only",action="store_true",
                                    default=False,
                                    help="Only store new objects in output file.",
                                    ),
                        make_option("--store-inputs",dest="store_inputs",action="store_true",
                                    default=False,
                                    help="Store all used input datasets.",
                                    ),
                        make_option("--mc-file",dest="mc_file",action="store",type="string",
                                    default=None,help="default: %default"),
                        make_option("--prepare-data",dest="prep_data",action="store_true",
                                    default=False,help="prepare templates only with data, no mc, signals, or templatesMC,mctruth)"),
                        make_option("--prepare-nosignal",dest="prep_nosig",action="store_true",
                                    default=False,help="prepare templates without signals"),
                        make_option("--mix-mc",dest="mix_mc",action="store_true",
                                    default=False,help="template mixing also with MC"),
                        make_option("--only-subset",dest="only_subset",action="callback",type="string", callback=optpars_utils.ScratchAppend(),
                    default=[],help="default: %default"),
                        ]
                      ),
                ( "Fit definition options. Usually specified through JSON configuration (see templates_maker.json for details)", [
                        make_option("--fit-categories",dest="fit_categories",action="callback",type="string",callback=optpars_utils.ScratchAppend(),help="sets specific category for fit, e.g. EBEB or EBEE",default=["EBEB","EBEE"]),
                        make_option("--fit-massbins",dest="fit_massbins",action="callback",type="string",callback=optpars_utils.ScratchAppend(),help="sets massbins for fit or templates comparison: first integer is total number of massbins, 2. how many bins we want to run over, 3. startbin",default=["1","1","0"]),
                        make_option("--fit-templates",dest="fit_templates",action="callback",type="string",callback=optpars_utils.ScratchAppend(),help="get templates for fit: either unrolled_template,unrolled_template_mix or unrolled_mctruth",default=["unrolled_template"]),
                        make_option("--plot-closure",dest="plot_closure",action="callback",callback=optpars_utils.ScratchAppend(),type="string",
                                    default=["template"],
                                    help="choose template or mctruth."),
                        make_option("--plot-purityvalue",dest="plot_purityvalue",action="callback",callback=optpars_utils.ScratchAppend(),type="string",
                                    default=["fraction"],
                                    help="purity either as fraction or as number of events in signalregion.Choose 'fraction' or 'events'"),
                        make_option("--plot-mctruth",dest="plotMCtruth",action="callback",callback=optpars_utils.ScratchAppend(),type="string",
                                    default=["mctruth"]),
                        make_option("--plot-purity",dest="plot_purity",action="store_true",default=False,
                                    help="Plot purities, purity vs massbin and pull function",
                                    ),
                        make_option("--fits",dest="fits",action="callback",callback=optpars_utils.Load(),type="string",
                                    default={},help="List of templates fits to be performed. Categories, componentd and templates can be specified."),
                        ### make_option("--template-binning",dest="template_binning",action="callback",callback=optpars_utils.ScratchAppend(float),
                        ###             type="string",
                        ###             default=[],
                        ###             help="Binning of the parametric observable to be used for templates",
                        ###             ),                        
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
        self.save_params_ = []
        self.workspace_ = None

        self.save_params_.append("signals")
        self.save_params_.append("aliases")
        self.save_params_.append("preselection")
        
        ## load ROOT (and libraries)
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils
        ROOT.gSystem.Load("libdiphotonsUtils")
        if ROOT.gROOT.GetVersionInt() >= 60000:
            ROOT.gSystem.Load("libdiphotonsRooUtils")
            ROOT.gSystem.AddIncludePath("-I$CMSSW_BASE/include")
            ROOT.gROOT.ProcessLine('#include "diphotons/Utils/interface/DataSetFiller.h"')
            ROOT.gROOT.ProcessLine('#include "diphotons/Utils/interface/DataSetMixer.h"')

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
            self.store_inputs_ = options.store_inputs
        
        if len(options.read_ws) > 0:
            options.read_ws_list = options.read_ws
            options.read_ws = options.read_ws_list[0]
        else:
            options.read_ws = False
            
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
            
        if options.verbose:
            print "Read workspace"

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
            if options.verbose:
                print "calling mix templates"
            self.mixTemplates(options,args)
            
            
        

    ## ------------------------------------------------------------------------------------------------------------
    def openOut(self,options):
        if options.read_ws and options.output_file == options.read_ws:
            name    = options.output_file
            tmpname = name.replace(".root","_tmp.root")
            fout    = self.open(tmpname,"recreate",folder=options.ws_dir)
            self.rename_  = (tmpname,name)
        else:
            fout = self.open(options.output_file,"recreate",folder=options.ws_dir)
        return fout

    ## ------------------------------------------------------------------------------------------------------------
    def saveWs(self,options,fout=None):            
        if not fout:
            fout = self.openOut(options)
        fout.cd()
        cfg = { "fits"   : options.fits,
                "mix"    : options.mix,
                "stored" : self.store_.keys(),
                }
        for name in self.save_params_:
            val = getattr(options,name,None)
            if val:
                cfg[name] = val

        print "--------------------------------------------------------------------------------------------------------------------------"
        print "saving output"
        print 
        ROOT.TObjString( json.dumps( cfg,indent=4,sort_keys=True) ).Write("cfg")
        
        nobjs = len(self.store_.keys())
        nprint = int(nobjs/20)
        if nprint == 0:
            nprint = 1
        iobj  = 0
        for key,val in self.store_.iteritems():
            val.CloneTree().Write(key,ROOT.TObject.kWriteDelete)
            iobj += 1
            if iobj % nprint == 1:
                print "written %d / %d trees" % ( iobj, nobjs )
        print "writing workspace ...",
        self.workspace_.Write()
        print "done"
        fout.Close()
        
        if self.rename_:
            os.rename( *self.rename_ )
        print "--------------------------------------------------------------------------------------------------------------------------"
        print

    ## ------------------------------------------------------------------------------------------------------------
    def rooImportItr(self,coll,verbose=False):        
        itr = coll.createIterator()
        if verbose:
            print "Importing collection to workspace"
            coll.Print()
        obj = itr.Next()
        while obj:
            if type(obj) == ROOT.TObject:
                ## FIXME: for some reason, instead of a null pointer, at the end of the iteration
                ##        a bare TObject is returned.... looks like a plain ROOT bug
                break          
            self.workspace_.rooImport(obj,ROOT.RooFit.RecycleConflictNodes(),ROOT.RooFit.Silence(not verbose))
            obj = itr.Next()
        
    ## ------------------------------------------------------------------------------------------------------------
    def mergeWs(self,options,read_ws):
        
        if os.path.dirname(read_ws) == "" and not os.path.exists(read_ws):
            print "Warning: %s does not exist. I will look for it in %s" % ( read_ws, options.ws_dir )
            fin = self.open(read_ws,folder=options.ws_dir)
        else:
            fin = self.open(read_ws)
        cfg = json.loads( str(fin.Get("cfg").GetString()) )
        for name,val in cfg["fits"].iteritems():
            options.fits[name] = val
        ws = fin.Get("wtemplates")

        if not self.workspace_:
            self.workspace_ = ws
            ## self.workspace_.rooImport = getattr(self.workspace_,"import")
            self.workspace_.rooImport = rooImport(self.workspace_)
        else:
            self.rooImportItr( ws.allVars(), verbose=options.verbose )
            self.rooImportItr( ws.allFunctions(), verbose=options.verbose )
            self.rooImportItr( ws.allPdfs(), verbose=options.verbose )
            
            alldata = ws.allData()
            for data in alldata:
                self.workspace_.rooImport(data)
            
        for name in cfg["stored"]:
            self.store_[name]=fin.Get(name)
            
        if not options.mix_templates:
            options.mix = cfg.get("mix",{})

        for name in self.save_params_:
            val = cfg.get(name,None)
            if val:
                print "Reading back saved parameter ", name # , val
                setattr(options,name,val)
    
    ## ------------------------------------------------------------------------------------------------------------
    def readWs(self,options,args):
        print
        print "--------------------------------------------------------------------------------------------------------------------------"
        print "Reading back workspace from %s " % options.read_ws
        print 
        ### fin = self.open(options.read_ws)
        ### cfg = json.loads( str(fin.Get("cfg").GetString()) )
        ### options.fits = cfg["fits"]
        ### self.workspace_ = fin.Get("wtemplates")
        ### self.workspace_.rooImport = getattr(self.workspace_,"import")
        ### for name in cfg["stored"]:
        ###     self.store_[name]=fin.Get(name)
        ###     
        ### if not options.mix_templates:
        ###     options.mix = cfg.get("mix",{})
        ### if not options.compare_templates:
        ###     options.comparisons = cfg.get("comparisons",{})
        ### 
        ### for name in self.save_params_:
        ###     val = cfg.get(name,None)
        ###     print name, val
        ###     if val:
        ###         setattr(options,name,val)
        
        for ws in options.read_ws_list:
            self.mergeWs(options,ws)
            
        
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
            
            self.workspace_input_ = WsList(self.workspace_)
            self.workspace_ = ROOT.RooWorkspace("wtemplates","wtemplates")
            ## self.workspace_.rooImport = getattr(self.workspace_,"import")
            self.workspace_.rooImport = rooImport(self.workspace_)
            
    
    
    ## ------------------------------------------------------------------------------------------------------------
    def prepareTemplates(self,options,args):
        
        fout = self.openOut(options)
        self.workspace_ = ROOT.RooWorkspace("wtemplates","wtemplates")
        tmp = fout
        ## read input trees
        self.datasets_["data"] = self.openDataset(None,options.data_file,options.infile,options.data)
        self.datasets_["templates"]   = self.openDataset(None,options.data_file,options.infile,options.templates)
        if not options.prep_data:
            self.datasets_["mc"]   = self.openDataset(None,options.mc_file,options.infile,options.mc)
            self.datasets_["templatesMC"]   = self.openDataset(None,options.mc_file,options.infile,options.templatesMC)
       
        if not (options.prep_data or options.prep_nosig):
            for name,trees in options.signals.iteritems():
                self.datasets_[name] = self.openDataset(None,options.mc_file,options.infile,trees)        
            # used by parent class PlotApp to read in objects
        self.template_ = options.treeName
        
        self.groups = options.groups
        if options.groups:
            self.categories_ = options.groups.keys()
        else:
            self.categories_ = options.categories
            
        ## create output workspace
        ## self.workspace_.rooImport = getattr(self.workspace_,"import")
        self.workspace_.rooImport = rooImport(self.workspace_)

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
            if name.startswith("_"): continue
            print
            print "--------------------------------------------------------------------------------------------------------------------------"
            print "Preparing fit %s" % name
            print 
            
            ndim            = fit["ndim"]
            bins            = fit["bins"]
            components      = fit["components"]
            categories      = fit["categories"]
            if not options.prep_data:
                truth_selection = fit["truth_selection"]
                if not options.prep_nosig:
                    signals         = fit.get("signals",[])
                    if signals == "__all__":
                        signals = options.signals.keys()
                        fit["signals"] = signals
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
            for cat in categories.keys():
                print "dataset - %s" % (cat), self.rooData("data_%s_%s" % (name,cat) ).sumEntries()
                print "number of entries data - %s" % (cat), self.rooData("data_%s_%s" % (name,cat) ).numEntries()
          ## prepare mc
            if not options.prep_data:
                mcTrees =  self.prepareTrees("mc",selection,options.verbose,"MC trees")
                self.buildRooDataSet(mcTrees,"mc",name,fit,categories,fulllist,weight,preselection,storeTrees)
          
          ## prepare signal
            if not (options.prep_data or options.prep_nosig):
                for sig in signals:
                    sigTrees =  self.prepareTrees(sig,selection,options.verbose,"Signal %s trees" % sig)
                    self.buildRooDataSet(sigTrees,sig,name,fit,categories,fulllist,weight,preselection,storeTrees)
            
          ## prepare truth templates
            if not options.prep_data:
                for truth,sel in truth_selection.iteritems():
                    cut = ROOT.TCut(preselection)
                    cut *= ROOT.TCut(sel)
                    legs = [""]
                    if "legs" in fit:
                        legs = fit["legs"]
                    self.buildRooDataSet(mcTrees,"mctruth_%s" % truth,name,fit,categories,fulllist,weight,cut.GetTitle(),storeTrees)
          
              
            print
          ## sanity check
            if not options.prep_data:
                for cat in categories.keys():
                    catCounts = {}
                    catCounts["tot"] = self.rooData("mc_%s_%s" % (name,cat) ).sumEntries()
                  
                    breakDown = 0.
                    for truth in truth_selection.keys():
                        count = self.rooData("mctruth_%s_%s_%s" % (truth,name,cat) ).sumEntries()
                        breakDown += count
                        catCounts[truth] = count
                    print "truth : ",cat, " ".join( "%s : %1.4g" % (key,val) for key,val in catCounts.iteritems() ),
                    if breakDown != catCounts["tot"]:
                        print "\n   Warning : total MC counts don't match sum of truths. Difference: ", catCounts["tot"]-breakDown
                    else:
                        print
                      
            ## prepare templates
            print 
            for component,cfg in fit["templates"].iteritems():
                if component.startswith("_"): continue
                # templates (data) is default one
                if options.prep_data:
                    datasets=cfg.get("dataset",["templates"])
                else: 
                    datasets=cfg.get("datasetmc",["templates"])  
                for dat in datasets:
                    print dat
                    trees = self.prepareTrees(dat,cfg["sel"],options.verbose,"Templates selection for %s %s" % (dat,component))
                    if dat=="data" or dat=="templates":
                        dat="_"
                    if not options.prep_data and dat=="templatesMC" or dat=="mc":
                        dat="_mc_"
                    cats = {}
                    presel = cfg.get("presel",preselection)
                    for cat,fill in cfg["fill_categories"].iteritems():
                        if cat.startswith("_"): continue
                        config = { "src" : categories[cat]["src"],
                                   "fill": fill
                                   }
                        cats[cat] = config
                    
                        
                        self.buildRooDataSet(trees,"template%s%s" % (dat,component),name,fit,cats,fulllist,weight,presel,storeTrees)

                    for cat in categories.keys():
                        tree=self.treeData("template%s%s_%s_%s" % (dat,component,name,cat) )
                        jk=cfg.get("jk",0) 
                        if jk !=0 and component=="pp" and options.prep_data:
                            n= int(tree.GetEntries())
                            d=n/jk
                            g=jk
                            if n % d != 0:
                                g += 1
                            g=int(g)
                            print "computing partitions: n=%d d=%d g=%i" % (n,d,g)
                            all_events= range(n)
                            random.shuffle(all_events)
                            for j in range(g):
                                lo=int(1+d*j)
                                hi=int(d+d*j)
                                print lo, hi
                                tree_temp=tree.CloneTree(0)
                                tree_temp.SetName("tree_template%s%s_%i_%s_%s" %(dat,component,j,name,cat))
                                for k in all_events:
                                    if not(k>= lo and k < hi ): tree_temp.Fill(tree.GetEntry(k))
                                self.store_[tree_temp.GetName()] = tree_temp
                                print"template%s%s_%i_%s_%s" % (dat,component,j,name,cat), self.rooData("template%s%s_%i_%s_%s" % (dat,component,j,name,cat),autofill=True,cloneFrom="template%s%s_%s_%s" % (dat,component,name,cat)).sumEntries()
                
                        else:
                            print "template -%s - %s" % (component,cat), self.rooData("template%s%s_%s_%s" % (dat,component,name,cat) ).sumEntries()
                            print "number of entries template %s - %s" % (component,cat), self.rooData("template%s%s_%s_%s" % (dat,component,name,cat) ).numEntries()
                    print 
                    print "--------------------------------------------------------------------------------------------------------------------------"
                    print
                
        if options.mix_templates:
            self.doMixTemplates(options,args)

        self.saveWs(options,fout)
    
    ## ------------------------------------------------------------------------------------------------------------
    def mixTemplates(self,options,args):
        fout = self.openOut(options)
        ## fout.Print()
        fout.cd()
        self.doMixTemplates(options,args)
        self.saveWs(options,fout)
    
    ## ------------------------------------------------------------------------------------------------------------
    def doMixTemplates(self,options,args):
        
        print
        print "--------------------------------------------------------------------------------------------------------------------------"
        print "Mixing templates "
        print 
        
        for name, mix in options.mix.iteritems():
            if name.startswith("_"): continue
            if not options.mix_mc and name.startswith("kDSinglePho2DMC"): continue
            print
            print "--------------------------------------------------------------------------------------------------------------------------"
            print "Mixing %s" % name
            print 

            targetName      = mix["target"]
            targetSrc       = "data"
            jks              = mix.get("jk_source",0)
            if ":" in targetName:
                targetName, targetSrc = targetName.split(":")
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
                if cat.startswith("_"): continue
                print
                print "Filling category :", cat
                for comp,source in sources.iteritems():
                    legs = []
                    legnams = []
                    print
                    print "Component :", comp
                    for leg,src in zip(fill["legs"],source):
                        if len(src) == 2:
                            sname,scomp = src
                            if "mc" in src:
                                stype = "template_mc"
                            else:
                                stype = "template"
                        else:
                            sname,stype,scomp = src
                        legname = "%s_%s_%s_%s" % (stype,scomp,sname,leg)
                        legnams.append( legname )
                        legs.append( (self.treeData(legname),ROOT.RooArgList(self.dsetVars(legname)) ) )
                        if jks !=0 and scomp!="p":
                            tree_all=self.treeData(legname)
                            n= int(tree_all.GetEntries())
                            d=n/jks
                            g=jks
                            if n % d != 0:
                                g += 1
                            g=int(g)
                            print "computing partitions: n=%d d=%d g=%i" % (n,d,g)
                            all_events= range(n)
                            random.shuffle(all_events)
                            for j in range(g):
                                lo=int(1+d*j)
                                hi=int(d+d*j)
                                tree_temp=tree_all.CloneTree(0)
                                tree_temp.SetName("%s_%i" %(legname,j))
                                for k in all_events:
                                    if not(k>= lo and k < hi ): tree_temp.Fill(tree_all.GetEntry(k))
                                legnams.append( tree_temp.GetName() )
                                legs.append( ( tree_temp,ROOT.RooArgList(self.dsetVars(legname)) ) )
                    if len(legs) != ndim and jks==0:
                        sys.exit(-1,"number of legs does not match number of dimensions for dataset mixing")
                    rndswap     = fill.get("rndswap",False)
                    rndmatch     = fill.get("rndmatch",0.)
                    
                    print "legs  :", " ".join(legnams)
                    print "type   :", mixType
                    if jks==0: g=0
                    for j in range(g+1):
                        if j==0:
                            (tree1, vars1)= legs[0]
                            (tree2, vars2)  = legs[g+1]
                        if len(legs)==ndim*g+2:
                            (tree1, vars1)= legs[j]
                            (tree2, vars2)  = legs[g+1+j]
                        elif len(legs) <ndim*g+2:
                            (tree1, vars1)= legs[0]
                            (tree2, vars2)  = legs[j+1]
                        else: "legs not correctly assigned to trees"
                        print tree1, tree2
                        print "----------------"
                    for j in range(0,g+1):
                        #variables to keep from target after mixing
                        constVariables        = mix["transfer_variables"]
                        varsT=ROOT.RooArgList()
                        for element in constVariables:
                            el = self.buildRooVar(*(self.getVar(element)))
                            varsT.add(el)
                        
                        
                        if mixType == "simple":
                            maxEvents   = fill.get("maxEvents",-1)
                            matchEffMap = fill.get("matchEff",{})
                            matchEff    = matchEffMap.get(comp,1.)
                            print "maxEvents :", maxEvents, "rndswap :", rndswap, "mathcEffMap"
                            mixer.fillFromTree(tree1,tree2,pt,eta,phi,energy,pt,eta,phi,energy,matchVars,rndswap,maxEvents,matchEff)
                            
                        elif mixType == "kdtree":
                            targetCat           = fill.get("targetCat",cat)
                            targetFraction      = fill.get("targetFraction",0.)
                            jkt                 = mix.get("jk_target",0)
                            nNeigh              = fill.get("nNeigh",10)
                            nMinNeigh           = fill.get("nMinNeigh",nNeigh)
                            useCdfDistance      = fill.get("useCdfDistance",False)
                            matchWithThreshold  = fill.get("matchWithThreshold",False)
                            targetWeight        = fill.get("targetWeight","weight")
                            maxWeightTarget     = fill.get("maxWeightTarget",0.)
                            maxWeightCache      = fill.get("maxWeightCache",0.)
                            dataname            = "%s_%s_%s" % (targetSrc,targetName,targetCat)       
                            print dataname
                            targets     = []
                            # only if single photon datasets the full ones
                            targets.append(self.treeData(dataname))
                            if j==0 and jkt!=0:
                                target_all          = self.treeData(dataname)
                                nt= int(target_all.GetEntries())
                                dt=nt/jkt
                                gt=jkt
                                if nt % dt != 0:
                                    gt += 1
                                gt=int(gt)
                                print "computing partitions for target: nt=%d dt=%d gt=%i" % (nt,dt,gt)
                                all_eventst= range(nt)
                                random.shuffle(all_eventst)
                                for jt in range(gt):
                                    lot=int(1+dt*jt)
                                    hit=int(dt+dt*jt)
                                    tree_tempt=target_all.CloneTree(0)
                                    tree_tempt.SetName("%s_%i" %(dataname,jt))
                                    for kt in all_eventst:
                                        if not(kt>= lot and kt < hit ): tree_tempt.Fill(target_all.GetEntry(kt))
                                    targets.append( tree_tempt )

                                #  target          = self.treeData(dataname)
                        print targets
                        
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
                        axesWeights     = fill.get( "axesWeights", [1.]*len(fill["match1"]) )
                        print "rndswap :", rndswap, " rndmatch :", rndmatch," useCdfDistance :", useCdfDistance, "matchWithThreshold :", matchWithThreshold
                        print "nNeigh :", nNeigh, "nMinNeigh :", nMinNeigh
                        print "axesWeights :", axesWeights
                        m=0
                        for target in targets:

                            print "------------------------------------------"
                            print "target :", target.GetName()
                            if      m==0 and j==0:   mixername="template_mix_%s_%s_%s" % ( comp,name, cat)
                            elif    m>0 and j==0:    mixername="template_mix_%s_%s_%i_%s" % ( comp,name, m-1,cat)
                            elif    m==0 and j>0:    mixername="template_mix_%s_%i_%s_%s" % ( comp,j-1,name,cat)
                            mixer = ROOT.DataSetMixer( mixername,mixername,
                                                   vars1, vars2, varsT,replace, replace,
                                                   ptLeadMin, ptSubleadMin, massMin,
                                                   "weight", "weight", True,                                               
                                                   )
                            mixer.fillLikeTarget(target,targetMatch1,targetMatch2,targetWeight,tree1,tree2,
                                             pt,eta,phi,energy,pt,eta,phi,energy,
                                             matchVars1,matchVars2,rndswap,rndmatch,nNeigh,nMinNeigh,targetFraction,
                                             useCdfDistance,matchWithThreshold, maxWeightTarget,maxWeightCache,
                                             array.array('d',axesWeights))
                    
                            dataset = mixer.get()
                            dataset.GetName()
                            self.workspace_.rooImport(dataset,ROOT.RooFit.RecycleConflictNodes())
                            tree = mixer.getTree()
                            print tree.GetName()
                            print "------------------------------------------"
                            print "------------------------------------------"
                            self.store_[tree.GetName()] = tree
                            m=m+1

                print 
                print "--------------------------------------------------------------------------------------------------------------------------"
                print 


    ## ------------------------------------------------------------------------------------------------------------
    def bookNewWs(self,keepOld=True):
        if keepOld:
            self.workspace_input_.append(self.workspace_)
        self.workspace_ = ROOT.RooWorkspace("wtemplates","wtemplates")
        ## self.workspace_.rooImport = getattr(self.workspace_,"import")
        self.workspace_.rooImport = rooImport(self.workspace_)
        
    
    ## ------------------------------------------------------------------------------------------------------------
    def setAliases(self,tree):
        """ Define all aliases in tees
        """
        for var,vdef in self.aliases_.iteritems():
            tree.SetAlias(var,vdef)
    

    ## ------------------------------------------------------------------------------------------------------------
    def reducedRooData(self,name,rooset,binned=False,weight="weight",sel=None,redo=False,importToWs=True):
        data = self.rooData("reduced_%s" % name,quiet=True)
        if not data or redo:
            data = self.rooData(name,rooset=rooset,weight=weight,sel=sel,redo=redo)
            if binned:
                data = data.binnedClone("reduced_%s" % name,"reduced_%s" % name)
            else:
                data.SetName("reduced_%s" % name)
        if importToWs:
            self.workspace_.rooImport(data)
        return data

    ## ------------------------------------------------------------------------------------------------------------
    def rooPdf(self,name,importToWs=False):
        pdf = self.workspace_.pdf(name)
        if not pdf and self.store_new_:
            pdf = self.workspace_input_.pdf(name)            
        if importToWs:
            self.workspace_.rooImport(pdf)
        return pdf

    ## ------------------------------------------------------------------------------------------------------------
    def rooFunc(self,name):
        rooHistFunc = self.workspace_.function(name)
        if not rooHistFunc and self.store_new_:
            rooHistFunc = self.workspace_input_.function(name)            
        if not rooHistFunc:
            print "Warning failed to read %s" % name
            self.workspace_.Print()
            self.workspace_input_.Print()
        return rooHistFunc



    ## ------------------------------------------------------------------------------------------------------------
    def rooData(self,name,autofill=True,rooset=None,weight="weight",sel=None,redo=False,quiet=False,cloneFrom=False):
            
        if name in self.cache_ and not redo:
            return self.cache_[name]        
        dataset = self.workspace_.data(name)
        if not dataset and self.store_new_:
            dataset = self.workspace_input_.data(name)
            if self.store_inputs_ and dataset:
                self.workspace_.rooImport(dataset)
                
        if not dataset and cloneFrom:
            origin=self.rooData(cloneFrom)
            dataset=origin.emptyClone()
            dataset.SetName(name)
            self.workspace_.rooImport(dataset)
        if not dataset:
            if not quiet:
                print "warning : dataset %s not found" % name
                self.workspace_.Print()
                self.workspace_input_.Print()
            return dataset

        if autofill and dataset.sumEntries() == 0.:
            tree = self.treeData(name)
            if not tree: 
                return dataset
            if rooset:
                dataset = dataset.reduce(RooFit.SelectVars(rooset))
            else:
                dataset = dataset.emptyClone()                
            if not redo:
                self.cache_[name] = dataset
            filler = ROOT.DataSetFiller(dataset)
            cut=ROOT.TCut(weight)
            if sel:
                cut *=sel
            filler.fillFromTree(tree,cut.GetTitle(),True)          
        elif sel or rooset:
            if rooset and sel:
                dataset = dataset.reduce(rooset,sel)
            elif rooset:
                dataset = dataset.reduce(rooset)
            elif sel:
                dataset = dataset.reduce(sel)
        
        return dataset

    ## ------------------------------------------------------------------------------------------------------------
    def treeData(self,name):
        if "tree_%s" % name in self.store_:
            return self.store_["tree_%s" % name]
        elif self.store_new_ and "tree_%s" % name in self.store_input_:
            rt = self.store_input_["tree_%s" % name]
            if self.store_inputs_:
                self.store_["tree_%s" % name] = rt
            return rt
        return None
        
    ## ------------------------------------------------------------------------------------------------------------
    def dsetVars(self,name):
        st = self.workspace_.set("variables_%s" %name)
        if not st and self.store_new_:
            st = self.workspace_input_.set("variables_%s" %name)
            if self.store_inputs_:
                self.workspace_.defineSet("variables_%s" %name,st)
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
    def rooVar(self,name):
        rooVar = self.workspace_.var(name)
        if not rooVar and self.store_new_:
            rooVar = self.workspace_input_.var(name)
        return rooVar
        
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
            val = None
            if len(binning)==1 or len(binning)>2 and binning[0]>=min(binning[1:]):
                rooVar.setVal(binning[0])
                binning = binning[1:]
            else:
                rooVar.setVal(0.5*(binning[0]+binning[-1]))
            if len(binning) > 1:
                rooVar.setMin(binning[0])
                rooVar.setMax(binning[-1])

                rooVar.setBinning(ROOT.RooBinning(len(binning)-1,array.array('d',binning)))
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

#  LocalWords:  workspaces


