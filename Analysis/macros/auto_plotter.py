#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os

from pprint import pprint

# -----------------------------------------------------------------------------------------------------------
def getObjects(folders,names=None,types=None):
    
    objs = map( lambda x: x.ReadObj(), reduce( lambda x,y: x+y, map( lambda x: list(x.GetListOfKeys()), folders ) ) )
    if names:
        objs = filter( lambda x: x.GetName() in names, objs )
    
    if types:
        objs = filter( lambda x: x.ClassName() in types, objs )
        
    return objs

# -----------------------------------------------------------------------------------------------------------
def mktdir(parent,name):
    
    dirname = os.path.dirname(name)
    basename = os.path.basename(name)
    
    if dirname != "": 
        parent = mktdir(parent, dirname)

    ret = parent.Get(basename)
    if ret: return ret
    return parent.mkdir(basename)

# -----------------------------------------------------------------------------------------------------------
def bookhisto(folder,hdef,prefix):
    
    name, xvar, nxbins, xbins, yvar, nybins, yvar = hdef
    name = "".join([prefix,name])
    
    histo = folder.Get(name)
    if histo: return histo
    
    if nxbins == 0:
        xbins=(len(xbins)-1,array.array('d',*xbins))
    else:
        xbins=[nxbins,xbins[0],xbins[1]]

    pwd = ROOT.gDirectory
    folder.cd()
    if yvar:
        if nybins == 0:
            ybins=(len(ybins)-1,array.array('d',*ybins))
        else:
            ybins=[nybins,ybins[0],ybins[1]]
    
        histo = ROOT.TH2D(name,name,*(xbins+ybins))
    else:
        histo = ROOT.TH1D(name,name,*xbins)
    pwd.cd()
    return histo
        
# -----------------------------------------------------------------------------------------------------------
class AutoPlot(PyRApp):

    def __init__(self):
        super(AutoPlot,self).__init__(option_list=[
                make_option("--selection",action="callback", dest="selections", type="string", callback=optpars_utils.ScratchAppend(),
                            default=[]),
                make_option("--move",action="callback", dest="move", type="string", callback=optpars_utils.ScratchAppend(),
                            default=[]),
                make_option("--file",action="callback", dest="files", type="string", callback=optpars_utils.ScratchAppend(),
                            default=[]),
                make_option("--histograms",action="callback", dest="histograms", type="string", callback=optpars_utils.ScratchAppend(),
                            default=["mass>>mass(1500,0,15000)",
                                     "mass>>lowmass(560,60,200)",
                                     "genMass>>genmass(1500,0,15000)",            
                                     "pt>>pt(200,0,200)",
                                     "rapidity>>rapidity(200,-5,5)",
                                     "deltaEta>>deltaEta(200,0,5)",
                                     "cosDeltaPhi>>cosDeltaPhi(200,0,1)",
                                     "rho>>rho(20,0,50)",
                                     "nvtx>>nvtx(51,0.5,50.5)",
                                     
                                     "leadPt>>phoPt(150,0,3000)",
                                     "subleadPt>>phoPt(150,0,3000)",
                                     
                                     "leadPt>>leadPt(150,0,3000)",
                                     "subleadPt>>subleadPt(150,0,3000)",
                                     "leadEta>>leadEta(55,-2.75,2.75)",
                                     "subleadEta>>subleadEta(55,-2.75,2.75)",
                                     
                                     "leadChIso>>leadChIso(120,-10,50)",
                                     "leadPhoIso>>leadPhoIso(120,-10,50)",
                                     "leadNeutIso>>leadNeutIso(120,-10,50)",
                                     "leadHoE>>leadHoE(40,0,0.2)",
                                     "leadSigmaIeIe>>leadSigmaIeIe(320,0,3.2e-2)",
                                     "leadPixSeed>>leadPixSeed(2,-0.5,1.5)",
                                     "leadPassEleVeto>>leadPassEleVeto(2,-0.5,1.5)",
                                     
                                     "subleadChIso>>subleadChIso(120,-10,50)",
                                     "subleadPhoIso>>subleadPhoIso(120,-10,50)",
                                     "subleadNeutIso>>subleadNeutIso(120,-10,50)",
                                     "subleadHoE>>subleadHoE(40,0,0.2)",
                                     "subleadSigmaIeIe>>subleadSigmaIeIe(320,0,3.2e-2)",
                                     "subleadPixSeed>>subleadPixSeed(2,-0.5,1.5)",
                                     "subleadPassEleVeto>>subleadPassEleVeto(2,-0.5,1.5)",
                                     
                                     
                                     "leadChIso>>phoChIso(120,-10,50)",
                                     "subleadChIso>>phoChIso(120,-10,50)",
                                     "leadPhoIso>>phoPhoIso(120,-10,50)",
                                     "subleadPhoIso>>phoPhoIso(120,-10,50)",
                                     "leadSigmaIeIe>>phoSigmaIeIe(320,0,3.2e-2)",
                                     "subleadSigmaIeIe>>phoSigmaIeIe(320,0,3.2e-2)",
                                     "leadHoE>>phoHoE(40,0,0.2)",                                   
                                     "subleadHoE>>phoHoE(40,0,0.2)",                                   
                                     "leadPassEleVeto>>phoPassEleVeto(2,-0.5,1.5)",
                                     "subleadPassEleVeto>>phoPassEleVeto(2,-0.5,1.5)",
                                     
                                     
                                     "leadRndConeChIso0>>leadRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso1>>leadRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso2>>leadRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso3>>leadRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso4>>leadRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso5>>leadRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso6>>leadRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso7>>leadRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso8>>leadRndConeChIso(120,-10,50)",
                                     
                                     "subleadRndConeChIso0>>subleadRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso1>>subleadRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso2>>subleadRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso3>>subleadRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso4>>subleadRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso5>>subleadRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso6>>subleadRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso7>>subleadRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso8>>subleadRndConeChIso(120,-10,50)",
                                     
                                     "leadRndConeChIso0>>phoRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso1>>phoRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso2>>phoRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso3>>phoRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso4>>phoRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso5>>phoRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso6>>phoRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso7>>phoRndConeChIso(120,-10,50)",
                                     "leadRndConeChIso8>>phoRndConeChIso(120,-10,50)",
                                     
                                     "subleadRndConeChIso0>>phoRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso1>>phoRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso2>>phoRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso3>>phoRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso4>>phoRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso5>>phoRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso6>>phoRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso7>>phoRndConeChIso(120,-10,50)",
                                     "subleadRndConeChIso8>>phoRndConeChIso(120,-10,50)",
                                     ]),
                make_option("--cut",action="store", dest="cut", type="string",
                            default="1"),
                make_option("--weight",action="store", dest="weight", type="string",
                            default="weight"),
                make_option("--output",action="store", dest="output", type="string",
                            default="output.root"),
                
            ])
    
        global ROOT, style_utils, RooFit
        import ROOT
    
        self.histos = []

    # -------------------------------------------------------------------------------------------------------
    def mkTreeHistos(self,folder,tree):

        objects = map( lambda x: (x[1],x[4],bookhisto(folder,x,tree.GetName())), self.histos )
        return objects
        
    # -------------------------------------------------------------------------------------------------------
    def mkAllHistos(self,target):
        folder,trees = target
        
        return folder,map(lambda x: (x,self.mkTreeHistos(folder,x)), trees )
    
    # -------------------------------------------------------------------------------------------------------
    def fillAllHistograms(self,histos):
        return histos[0],map(lambda x: self.fillHistograms(*x), histos[1] )

    # -------------------------------------------------------------------------------------------------------
    def fillHistograms(self,tree,histos):
        allvars = list(set(filter( lambda x: x != None, reduce( lambda x,y: x+y,  map( lambda x: [x[0],x[1]], histos )))))
        ## tree.SetBranchStatus("*",0)
        myvars = { None: None }
        for v in allvars:
            myvars[v] = ROOT.TTreeFormula(v,v,tree)

        selection = ROOT.TTreeFormula(self.selection,self.selection,tree)
        weight = ROOT.TTreeFormula(self.weight,self.weight,tree)
        
        handy = map( lambda x: (myvars[x[0]],myvars[x[1]],x[2]), histos )
        print "Filling %s (%s entries) selection: %s" % (tree.GetName(), tree.GetEntries(), self.selection)
        for iev in xrange(tree.GetEntries()):
        ## for iev in xrange(1):
            tree.GetEntry(iev)  
            if selection.EvalInstance() == 0: 
                continue
            for h in handy:
                if h[1]:
                    h[2].Fill(h[0].EvalInstance(),h[1].EvalInstance(),weight.EvalInstance())
                else:
                    h[2].Fill(h[0].EvalInstance(),weight.EvalInstance())
        return map( lambda x: x[2], handy )

    def getDestination(self,origin):
        ret = self.destinations.get(origin,origin)
        return ret
    
    # -------------------------------------------------------------------------------------------------------
    def __call__(self,options,args):
        
        from flashgg.Taggers.dumperConfigTools import parseHistoDef
        self.histos = map( parseHistoDef, options.histograms )
        self.selection = options.cut
        self.weight = options.weight
        self.destinations = {}
        for mv in options.move: 
            mvTo = mv.split(":")
            self.destinations[mvTo[0]]=mvTo[1]
        print self.destinations
        
        output = self.open(options.output,"recreate")
        
        # open input files
        infiles = map( lambda x: self.open(x), options.files )
        # get <selection>/trees folders from input files
        inputs = getObjects( getObjects( infiles, options.selections ), ["trees"] )
        # book output folders
        outputs = map( lambda x: (mktdir(output,os.path.join(self.getDestination(os.path.basename(os.path.dirname(x.GetPath()))),"histograms")), getObjects([x],types=["TTree"])), inputs)
        # and histogras
        histograms = map(self.mkAllHistos, outputs )
        
        filled = map(self.fillAllHistograms, histograms)
        
        for folder,histos in filled:
            print folder.GetPath()
            folder.cd()
            for h in set(reduce(lambda x,y: x+y, histos)): 
                h.Write(h.GetName(),ROOT.TObject.kWriteDelete)
        
        output.Close()
        
# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app = AutoPlot()
    app.run()
