#!/bin/env python

from pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os

import array

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
    def __init__(self):
        """ 
        Constructor
        """
        super(TemplatesApp,self).__init__(option_groups=[
                ( "Fit definition options. Usually specified through JSON configuration. See documentation for more details.", [
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
                        make_option("--dataset-variables",dest="dataset_variables",action="callback",callback=optpars_utils.ScratchAppend(),type="string",
                                    default=[],help="List of variables to be added to dataets."),
                        make_option("--weight-expression",dest="weight_expression",action="store",type="string",
                                    default="",help="Expression used to define datasets weight."),
                        ]
                  ), ("General templates preparation options", [
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
                        ]
                      )
            ])
        
        ## initialize data members
        self.trees_ = {}
        self.datasets_ = {}
        self.aliases_ = {}
        self.variables_ = {}
        
        ## load ROOT (and libraries)
        global ROOT, style_utils
        import ROOT
        import pyrapp.style_utils as style_utils
        ROOT.gSystem.Load("libdiphotonsUtils")
    
    ## ------------------------------------------------------------------------------------------------------------
    def __call__(self,options,args):
        """ 
        Main method. Called automatically by PyRoot class.
        """
        ## load ROOT style
        self.loadRootStyle()
        from ROOT import RooFit

        printLevel = ROOT.RooMsgService.instance().globalKillBelow()
        ROOT.RooMsgService.instance().setGlobalKillBelow(RooFit.FATAL)

        
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
        self.workspace_ = ROOT.RooWorkspace("wtemplates","wtemplates")
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
            
            fulllist = varlist.Clone()
            for dim in range(ndim):
                dimVar = self.buildRooVar("template%sDim%d" % (name,dim),template_binning)
                fulllist.add( dimVar )
            print "Will put the following variables in the dataset : ",
            fulllist.Print()
            
            print "Number of dimentions : %d" % ndim
            print "Components           : %s" % ",".join(components)
            print 
            
            ## prepare data
            dataTrees = self.prepareTrees("data",options.selection,True,"Data trees")
            self.buildDatasets(dataTrees,"data",name,fit,categories,fulllist,weight,options.preselection)
            
            ## prepare mc
            mcTrees =  self.prepareTrees("mc",options.selection,True,"MC trees")
            self.buildDatasets(mcTrees,"mc",name,fit,categories,fulllist,weight,options.preselection)
            
            ## prepare truth templates
            for truth,sel in truth_selection.iteritems():
                cut = ROOT.TCut(options.preselection)
                cut *= ROOT.TCut(sel)
                legs = [""]
                if "legs" in fit:
                    legs = fit["legs"]
                self.buildDatasets(mcTrees,"mc_truth_%s" % truth,name,fit,categories,fulllist,weight,cut.GetTitle())
            
                        
            print
            ## sanity check
            for cat in categories.keys():
                catCounts = {}
                catCounts["tot"] = self.workspace_.data("mc_%s_%s" % (name,cat) ).sumEntries()
                
                breakDown = 0.
                for truth in truth_selection.keys():
                    count = self.workspace_.data("mc_truth_%s_%s_%s" % (truth,name,cat) ).sumEntries()
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
                trees = self.prepareTrees(dataset,cfg["sel"],True,"Templates selection for %s" % component)
                cats = {}
                for cat,fill in cfg["fill_categories"].iteritems():
                    config = { "src" : categories[cat]["src"],
                               "fill": fill
                               }
                    cats[cat] = config
                self.buildDatasets(trees,"template_%s" % component,name,fit,cats,fulllist,weight,options.preselection)
                for cat in categories.keys():
                    print "template %s - %s" % (component,cat), self.workspace_.data("template_%s_%s_%s" % (component,name,cat) ).sumEntries()
            print 
            print "--------------------------------------------------------------------------------------------------------------------------"
            print 
            

        self.workspace_.writeToFile("pippo.root")

    ## ------------------------------------------------------------------------------------------------------------
    def setAliases(self,tree):
        """ Define all aliases in tees
        """
        for var,vdef in self.aliases_.iteritems():
            tree.SetAlias(var,vdef)
    
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
    def buildRooVar(self,name,binning):
        rooVar = ROOT.RooRealVar(name,name,0.)
        if len(binning) > 0:
            rooVar.setMin(binning[0])
            rooVar.setMax(binning[-1])
            rooVar.setVal(0.5*(binning[0]+binning[-1]))
            rooVar.setBinning(ROOT.RooBinning(len(binning)-1,binning))
        self.workspace_.rooImport(rooVar)
        self.keep(rooVar) ## make sure the variable is not destroyed by the garbage collector
        return rooVar

    ## ------------------------------------------------------------------------------------------------------------
    def buildDatasets(self,trees,name,fitname,fit,categories,fulllist,weight,preselection):
        """ Build per-category RooDataSet starting from trees
        """
        ## fill datasets
        for catname,cfg in categories.iteritems():
            filler = ROOT.DataSetFiller( "%s_%s_%s" % (name,fitname,catname), "%s_%s_%s" % (name,fitname,catname), fulllist, weight )
            
            ## source category
            src = trees[cfg["src"]]
            
            ## filling directives
            fill  = cfg["fill"]
            legs = [""]
            if "legs" in fit:
                legs = fit["legs"]

            ## loop over directives 
            for cut,variables in fill.iteritems():
                # assume template vars are at the end
                firstVar = fulllist.getSize()-len(variables)
                ## loop over all legs
                for leg in legs:
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
            
            ## and we are done
            dataset = filler.get()
            self.workspace_.rooImport(dataset)
            ## dataset.Print()

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
