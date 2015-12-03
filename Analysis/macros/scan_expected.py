#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os, sys, glob, json
import random, commands

from auto_plotter import getObjects

import itertools

from math import fabs

# -----------------------------------------------------------------------------------------------------------
class ScanExpected(PlotApp):

    def __init__(self):
        super(ScanExpected,self).__init__(option_list=[
                make_option("--datacard",action="store",dest="datacard",default=None),
                make_option("-m","--mass",action="store",dest="mass",default=None),
                make_option("-k","--kmpl",action="store",dest="kmpl",default=None),
                make_option("-T","--target",action="store",type="float",dest="target",default=3.),
                make_option("--tolerance",action="store",type="float",dest="tolerance",default=0.01),
                make_option("-t","--toys",action="store",type="int",dest="toys",default=-1),
                make_option("-g","--grid",action="callback",type="string",dest="grid",callback=optpars_utils.ScratchAppend(float),default=[]),
                make_option("--options",action="store",type="string",dest="options",default=""),
                make_option("--label",action="store",type="string",dest="label",default=""),
                make_option("--max-points",action="store",type="int",dest="max_points",default=15),
                make_option("--plot-results",action="store_true",dest="plot_results",default=False),
                make_option("--run-scan",action="store_true",dest="run_scan",default=True),
                make_option("--min-mass",action="store",type="float",dest="min_mass",default=None),
                make_option("--max-mass",action="store",type="float",dest="max_mass",default=None),
                make_option("--plot-bias",action="store_true",dest="plot_bias",default=False),
                make_option("--bias",action="callback",type="string",dest="bias",callback=optpars_utils.Csv(),default=[]),
                make_option("--compare-sensitivity",action="store_true",dest="compare_sensitivity",default=False),
                make_option("--bias-file",action="store",dest="bias_file",default=None),
                make_option("--limit-bias-file",action="store",dest="limit_bias_file",default=None),
                make_option("--limit-no-bias-file",action="store",dest="limit_no_bias_file",default=None),
                make_option("--zscan-bias-file",action="store",dest="zscan_bias_file",default=None),
                make_option("--zscan-no-bias-file",action="store",dest="zscan_no_bias_file",default=None),
                
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

        
        if options.plot_results:
            self.plotResults(options,args)
        elif options.plot_bias:
            self.plotBias(options,args)
        elif options.compare_sensitivity:
            self.compareSensitivity(options,args)
        elif options.run_scan:
            self.runScan(options,args)
            

    def compareSensitivity(self,options,args):
        

        ## self.open(options.limit_bias_file).GetListOfKeys().Print()
        bias_limits = filter(lambda x: "expected_" in x.GetName(),  getObjects( [self.open(options.limit_bias_file)], types=["TGraph"] ) ) if options.limit_bias_file else []
        
        no_bias_limits = filter(lambda x: "expected_" in x.GetName(), getObjects( [self.open(options.limit_no_bias_file)], types=["TGraph"] ) ) if options.limit_no_bias_file else []

        bias_zscans = filter(lambda x: "_bias" in x.GetName(), getObjects( [self.open(options.zscan_bias_file)], types=["TGraph"] ) ) if options.zscan_bias_file else []
        no_bias_zscans = filter(lambda x: "_nobias" in x.GetName(), getObjects( [self.open(options.zscan_no_bias_file)], types=["TGraph"] ) ) if options.zscan_bias_file else []

        biases = getObjects( [self.open(options.bias_file)], types=["TGraph"] ) if options.bias_file else []
        

        ## print bias_limits, no_bias_limits, bias_zscan, no_bias_zscan, biases
        print bias_zscans, no_bias_zscans,
        
        coups = map( lambda x: x.GetName().replace("expected_",""), bias_limits )
        print coups
        
        common = [["SetLineStyle",0],["SetMarkerSize",1],["SetLineWidth",2]]
        styles = [ [["colors",ROOT.kRed],["SetMarkerStyle",ROOT.kFullCircle]], 
                   [["colors",ROOT.kBlue],["SetMarkerStyle",ROOT.kFullTriangleUp]],
                   [["colors",ROOT.kGreen],["SetMarkerStyle",ROOT.kFullDiamond]],
                   [["colors",ROOT.kGreen+1],["SetMarkerStyle",ROOT.kOpenDiamond]],
                   ]
        

        for coup in coups:
            bias_limit = filter(lambda x: x.GetName().endswith("_%s" % coup), bias_limits )
            no_bias_limit = filter(lambda x: x.GetName().endswith("_%s" % coup), no_bias_limits )
     
            bias_zscan = filter(lambda x: x.GetName().startswith("%s_" % coup), bias_zscans )
            no_bias_zscan = filter(lambda x: x.GetName().startswith("%s_" % coup), no_bias_zscans )
            
            bias_terms = filter(lambda x: x.GetName().startswith("%s_" % coup), biases )
            
            for g in bias_limit: g.SetTitle("expected limit bias")
            for g in no_bias_limit: g.SetTitle("expected limit no bias")
            for g in bias_zscan: g.SetTitle("expected #sigma #times BR (Z_{0}=3) bias")
            for g in no_bias_zscan: g.SetTitle("expected #sigma #times BR (Z_{0}=3) no bias")

            for g in bias_terms: g.SetTitle("bias term %s" % g.GetName().split("_")[-1])
            
            print bias_zscan, no_bias_zscan

            if len(bias_limit) > 0:
                graphs = bias_limit+no_bias_limit+bias_terms
                map( lambda x: style_utils.apply(x[0],common+x[1]), zip(graphs,styles) )
                
                canv  = ROOT.TCanvas("limits_comparison_%s" % coup,"limits_comparison_%s" % coup)
                legend = ROOT.TLegend(0.5,0.6,0.8,0.9)
                legend.AddEntry(None,"#tilde{#kappa} = 0.%s" % coup[1:],"")
                
                ypoints = reduce( lambda x,y: x+y, map(lambda g: [g.GetY()[ip] for ip in xrange(g.GetN())], graphs ) )
                ymin = min( ypoints ) 
                ymax = max( ypoints ) 
                
                
                graphs[0].Draw("al")
                graphs[0].GetYaxis().SetRangeUser(0.5*ymin,1.5*ymax)
                for g in graphs[1:]: g.Draw("l")
                
                for g in graphs: legend.AddEntry(g,g.GetTitle(),"l")
                legend.Draw()
                
                self.keep([canv,graphs,legend])
                self.autosave(True)
    
            if len(bias_zscan) > 0:
                graphs = bias_zscan+no_bias_zscan+bias_terms
                map( lambda x: style_utils.apply(x[0],common+x[1]), zip(graphs,styles) )
                
                canv  = ROOT.TCanvas("zscans_comparison_%s" % coup,"zscans_comparison_%s" % coup)
                legend = ROOT.TLegend(0.5,0.6,0.8,0.9)
                legend.AddEntry(None,"#tilde{#kappa} = 0.%s" % coup[1:],"")
                
                ypoints = reduce( lambda x,y: x+y, map(lambda g: [g.GetY()[ip] for ip in xrange(g.GetN())], graphs ) )
                ymin = min( ypoints ) 
                ymax = max( ypoints ) 
                
                
                graphs[0].Draw("al")
                graphs[0].GetYaxis().SetRangeUser(0.5*ymin,1.5*ymax)
                for g in graphs[1:]: g.Draw("l")
                
                for g in graphs: legend.AddEntry(g,g.GetTitle(),"l")
                legend.Draw()
                
                print canv
                self.keep([canv,graphs,legend])
                self.autosave(True)
    
    def graphFromBiasPoints(self,name,points):
        gr = ROOT.TGraph()
        gr.SetName(name)
        
        print set( map( lambda x: x['coup'], points ) )
        map( lambda x: gr.SetPoint(gr.GetN(),float(x['mass']),float(x['bias'])*1.e-3), points)
        gr.Sort()
        return gr
    
    def plotBias(self,options,args):
        
        coups = sorted( set( map(lambda x: x['coup'], options.bias ) ), cmp=lambda x,y: -cmp( float("0."+x[1:]), float("0."+y[1:]), ) )
        cats  = set( map(lambda x: x['cat'], options.bias ) )
        prod  = itertools.product(coups,cats)
        graphs = map(lambda x: self.graphFromBiasPoints("_".join(x[0]),x[1]), map(lambda y: (y,filter(lambda z: z['coup'] == y[0] and z['cat'] == y[1], options.bias )), prod ) )
        
        ## map( lambda x: x.Print("V"), graphs )
        common = [["SetLineWidth",2],["SetTitle",";M_{G} (GeV);N_{bias} / ( #varepsilon A L ) (pb)"  ]]
        styles = [ [["colors",ROOT.kRed],["SetMarkerStyle",ROOT.kFullCircle]], 
                   [["colors",ROOT.kRed+1],["SetMarkerStyle",ROOT.kOpenCircle],["SetLineStyle",7]], 
                   [["colors",ROOT.kBlue],["SetMarkerStyle",ROOT.kFullTriangleUp]],
                   [["colors",ROOT.kBlue+1],["SetMarkerStyle",ROOT.kOpenTriangleUp],["SetLineStyle",7]],
                   [["colors",ROOT.kGreen],["SetMarkerStyle",ROOT.kFullDiamond]],
                   [["colors",ROOT.kGreen+1],["SetMarkerStyle",ROOT.kOpenDiamond],["SetLineStyle",7]],
                   [["colors",ROOT.kOrange]],
                   [["colors",ROOT.kOrange+1],["SetLineStyle",7]],
                   ]

        for cat in cats:
            canv  = ROOT.TCanvas("bias_%s" % cat,"bias_%s" % cat)
            canv.SetLogy()
            legend = ROOT.TLegend(0.5,0.6,0.8,0.9)
            
            cgraphs = filter(lambda x: cat in x.GetName(), graphs )
            map( lambda x: style_utils.apply(x[0],common+x[1]), zip(cgraphs,styles) )
            cgraphs[0].Draw("al")
            for g in cgraphs[1:]: g.Draw("l")
            
            for g in cgraphs: legend.AddEntry(g,g.GetName(),"l")
            
            legend.Draw()
            self.keep([canv,cgraphs,legend])
            self.autosave(True)

        gfile = self.open("graphs_bias.root","recreate")
        gfile.cd()
        for gr in graphs: gr.Write()
        gfile.Close()

    def getPoint(self,content):
        
        mu, z0 = content["closest"]
        
        ## print content
        ## print content["label"]
        
        mass =  float(content["mass"])
        if( self.options.min_mass and mass < self.options.min_mass ): return None
        if( self.options.max_mass and mass > self.options.max_mass ): return None
        
        if( fabs(z0 - self.options.target) > self.options.tolerance ):
            tmp = ROOT.TGraph()
            map( lambda x: tmp.SetPoint(tmp.GetN(),x[1],x[0]), content["scan"] )
            tmp.Sort()
            tmp.Fit("pol1","+")
            mu = tmp.GetListOfFunctions().At(0).Eval(z0)
            
            ## cname = "_".join( [content["label"],content["kmpl"],content["mass"]] )
            ## canv = ROOT.TCanvas(cname,cname)
            ## tmp.SetMarkerStyle(ROOT.kFullCircle)
            ## tmp.Draw("apl")
            ## self.keep(canv)
            ## self.autosave(True)
        
        return mass,mu*1e-3
    
        
    def graphFromScan(self,scan):
        
        label, content = scan
        
        ## print label
        
        gr = ROOT.TGraph()
        label = "_".join( reversed(label.split("_") ) )
        gr.SetName(label), gr.SetTitle(label)
        
        map( lambda x: gr.SetPoint(gr.GetN(),x[0],x[1]) if x else None, map( self.getPoint, content ) )
        
        gr.Sort()
        self.keep(gr)
        ## print content
        ## gr.Print()
        return gr
        
    def loadPoint(self,fname):
        
        ret = None
        
        if "toys" in fname: return ret
        
        with open(fname) as fin:
            ret = json.loads(fin.read())
            ## if ret["label"] == "" or not ret["label"].startswith("nobias"): ret["label"] = "bias"+ret["label"] # FIXME
            fin.close()
        return ret
    
    def plotResults(self,options,args):
        
        flist = glob.glob("%s/scan*.json" % (options.input_dir) )
        
        points = filter(lambda x: x!=None, map(self.loadPoint, flist) )

        keys = set( map( lambda x: "%s_%s" % (x["label"],x["kmpl"]), points ) )
        scans = {}
        for key in keys: scans[key] = []
        map( lambda x: scans["%s_%s" % (x["label"],x["kmpl"])].append(x), points )
        
        graphs = map( self.graphFromScan, scans.iteritems() )

        kmpls = set( map( lambda x: x["kmpl"], points ) )
        
        common = [["SetTitle",";M_{G} (GeV);#sigma #times BR (Z_{0} = %1.1f) (pb)" % self.options.target ]]
        styles = [ [["colors",ROOT.kRed],["SetMarkerStyle",ROOT.kFullCircle]], 
                   [["colors",ROOT.kRed+1],["SetMarkerStyle",ROOT.kOpenCircle],["SetLineStyle",7]], 
                   [["colors",ROOT.kBlue],["SetMarkerStyle",ROOT.kFullTriangleUp]],
                   [["colors",ROOT.kBlue+1],["SetMarkerStyle",ROOT.kOpenTriangleUp],["SetLineStyle",7]],
                   [["colors",ROOT.kGreen],["SetMarkerStyle",ROOT.kFullDiamond]],
                   [["colors",ROOT.kGreen+1],["SetMarkerStyle",ROOT.kOpenDiamond],["SetLineStyle",7]],
                   ]
        
        for kmpl in kmpls:
            kgraphs = sorted( filter( lambda x: x.GetName().startswith("%s_" % kmpl), graphs ), key=lambda x: x.GetName() )
            kstyles = [ styles[0], styles[3] ] if len( kgraphs ) == 2 else styles
            map( lambda x: style_utils.apply(x[0],common+x[1]), zip(kgraphs,kstyles) )
            cname = "zscan_k%s"%kmpl
            if options.min_mass: cname += "_%1.0f" % options.min_mass
            if options.max_mass: cname += "_%1.0f" % options.max_mass
            canv  = ROOT.TCanvas(cname,cname)
            ## canv.SetLogy()
            legend = ROOT.TLegend(0.5,0.6,0.8,0.9)
            legend.AddEntry(None,"#tilde{#kappa} = 0.%s" % kmpl[1:],"")
                    
            kgraphs[0].Draw("apl")
            for gr in kgraphs: gr.Draw("pl")
            map( lambda x: legend.AddEntry(x,x.GetName(),"lp" ), kgraphs )
            
            legend.Draw("same")
            self.keep( [canv,legend] )
            self.autosave(True)
        
        gfile = self.open("%s/graphs_zscan.root" % (options.input_dir),"recreate")
        gfile.cd()
        for gr in graphs: gr.Write()
        gfile.Close()


    def runScan(self,options,args):
        if not options.datacard:
            options.datacard = args[0]
            
        if not options.mass or not options.kmpl: 
            guess =  options.datacard.split("grav_")[-1].replace(".txt","").split("_")
        
        if not options.mass: options.mass = guess[1]
        if not options.kmpl: options.kmpl = guess[0]
        
        if len(options.grid) == 0:
            options.grid.append( self.runOnePoint(options,0.,-1,"Asymptotic")  )

        distance = options.tolerance * 10.
        scan = []
        tested = []
        last = None
        graph = ROOT.TGraph()
        while distance > options.tolerance and len(scan) < options.max_points:
            print scan
            print options.grid
            while len(options.grid) > 0:
                point = options.grid.pop()
                scan.append( (point, self.runOnePoint(options,point,options.toys)) )
                tested.append( point )
            scan = sorted( scan, cmp=lambda x,y: cmp(fabs(options.target-x[1]),fabs(options.target-y[1])) )
            closest = scan[0]
            delta = options.target - closest[1]
            distance = fabs( delta )
            if distance > options.tolerance:
                newpoint = -1.
                guesses = []
                if len(scan) == 1:
                    guesses.append( (0,0) )
                else:
                    guesses = scan[1:]
                p1 = closest
                while newpoint < 0 or newpoint in tested:
                    if len(guesses) == 0:
                        newpoint = random.uniform(0.5,2.)*p1[0]
                    else:
                        p2 = guesses.pop(0)
                        deltax = p2[0] - p1[0] 
                        deltay = p2[1] - p1[1]
                        slope = deltax / deltay
                        newpoint = p1[0] + slope * delta
                options.grid.append( newpoint  )
            last = closest
            
        with open("scan%s_%s_%s.json" % (options.label, options.kmpl, options.mass) ,"w+") as outf:
            outf.write(json.dumps(
                    { "kmpl" : options.kmpl,
                      "mass" : options.mass,
                      "label" : options.label,
                      "closest" : closest,
                      "scan" : scan
                    }))
            outf.close()

        print closest
        
    def runOnePoint(self,options,point,ntoys,method="ProfileLikelihood"):
        
        seed = random.randint(0,1000)
        
        if method == "Asymptotic":
            cmd = "combine -L libdiphotonsUtils -m %s -M %s --run expected -n _scan%s%s -s %d %s" % ( 
                options.mass, method, options.label, options.kmpl, 
                seed, options.datacard  )
        else:                                    
            cmd = "combine -L libdiphotonsUtils --significance -m %s -M %s -t %d -n _scan%s%s --expectSignal %f --rMax %f -s %d %s %s" % ( 
                options.mass, method, ntoys, options.label, options.kmpl, 
                point, 20.*point, seed, options.options, options.datacard  )
        
        if options.verbose:
            print cmd
        stat,out = commands.getstatusoutput(cmd)
        if options.verbose:
            print out
        
        fname  = "higgsCombine_scan%s%s.%s.mH%s.%d.root" % ( options.label, options.kmpl, method, options.mass, seed )
        
        tfile = self.open(fname)
        if method == "Asymptotic":
            ROOT.use_precomputed_quantiles = True
            bandType =  ROOT.Median
        elif options.toys == -1:
            ROOT.use_precomputed_quantiles = True
            bandType = ROOT.Asimov
        else:
            bandType = ROOT.Median
            ROOT.use_precomputed_quantiles = False

        expected = ROOT.theBand( tfile, 1, 0, bandType, 0.68 )
        
        
        ## print expected
        if options.verbose:
            expected.Print()
        
        return expected.Eval( float(options.mass) )
        
        
        
        
# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app = ScanExpected()
    app.run()
