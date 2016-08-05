#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, sys, glob, json

from math import ceil,floor,log,pow

from auto_plotter import getObjects

def scan1D(tree,x,label,xtitle):
  y='2*deltaNLL:' if not ":" in x else ""
  tree.Draw('%s%s'%(y,x),'','')
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
                make_option("--cleanup",action="store_true", dest="cleanup", 
                            default=False),
                make_option("--smoothen",action="store_true", dest="smoothen", 
                            default=False),
                make_option("--smoothen-boundary",action="store", dest="smoothen_boundary", type="float",
                            default=0.),
                make_option("--do-pvalues",action="store_true", dest="do_pvalues", 
                            default=False),
                make_option("--do-comparison",action="store_true", dest="do_comparison", 
                            default=False),
                make_option("--do-summary",action="store_true", dest="do_summary", 
                            default=False),
                make_option("--do-simple-summary",action="store_true", dest="do_simple_summary", 
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
                make_option("--use-mx",action="store_true", dest="use_mx", 
                            default=False),
                make_option("--use-narrow-width",action="store_true", dest="use_narrow_width", 
                            default=False),
                make_option("--add-inset",action="store_true", dest="add_inset", 
                            default=False),
                make_option("--inset",dest="inset",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[700,800]),
                make_option("--x-range",dest="x_range",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[450.,5000.]),
                make_option("--y-range",dest="y_range",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[]),
                make_option("--legend",dest="legend",action="callback",type="string", callback=optpars_utils.ScratchAppend(float),
                            default=[0.56,0.51,0.86,0.76]),
                make_option("--legend-head-maxw",action="store", dest="legend_head_maxw", type=float, 
                            default=1.),                
                make_option("--legend-head-maxh",action="store", dest="legend_head_maxh", type=float, 
                            default=1.),                
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

        print options.rootstyle_extra

        ROOT.gStyle.SetTitleYOffset(1.)
        ROOT.gStyle.SetTitleXOffset(1.15)

        if options.do_comparison or options.do_nll or options.do_summary or options.do_simple_summary:
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

        if options.do_summary:
            self.plotSummary(options)
            return

        if options.do_simple_summary:
            self.plotSimpleSummary(options)
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
            graphs = self.open("%s/graphs_%s%s.root" % (options.input_dir,options.label,options.method),"recreate")
        else:
            graphs = self.open("%s/graphs_%s_%s%s.root" % (options.input_dir,"_".join(options.couplings),options.label,options.method),"recreate")
        graphs.cd()
        for gr in self.graphs: gr.Write()
        graphs.Close()
        

    def getLegendHeader(self,kappa,g0):
      txt = ""
      if self.options.width_in_header:
        if kappa >= 0.1:
          if self.options.use_mx: 
             txt += "#frac{#it{#Gamma}_{X}}{#it{m}_{X}}#kern[0.7]{=}#kern[0.2]{%g}#times10^{-2}" % (1.4*kappa*kappa*100.)
          else:
            txt += "#it{#frac{#Gamma}{m}}#kern[0.7]{=}#kern[0.2]{%g}#times10^{-2}" % (1.4*kappa*kappa*100.)
        else:
          gamma = 1.4*kappa*kappa*10000.
          if gamma < 30. and self.options.use_narrow_width:
            txt += "Narrow width "
          else:
            if self.options.use_mx: 
              txt += "#frac{#it{#Gamma}_{X}}{#it{m}_{X}}#kern[0.7]{=}#kern[0.2]{%g}#times10^{-4}" % (1.4*kappa*kappa*10000.)
            else:
              txt += "#it{#frac{#Gamma}{m}}#kern[0.7]{=}#kern[0.2]{%g}#times10^{-4}" % (1.4*kappa*kappa*10000.)
        
      if self.options.spin_in_header:
        if txt != "": txt += ", "
        if self.options.spin2:          
          txt += "J=2"
          if g0: g0.GetXaxis().SetTitle("#it{m}_{G} (GeV)")
        else:
          txt += "J=0"
          if g0: g0.GetXaxis().SetTitle("#it{m}_{S} (GeV)")
      if not self.options.spin_in_header or self.options.use_mx:
        if g0: g0.GetXaxis().SetTitle("#it{m}_{X} (GeV)")
        
      return txt


    def plotNLLScan(self,options):
        graphs = map(lambda x: (map(lambda z: scan1D(z,x[2],x[1],"#sigma^{13TeV} B_{#gamma#gamma} (fb)"),##"\sigma^{13TeV} \cdot BR_{\gamma \gamma}    (fb)"), "#sigma^{13TeV} #upoint B_{#gamma #gamma} (fb)"
                                    filter(lambda y: y.GetName() == "limit", x[0]))[0],x[1]), self.compare)
        
        styles = [ [["colors",ROOT.kBlack]], [["colors",ROOT.kBlue]], [["colors",ROOT.kRed]], [["colors",ROOT.kAzure+1]], ]
        map(lambda x: style_utils.apply(x[0],[["SetMarkerSize",0.3],["SetLineWidth",2]]+styles.pop(0)), graphs)

        canv = ROOT.TCanvas("nll_scan_%s" % options.label,"nll_scan_%s" % options.label)
        legend = ROOT.TLegend(*options.legend)

        g0 = graphs[0][0]
        txt = ""
        if options.spin2:
          txt += "J=2"
        else:
          txt += "J=0"
        legend.AddEntry(None,"#it{m}_{X} = %1.0f GeV, %s" % (options.mass,txt),"")
        fake = legend.AddEntry(None," ","")
        ## fake.SetTextSize(0.2*fake.GetTextSize())
        if len(options.couplings) == 1:
          kappa = float("0.%s" % options.couplings[0][1:])
          legend.AddEntry(None,"#frac{#it{#Gamma}_{X}}{#it{m}_{X}} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.),"")
        legend.AddEntry(None," ","")
        ## if options.spin2:
        ###     legend.AddEntry(None,"#tilde{k} = %s" % kappa,"")
        ### else:
        ###     kappa = float(kappa)
        ###     legend.AddEntry(None,"#frac{#Gamma}{m} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.),"")
        if len(options.x_range) != 0:
          g0.GetXaxis().SetRangeUser(*options.x_range)
          g0.GetXaxis().SetLimits(*options.x_range)
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
        if ROOT.cmsText == "":
          canv.cd()
          ptCMS=ROOT.TLatex(0.18,0.15,"CMS")
          ptCMS.Setndc()
          ptCMS.SetTextFont(61)
          ptCMS.SetTextSize(0.06)
          ptCMS.Draw("same")
          ptCMS.Print()
          self.keep(ptCMS)

        self.keep([canv,legend])
        self.format(canv,options.postproc)


    def takeout(self,gr):
      gr2 = ROOT.TGraphAsymmErrors()
      for ii in xrange(gr.GetN()/10):
        ip = 10*ii
        print ip
        jp = gr2.GetN()
        gr2.SetPoint(jp,gr.GetX()[ip],gr.GetY()[ip])
        gr2.SetPointError(jp,gr.GetEXlow()[ip],gr.GetEXhigh()[ip],gr.GetEYlow()[ip],gr.GetEYhigh()[ip])
      return gr2

    def cleanup(self,gr):
      
      for ip in xrange(gr.GetN()-1,0,-1):
        iy = gr.GetY()[ip]
        if iy<1e-2:
          gr.RemovePoint(ip)
        if ip>1:
          idy = (iy-gr.GetY()[ip-1]) / gr.GetY()[ip-1]
          ##print gr.GetX()[ip], iy, idy
          if idy > 0.1:
            ##print "remove  ------------ "
            gr.RemovePoint(ip)        

      weights = [ 0., 0.25, 0.5, 1., 0.5, 0.25, 0.  ] 
      navg = len(weights)/2
      cutoff = .5e-2
      points = []
      for ip in xrange(gr.GetN()):
        ix = gr.GetX()[ip]
        sumy = 0.
        sumwe = 0.
        for jp in xrange(-navg, navg+1):
          if ip+jp >= 0 and ip+jp<gr.GetN():
            jx = gr.GetX()[ip+jp]
            if abs(1. - jx/ix) < cutoff:
              we = weights[navg+jp]
              sumy += gr.GetY()[ip+jp]*we
              sumwe += we
        iy = sumy / sumwe
        points.append( (ip,ix,iy) )

      for point in points:
        (ip,ix,iy) = point
        gr.SetPoint(ip,ix,iy)

      
    def smoothen(self,gr):
      hardBound=self.options.smoothen_boundary
      relwindow = 5.e-2
      points = []
      for ip in xrange(gr.GetN()):
        ix = gr.GetX()[ip]
        window = relwindow*ix
        ipoints = []
        for jp in xrange(gr.GetN()):
          jx = gr.GetX()[jp]
          if abs(ix-jx)<window:
            ## print ix, jx, (hardBound-ix)*(hardBound-jx)
            if (hardBound-ix)*(hardBound-jx)>0.:
              ipoints.append(jp)
        if len(ipoints) < 3: continue
        minw = gr.GetX()[min(ipoints)]
        maxw = gr.GetX()[max(ipoints)]
        left = ix-window/2.
        right = ix+window/2.
        if minw>left:
          left = minw
        if maxw<right:
          right = maxw
        gr2 = ROOT.TGraph()
        map(lambda y: gr2.SetPoint(y[0],gr.GetX()[y[1]],gr.GetY()[y[1]]), enumerate(filter(lambda x:  gr.GetX()[x]>=minw and gr.GetX()[x]<=maxw, ipoints )))
        ## print "Smoothing ", ix, left, right
        func = ROOT.TF1("f","[0]*pow(x,[1])")
        ## gr2.Print()
        slope = ( log(gr2.GetY()[gr2.GetN()-1])-log(gr2.GetY()[0]) ) / ( log(gr2.GetX()[gr2.GetN()-1])-log(gr2.GetX()[0]) )
        intercept = pow(gr2.GetY()[0],-slope)
        gr2.Fit(func,"Q")
        points.append( (ip, ix, func.Eval(ix)) )
      
      for point in points:
        ip, ix, iy = point
        scl = iy / gr.GetY()[ip] 
        gr.SetPoint(ip,ix,iy)
        gr.SetPointEYlow( ip, gr.GetEYlow()[ip]*scl )
        gr.SetPointEYhigh( ip, gr.GetEYhigh()[ip]*scl )
        

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
        if options.cleanup:
          self.cleanup(observed)
        if options.smoothen:
          ## observed
          map(self.smoothen, (expected68,expected95) )
          ## expected68,expected95 = map(self.takeout, (expected68,expected95))
        unit = "fb" if options.use_fb else "pb"
        basicStyle = [["SetMarkerSize",0.6],["SetLineWidth",3],
                       ["SetTitle",";#it{m}_{G} (GeV);95%% CL limit #sigma(pp#rightarrow G#rightarrow#gamma#gamma) (%s)" % unit]]
        if not options.spin2:
            basicStyle.append(["SetTitle",";#it{m}_{S} (GeV);95%% CL limit #sigma(pp#rightarrow S#rightarrow#gamma#gamma) (%s)" % unit])
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
        ###     legend.AddEntry(None,"#tilde{k} = %s" % kappa,"")
        ### else:
        ###     kappa = float(kappa)
        ###     legend.AddEntry(None,"#frac{#Gamma}{m} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.),"")
        legend.AddEntry(None,self.getLegendHeader(float(kappa),None),"")
        legend.AddEntry(expected,"Expected limit","l")
        legend.AddEntry(expected68," #pm 1 s.d.","f")
        legend.AddEntry(expected95," #pm 2 s.d.","f")
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

    def plotSummary(self,options):
      print self.compare
      expected0 = map(lambda x: ( sorted(filter(lambda y: "expected_" in y.GetName(), x[0]),key=lambda h: h.GetName()),x[1]), self.compare)
      expected = map(lambda x: ( sorted(filter(lambda y: "expected68_" in y.GetName(), x[0]),key=lambda h: h.GetName()),x[1]), self.compare)
      ocoups = set(map(lambda x: x.GetName().rsplit("_",1)[-1], reduce(lambda x,y: x+y, map(lambda z: z[0], expected), [])))
      observed = map(lambda x: ( sorted(filter(lambda y: "observed" in y.GetName(), x[0]),key=lambda h: h.GetName()),x[1]), self.compare)
      ecoups = set(map(lambda x: x.GetName().rsplit("_",1)[-1], reduce(lambda x,y: x+y, map(lambda z: z[0], observed), [])))
      
      print expected0

      assert( ocoups == ecoups )
      coups = sorted(list(ocoups))
      
      if len(options.couplings) > 0:
        coups = options.couplings
        
      observed = map(lambda w: (map(lambda x: x[1], filter(lambda y: y[0] in coups, map(lambda z: (z.GetName().rsplit("_",1)[-1],z), w[0] ) ) ),w[1] ), observed )
      expected = map(lambda w: (map(lambda x: x[1], filter(lambda y: y[0] in coups, map(lambda z: (z.GetName().rsplit("_",1)[-1],z), w[0] ) ) ),w[1] ), expected )
      expected0 = map(lambda w: (map(lambda x: x[1], filter(lambda y: y[0] in coups, map(lambda z: (z.GetName().rsplit("_",1)[-1],z), w[0] ) ) ),w[1] ), expected0 )
      if self.options.do_pvalues:
        name = "summary_pvalue_%s" % self.options.label
      else:
        name = "summary_limits_%s" % self.options.label
        
      ## canv = ROOT.TCanvas(name,name,450,600)
      ## canv = ROOT.TCanvas(name,name,600,800)
      canv = ROOT.TCanvas(name,name,900,900)
      ## canv = ROOT.TCanvas(name,name,900,800)
      canv.SetRightMargin(0.03)
      canv.SetLeftMargin(0.13)
      
      nc = len(coups)
      canv.Divide(1,nc)
      
      ### if len(coups) < 3:
      ###   offset = 0.0
      ###   height = 0.92/float(nc)
      ###   sclfirst = 1.1
      ###   first  = offset+height*sclfirst
      ###   bottom = 0.25
      ### else:
      ### offset = 0.0
      ### height = 0.92/float(nc)
      ### sclfirst = 1.04
      ### first  = offset+height*sclfirst
      ### bottom = 0.18
      if len(coups) < 3:
        offset = 0.0
      else:
        offset = 0.025
      ## height = 0.85/float(nc)
      height = 0.84/float(nc)
      sclfirst = 1.25
      first  = offset+height*sclfirst
      bottom = 0.25
      self.newRootColor(ROOT.kRed,"myRedTr",0.2)
      self.newRootColor(ROOT.kBlue,"myBlueTr",0.2)
      ROOT.gStyle.SetHatchesLineWidth(2)
      ### exp_sty = [ [["colors",ROOT.kRed+1], ["SetFillColor",[ROOT.myRedTr]],  ["SetLineStyle",7], ["SetLineWidth",2] ], 
      ###             [["colors",ROOT.kBlue+1],["SetFillColor",[ROOT.myBlueTr]], ["SetLineStyle",7], ["SetLineWidth",2] ] 
      ###             ]
      ### obs_sty = [ [["colors",ROOT.kRed+1],  ["SetLineWidth",2]],
      ###             [["colors",ROOT.kBlue+1], ["SetLineWidth",2]]
      ###             ]

      exp_sty = [ [["colors",ROOT.myColorA1], ["SetFillColor",[ROOT.myColorA3tr]], ["SetLineStyle",7], ["SetLineWidth",2] ], 
                  [["colors",ROOT.myColorD1], ["SetFillColor",[ROOT.myColorD2]], ["SetFillStyle",[3357]],  ["SetLineStyle",7], ["SetLineWidth",2] ] 
                  ]
      obs_sty = [ [["colors",ROOT.myColorA1],  ["SetLineWidth",2]],
                  [["colors",ROOT.myColorD1], ["SetLineWidth",2]]
                  ]
      
      for ic in range(nc): 
        canv.cd(ic+1)
        if ic == 0:
          ROOT.gPad.SetPad(0.,offset,1.,first)
        else:
          ROOT.gPad.SetPad(0.,first+height*float(ic-1),1.,first+height*float(ic))
        ROOT.gPad.SetTicks(1)
        ROOT.gPad.SetTopMargin(0.03)
        ROOT.gPad.SetRightMargin(canv.GetRightMargin())
        ROOT.gPad.SetLeftMargin(canv.GetLeftMargin())
        if ic == 0:
          ROOT.gPad.SetBottomMargin(bottom)
        else:
          ROOT.gPad.SetBottomMargin(0.04)
        ROOT.gPad.SetLogx()
        
        exp0 = map(lambda x: x[0][nc-ic-1], expected0)
        exp = map(lambda x: x[0][nc-ic-1], expected)
        obs = map(lambda x: x[0][nc-ic-1], observed)
        
        map(lambda x: style_utils.apply(x[0],x[1]), zip(exp0,exp_sty) )
        map(lambda x: style_utils.apply(x[0],x[1]), zip(exp,exp_sty) )
        map(lambda x: style_utils.apply(x[0],x[1]), zip(obs,obs_sty) )

        ## graphs = exp+exp0+obs
        g0 = exp[0]
        ## allylow = reduce(lambda x,y: x+y, map(lambda z: map(lambda ip: z.GetY()[ip]-z.GetErrorY(ip), range(z.GetN()) ), exp ) )
        ## ymin = floor(min(allylow))*0.5
        ymin = 1.2e-1
        allyup = reduce(lambda x,y: x+y, map(lambda z: map(lambda ip: z.GetY()[ip]+z.GetErrorY(ip), range(z.GetN()) ), exp ) )
        ymax = ceil(max(allyup))*1.1
        
        leghead = copy(options.legend)
        leghead[2] = leghead[0] - 0.02
        leghead[0] = leghead[2] - 0.3
        if len(coups) == 2:
          leghead[1] = leghead[3]-0.15
        else:
          if ic == 0:
            leghead[1] = leghead[3]-0.1/sclfirst
          else:
            leghead[1] = leghead[3]-0.1
        leghead[2] = (leghead[2]-leghead[0])+0.1
        leghead[0] = 0.1
        leghead[1] -= 0.05
        leghead[3] -= 0.05
        leghead = ROOT.TLegend(*leghead)
        leghead.SetFillStyle(0)
        skappa = "0."+coups[nc-ic-1][1:]
        kappa = float(skappa)
        txt = self.getLegendHeader(kappa,g0)
        leghead.AddEntry(None,txt,"")

        ## legend = ROOT.TLegend(*options.legend)
        legend = copy(options.legend)
        print ic, nc-1
        if ic != nc - 1:
          legend[1] = legend[3] - 0.18*(legend[3]-legend[1])
          legend[1] -= 0.03
          legend[3] -= 0.03
        if ic != 0:
          legend[1] = legend[3] - sclfirst*(legend[3]-legend[1])
        legend = ROOT.TLegend(*legend)

        ## g0.GetYaxis().SetTitle("95% CL on #sigma_{X}^{13TeV} #upoint B_{#gamma #gamma} (fb)")
        ## g0.GetYaxis().SetTitle("#sigma_{X}^{13TeV} #upoint B_{#gamma #gamma} (fb)")
        ## g0.GetYaxis().SetTitle("95% CL limit on #sigma_{X}^{13TeV} #upoint B_{#gamma #gamma} (fb)")
        ## g0.GetYaxis().SetTitle("95% CL limit on #sigma_{X}^{13TeV} B_{#gamma#gamma} (fb)")
        ## g0.GetYaxis().SetTitle("95% CL limit on #sigma_{X}^{13TeV} B_{#gamma#gamma} (fb)")
        g0.GetYaxis().SetTitle("")
        g0.GetYaxis().SetRangeUser(ymin,ymax)
        g0.GetYaxis().SetLimits(ymin,ymax)
        g0.GetYaxis().SetLabelSize( g0.GetXaxis().GetLabelSize() * canv.GetWh() / ROOT.gPad.GetWh() * (2.1 if ic != 0 else 2.1/sclfirst)*bottom/0.22 )
        g0.GetYaxis().SetTitleSize( g0.GetXaxis().GetTitleSize() * canv.GetWh() / ROOT.gPad.GetWh() * (2.1 if ic != 0 else 2.1/sclfirst)*bottom/0.22 )
        g0.GetYaxis().SetNdivisions( 505 )
        
        g0.GetYaxis().SetTitleOffset( 0.5*0.25/((1. if ic==0 else sclfirst)*bottom) )
        g0.GetXaxis().SetLabelSize( ( 2.2*g0.GetXaxis().GetLabelSize() if ic == 0 else 0. )*bottom/0.23 )
        g0.GetXaxis().SetTitleSize( ( 2.2*g0.GetXaxis().GetTitleSize() if ic == 0 else 0. )*bottom/0.25 )
        g0.GetXaxis().SetMoreLogLabels()
        
        if len(options.x_range) == 2:
          g0.GetXaxis().SetRangeUser(*options.x_range)
          
        maxe = max( map(lambda y: max( map( lambda x: y.GetY()[x]+y.GetErrorYhigh(x) , range(y.GetN()) ) ), exp ) )
        maxo = max( map(lambda y: y.GetMaximum(), obs ) )
        ## ymax = max(maxe,maxo)*1.3
        ymax = max(maxe,maxo)*7.
        g0.GetYaxis().SetRangeUser(ymin,ymax)
        ROOT.gPad.SetLogy()

        print expected
        print observed
        if ic == nc -1:
          for ip,pair in enumerate(zip(exp,obs)):
            ex,ob = pair
            legend.AddEntry(ex,"%s expected #pm 1 s.d." % expected[ip][1],"lf")
            legend.AddEntry(ob,"%s observed" % observed[ip][1],"l")
          

        g0.Draw("ALE3")
        map(lambda x: x.Draw("LE3"), exp[1:])
        map(lambda x: x.Draw("L"), exp0)
        map(lambda x: x.Draw("L"), obs)
        
        coup = coups[nc-ic-1]
        if coup in self.xsections_:
            grav = self.xsections_[coup]
            style_utils.apply( grav, [["SetLineWidth",2],["SetLineStyle",5],["colors",ROOT.kRed+1]] )
            grav.Draw("C")
            legend.AddEntry(grav,"G_{RS}#rightarrow#gamma#gamma, #tilde{k}=%s (LO)" % skappa,"l").SetLineStyle(0)
            self.keep(grav)

        legend.Draw("same")
        leghead.Draw("same")
        self.keep( [legend,leghead] )
        self.keep( exp+obs )
        
        ## pad = ROOT.gPad
        ## canv.cd()
        ## pad.Draw("same")
        
      tl = ROOT.TLatex(0.05,0.3,"95% CL limit on #sigma_{X}^{13TeV} B_{#gamma#gamma} (fb)")
      tl.SetTextAngle(90.)
      tl.SetTextAlign(12)
      tl.SetTextSize(0.04)
      canv.cd()
      tl.Draw("same")
      self.keep(tl)
      

      if ROOT.cmsText == "":
        canv.cd()
        ## ptCMS=ROOT.TLatex(0.18,0.88,"CMS")
        ptCMS=ROOT.TLatex(0.18,0.14,"CMS")
        ptCMS.SetNDC()
        ptCMS.SetTextFont(61)
        ptCMS.SetTextSize(0.04)
        ptCMS.Draw("same")
        ptCMS.Print()
        self.keep(ptCMS)
        

      self.keep(canv)
      self.format(canv,options.postproc)
      
      self.autosave()


    def plotSimpleSummary(self,options):
      print self.compare
      expected0 = map(lambda x: ( sorted(filter(lambda y: "expected_" in y.GetName(), x[0]),key=lambda h: h.GetName()),x[1]), self.compare)
      expected = map(lambda x: ( sorted(filter(lambda y: "expected68_" in y.GetName(), x[0]),key=lambda h: h.GetName()),x[1]), self.compare)
      ocoups = set(map(lambda x: x.GetName().rsplit("_",1)[-1], reduce(lambda x,y: x+y, map(lambda z: z[0], expected), [])))
      observed = map(lambda x: ( sorted(filter(lambda y: "observed" in y.GetName(), x[0]),key=lambda h: h.GetName()),x[1]), self.compare)
      ecoups = set(map(lambda x: x.GetName().rsplit("_",1)[-1], reduce(lambda x,y: x+y, map(lambda z: z[0], observed), [])))
      
      print expected0

      assert( ocoups == ecoups )
      coups = sorted(list(ocoups))
      
      if len(options.couplings) > 0:
        coups = options.couplings
        
      observed = map(lambda w: (map(lambda x: x[1], filter(lambda y: y[0] in coups, map(lambda z: (z.GetName().rsplit("_",1)[-1],z), w[0] ) ) ),w[1] ), observed )
      expected = map(lambda w: (map(lambda x: x[1], filter(lambda y: y[0] in coups, map(lambda z: (z.GetName().rsplit("_",1)[-1],z), w[0] ) ) ),w[1] ), expected )
      expected0 = map(lambda w: (map(lambda x: x[1], filter(lambda y: y[0] in coups, map(lambda z: (z.GetName().rsplit("_",1)[-1],z), w[0] ) ) ),w[1] ), expected0 )
      
      nc = len(coups)
      for ic in range(nc): 
        if self.options.do_pvalues:
          name = "summary_pvalue_%s_%s" % ( self.options.label, coups[ic] )
        else:
          name = "summary_limits_%s_%s" % ( self.options.label, coups[ic] )
          
          
        canv = ROOT.TCanvas(name,name,900,600)

        exp_sty = [ [["colors",ROOT.kBlack], ["SetFillColor",[ROOT.kOrange]], ["SetLineStyle",7], ["SetLineWidth",2] ], 
                    [["colors",ROOT.kBlue], ["SetFillColor",[ROOT.kAzure+1]], ["SetFillStyle",[3357]],  ["SetLineStyle",7], ["SetLineWidth",2] ] 
                    ]
        obs_sty = [ [["colors",ROOT.kBlack],  ["SetLineWidth",2]],
                    [["colors",ROOT.kBlue], ["SetLineWidth",2]]
                    ]
        
        ### exp_sty = [ [["colors",ROOT.myColorA1], ["SetFillColor",[ROOT.myColorA3tr]], ["SetLineStyle",7], ["SetLineWidth",2] ], 
        ###             [["colors",ROOT.myColorB1], ["SetFillColor",[ROOT.myColorB3tr]], ["SetFillStyle",[3357]],  ["SetLineStyle",7], ["SetLineWidth",2] ] 
        ###             ]
        ### obs_sty = [ [["colors",ROOT.myColorA1],  ["SetLineWidth",2]],
        ###             [["colors",ROOT.myColorB1], ["SetLineWidth",2]]
        ### ]
      
        exp0 = map(lambda x: x[0][nc-ic-1], expected0)
        exp = map(lambda x: x[0][nc-ic-1], expected)
        obs = map(lambda x: x[0][nc-ic-1], observed)
        
        map(lambda x: style_utils.apply(x[0],x[1]), zip(exp0,exp_sty) )
        map(lambda x: style_utils.apply(x[0],x[1]), zip(exp,exp_sty) )
        map(lambda x: style_utils.apply(x[0],x[1]), zip(obs,obs_sty) )
        
        g0 = exp[0]
        ymin = 0.
        allyup = reduce(lambda x,y: x+y, map(lambda z: map(lambda ip: z.GetY()[ip]+z.GetErrorY(ip), range(z.GetN()) ), exp ) )
        ymax = ceil(max(allyup))*1.1
        
        maxe = max( map(lambda y: max( map( lambda x: y.GetY()[x]+y.GetErrorYhigh(x) , range(y.GetN()) ) ), exp ) )
        maxo = max( map(lambda y: y.GetMaximum(), obs ) )
        ymax = max(maxe,maxo)*1.3
        g0.GetYaxis().SetRangeUser(0,ymax)
        if len(options.x_range) != 0:
          g0.GetXaxis().SetRangeUser(*options.x_range)
          ## g0.GetXaxis().SetLimits(*options.x_range)
        ROOT.gPad.SetLogx()
        g0.GetXaxis().SetMoreLogLabels()

        leghead = copy(options.legend)        
        legend = copy(options.legend)
        nent = nc+2
        y0 = min(leghead[1],leghead[3])
        y1 = max(leghead[1],leghead[3])
        height = (y1-y0)/float(nent)
        leghead[3] = y1
        leghead[1] = y1 - height
        leghead[0] -= 0.07
        leghead[2] -= 0.05
        legend[3] = y1 - height
        legend[1] = y0
        print legend, leghead
        leghead = ROOT.TLegend(*leghead)
        leghead.SetFillStyle(0)
        legend = ROOT.TLegend(*legend)

        kappa = float("0."+coups[ic][1:])
        txt = self.getLegendHeader(kappa,g0)
        leghead.AddEntry(None,txt,"")

        for ip,pair in enumerate(zip(exp,obs)):
            ex,ob = pair
            if ip == 0:
              legend.AddEntry(ex,"%s expected #pm 1 s.d." % expected[ip][1],"lf")
            else:
              legend.AddEntry(ex,"%s expected" % expected[ip][1],"l")
            legend.AddEntry(ob,"%s observed" % observed[ip][1],"l")
        

        g0.Draw("ALE3")
        ## map(lambda x: x.Draw("LE3"), exp[1:])
        map(lambda x: x.Draw("L"), exp0)
        map(lambda x: x.Draw("L"), obs)
        
        coup = coups[nc-ic-1]
        if coup in self.xsections_:
          grav = self.xsections_[coup]
          style_utils.apply( grav, [["SetLineWidth",2],["SetLineStyle",5],["colors",ROOT.kRed+1]] )
          grav.Draw("C")
          legend.AddEntry(grav,"G_{RS}#rightarrow#gamma#gamma, #tilde{k}=%s (LO)" % skappa,"l").SetLineStyle(0)
          self.keep(grav)

        legend.Draw("same")
        leghead.Draw("same")
        self.keep( [legend,leghead] )
        self.keep( exp+obs )
        
        
        self.keep(canv)
        self.format(canv,options.postproc)
      
        self.autosave()

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
        styles = [ [["colors",ROOT.kBlack]], [["colors",ROOT.kBlue],["SetLineStyle",7]], [["colors",ROOT.kRed],["SetLineStyle",options.extra_lines_style]],
                   [["colors",ROOT.kAzure+1],["SetLineStyle",9] ], ]
        
        map(lambda x: style_utils.apply(x[0],[["SetMarkerSize",0.3],["SetLineWidth",2]]+styles.pop(0)), cobserved)

        ## if options.add_inset:
        canv = ROOT.TCanvas("comparison_%s%s" % (options.label,coup),"comparison_%s%s"  % (options.label,coup),900,600 )
        canv.SetLeftMargin(0.12)
        canv.SetRightMargin(0.07)
        ##else:
        ##  canv = ROOT.TCanvas("comparison_%s%s" % (options.label,coup),"comparison_%s%s"  % (options.label,coup) )
          
        leghead = copy(options.legend)        
        legend = copy(options.legend)
        nent = len(cobserved)+1
        y0 = min(leghead[1],leghead[3])
        y1 = max(leghead[1],leghead[3])
        x0 = min(leghead[0],leghead[2])
        x1 = max(leghead[0],leghead[2])
        height = min((y1-y0)/float(nent),options.legend_head_maxh)
        width = min((x1-x0),options.legend_head_maxw)
        leghead[3] = y1
        leghead[1] = y1 - height
        legend[3] = y1 - height
        legend[1] = y0
        leghead[0] = x0 - 0.05
        leghead[2] = x0 - 0.03 + width
        print legend, leghead
        leghead = ROOT.TLegend(*leghead)
        leghead.SetFillStyle(0)
        legend = ROOT.TLegend(*legend)
        

        legend.SetFillStyle(0)
        kappa = "0."+coup[1:]
        
        g0 = cobserved[0][0]
        
        kappa = float(kappa)
        txt = self.getLegendHeader(kappa,g0)
        leghead.AddEntry(None,txt,"")
        ## legend.AddEntry(None,txt,"")
            
        if options.xtitle:
          g0.GetXaxis().SetTitle(options.xtitle)
        g0.Draw("ac")
        
        xmin,xmax = options.x_range
        g0.GetXaxis().SetRangeUser(xmin,xmax)
        g0.GetXaxis().SetMoreLogLabels()
          
        canv.SetLogx()
        if options.do_pvalues:
            canv.SetLogy()
            g0.GetYaxis().SetRangeUser(1e-3,0.55)
            g0.GetYaxis().SetTitle("#it{p_{0}}")
            
        if len(options.y_range) > 0:
          g0.GetYaxis().SetRangeUser(*options.y_range)
          g0.GetYaxis().SetLimits(*options.y_range)
          
        if options.do_pvalues:
          self.drawLines(g0,xmin,xmax)

        for gr,nam in cobserved:
            legend.AddEntry(gr,nam,"l")
        for gr,nam in reversed(cobserved):
            gr.Draw("c")
        leghead.Draw("same")
        legend.Draw("same")

        if options.add_inset:
          ## inset = ROOT.TPad("inset","inset",0.58,0.135,0.91,0.765)
          inset = ROOT.TPad("inset","inset",0.58,0.133,0.91,0.763)
          inset.SetLeftMargin(0.12)
          inset.SetRightMargin(0.1)
          inset.Draw("")
          inset.SetLogy()
          # inset.SetFillStyle(0)
          inset.cd()
          g1 = g0.Clone()
          g1.Draw("al")
          g1.GetXaxis().SetRangeUser(*options.inset)
          if len(options.y_range) > 0:
            g1.GetYaxis().SetRangeUser(*options.y_range)
            g1.GetYaxis().SetLimits(*options.y_range)
          self.drawLines(g1,*options.inset)
          for gr,nam in reversed(cobserved):
            gr.Draw("l")
          self.keep([inset,g1])

        self.keep([canv,legend,leghead])
        self.format(canv,options.postproc)
        if ROOT.cmsText == "":
          canv.cd()
          ptCMS=ROOT.TLatex(0.15,0.2,"CMS")
          ptCMS.SetNDC()
          ptCMS.SetTextFont(61)
          ptCMS.SetTextSize(0.07)
          ptCMS.Draw("same")
          ptCMS.Print()
          self.keep(ptCMS)
        self.autosave(True)
        
    def drawLines(self,ref,xmin=450,xmax=5000):
        
        spots = filter(lambda y: y[1]>float(ref.GetYaxis().GetXmin()),  map(lambda x: (x,ROOT.RooStats.SignificanceToPValue(x)), xrange(1,5) ) )
        lines = map( lambda y: ROOT.TLine(xmin,y[1],xmax,y[1]), spots )
        map( lambda x: style_utils.apply(x,[["SetLineColor",ROOT.kGray+1],["SetLineStyle",9]]), lines )
        
        labels = map( lambda y: ROOT.TLatex(xmax*1.01,y[1]*0.9,"#color[%d]{%d #sigma}" % (ROOT.kGray+2,y[0])), spots )
        map( lambda x: style_utils.apply(x,[["SetTextSize",0.05]]), labels )

        map( lambda x: x.Draw("same"), lines+labels )
        self.keep(lines+labels)
        
    def plotPval(self,options,coup,tfile):
        observed = ROOT.theBand( tfile, 1, 0, ROOT.Observed, 0.95 )
        basicStyle = [["SetMarkerSize",0.6],["SetLineWidth",3],
                       ["SetTitle",";#it{m}_{G} (GeV);#it{p_{0}}"]]
        if not options.spin2:
            basicStyle.append(["SetTitle",";#it{m}_{S} (GeV);#it{p_{0}}"])
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
            legend.AddEntry(None,"#tilde{k} = %s" % kappa,"")
        else:
            kappa = float(kappa)
            legend.AddEntry(None,"#frac{#Gamma}{m} = %g #times 10^{-2}" % (1.4*kappa*kappa*100.),"")
        legend.AddEntry(observed,"Observed #it{p_{0}}","l")
        
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
