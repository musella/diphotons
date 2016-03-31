#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, sys, glob, json

from auto_plotter import getObjects

def scan1D(tree,x,label,xtitle):
  tree.Draw('2*deltaNLL:%s'%x,'','')
  gr = ROOT.gROOT.FindObject('Graph').Clone('gr_%s'%(label))
  gr.SetTitle("")
  gr.GetXaxis().SetTitle(xtitle)
  gr.GetYaxis().SetTitle("-2 #Delta log L")

  gr.Sort()
  last = None
  for i in range(gr.GetN(),0,-1):
    if gr.GetX()[i-1] == last:
      gr.RemovePoint(i-1)
    last = gr.GetX()[i-1]
  return gr

def read1D(fin,x,label,xtitle):
  tree = fin.Get('limit')
  return scan1D(tree,x,label,xtitle)



def guessLabel(name):
    if "EBEB" in name: return "EBEB"
    elif "EBEE" in name: return "EBEE"
    elif "8TeV" in name: return "8TeV"
    elif "13TeV" in name: return "13TeV"
    else: return "Combined"
    

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
                make_option("--do-comparison",action="store_true", dest="do_comparison", 
                            default=False),
                make_option("--do-nll",action="store_true", dest="do_nll", 
                            default=False),
                make_option("--compare-expected",action="store_true", dest="compare_expected", 
                            default=False),
                make_option("--compare-file","--compare-files",dest="compare_files",action="callback",type="string", callback=optpars_utils.ScratchAppend(str),
                            default=[]),
                make_option("--compare-label","--compare-labels",dest="compare_labels",action="callback",type="string", callback=optpars_utils.ScratchAppend(str),
                            default=[]),
                make_option("--compare-obs",dest="compare_obs",action="callback",type="string", callback=optpars_utils.ScratchAppend(str),
                            default=[]),
                make_option("--asimov-expected",action="store_true", dest="asimov_expected", 
                            default=True),
                make_option("--toys-expected",action="store_false", dest="asimov_expected", 
                            ),
                make_option("--x-range",dest="x_range",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[450.,5000.]),
                make_option("--y-range",dest="y_range",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[]),
                make_option("--legend",dest="legend",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[0.56,0.51,0.86,0.76]),
                make_option("-n","--label",action="store", dest="label", 
                            default=""),                
                make_option("-m","--mass",action="store", dest="mass", type=float, 
                            default=750.),                
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
                make_option("--fixed-x-section-ratio",action="store", dest="fixed_x_section_ratio", type="string", 
                            default=None), 
                make_option("--use-fb",dest="use_fb", action="store_true", 
                            default=False), 
                make_option("--spin2",action="store_true", dest="spin2", 
                            default=True),
                make_option("--spin0",action="store_false", dest="spin2", 
                            ),
                make_option("--width-in-header",action="store_true", dest="width_in_header", 
                            default=True),
                make_option("--no-width-in-header",action="store_false", dest="width_in_header", 
                            ),
                make_option("--spin-in-header",action="store_true", dest="spin_in_header", 
                            default=True),
                make_option("--no-spin-in-header",action="store_false", dest="spin_in_header", 
                            ),
                make_option("--extra-lines-style", action="store", type="int", default=2, dest="extra_lines_style",
                            ),
                make_option("--xtitle",action="store", dest="xtitle", type="string", default=None,
                            ),
            ])
        
        global ROOT, style_utils, RooFit
        import ROOT
        from ROOT import RooFit
        from ROOT import RooAbsData
        import diphotons.Utils.pyrapp.style_utils as style_utils


    def __call__(self,options,args):
        self.loadRootStyle()
        
        ROOT.gStyle.SetTitleYOffset(1.)
        ROOT.gStyle.SetTitleXOffset(1.15)

        if options.do_comparison or options.do_nll:
            if len(options.compare_labels) > 0: assert( len(options.compare_labels) == len(options.compare_files) )
            else: options.compare_labels = map(guessLabel, options.compare_files)
            if len(options.compare_obs) == 0:
              options.compare_obs = ["r"]*len(options.compare_files)
            assert( len(options.compare_obs) == len(options.compare_files)  )
            
            self.compare = map(lambda x: (getObjects([self.open(x[0])]),x[1],x[2]), zip(options.compare_files,options.compare_labels,options.compare_obs) )

        if options.do_nll:
            self.plotNLLScan(options)
            return

        # ROOT.gSystem.AddIncludePath( "$ROOTSYS/include" )
        ROOT.gROOT.LoadMacro( "$CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/plotting/bandUtils.cxx+" )
        
        self.loadXsections(options.x_sections)

        if options.do_comparison:
            self.plotComparisons(options)
            return

        print options.couplings
        if len(options.couplings) == 0:
            flist = glob.glob("%s/higgsCombine%s_k*.%s.root" % (options.input_dir, options.label, options.method) )
        else:
            flist = [ "%s/higgsCombine%s_k%s.%s.root" % (options.input_dir, options.label, coup, options.method) for coup in options.couplings ]
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
        

    def getLegendHeader(self,kappa,g0):
      txt = ""
      if self.options.width_in_header:
        if kappa >= 0.1:
          txt += "#frac{#Gamma}{m} = %g #times 10^{-2}  " % (1.4*kappa*kappa*100.)
        else:
          txt += "#frac{#Gamma}{m} = %g #times 10^{-4}  " % (1.4*kappa*kappa*10000.)
        
      if self.options.spin_in_header:
        if self.options.spin2:
          txt += "J=2"
          if g0: g0.GetXaxis().SetTitle("m_{G} (GeV)")
        else:
          txt += "J=0"
          if g0: g0.GetXaxis().SetTitle("m_{S} (GeV)")
      else:
        if g0: g0.GetXaxis().SetTitle("m_{X} (GeV)")
        
      return txt


    def plotNLLScan(self,options):
        graphs = map(lambda x: (map(lambda z: scan1D(z,x[2],x[1],"#sigma^{13TeV} #upoint B_{#gamma #gamma} (fb)"),##"\sigma^{13TeV} \cdot BR_{\gamma \gamma}    (fb)"),
                                    filter(lambda y: y.GetName() == "limit", x[0]))[0],x[1]), self.compare)
        
        styles = [ [["colors",ROOT.kBlack]], [["colors",ROOT.kBlue]], [["colors",ROOT.kRed]] ]
        map(lambda x: style_utils.apply(x[0],[["SetMarkerSize",0.3],["SetLineWidth",2]]+styles.pop(0)), graphs)

        canv = ROOT.TCanvas("nll_scan_%s" % options.label,"nll_scan_%s" % options.label)
        legend = ROOT.TLegend(*options.legend)

        g0 = graphs[0][0]
        txt = ""
        if options.spin2:
          txt += "J=2"
        else:
          txt += "J=0"
        legend.AddEntry(None,"m=%1.0f GeV, %s" % (options.mass,txt),"")
        if len(options.couplings) == 1:
          kappa = float("0.%s" % options.couplings[0][1:])
          legend.AddEntry(None,"#frac{#Gamma}{m} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.),"")
        legend.AddEntry(None," ","")
        ## if options.spin2:
        ###     legend.AddEntry(None,"#tilde{#kappa} = %s" % kappa,"")
        ### else:
        ###     kappa = float(kappa)
        ###     legend.AddEntry(None,"#frac{#Gamma}{m} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.),"")
        if len(options.x_range) != 0:
          g0.GetXaxis().SetRangeUser(*options.x_range)
        if len(options.y_range) != 0:
          g0.GetYaxis().SetRangeUser(*options.y_range)
          g0.GetYaxis().SetLimits(*options.y_range)
        g0.Draw("ac")
        for gr,nam in graphs:
            legend.AddEntry(gr,nam,"l")
            self.keep(gr)
        for gr,nam in reversed(graphs):
            gr.Draw("c")
        legend.Draw("same")
        
        ## xmin,xmax = options.x_range

        self.keep([canv,legend])
        self.format(canv,options.postproc)



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
        unit = "fb" if options.use_fb else "pb"
        basicStyle = [["SetMarkerSize",0.6],["SetLineWidth",3],
                       ["SetTitle",";m_{G} (GeV);95%% C.L. limit #sigma(pp#rightarrow G#rightarrow#gamma#gamma) (%s)" % unit]]
        if not options.spin2:
            basicStyle.append(["SetTitle",";m_{S} (GeV);95%% C.L. limit #sigma(pp#rightarrow S#rightarrow#gamma#gamma) (%s)" % unit])
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
        legend = ROOT.TLegend(0.55,0.58,0.85,0.88)
        expected95.Draw("AE3")        
        expected95.GetXaxis().SetRangeUser(450,5500)
        expected95.GetXaxis().SetMoreLogLabels()
        expected68.Draw("E3L")
        expected.Draw("L")
        kappa = "0."+coup[1:]
        ### if options.spin2:
        ###     legend.AddEntry(None,"#tilde{#kappa} = %s" % kappa,"")
        ### else:
        ###     kappa = float(kappa)
        ###     legend.AddEntry(None,"#frac{#Gamma}{m} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.),"")
        legend.AddEntry(None,self.getLegendHeader(float(kappa),None),"")
        legend.AddEntry(expected,"Expected limit","l")
        legend.AddEntry(expected68," #pm 1 #sigma","f")
        legend.AddEntry(expected95," #pm 2 #sigma","f")
        if options.unblind:
          # observed.Draw("PL")
          observed.Draw("L")
          legend.AddEntry(observed,"Observed limit","l")
        if coup in self.xsections_:
            grav = self.xsections_[coup]
            style_utils.apply( grav, basicStyle+[["SetLineStyle",9],["colors",ROOT.myColorB2]] )
            grav.Draw("L")
            legend.AddEntry(grav,"G_{RS}#rightarrow#gamma#gamma (LO)","l").SetLineStyle(0)
            
        self.keep(legend,True)
        legend.Draw()
        
        self.graphs.extend([observed,expected,expected68,expected95])
        
        self.keep( [canv,observed,expected,expected68,expected95] )
        self.format(canv,options.postproc)

    def plotComparisons(self,options):
        if options.compare_expected:
            observed = map(lambda x: (filter(lambda y: "expected" in y.GetName(), x[0]),x[1]), self.compare)
            coups = set(map(lambda x: x.GetName().replace("expected_",""), reduce(lambda x,y: x+y, map(lambda z: z[0], observed), [])))
        else:
            observed = map(lambda x: (filter(lambda y: "observed" in y.GetName(), x[0]),x[1]), self.compare)
            coups = set(map(lambda x: x.GetName().replace("observed_",""), reduce(lambda x,y: x+y, map(lambda z: z[0], observed), [])))
        ## coups = ["001","01","02"]
        coups = ["001"]
        if len(options.couplings) > 0:
            coups = options.couplings

        map(lambda x: self.plotComparison(options,x,observed), coups)

    def plotComparison(self,options,coup,observed):
        
        cobserved = map(lambda x: (filter(lambda y: y.GetName().endswith("_%s" % coup), x[0])[0],x[1]), observed)
        print cobserved
        
        ## styles = [ [["colors",ROOT.kBlue]], [["colors",ROOT.kRed+1]], [["colors",ROOT.kMagenta-2]] ] 
        styles = [ [["colors",ROOT.kBlack]], [["colors",ROOT.kBlue],["SetLineStyle",options.extra_lines_style]], [["colors",ROOT.kRed],["SetLineStyle",options.extra_lines_style]] ]
        map(lambda x: style_utils.apply(x[0],[["SetMarkerSize",0.3],["SetLineWidth",2]]+styles.pop(0)), cobserved)
    
        canv = ROOT.TCanvas("comparison_%s%s" % (options.label,coup),"comparison_%s%s"  % (options.label,coup) )
        legend = ROOT.TLegend(*options.legend)
        ## legend = ROOT.TLegend(0.56,0.51,0.86,0.76)
        ## legend = ROOT.TLegend(0.6,0.51,0.9,0.76)
        ## legend = ROOT.TLegend(0.6,0.2,0.9,0.42)
        ## legend = ROOT.TLegend(0.45,0.2,0.75,0.42)
        legend.SetFillStyle(0)
        kappa = "0."+coup[1:]
        
        g0 = cobserved[0][0]
        
        ### kappa = float(kappa)
        ### if kappa >= 0.1:
        ###   txt = "#frac{#Gamma}{m} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.)
        ### else:
        ###   txt = "#frac{#Gamma}{m} = %g #times 10^{-4}" % (1.4*kappa*kappa*10000.)
        ### 
        ### if options.spin2:
        ###   txt += "  J=2"
        ###   g0.GetXaxis().SetTitle("m_{G} (GeV)")
        ### else:
        ###   txt += "  J=0"
        ###   g0.GetXaxis().SetTitle("m_{S} (GeV)")
        kappa = float(kappa)
        txt = self.getLegendHeader(kappa,g0)
        legend.AddEntry(None,txt,"")
            
        if options.xtitle:
          g0.GetXaxis().SetTitle(options.xtitle)
        g0.Draw("al")
        for gr,nam in cobserved:
            legend.AddEntry(gr,nam,"l")
        for gr,nam in reversed(cobserved):
            gr.Draw("l")
        legend.Draw("same")
        
        xmin,xmax = options.x_range
        ## g0.GetXaxis().SetRangeUser(450,5000)
        ## g0.GetXaxis().SetRangeUser(500,3000)
        ## g0.GetXaxis().SetRangeUser(500,850)
        ## g0.GetXaxis().SetRangeUser(850,3000)
        g0.GetXaxis().SetRangeUser(xmin,xmax)
        g0.GetXaxis().SetMoreLogLabels()
          
        canv.SetLogx()
        if options.do_pvalues:
            canv.SetLogy()
            g0.GetYaxis().SetRangeUser(1e-3,0.55)
            ## g0.GetYaxis().SetRangeUser(1e-4,0.55)

        if len(options.y_range) > 0:
          ## print "AAAAAAAAAAAAAAAA"
          g0.GetYaxis().SetRangeUser(*options.y_range)
          g0.GetYaxis().SetLimits(*options.y_range)
          
        if options.do_pvalues:
          self.drawLines(g0,xmin,xmax)
          
        
        self.keep([canv,legend])
        self.format(canv,options.postproc)
        
    def drawLines(self,ref,xmin=450,xmax=5000):
        
        spots = filter(lambda y: y[1]>float(ref.GetYaxis().GetXmin()),  map(lambda x: (x,ROOT.RooStats.SignificanceToPValue(x)), xrange(1,5) ) )
        lines = map( lambda y: ROOT.TLine(xmin,y[1],xmax,y[1]), spots )
        map( lambda x: style_utils.apply(x,[["SetLineColor",ROOT.kGray+3],["SetLineStyle",7]]), lines )
        
        labels = map( lambda y: ROOT.TLatex(xmax*1.01,y[1]*0.9,"#color[%d]{%d #sigma}" % (ROOT.kGray+2,y[0])), spots )
        map( lambda x: style_utils.apply(x,[["SetTextSize",0.05]]), labels )

        map( lambda x: x.Draw("same"), lines+labels )
        self.keep(lines+labels)
        
    def plotPval(self,options,coup,tfile):
        observed = ROOT.theBand( tfile, 1, 0, ROOT.Observed, 0.95 )
        basicStyle = [["SetMarkerSize",0.6],["SetLineWidth",3],
                       ["SetTitle",";m_{G} (GeV);p_{0}"]]
        if not options.spin2:
            basicStyle.append(["SetTitle",";m_{S} (GeV);p_{0}"])
        commonStyle = ["Sort"]+basicStyle
        observedStyle = commonStyle+[["SetMarkerStyle",ROOT.kFullCircle],["colors",ROOT.kBlue]]
        
        style_utils.apply(observed,[["SetName","observed_%s"%coup]]+observedStyle)
      
        
        xmin,xmax = options.x_range
        canv  = ROOT.TCanvas("pvalues_k%s"%coup,"pvalues_k%s"%coup)
        canv.SetLogy()
        canv.SetLogx()
        ## legend = ROOT.TLegend(0.5,0.6,0.8,0.75)
        ## legend = ROOT.TLegend(0.6,0.6,0.9,0.75)
        ## legend = ROOT.TLegend(0.56,0.6,0.86,0.75)
        legend = ROOT.TLegend(*options.legend)
        legend.SetFillStyle(0)
        kappa = "0."+coup[1:]
        observed.Draw("apl")
        ## observed.Draw("al")
        ## observed.GetYaxis().SetRangeUser(1e-5,0.55)
        observed.GetYaxis().SetRangeUser(1e-3,0.55)
        if len(options.y_range) > 0:
          ## print "AAAAAAAAAAAAAAAA"
          observed.GetYaxis().SetRangeUser(*options.y_range)
          observed.GetYaxis().SetLimits(*options.y_range)

        observed.GetXaxis().SetRangeUser(xmin,xmax)
        observed.GetXaxis().SetMoreLogLabels()
        
        ## xmin,xmax=observed.GetXaxis().GetXmin(),observed.GetXaxis().GetXmax()
        ## xmin,xmax=450,3000
        # xmin,xmax=450,5000
        ### spots = filter(lambda x: x>observed.GetYaxis().GetXmin(),  map(lambda x: (x,ROOT.RooStats.SignificanceToPValue(x)), xrange(1,5) ) )
        ### 
        ### lines = map( lambda y: ROOT.TLine(xmin,y[1],xmax,y[1]), spots )
        ### map( lambda x: style_utils.apply(x,[["SetLineColor",ROOT.kGray+3],["SetLineStyle",7]]), lines )
        ### 
        ### labels = map( lambda y: ROOT.TLatex(xmax*1.01,y[1]*0.9,"#color[%d]{%d #sigma}" % (ROOT.kGray+2,y[0])), spots )
        ### map( lambda x: style_utils.apply(x,[["SetTextSize",0.05]]), labels )
        ### 
        ### map( lambda x: x.Draw("same"), lines+labels )

        self.drawLines(observed,xmin,xmax)

        ## self.keep(lines+labels)
        
        if options.spin2:
            legend.AddEntry(None,"#tilde{#kappa} = %s" % kappa,"")
        else:
            kappa = float(kappa)
            legend.AddEntry(None,"#frac{#Gamma}{m} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.),"")
        legend.AddEntry(observed,"Observed p_{0}","l")
        
        self.keep(legend,True)
        legend.Draw()
        
        self.graphs.extend([observed])
        
        self.keep( [canv,observed] )
        self.format(canv,options.postproc)

    def scaleByXsec(self,graph,coup):
        if self.options.fixed_x_section:
            scale = self.options.fixed_x_section
            rescaleFunc = None
            if self.options.fixed_x_section_ratio:
                rescaleFunc = ROOT.TF1("rescale",self.options.fixed_x_section_ratio)
                print rescaleFunc
            if self.options.use_fb: scale *= 1e+3
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
            iscale = scale
            if not self.options.fixed_x_section:
                iscale = xsec.Eval(xvals[ip])
            elif rescaleFunc: 
                print xvals[ip],iscale,
                iscale *= rescaleFunc.Eval(xvals[ip])
                print iscale
            graph.SetPoint( ip, xvals[ip], yvals[ip]*iscale )
            graph.SetPointEYlow( ip, yerrl[ip]*iscale )
            graph.SetPointEYhigh( ip, yerrh[ip]*iscale )
        
    def loadXsections(self,inmap):
        self.xsections_ = {}
        scl = 1e+3 if self.options.use_fb else 1.
        for name,val in inmap.iteritems():
            if name.startswith("RSGravToGG") or name.startswith("RSGravitonToGG"):
                coup,mass = name.split("kMpl")[1].split("_Tune")[0].replace("_","").replace("-","").split("M")
                mass = float(mass)
                if not coup in self.xsections_:
                    self.xsections_[coup] = ROOT.TGraph()
                self.xsections_[coup].SetPoint(self.xsections_[coup].GetN(),mass,val["xs"]*scl)
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
