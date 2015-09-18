
from pyrapp import *
import array

from style_utils import scaleFonts

# -----------------------------------------------------------------------------------------------------------
def rename(h,cmap):
    ## print "renaming ", h.GetNam()
    toks = h.GetName().split("_")
    cat = None
    for t in toks:
        if t in cmap:
            cat = t
            ## print "cat: ", cat
            break
    if cat:
        newname = h.GetName().replace(cat,cmap[cat])
        h.SetName(newname)

    ## print "renamed ", h.GetName()

# -----------------------------------------------------------------------------------------------------------
def smoothErrors(grae):
   w = [ 0.10,0.25,0.30,0.25,0.10 ]
   np = grae.GetN()
   e1 = [0. for ip in range(np)]
   e2 = [0. for ip in range(np)]

   for ip in range(2,np-2):
       for jp in range(-2,3):
           e1[ip] += w[jp+2]*grae.GetErrorYlow(ip-jp)
           e2[ip] += w[jp+2]*grae.GetErrorYhigh(ip-jp)
   for ip in 0,1,np-1,np-2:
       e1[ip] = grae.GetErrorYlow(ip)
       e2[ip] = grae.GetErrorYhigh(ip)

   for ip in range(np):
       grae.SetPointEYhigh(ip,e1[ip])
       grae.SetPointEYlow(ip,e2[ip])
       
       
## 
def getQuantilesGraphs(histo,probs,twosided=False,errors=True,sign=1):
    from math import sqrt
    ## histo.Print("all")
    graphs = [ ]
    if len(probs) == 0:
        return graphs
    for p in probs:
        gr = ROOT.TGraphErrors(histo.GetNbinsX())
        gr.SetName("%s_quantile_%1.2f"%(histo.GetName(),100.*p))
        graphs.append(gr)
    ## graphs = [ ROOT.TGraphErrors(histo.GetNbinsX()) for p in probs ]
    if twosided:
        qtiles = []
        for p in probs:
            t = 0.5 - p*0.5
            qtiles.append( t )
            qtiles.append( 1-t )
    else:
        if sign > 0:
            qtiles=probs
        else:
            qtiles = [1.-p for p in probs]
        
    nq = len(qtiles)
    graph = ROOT.TGraph(nq+2)
    for iq in range(nq):
        graph.SetPoint(iq,qtiles[iq],0.)
    for g in graphs:        
        g.GetXaxis().SetTitle(histo.GetXaxis().GetTitle())
        g.SetMarkerStyle(histo.GetMarkerStyle())
    graph.SetPoint(nq,0.25,0.)
    graph.SetPoint(nq+1,0.75,0.)
        
    for ix in range(1,histo.GetNbinsX()+1):
        proj = histo.ProjectionY("qtiles",ix,ix)
        
        ## proj.Print("all")
        ## graph.Print("all")
        proj.GetQuantiles(nq+2,graph.GetY(),graph.GetX())
        ntot = proj.Integral()
        if ntot == 0: continue
        h = 1.2*( graph.GetY()[nq+1] - graph.GetY()[nq] ) * pow(ntot,-0.2)
        
        if twosided:
            for ig in range(nq/2):                
                quant1 = graph.GetY()[ig]
                quant2 = graph.GetY()[ig+1]
                quant = (quant2 - quant1)*0.5                
                quant1mh = proj.FindBin( quant1 - h*0.5 )
                quant1ph = proj.FindBin( quant1 + h*0.5 )
                quant2mh = proj.FindBin( quant2 - h*0.5 )
                quant2ph = proj.FindBin( quant2 + h*0.5 )
                
                nint = proj.Integral( quant1mh, quant1ph ) + proj.Integral( quant2mh, quant2ph )
                fq = nint / (2.*h*ntot)
                
                err = 1./(2.*sqrt(ntot)*fq)

                graphs[ig/2].SetPoint(ix-1,histo.GetXaxis().GetBinCenter(ix),quant)
                if errors:
                    graphs[ig/2].SetPointError(ix-1,histo.GetXaxis().GetBinWidth(ix)*0.5,err)
                else:
                    graphs[ig/2].SetPointError(ix-1,histo.GetXaxis().GetBinWidth(ix)*0.5,0.)
                
        else:
            for ig in range(nq):
                quant = graph.GetY()[ig]
                quantmh = proj.FindBin( quant - h )
                quantph = proj.FindBin( quant + h )
                nint = proj.Integral( quantmh, quantph )
                fq = nint / (2.*h*ntot)
                
                err = 1./(2.*sqrt(ntot)*fq)
                
                graphs[ig].SetPoint(ix-1,histo.GetXaxis().GetBinCenter(ix),quant)
                if errors:
                    graphs[ig].SetPointError(ix-1,histo.GetXaxis().GetBinWidth(ix)*0.5,err)
                else:
                    graphs[ig].SetPointError(ix-1,histo.GetXaxis().GetBinWidth(ix)*0.5,0.)
                
    return graphs

## 
def getEfficiencyGraphs(histo,thresholds,sign=+1):
    
    graphs = []
    total = histo.ProjectionX()
    for thr in thresholds:
        thrbin = histo.GetYaxis().FindBin(thr)
        print thrbin, histo.GetYaxis().GetBinCenter(thrbin)
        if sign>0:
            num = histo.ProjectionX("_num",thrbin,-1)
        else:
            num = histo.ProjectionX("_num",0,thrbin)
        num.Print()
        total.Print()
        histo.Print()
        graph = ROOT.TGraphAsymmErrors(num,total,"cp")
        graphs.append(graph)
        
        histo.GetYaxis().SetRange(-1,-1)
        
    return graphs
        

# -----------------------------------------------------------------------------------------------------------
class PlotApp(PyRApp):

    def __init__(self,option_list=[],option_groups=[],default_cats=[]):
        super(PlotApp,self).__init__(option_groups=[
                ("PlotApp options", [
                        make_option("-c","--categories",dest="categories",action="callback",callback=ScratchAppend(),type="string",
                                    default=default_cats,help="default: %default"),
                        make_option("-l","--labels",dest="labels",action="callback",callback=Load(),metavar="JSON",
                                    default={},help="default: %default"),
                        make_option("-p","--plots",dest="plots",action="callback",callback=Load(),metavar="JSON",
                                    default=[],help="default: %default"),
                        make_option("-t","--template",dest="template",action="store",type="string",
                                    default="%(name)s_%(cat)s_%(sample)s",help="default: %default"),
                        make_option("--postproc",dest="postproc",action="callback",callback=Load(),metavar="JSON",
                                    default={},help="default: %default"),
                        make_option("--input-dir",dest="input_dir",action="store",type="string",
                                    default=None,help="default: %default"),
                        make_option("--data",dest="data",action="callback",callback=Load(),metavar="JSON",
                                    default=None,help="default: %default"),
                        make_option("--sig",dest="sig",action="callback",callback=Load(),metavar="JSON",
                                    default=None,help="default: %default"),
                        make_option("--bkg",dest="bkg",action="callback",callback=Load(),metavar="JSON",
                                    default=None,help="default: %default"),
                        make_option("-i","--infile",dest="infile",action="store",type="string",
                                    default=None,help="default: %default"),
                        make_option("--data-file",dest="data_file",action="store",type="string",
                                    default=None,help="default: %default"),
                        make_option("--lumi",dest="lumi",action="store",type="float",
                                    default=None,help="default: %default"),
                        make_option("--sig-file",dest="sig_file",action="store",type="string",
                                    default=None,help="default: %default"),
                        make_option("--bkg-file",dest="bkg_file",action="store",type="string",
                                    default=None,help="default: %default"),
                        make_option("--legpos",dest="legpos",action="callback",callback=ScratchAppend(float),type="string",
                                    default=[],help="default: %default"),
                        make_option("--rootstyle-extra",dest="rootstyle_extra",action="callback",callback=ScratchAppend(str),type="string",
                                    default=[],help="default: %default"),
                        ])
                        ]+option_groups,
                 option_list=option_list)
        

        self.data_ = None
        self.sig_  = None
        self.bkg_  = None
        self.init_ = False

        global ROOT, style_utils
        import ROOT
        import style_utils

    def __call__(self,options,args):

        if not self.init_:
            self.template_ = options.template
            
            self.loadRootStyle()
            
            ## ROOT.gStyle.Print()
            ROOT.gROOT.ForceStyle()
            self.init_ = True

        ## print options.bkg_file
        self.data_ = self.openDataset(self.data_,options.data_file,options.infile,options.data)
        self.sig_  = self.openDataset(self.sig_ ,options.sig_file ,options.infile,options.sig )
        self.bkg_  = self.openDataset(self.bkg_ ,options.bkg_file ,options.infile,options.bkg )
        
        
        self.setStyle("*", [(rename,options.labels)] )
        self.setStyle("*_legend",[("SetFillStyle",0),("SetLineColor","kWhite"),("SetShadowColor","kWhite")] )
        
        self.dataMcComparison(options,args)

    def openDataset(self,dst,fname,default,extra):
        global ROOT
        
        ### print "openDataset",fname

        if not fname:
            fname = default
        if fname == "_per_subproc_":
            return fname,extra
        if (not dst) and fname and extra:
            fin = self.open(fname, folder=self.options.input_dir)
            return fin,extra
        
        return dst
    
    def dataMcComparison(self,options,args):
        data  = self.data_
        bkg   = self.bkg_
        sig   = self.sig_
        plots = options.plots
        
        ## ROOT.gStyle.Print()

        # loop over categories
        categories = options.categories
        if options.groups:
            categories = options.groups.keys()
        for cat in categories:
            if type(cat) == int: 
                catname = "cat%d" % cat
            else:
                catname = cat
            group = None
            if options.groups:
                group = options.groups[cat]
            ## print catname 
            # loop over plots
            for plot in plots:

                if len(plot) == 4:
                    plotname, plotmodifs, drawopts, legPos = plot
                    skip = []
                elif len(plot) == 5:
                    plotname, plotmodifs, drawopts, legPos, skip = plot
                if drawopts == None:
                    drawopts = options.drawopts
                if legPos == None:
                    legPos = options.legpos
                    
                drawmethod, dataopt, bkgopt, sigopt = drawopts
                doRatio = False
                if "DrawRatio" in drawmethod:
                    doRatio = True

                bkghists = []
                sighists = []
                datahists = []
                
                ## print bkg
                
                # read background MC
                if bkg != None and not "bkg" in skip:
                    bkgfile, bkgprocs = bkg
                    bkghists = [ self.readProcess(bkgfile,*subprocs,plot=plotname,plotmodifs=plotmodifs,category=catname,group=group,scale=options.lumi) 
                                 for subprocs in bkgprocs ]
                    
                # read signal MC
                if sig != None and not "sig" in skip:
                    sigfile, sigprocs = sig
                    sighists = [ self.readProcess(sigfile,*subprocs,plot=plotname,plotmodifs=plotmodifs,category=catname,group=group,scale=options.lumi) 
                                 for subprocs in sigprocs ]
                    
                # read data
                if data != None and not "data" in skip:
                    datafile, dataprocs = data
                    datahists = [ self.readProcess(datafile,*subprocs,plot=plotname,plotmodifs=plotmodifs,category=catname,group=group) for subprocs in dataprocs ]
                    
                # collect histograms
                allhists = datahists+list(reversed(bkghists))+sighists
                if len(allhists) == 0:
                    print "Nothing to plot for %s %s" % (plotname,catname)
                    continue
                self.keep(allhists)
                
                # make empty frame histogram for convenience
                frame = allhists[0].Clone("%s_%s_frame" % (plotname,catname) )
                frame.Reset("ICE")
                frame.SetEntries(0)
                self.keep(frame,True)
                ymax = 0.
                ymin = 0.
                
                # allocate canvas and legend and draw frame
                canv,leg = self.makeCanvAndLeg("%s_%s" % ( plotname, catname), legPos )
                pads=[canv]
                if doRatio:
                    ratio =  [ float(f) for f in drawmethod.split("DrawRatio")[1].split("[")[1].split("]")[0].split(",") ][0]
                    pads  = [ROOT.TPad("%s_main"%canv.GetName(),"%s_main"%canv.GetName(),0.,1.-ratio,1.,1.),
                             ROOT.TPad("%s_rat"%canv.GetName(),"%s_rat"%canv.GetName(),0.,0.,1.,1.-ratio)
                             ]
                    pads[0].SetBottomMargin(ratio*0.03)
                    pads[1].SetBottomMargin(pads[1].GetBottomMargin()+pads[1].GetTopMargin()+ratio*0.2)
                    pads[1].SetTopMargin(ratio*0.03)
                    pads[1].SetTickx()
                    for p in pads:
                        p.Draw()
                    pads[0].cd()
                    self.keep(pads,True)
                    pads[1].SetLogy(False)
                if canv.GetLogy():
                    ymin = getattr(canv,"ymin",1.e-4)
                if doRatio:
                    frame.SetLabelSize(0.)
                    frame.SetTitleSize(0.)
                frame.Draw()
            
                # draw background first
                if len(bkghists) > 0:
                    bkgstk = self.makeStack("bkg_%s_%s" % ( plotname, catname), bkghists)
                    ymax = max(ymax,self.drawStack(bkgstk,drawmethod,"%s SAME"%bkgopt))
                    
                # then signal
                if len(sighists) > 0:
                    sigstk = self.makeStack("sig_%s_%s" % ( plotname, catname),sighists)
                    ymax = max(ymax,self.drawStack(sigstk,drawmethod,"%s SAME"%sigopt))

                # then data
                if len(datahists) > 0:
                    datastk = self.makeStack("data_%s_%s" % ( plotname, catname),datahists)
                    ymax = max(ymax,self.drawStack(datastk,drawmethod,"%s SAME"%dataopt))
            
            
                # make legend
                for h in allhists:
                    legopt = "f"
                    if hasattr(h,"legopt"):
                        legopt = h.legopt
                    if legopt != "omit":
                        leg.AddEntry(h,"",legopt)
            
                # adjust yaxis
                if canv.GetLogy():
                    frame.GetYaxis().SetRangeUser(ymin,ymax*15)
                else:
                    frame.GetYaxis().SetRangeUser(ymin,ymax*1.2)
                leg.Draw("same")
                for pad in pads:
                    pad.RedrawAxis()
                    pad.Modified()
                    pad.Update()
                
                if doRatio:
                    pads[1].cd()
                    ratio = datastk.GetStack().At(datastk.GetStack().GetEntries()-1).Clone("%s_%s_ratio" % (plotname,catname))
                    den = bkgstk.GetStack().At(bkgstk.GetStack().GetEntries()-1)
                    ratio.Sumw2()
                    den.Sumw2()
                    ratio.Divide(ratio,den,1.,1.,"B")
                    self.keep(ratio,True)
                    
                    scaleFonts(ratio,pads[0].GetHNDC()/pads[1].GetHNDC())
                    
                    ratio.Draw("e")
                    pads[0].cd()
                    
                # if needed draw inset with zoom-in
                ##   DrawInset[rngmin,rngmax,posx1,posy1,posx2,posy2]
                if "DrawInset" in drawmethod:
                    inset =  [ float(f) for f in drawmethod.split("DrawInset")[1].split("[")[1].split("]")[0].split(",") ]
                    rng = inset[0:2]
                    pos = inset[2:]
                    
                    padname = "%s_%s_inset" % ( plotname, catname)
                    pad = ROOT.TPad(padname, padname, *pos)
                    self.keep(pad,True)
                    pad.Draw("")
                    pad.SetFillStyle(0)
                    
                    pad.cd()
                    padframe = frame.Clone()
                    padframe.GetXaxis().SetRangeUser(*rng)
                    padframe.GetYaxis().SetRangeUser(ymin,ymax*1.2)
                    padframe.Draw()
                    self.keep(padframe)
                    
                    if len(bkghists) > 0:
                        self.drawStack(bkgstk,"Draw",bkgopt+" same")
                    
                    if len(datahists) > 0:
                        self.drawStack(datastk,"Draw",dataopt+" same")
                        
                    if len(sighists) > 0:
                        self.drawStack(sigstk,"Draw",sigopt+" same")
            
                    pad.RedrawAxis()
                    self.format(pad,options.postproc)

                self.format(canv,options.postproc)
                # FIXME ratio plots
                
    #
    # Read histograms for a given process, applying manipulators
    #
    def readProcess(self,fin,name,title,style,subproc,plot,plotmodifs,category,group,scale=None):
        
        ## print "readProcess", fin,name,title,style,subproc,plot,plotmodifs,category
        names = subproc.keys()               
        histos = self.readObjects(fin,plot,samples=names,cat=category,group=group)
        for iplot in range(len(histos)):
            h = histos[iplot]
            hname = names[iplot]
            h = style_utils.apply(h,subproc[hname])
        
        hsum = histos[0].Clone("%s_%s_%s" %( plot, name, category) )
        hsum.SetTitle(title)
        
        for h in histos[1:]:
            hsum.Add(h)

        self.keep(hsum,True)
        hsum = style_utils.apply(hsum,plotmodifs)
        hsum = style_utils.apply(hsum,style)
        
        if scale:
            hsum.Scale(scale)
        
        return hsum

    #
    # Read plots from globe histogram files
    #
    def readObjects(self,fin, name, cat="cat0", samples=[],group=None):
    
        ret = []
        for s in samples:
            sfin = fin
            if ":" in s:
                sa = s
                s,fname = s.split(":")
                folder = None
                if ".root/" in fname:
                    fname, folder = fname.rsplit("/",1)
                ## print fname, folder
                sfin = self.open(fname, folder=self.options.input_dir)
                if folder:
                    try:
                        sfin = sfin.Get(folder)
                    except Exception, e:
                        print "Unable to read %s from %s" % ( folder, fname)
                        print e
                        sfin = None
                if not sfin:
                    print "Could not read %s, %s" % (sa, self.options.input_dir)
                    sys.exit(1)
            if group:
                h = None
                for gr in group:
                    nam = self.template_ % { "name" : name, "cat" : gr, "sample" : s }
                    # print nam
                    hgr = sfin.Get(nam)
                    if not hgr:
                        raise IOError("Could not read %s from %s" % (nam, str(sfin)) )
                    if hgr.IsA().InheritsFrom("TH1"):
                        hgr.Sumw2()
                        if not h:
                            h = hgr.Clone(self.template_ % { "name" : name, "cat" : cat, "sample" : s })
                            h.GetXaxis().SetTitle(  h.GetXaxis().GetTitle().replace("@"," ") )
                            h.Sumw2()
                            self.keep(h)
                            ret.append(h)
                        else:
                            h.Add(hgr)
                    elif hgr.IsA().InheritsFrom("TTree"):
                        ret.append(hgr)
            else:
                nam = self.template_ % { "name" : name, "cat" : cat, "sample" : s }
                h = sfin.Get(nam)
                if not hgr:
                    raise IOError("Could not read %s from %s" % (nam, str(sfin)) )
                if h.IsA().InheritsFrom("TH1"):
                    h.GetXaxis().SetTitle(  h.GetXaxis().GetTitle().replace("@"," ") )
                    h.Sumw2()
                ret.append(h)
            
        return ret

    #
    # Make THStack out of python list
    #
    def makeStack(self,name,histos):
        stk = ROOT.THStack()
        for h in histos:
            stk.Add(h)
        self.keep(stk)
        return stk


    #
    # Build envelope of variations
    #
    def makeEnvelope(self,name,histos):
        
        nominal = histos[0]
        errPlus  = nominal.Clone( "%s_ErrPlus" % name )
        errMinus = nominal.Clone( "%s_ErrMinus" % name )

        style = self.getStyle("envelope")
        if style:
            style_utils.apply( errPlus,  style )
            style_utils.apply( errMinus, style )
            
        for ibin in range(nominal.GetNbinsX()):
            hist = ROOT.TH1F("hist","hist",11,-5.,5.)
            hist.Reset("ICE")
            hist.Sumw2()
            points = []
            
            plus  = nominal.GetBinContent(ibin+1)
            minus = nominal.GetBinContent(ibin+1)
            nom   = nominal.GetBinContent(ibin+1)
            err   = nominal.GetBinError(ibin+1)
            
            points.append( [nom,err] )
            hist.Fill(nom)
            for h in histos[1:]:
                content =  h.GetBinContent(ibin+1)
                err     =  h.GetBinError(ibin+1)
                hist.Fill(content)
                points.append( [content,err] )
                if content < minus:
                    minus = content
                if content > plus:
                    plus = content
    
            if hist.GetRMS() == 0.:
                errPlus.SetBinContent(ibin+1,plus)
                errMinus.SetBinContent(ibin+1,minus)
                continue
                
            hist2 = ROOT.TH1F("hist2","hist2",11,hist.GetMean()-5.*hist.GetRMS(),hist.GetMean()+5.*hist.GetRMS())
            hist2.Sumw2()
            for p,e in points:
                hist2.Fill(p)
                
            func = ROOT.TF1("func","[0]*exp( -0.5*pow( (x-[1])/( (x>=0)*[2] + (x<=0)*[3] ) ,2.) )",hist2.GetMean()-5.*hist2.GetRMS(),hist2.GetMean()+5.*hist2.GetRMS())
            func.SetParameters(len(histos),hist2.GetMean(),hist2.GetRMS(), hist2.GetRMS())

            stat = int( hist2.Fit( func, "L" ) )
            if stat == 0:
                errPlus.SetBinContent(ibin+1, max(nom,func.GetParameter(1)+fabs(func.GetParameter(2))))
                errMinus.SetBinContent(ibin+1,min(nom,func.GetParameter(1)-fabs(func.GetParameter(3))))
            else:
                errPlus.SetBinContent(ibin+1,plus)
                errMinus.SetBinContent(ibin+1,minus)

        self.keep([errPlus,errMinus,nominal])
        return self.makeStack(name,[errPlus,errMinus,nominal])

    #
    # Prepare canvas and legend
    #
    def makeCanvAndLeg(self,name,legPos):
        canv = ROOT.TCanvas(name,name)
        leg  = ROOT.TLegend(*legPos)
        leg.SetName("%s_legend" % name)
        
        self.keep( [canv,leg], True )
        return canv,leg


    #
    # Draw a THStack
    #
    def drawStack(self,stk, method, option):
        ymax = 0.
        if "DrawNormalized" in method:
            rng = None
            if "[" in method:
                rng = [ float(f) for f in method.split("DrawNormalized")[1].split("[")[1].split("]")[0].split(",") ]
            histos = [ stk.GetStack().At(0) ]
            if "nostack" in option:
                histos = stk.GetHists()
                option = option.replace("nostack","")
            for h in histos:
                h.SetFillStyle(0)
                h.SetLineWidth(2)
                bmin = -1
                bmax = -1
                if rng:
                    bmin = h.FindBin(rng[0])
                    bmax = h.FindBin(rng[1])
                h.Scale(1./h.Integral(bmin,bmax))
                h.Draw("%s SAME" % option)
                ymax = max(ymax, h.GetMaximum()) 
        else:
            getattr(stk,method.split(",")[0])("%s" % option)
            ymax = stk.GetMaximum(option)
            
        return ymax

    # -----------------------------------------------------------------------------------------------------------
    def loadRootStyle(self):
        mypath = os.path.dirname(__file__)

        ROOT.gROOT.LoadMacro( "%s/hggPaperStyle.C" % mypath )
        ROOT.gROOT.LoadMacro( "%s/CMS_lumi.C" % mypath )
        
        ROOT.hggPaperStyle()
        ROOT.hggStyle.cd()
        
        ## self.dev_null = ROOT.std.ofstream("/dev/null")
        ## ROOT.std.cout.rdbuf(self.dev_null.rdbuf())
        ## ROOT.std.cerr.rdbuf(self.dev_null.rdbuf())

        for l in self.options.rootstyle_extra:
            ROOT.gROOT.ProcessLine(l)
            
        ci = 2756
        myNewColor = ROOT.TColor(ci, 1.0, 0., 0., "", 0.5)
        ci2 = 2974
        myNewColor2 = ROOT.TColor(ci2, .75, .75, .75, "", 0.5)
        ci3 = 3974
        myNewColor3 = ROOT.TColor(ci3, .0, 1., .0, "", 0.3)
        ci4 = 4974
        myNewColor4 = ROOT.TColor(ci4, .25, .5, .25, "", 0.5)
        ci5 = 5974
        myNewColor5 = ROOT.TColor(ci5, 0., 0., 1., "", 0.5)
        ci6 = 6974
        myNewColor5 = ROOT.TColor(ci5, 0., 0., 1., "", 0.5)
        
        self.keep( [myNewColor,myNewColor2,myNewColor3,myNewColor4,myNewColor5] )
        
        red    = array.array('d', [ 1.00, 48. /255. ] )
        green  = array.array('d', [ 1.00, 48. /255. ] )
        blue   = array.array('d', [ 1.00, 131./255. ] )
        length = array.array('d', [ 0.00, 1.00] )
        number = len(red)
        nb=2048
        ROOT.TColor.CreateGradientColorTable(number,length,red,green,blue,nb)
        
        ROOT.myColorA1   = ROOT.TColor.GetColor("#303083")
        ROOT.myColorA2   = ROOT.TColor.GetColor("#8a8ab9")
        ROOT.myColorA3   = ROOT.TColor.GetColor("#9e9ec5")
        ROOT.myColorA4   = ROOT.TColor.GetColor("#cecee2")
        ROOT.myColorA3tr = ROOT.gROOT.GetListOfColors().Last().GetNumber()+1;
        tmp = ROOT.gROOT.GetColor( ROOT.myColorA3 )
        self.keep( ROOT.TColor( ROOT.myColorA3tr, tmp.GetRed(), tmp.GetGreen(), tmp.GetBlue(), "", 0.5  ) )
        
        ROOT.myColorC1   = ROOT.TColor.GetColor("#308230")
        ROOT.myColorC2   = ROOT.TColor.GetColor("#8cbb8c")
        ROOT.myColorC3   = ROOT.TColor.GetColor("#9dc49d")
        ROOT.myColorC4   = ROOT.TColor.GetColor("#cfe3cf")
        ROOT.myColorC3tr = ROOT.gROOT.GetListOfColors().Last().GetNumber()+2;
        tmp = ROOT.gROOT.GetColor( ROOT.myColorC3 )
        self.keep( ROOT.TColor( ROOT.myColorC3tr, tmp.GetRed(), tmp.GetGreen(), tmp.GetBlue(), "", 0.5  ) )

        ROOT.myColorD1   = ROOT.TColor.GetColor("#7E822E")
        ROOT.myColorD2   = ROOT.TColor.GetColor("#BABB8C")
        ROOT.myColorD3   = ROOT.TColor.GetColor("#C3C49E")
        ROOT.myColorD4   = ROOT.TColor.GetColor("#E3E3D0")
        ROOT.myColorD3tr = ROOT.gROOT.GetListOfColors().Last().GetNumber()+2;
        tmp = ROOT.gROOT.GetColor( ROOT.myColorD3 )
        self.keep( ROOT.TColor( ROOT.myColorD3tr, tmp.GetRed(), tmp.GetGreen(), tmp.GetBlue(), "", 0.5  ) )
        
        ROOT.myColorB0   = ROOT.TColor.GetColor("#540000")
        ROOT.myColorB1   = ROOT.TColor.GetColor("#cc0000")
        ROOT.myColorB2   = ROOT.TColor.GetColor("#e65353")
        ROOT.myColorB3   = ROOT.TColor.GetColor("#f7baba")
        ROOT.myColorB4   = ROOT.TColor.GetColor("#f29191")
        ROOT.myColorB3tr = ROOT.gROOT.GetListOfColors().Last().GetNumber()+3;
        tmp = ROOT.gROOT.GetColor( ROOT.myColorB3 )
        self.keep( ROOT.TColor( ROOT.myColorB3tr, tmp.GetRed(), tmp.GetGreen(), tmp.GetBlue(), "", 0.5  ) )
        
        ROOT.gStyle.SetHatchesLineWidth(2)
        ## ROOT.gStyle.SetErrorX(1.e-6)
        
        ROOT.gStyle.SetOptTitle(0);
        ROOT.gStyle.SetOptStat(0);
        

if __name__ == "__main__":

    app = PlotApp()

    app.run()
