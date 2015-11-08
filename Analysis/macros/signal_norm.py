#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, sys, glob, json

import itertools

from pprint import pprint

from auto_plotter import getObjects
from copy import deepcopy
                
def scaleGraph(graph,scale):
    graph = graph.Clone()
    graph.GetListOfFunctions().Clear()
    ## graph.Print()
    
    xvals = graph.GetX()
    yvals = graph.GetY()
    yerrl = graph.GetEYlow()
    yerrh = graph.GetEYhigh()
    for ip in xrange(graph.GetN()):
        scl = scale(xvals[ip]) 
        ## print scl
        graph.SetPoint( ip, xvals[ip], yvals[ip]*scl )
        try:
            graph.SetPointEYlow( ip, yerrl[ip]*scl )
            graph.SetPointEYhigh( ip, yerrh[ip]*scl )
        except:
            pass
    
    ## graph.Print()
    
    return graph

def divideMap(num,den):
    for key in  num.keys(): 
        divby = den(key)
        if num[key] != 0. and divby != 0: 
            num[key] /= den(key)
        else:
            num[key] = 0.
    return None

def makeMap(sels,coups,masses,cats=None):
    ret = {}
    for sel in sels:
        ret[sel] = {}
        for coup in coups:
            ret[sel][coup] = {}
            if cats:
                for cat in cats:
                    ret[sel][coup][cat] = dict.fromkeys(masses,0.)
            else:
                ret[sel][coup] = dict.fromkeys(masses,0.)
    return ret

def fitFunc(graph,func):
    ## func = func.Clone()
    graph.Fit(func)
    return func.Clone()

def addTo(dest,src,doCat=False):


    dest =dest[src[0][1]]
    if doCat: 
        dest=dest[src[1]]

    key = src[0][0]
    dest[key] += src[2]
    


# -----------------------------------------------------------------------------------------------------------
class SignalNorm(PlotApp):

    def __init__(self):
        super(SignalNorm,self).__init__(option_list=[
                make_option("-x","--x-sections",action="callback", dest="x_sections", type="string", callback=optpars_utils.Load(),
                            default={}),                                
                make_option("--plot-xsections",action="store_true", dest="plot_xsections", 
                            default=False),
                make_option("--plot-acceptance",action="store_true", dest="plot_acceptance", 
                            default=False),
                make_option("--reco-file",action="store", dest="reco_file", type="string",
                            default=None),
                make_option("--gen-file",action="store", dest="gen_file", type="string",
                            default=None),
            ])
        
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils

        self.remap_ = { "EB"       : "EBEB",
                        "EBHighR9" : "EBEB",
                        "EBLowR9"  : "EBEB",
                        "EE"       : "EBEE",
                        "EEHighR9" : "EBEE",
                        "EELowR9"  : "EBEE",
                        }

        self.masses_ = set()
        self.coups_  = set()
        self.cats_   = set()

    def __call__(self,options,args):
        self.loadRootStyle()
        
        self.loadXsections(options.x_sections)

        if options.plot_xsections:
            self.plotXsections()

        if options.plot_acceptance:
            self.plotAcceptance()

        self.autosave()
        
    def loadXsections(self,inmap):
        self.xsections_ = {}
        for name,val in inmap.iteritems():
            if name.startswith("RSGravToGG") or name.startswith("RSGravitonToGG"):
                coup,mass = name.split("kMpl")[1].split("_Tune")[0].replace("_","").replace("-","").split("M")
                mass = float(mass)
                if not coup in self.xsections_:
                    self.xsections_[coup] = ROOT.TGraph()
                self.xsections_[coup].SetPoint(self.xsections_[coup].GetN(),mass,val["xs"])
        for name,val in self.xsections_.iteritems():
            val.Sort()

    def getIntegrals(self,fin,sels):
        folders = getObjects( getObjects( [fin], sels ), ["histograms"] )
        histograms = map(lambda x: (os.path.basename(os.path.dirname(x.GetPath())), 
                                    filter(lambda y: "genmass" in y.GetName() and "Grav" in y.GetName(),  getObjects([x]))),
                         folders )
        
        return map(lambda x: (x[0].replace("GenIso",""),map(lambda y: (self.getMassAndCoup(y.GetName()),self.getCategory(y.GetName()),y.Integral()), x[1])), histograms )


    def getCategory(self,name):
        
        cat = name.replace("genmass","").rsplit("_")[-1]
        cat = self.remap_.get(cat,cat)
        self.cats_.add(cat)
        
        return cat
    
    def getMassAndCoup(self,name,numCoup=False):
        coup,mass = name.split("kMpl")[1].split("_Tune")[0].replace("_","").replace("-","").split("M")
        mass = float(mass)
        if numCoup:
            coup = float("0."+coup[1:])
            
        self.coups_.add(coup)
        self.masses_.add(mass)
        return mass,coup

    
    def plotGraphs(self,graphs,name,rng=[0.,.8]):
        canv = ROOT.TCanvas(name,name)
        graphs[0].Draw("ap")
        if rng:
            graphs[0].GetYaxis().SetRangeUser(*rng)
        for g in graphs[1:]: g.Draw("p")
        leg = ROOT.TLegend(0.54,0.28,0.84,0.66)
        for g in graphs:
            leg.AddEntry(g,"","lp")
        leg.Draw("same")
        self.keep( graphs, True )
        self.keep(leg,True)
        self.keep(canv)
    
    def setGraphTitle(self,gr):
        name = gr.GetName()
        ytit = ""
        if "eff_acc" in name:
            ytit = "#varepsilon #otimes A"
        elif "eff" in name :
            ytit = "(#varepsilon #otimes A) / A"
        elif "acc" in name :
            ytit = "A"
        print name, ytit
            
        toks = name.split("_")
        print toks
        if toks[0].startswith("cic"):
            tit = toks[0].replace("cic","")+" "
        else:
            tit = ""
        cat = toks[-1]
        if cat.startswith("0"):
            coup = cat
            cat = None
        else:
            coup = toks[-2]
        if coup.startswith("0"):
            tit += "k = 0.%s" % coup[1:]
        if cat: tit += " "+cat
        gr.SetTitle(tit)
        gr.GetXaxis().SetTitle("m_{G}")
        gr.GetYaxis().SetTitle(ytit)

    def mapToGraph(self,name,inp):
        points = filter( lambda x: x[1] != 0., list(inp.iteritems()) )
        if len(points) == 0: return None
        points = sorted(points,key=lambda x: x[0])
    
        graph = ROOT.TGraph()
        graph.SetName(name)
        map( lambda x: graph.SetPoint(graph.GetN(),x[0],x[1]), points) 
        fit = ROOT.TF1(name,"pol2")
        self.keep( fit, True )
        graph.Fit(fit)
        self.keep( graph, True )
        
        return graph
        
    
    def getGraphs(self,inputs,name,doCats=False):
        graphs = []
        for sel,sel_vals in inputs.iteritems():
            for coup,coup_vals in sel_vals.iteritems():
                if doCats:
                    for cat,vals in coup_vals.iteritems():
                        graph = self.mapToGraph("%s_%s_%s_%s" %( sel, name, coup, cat) ,vals)
                        if graph:
                            graphs.append(graph)
                else:
                    graph = self.mapToGraph("%s_%s_%s" %( sel, name, coup) ,coup_vals)
                    if graph:
                        graphs.append(graph)
        return graphs


    def plotAcceptance(self):
        gen_fin = self.open(self.options.gen_file)
        gen_integrals=self.getIntegrals(gen_fin,["genGenIso"])

        reco_fin = self.open(self.options.reco_file)
        reco_integrals=self.getIntegrals(reco_fin,["cic","cicNoChIso"])
        
        gen_sels   = set(map(lambda x: x[0], gen_integrals))
        reco_sels   = set(map(lambda x: x[0], reco_integrals))
        
        cat_gen = makeMap(gen_sels,self.coups_,self.masses_,self.cats_)
        gen = makeMap(gen_sels,self.coups_,self.masses_)

        cat_reco = makeMap(reco_sels,self.coups_,self.masses_,self.cats_)
        reco = makeMap(reco_sels,self.coups_,self.masses_)
        
        ## cat_gen,gen = {},{}
        ## for sel in gen_sels:
        ##     cat_gen[sel] = {}
        ##     gen[sel] = {}
        ##     for coup in self.coups_:
        ##         cat_gen[sel][coup] = {}
        ##         gen[sel][coup] = {}
        ##         for cat in self.cats_:
        ##             cat_gen[sel][coup][cat] = dict.fromkeys(self.masses_,0.)
        ##         gen[sel][coup] = dict.fromkeys(self.masses_,0.)
        
            
        map( lambda x: map(lambda y: (addTo(gen[x[0]],y), addTo(cat_gen[x[0]],y,True)), x[1]), gen_integrals  )
        map( lambda x: map(lambda y: (addTo(reco[x[0]],y), addTo(cat_reco[x[0]],y,True)), x[1]), reco_integrals  )

        pprint( cat_reco )
        eff_reco = deepcopy(cat_reco)
        eff_acc_reco = deepcopy(cat_reco)
        eff_acc_tot_reco = deepcopy(reco)
        gen_acc = deepcopy(cat_gen)
        gen_acc_tot = deepcopy(gen)
        
        [ divideMap( eff_reco[sel][coup][cat],     lambda x: cat_gen["gen"][coup][cat][x]       ) for sel,coup,cat in itertools.product(reco_sels,self.coups_,self.cats_) ]
        [ divideMap( eff_acc_reco[sel][coup][cat], lambda x: self.xsections_[coup].Eval(x)*1e+3 ) for sel,coup,cat in itertools.product(reco_sels,self.coups_,self.cats_) ]

        [ divideMap( gen_acc[sel][coup][cat], lambda x: self.xsections_[coup].Eval(x)*1e+3 ) for sel,coup,cat in itertools.product(gen_sels,self.coups_,self.cats_) ]
        [ divideMap( gen_acc_tot[sel][coup],  lambda x: self.xsections_[coup].Eval(x)*1e+3 ) for sel,coup in itertools.product(gen_sels,self.coups_) ]
        
        [ divideMap( eff_acc_tot_reco[sel][coup], lambda x: self.xsections_[coup].Eval(x)*1e+3 ) for sel,coup in itertools.product(reco_sels,self.coups_) ]
        
        ## pprint( self.xsections_ )
        ## pprint( cat_gen )
        ## pprint( gen )
        ## pprint( gen_acc )
        ## pprint( eff_acc_reco )


        self.setStyle("*acc*",[["SetMarkerSize",1.5],self.setGraphTitle])
        self.setStyle("*eff*",[["SetMarkerSize",1.5],self.setGraphTitle])
        ## self.setStyle("*gen*",[["SetTitle",";m_{G}(GeV);A"]])
        ### self.setStyle("*eff_reco*",[["SetTitle",";m_{G}(GeV);(#varepsilon #otimes A )/ A"]])
        ### self.setStyle("*eff_acc*",[["SetTitle",";m_{G}(GeV);#varepsilon #otimes A"]])
        self.setStyle("*EBEB",[["colors",ROOT.kRed]])
        self.setStyle("*EBEE",[["colors",ROOT.kBlue]])
        coustyles = [("001",20),("005",21),("007",22),("01",23),("015",24),("02",25),("03",26),("035",27),("04",28),("06",29)]
        map( lambda x: self.setStyle("*gen*_%s" %x[0], [["SetMarkerStyle",x[1]]]), coustyles )
        map( lambda x: self.setStyle("*gen*_%s*" %x[0], [["SetMarkerStyle",x[1]]]), coustyles )
        
        self.setStyle("*cic_", [["SetLineStyle",1]])
        self.setStyle("*cicNoChIso_*", [["SetLineStyle",ROOT.kDashed]])
        
        self.setStyle("cic_avg*",        [["SetMarkerStyle",ROOT.kFullCircle]])
        self.setStyle("cicNoChIso_avg*", [["SetMarkerStyle",ROOT.kOpenCircle]])

        self.setStyle("*cic*_01",        [["SetMarkerStyle",ROOT.kFullCircle]])
        self.setStyle("*cicNoChIso*_01", [["SetMarkerStyle",ROOT.kOpenCircle]])
        self.setStyle("*cic*_02",        [["SetMarkerStyle",ROOT.kFullTriangleUp]])
        self.setStyle("*cicNoChIso*_02", [["SetMarkerStyle",ROOT.kOpenTriangleUp]])
        self.setStyle("*cic*_001",       [["SetMarkerStyle",ROOT.kOpenDiamond]])
        self.setStyle("*cicNoChIso*_001",[["SetMarkerStyle",ROOT.kFullDiamond]])

        self.setStyle("*cic*_01_*",        [["SetMarkerStyle",ROOT.kFullCircle]])
        self.setStyle("*cicNoChIso*_01_*", [["SetMarkerStyle",ROOT.kOpenCircle]])
        self.setStyle("*cic*_02_*",        [["SetMarkerStyle",ROOT.kFullTriangleUp]])
        self.setStyle("*cicNoChIso*_02_*", [["SetMarkerStyle",ROOT.kOpenTriangleUp]])
        self.setStyle("*cic*_001_*",       [["SetMarkerStyle",ROOT.kOpenDiamond]])
        self.setStyle("*cicNoChIso*_001_*",[["SetMarkerStyle",ROOT.kFullDiamond]])
        
        eff_reco = self.getGraphs(eff_reco,"eff_reco",True)
        eff_acc_reco = self.getGraphs(eff_acc_reco,"eff_acc_reco",True)
        eff_acc_tot_reco = self.getGraphs(eff_acc_tot_reco,"eff_acc_tot_reco",False)
        gen_acc = self.getGraphs(gen_acc,"gen_acc",True)
        gen_acc_tot = self.getGraphs(gen_acc_tot,"gen_acc_tot",False)

        sumg = { "cic" : { "EBEB" : {}, "EBEE" : {} }, "cicNoChIso" : {  "EBEB" : {}, "EBEE" : {} }  }
        for gr in eff_reco:
            toks = gr.GetName().split("_")
            sel = toks[0]
            cat = toks[-1]
            xvals = gr.GetX()
            yvals = gr.GetY()
            for ip in xrange(gr.GetN()):
                x,y = xvals[ip],yvals[ip]
                if not x in sumg: sumg[sel][cat][x] = [0.,0]
                sumg[sel][cat][x][0] += y
                sumg[sel][cat][x][1] += 1
        
        avg_eff_reco = []
        for sel,vals in sumg.iteritems():
            for cat, points in vals.iteritems():
                graph = ROOT.TGraph()
                graph.SetName("%s_avg_reco_eff_%s" % (sel,cat))
                print points
                for x,y in points.iteritems():
                    graph.SetPoint(graph.GetN(),x,y[0]/y[1])
                graph.Sort()
                fit = ROOT.TF1(graph.GetName(),"pol2")
                self.keep( [graph,fit], True )
                graph.Fit(fit)
                avg_eff_reco.append(graph)

        self.plotGraphs(avg_eff_reco,"avg_eff_reco",rng=[0.5,0.9])
        self.plotGraphs(eff_reco,"eff_reco",rng=[0.5,0.9])
        self.plotGraphs(eff_acc_reco,"eff_acc_reco")
        self.plotGraphs(eff_acc_tot_reco,"eff_acc_tot_reco")
        self.plotGraphs(gen_acc,"gen_acc",rng=[0.,0.9])
        self.plotGraphs(gen_acc_tot,"gen_acc_tot",rng=[0.2,0.9])
        
        ## self.keep( eff_reco, True )
        ## eff_reco_canv = ROOT.TCanvas("eff_reco","eff_reco")
        ## eff_reco[0].Draw("ap")
        ## eff_reco[0].GetYaxis().SetRangeUser(0.,1.)
        ## for g in eff_reco[1:]: g.Draw("p")
        ## self.keep(eff_reco_canv)
        ## 
        ## self.keep( eff_acc_reco, True )
        ## eff_acc_reco_canv = ROOT.TCanvas("eff_acc_reco","eff_acc_reco")
        ## eff_acc_reco[0].Draw("ap")
        ## eff_acc_reco[0].GetYaxis().SetRangeUser(0.,1.)
        ## for g in eff_acc_reco[1:]: g.Draw("p")
        ## self.keep(eff_acc_reco_canv)
        ## 
        ## self.keep( eff_acc_tot_reco, True )
        ## eff_acc_tot_reco_canv = ROOT.TCanvas("eff_acc_tot_reco","eff_acc_tot_reco")
        ## eff_acc_tot_reco[0].Draw("ap")
        ## eff_acc_tot_reco[0].GetYaxis().SetRangeUser(0.,1.)
        ## for g in eff_acc_tot_reco[1:]: g.Draw("p")
        ## self.keep(eff_acc_tot_reco_canv)
        ## 
        ## self.keep( gen_acc, True )
        ## gen_acc_canv = ROOT.TCanvas("gen_acc","gen_acc")
        ## gen_acc[0].Draw("ap")
        ## gen_acc[0].GetYaxis().SetRangeUser(0.,1.)
        ## for g in gen_acc[1:]: g.Draw("p")
        ## self.keep(gen_acc_canv)
        
        param_graphs = { "EBEB" : [ROOT.TGraph(), ROOT.TGraph(), ROOT.TGraph()], "EBEE" : [ROOT.TGraph(), ROOT.TGraph(), ROOT.TGraph()], }
        map( lambda x: (param_graphs[x[0]][0].SetPoint(param_graphs[x[0]][0].GetN(),x[1],x[2].GetParameter(0)),
                        param_graphs[x[0]][1].SetPoint(param_graphs[x[0]][1].GetN(),x[1],x[2].GetParameter(1)),
                        param_graphs[x[0]][2].SetPoint(param_graphs[x[0]][2].GetN(),x[1],x[2].GetParameter(2))), 
             map(lambda y:  (y.GetName().rsplit("_")[-1], float("0."+y.GetName().rsplit("_")[-2][1:]), y.GetListOfFunctions().At(0)), gen_acc ) )
        
        for cat, graphs in param_graphs.iteritems():
            for ip, gr in enumerate(graphs):
                gr.Sort()
                pcanv = ROOT.TCanvas("acc_%s_p%d"%(cat,ip),"acc_%s_p%d"%(cat,ip))
                gr.SetName(pcanv.GetName())
                gr.Draw()
                gr.Fit("pol2")
                self.keep( [gr,pcanv] )
             
        ## rescaled = map( lambda x: (x[0],scaleGraph(x[1], lambda y: 1./(x[2].Eval(y)) )), fit )
        save = {
            }
        for gr in avg_eff_reco+param_graphs["EBEB"]+param_graphs["EBEE"]:
            save[gr.GetName()] = [ gr.GetListOfFunctions().At(0).GetParameter(0), gr.GetListOfFunctions().At(0).GetParameter(1), gr.GetListOfFunctions().At(0).GetParameter(2) ]
        with open("acceptance.json","w+") as accept_file:
            accept_file.write(json.dumps(save,indent=4,sort_keys=True))
            accept_file.close()

        self.autosave(True)
        

    def plotXsections(self):
        coups = sorted( map( lambda x: (float("0."+x[0][1:]),x[1]), self.xsections_.iteritems() ), key=lambda x: x[0] )
        refc = coups[-4]
        print refc, coups
        scaled = map( lambda x: (x[0],scaleGraph(x[1], lambda y: refc[0]*refc[0]/((x[0]*x[0])*refc[1].Eval(y)))), coups )
        
        mypol = ROOT.TF1("mypol","[0]+[1]*(x-[2])**2")
        fit = map( lambda x: (x[0],x[1],fitFunc(x[1],mypol)),  scaled )
        
        rescaled = map( lambda x: (x[0],scaleGraph(x[1], lambda y: 1./(x[2].Eval(y)) )), fit )

        canv = ROOT.TCanvas("xsections","xsections")
        scaled[0][1].Draw("apl")
        for g in scaled[1:]: g[1].Draw("pl")
        print scaled
        self.keep( list(scaled) )
        self.keep(canv)

        recanv = ROOT.TCanvas("xsections_rescaled","xsections_rescaled")
        rescaled[0][1].Draw("apl")
        for g in rescaled[1:]: g[1].Draw("pl")
        print rescaled
        self.keep( list(rescaled) )
        self.keep(recanv)

        params = map( lambda x: (x[0], x[2].GetParameter(0), x[2].GetParameter(1), x[2].GetParameter(2)), fit  )
        
        param_graphs = ROOT.TGraph(), ROOT.TGraph(), ROOT.TGraph()
        map( lambda x: (param_graphs[0].SetPoint(param_graphs[0].GetN(),x[0],x[1]),
                        param_graphs[1].SetPoint(param_graphs[1].GetN(),x[0],x[2]),
                        (param_graphs[2].SetPoint(param_graphs[2].GetN(),x[0],x[3])) if x[0] != refc[0] else None), 
             params )
        for ip, gr in enumerate(param_graphs):
            gr.Sort()
            pcanv = ROOT.TCanvas("p%d"%ip,"p%d"%ip)
            gr.Draw()
            gr.Fit("pol2")
            self.keep( [gr,pcanv] )

        ### p0 = ROOT.TF1("p0","pol2")
        ### p0.SetParameters(1.09141,-0.0977154,-0.670345)
        ### 
        ### p1 = ROOT.TF1("p1","pol2")
        ### p1.SetParameters(-3.44266e-08,5.194e-08,2.02169e-07)
        ### 
        ### p2 = ROOT.TF1("p2","pol2")
        ### p2.SetParameters(2718.59,69.1401,-772.539)
        
        p0,p1,p2 = map( lambda x: x.GetListOfFunctions().At(0), param_graphs )

        ## refc[0] = 3
        equalized = map( lambda x: (x[0],scaleGraph(x[1], lambda y: refc[0]*refc[0]/((x[0]*x[0])*(p0.Eval(x[0]) + p1.Eval(x[0])*(y-p2.Eval(x[0]))**2)) )), coups )        

        eqcanv = ROOT.TCanvas("xsections_equalized","xsections_equalized")
        eqcanv.SetLogy()
        equalized[0][1].Draw("apl")
        for g in equalized[1:]: g[1].Draw("pl")
        self.keep( list(equalized) )
        self.keep(eqcanv)

        sumg = {}
        for gr in equalized:
            gr = gr[1]
            xvals = gr.GetX()
            yvals = gr.GetY()
            for ip in xrange(gr.GetN()):
                x,y = xvals[ip],yvals[ip]
                if not x in sumg: sumg[x] = [0.,0]
                sumg[x][0] += y
                sumg[x][1] += 1
                

        avcanv = ROOT.TCanvas("xsections_averaged","xsections_averaged")
        avcanv.SetLogy()
        averaged = ROOT.TGraph()
        for x,y in sumg.iteritems():
            averaged.SetPoint(averaged.GetN(),x,y[0]/y[1])
        averaged.Sort()
        averaged.Draw("apl")
        self.keep(averaged)
        self.keep(avcanv)
        
        xsec = {
            "ref" : refc[0],
            "xsec_p0"  : [ p0.GetParameter(0), p0.GetParameter(1), p0.GetParameter(2) ],
            "xsec_p1"  : [ p1.GetParameter(0), p1.GetParameter(1), p1.GetParameter(2) ],
            "xsec_p2"  : [ p2.GetParameter(0), p2.GetParameter(1), p2.GetParameter(2) ],
            "xsec" : [ (averaged.GetX()[i],averaged.GetY()[i]) for i in xrange(averaged.GetN()) ]
            }
        
        with open("xsections.json","w+") as xsec_file:
            xsec_file.write(json.dumps(xsec,indent=4,sort_keys=True))
            xsec_file.close()
            
        self.autosave(True)
        
        
# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app = SignalNorm()
    app.run()
