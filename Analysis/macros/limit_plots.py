#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, sys, glob, json

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

def fitFunc(graph,func):
    
    ## func = func.Clone()
    graph.Fit(func)
    return func.Clone()

# -----------------------------------------------------------------------------------------------------------
class LimitPlot(PlotApp):

    def __init__(self):
        super(LimitPlot,self).__init__(option_list=[
                make_option("--do-limits",action="store_true", dest="do_limits", 
                            default=False),
                make_option("--do-pvalues",action="store_true", dest="do_pvalues", 
                            default=False),
                make_option("--asimov-expected",action="store_true", dest="asimov_expected", 
                            default=True),
                make_option("--toys-expected",action="store_false", dest="asimov_expected", 
                            ),
                make_option("-M","--method",action="store", dest="method", 
                            default="Asymptotic"),                
                make_option("-U","--unblind",action="store_true", dest="unblind", 
                            default=False),                
                make_option("-B","--blind",action="store_false", dest="unblind", 
                            ),                
                make_option("-k","--couplings",action="callback", dest="couplings", type="string", callback=optpars_utils.ScratchAppend(str),
                            default=[]),                
                make_option("-x","--x-sections",action="callback", dest="x_sections", type="string", callback=optpars_utils.Load(),
                            default={}),                
                make_option("--fixed-x-section",action="store", dest="fixed_x_section", type="float", 
                            default=None), 
                
            ])
        
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils


    def __call__(self,options,args):
        self.loadRootStyle()
        
        # ROOT.gSystem.AddIncludePath( "$ROOTSYS/include" )
        ROOT.gROOT.LoadMacro( "$CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/plotting/bandUtils.cxx+" )
        
        self.loadXsections(options.x_sections)

        print options.couplings
        if len(options.couplings) == 0:
            flist = glob.glob("%s/higgsCombine_k*.%s.root" % (options.input_dir, options.method) )
        else:
            flist = [ "%s/higgsCombine_k%s.%s.root" % (options.input_dir, coup, options.method) for coup in options.couplings ]
        print options.input_dir, flist
            
        tflist = {}
        for fname in flist:
            bname = os.path.basename(fname)
            coup = bname.split("_",1)
            print coup
            coup = coup[1].split(".")
            print coup
            coup = coup[0].replace("k","")
            print coup
            ## coup = bname.split("_",1)[1].split(".")[0].replace("k","")
            tfin = self.open(fname)
            if not tfin: 
                print ("unable to open %s" % fname)
                sys.exit(-1)
            tree = tfin.Get("limit")
            if not tree: 
                print ("unable to find limit tree in %s" % fname)
                sys.exit(-1)
        
            tflist[coup] = tfin
        
        self.graphs = []
        for coup,tfile in tflist.iteritems():

            if options.do_limits:
                print coup, tfile
                self.plotLimit(options,coup,tfile)
            if options.do_pvalues:
                self.plotPval(options,coup,tfile)
                
        self.autosave()

        if len(options.couplings) == 0:
            graphs = self.open("%s/graphs_%s.root" % (options.input_dir,options.method),"recreate")
        else:
            graphs = self.open("%s/graphs_%s_%s.root" % (options.input_dir,"_".join(options.couplings),options.method),"recreate")
        graphs.cd()
        for gr in self.graphs: gr.Write()
        graphs.Close()
        
    def plotLimit(self,options,coup,tfile):
        ## TGraphAsymmErrors *theBand(TFile *file, int doSyst, int whichChannel, BandType type, double width=0.68) {
        if options.asimov_expected:
            ROOT.use_precomputed_quantiles = True
            bandType = ROOT.Median 
        else:
            bandType = ROOT.Median
        expected68 = ROOT.theBand( tfile, 1, 0, bandType, 0.68 )
        expected95 = ROOT.theBand( tfile, 1, 0, bandType, 0.95 )
        observed = ROOT.theBand( tfile, 1, 0, ROOT.Observed, 0.95 )
        basicStyle = [["SetMarkerSize",1],["SetLineWidth",3],
                       ["SetTitle",";M_{G} (GeV);95% C.L. limit #sigma(pp#rightarrow G#rightarrow#gamma#gamma) (pb)"]]
        commonStyle = [[self.scaleByXsec,coup],"Sort"]+basicStyle
        ## expectedStyle = commonStyle+[["SetMarkerStyle",ROOT.kOpenCircle]]
        expectedStyle = commonStyle+[["SetMarkerSize",0]]
        observedStyle = commonStyle+[["SetMarkerStyle",ROOT.kFullCircle]]
        
        style_utils.apply( expected68, [["colors",ROOT.kYellow],["SetName","expected68_%s"%coup]]+expectedStyle )
        style_utils.apply( expected95, [["colors",ROOT.kGreen],["SetName","expected95_%s"%coup]]+expectedStyle )
        
        expected = ROOT.TGraph(expected68)
        style_utils.apply( expected, [["colors",ROOT.kBlack],["SetLineStyle",7],["SetName","expected_%s"%coup]])
        
        style_utils.apply(observed,[["SetName","observed_%s"%coup]]+observedStyle)
      
        canv  = ROOT.TCanvas("limits_k%s"%coup,"limits_k%s"%coup)
        canv.SetLogx()
        legend = ROOT.TLegend(0.5,0.6,0.8,0.9)
        expected95.Draw("APE3")        
        expected95.GetXaxis().SetRangeUser(450,5500)
        expected95.GetXaxis().SetMoreLogLabels()
        expected68.Draw("E3PL")
        expected.Draw("PL")
        kappa = "0."+coup[1:]
        legend.AddEntry(None,"#tilde{#kappa} = %s" % kappa,"")
        legend.AddEntry(expected,"Expected limit","l")
        legend.AddEntry(expected68," \pm 1 \sigma","f")
        legend.AddEntry(expected95," \pm 2 \sigma","f")
        if options.unblind:
            observed.Draw("PL")
            legend.AddEntry(observed,"Observed limit","l")
        if coup in self.xsections_:
            grav = self.xsections_[coup]
            style_utils.apply( grav, basicStyle+[["SetLineStyle",9],["colors",ROOT.myColorB2]] )
            grav.Draw("L")
            legend.AddEntry(grav,"G_{RS}#rightarrow#gamma#gamma","l").SetLineStyle(0)
            
        self.keep(legend,True)
        legend.Draw()
        
        self.graphs.extend([observed,expected,expected68,expected95])
        
        self.keep( [canv,observed,expected,expected68,expected95] )

    def plotPval(self,options,coup,tfile):
        observed = ROOT.theBand( tfile, 1, 0, ROOT.Observed, 0.95 )
        basicStyle = [["SetMarkerSize",1],["SetLineWidth",3],
                       ["SetTitle",";M_{G} (GeV);p_{0}"]]
        commonStyle = ["Sort"]+basicStyle
        observedStyle = commonStyle+[["SetMarkerStyle",ROOT.kFullCircle],["colors",ROOT.kBlue]]
        
        style_utils.apply(observed,[["SetName","observed_%s"%coup]]+observedStyle)
      
        
        canv  = ROOT.TCanvas("pvalues_k%s"%coup,"pvalues_k%s"%coup)
        canv.SetLogy()
        canv.SetLogx()
        legend = ROOT.TLegend(0.5,0.6,0.8,0.75)
        kappa = "0."+coup[1:]
        observed.Draw("apl")
        observed.GetYaxis().SetRangeUser(1e-3,0.55)
        observed.GetXaxis().SetRangeUser(450,5500)
        observed.GetXaxis().SetMoreLogLabels()
        
        ## xmin,xmax=observed.GetXaxis().GetXmin(),observed.GetXaxis().GetXmax()
        xmin,xmax=450,5500
        spots = map(lambda x: (x,ROOT.RooStats.SignificanceToPValue(x)), xrange(1,3) )
        
        lines = map( lambda y: ROOT.TLine(xmin,y[1],xmax,y[1]), spots )
        map( lambda x: style_utils.apply(x,[["SetLineColor",ROOT.kGray+3],["SetLineStyle",7]]), lines )

        labels = map( lambda y: ROOT.TLatex(xmax*1.01,y[1]*0.9,"#color[%d]{%d #sigma}" % (ROOT.kGray+2,y[0])), spots )
        map( lambda x: style_utils.apply(x,[["SetTextSize",0.05]]), labels )

        map( lambda x: x.Draw("same"), lines+labels )
        self.keep(lines+labels)
        
        legend.AddEntry(None,"#tilde{#kappa} = %s" % kappa,"")
        legend.AddEntry(observed,"Observed p_{0}","l")
        
        self.keep(legend,True)
        legend.Draw()
        
        self.graphs.extend([observed])
        
        self.keep( [canv,observed] )

    def scaleByXsec(self,graph,coup):
        if self.options.fixed_x_section:
            scale = self.options.fixed_x_section
        xvals = graph.GetX()
        yvals = graph.GetY()
        yerrl = graph.GetEYlow()
        yerrh = graph.GetEYhigh()
        if not self.options.fixed_x_section:
            if not coup in self.xsections_:
                print("Cross section for k = %s not found" % coup)
                sys.exit(-1)
            else:
                xsec = self.xsections_[coup]
        for ip in range(graph.GetN()):
            if not self.options.fixed_x_section:
                scale = xsec.Eval(xvals[ip])
            graph.SetPoint( ip, xvals[ip], yvals[ip]*scale )
            graph.SetPointEYlow( ip, yerrl[ip]*scale )
            graph.SetPointEYhigh( ip, yerrh[ip]*scale )
        
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

    def plotXsections(self):
        coups = sorted( map( lambda x: (float("0."+x[0][1:]),x[1]), self.xsections_.iteritems() ), key=lambda x: x[0] )
        ## minc = min( map( lambda x: x[0], coups) )
        refc = coups[-4]
        print refc, coups
        scaled = map( lambda x: (x[0],scaleGraph(x[1], lambda y: refc[0]*refc[0]/((x[0]*x[0])*refc[1].Eval(y)))), coups )
        
        mypol = ROOT.TF1("mypol","[0]+[1]*(x-[2])**2")
        fit = map( lambda x: (x[0],x[1],fitFunc(x[1],mypol)),  scaled )
        
        rescaled = map( lambda x: (x[0],scaleGraph(x[1], lambda y: 1./(x[2].Eval(y)) )), fit )

        canv = ROOT.TCanvas("xsections","xsections")
        scaled[0][1].Draw("apl")
        # scaled[0].GetYaxis().SetRange(0,5)
        for g in scaled[1:]: g[1].Draw("pl")
        print scaled
        self.keep( list(scaled) )
        self.keep(canv)

        recanv = ROOT.TCanvas("xsections_rescaled","xsections_rescaled")
        rescaled[0][1].Draw("apl")
        # scaled[0].GetYaxis().SetRange(0,5)
        for g in rescaled[1:]: g[1].Draw("pl")
        print rescaled
        self.keep( list(rescaled) )
        self.keep(recanv)

        params = map( lambda x: (x[0], x[2].GetParameter(0), x[2].GetParameter(1), x[2].GetParameter(2)), fit  )
        
        param_graphs = ROOT.TGraph(), ROOT.TGraph(), ROOT.TGraph()
        map( lambda x: (param_graphs[0].SetPoint(param_graphs[0].GetN(),x[0],x[1]),param_graphs[1].SetPoint(param_graphs[1].GetN(),x[0],x[2]),param_graphs[2].SetPoint(param_graphs[2].GetN(),x[0],x[3])), params )
        for ip, gr in enumerate(param_graphs):
            gr.Sort()
            pcanv = ROOT.TCanvas("p%d"%ip,"p%d"%ip)
            gr.Draw()
            self.keep( [gr,pcanv] )

        p0 = ROOT.TF1("p0","pol2")
        p0.SetParameters(1.09141,-0.0977154,-0.670345)

        p1 = ROOT.TF1("p1","pol2")
        p1.SetParameters(-3.44266e-08,5.194e-08,2.02169e-07)

        p2 = ROOT.TF1("p2","pol2")
        p2.SetParameters(2718.59,69.1401,-772.539)
        
        ## refc[0] = 3
        equalized = map( lambda x: (x[0],scaleGraph(x[1], lambda y: refc[0]*refc[0]/((x[0]*x[0])*(p0.Eval(x[0]) + p1.Eval(x[0])*(y-p2.Eval(x[0]))**2)) )), coups )        

        eqcanv = ROOT.TCanvas("xsections_equalized","xsections_equalized")
        ## equalized[0][1].Draw("apl")
        ## # scaled[0].GetYaxis().SetRange(0,5)
        ## for g in equalized[1:]: g[1].Draw("pl")
        ## self.keep( list(equalized) )
        ## self.keep(eqcanv)

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
        averaged = ROOT.TGraph()
        for x,y in sumg.iteritems():
            averaged.SetPoint(averaged.GetN(),x,y[0]/y[1])
        averaged.Sort()
        averaged.Draw("apl")
        self.keep(averaged)
        self.keep(eqcanv)
        
        xsec = {
            "ref" : refc[0],
            "p0"  : [ p0.GetParameter(0), p0.GetParameter(1), p0.GetParameter(2) ],
            "p1"  : [ p1.GetParameter(0), p1.GetParameter(1), p1.GetParameter(2) ],
            "p2"  : [ p2.GetParameter(0), p2.GetParameter(1), p2.GetParameter(2) ],
            "xsec" : [ (averaged.GetX()[i],averaged.GetY()[i]) for i in xrange(averaged.GetN()) ]
            }
        
        with open("xsecions.json","w+") as xsec_file:
            xsec_file.write(json.dumps(xsec))
            xsec_file.close()
        
        
        
# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app = LimitPlot()
    app.run()
