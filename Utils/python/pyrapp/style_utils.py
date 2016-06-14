import ROOT
import array

# -----------------------------------------------------------------------------------------------------------
def applyTo(obj,method,style):
    res = getattr(obj,method)()
    apply(res,style)

# -----------------------------------------------------------------------------------------------------------
def cloneRename(h,search,replace):
    newname = h.GetName().replace(search,replace)
    return h.Clone(newname)

# -----------------------------------------------------------------------------------------------------------
def xtitle(h,tit):
    h.GetXaxis().SetTitle(tit)

# -----------------------------------------------------------------------------------------------------------
def ytitle(h,tit):
    ## print "ytitle", h.GetName(), tit
    binw = h.GetBinWidth(1) if not getattr(h,"isDensity",False) else h.isDensity
    h.GetYaxis().SetTitle( tit % { "binw" : binw } )
    # h.GetYaxis().SetTitle(tit)

# -----------------------------------------------------------------------------------------------------------
def ztitle(h,tit):
    h.GetZaxis().SetTitle(tit)

# -----------------------------------------------------------------------------------------------------------
def yrezoom(h,scale):
    last=h.GetMaximum()
    first=h.GetMinimum()
    last=first+(last-first)*scale 
    h.GetYaxis().SetRangeUser(first,last)

# -----------------------------------------------------------------------------------------------------------
def logy(h,ymin=None):
    if h.IsA().InheritsFrom(ROOT.TPad.Class()):
        h.SetLogy()
        if ymin:
            h.ymin = ymin

# -----------------------------------------------------------------------------------------------------------
def overflow(h,limit=None):
    if not limit:
        limitBin=h.GetXaxis().GetLast()
    else:
        limitBin = h.GetXaxis().FindBin(limit)
    maxBin = h.GetNbinsX()
    ## print "overflow %s %s %1.2f %1.2f" % (h.GetName(), str(limit), h.GetXaxis().GetBinLowEdge(limitBin), h.GetXaxis().GetBinLowEdge(maxBin))
    ## h.Print("all")
    sumw = 0.
    sumw2 = 0.
    for ibin in range(limitBin,maxBin+1):
        sumw += h.GetBinContent(ibin)
        sumw2 += h.GetBinError(ibin)**2
        h.SetBinContent(ibin,0.)
        h.SetBinError(ibin,0.)
    h.SetBinContent(limitBin,sumw)
    h.SetBinError(limitBin,sumw2)

# -----------------------------------------------------------------------------------------------------------
def rebin(h,rebin):
    if type(rebin) == list:
        bins = array.array('d',rebin)
        return h.Rebin(len(bins)-1,h.GetName(),bins)
    else:
        h.Rebin(rebin)

# -----------------------------------------------------------------------------------------------------------
def density(h,ref=None):
    if ref == None:
        ref = h.GetBinWidth(1)
    for ibin in range(h.GetNbinsX()):
        width = h.GetXaxis().GetBinWidth(ibin+1)
        h.SetBinContent(ibin+1, h.GetBinContent(ibin+1) / width * ref)
        h.SetBinError(ibin+1, h.GetBinError(ibin+1) / width * ref)
        h.isDensity = ref
        ## h.isDensity = 1.

# -----------------------------------------------------------------------------------------------------------
def scaleFonts(h,scale):
    ## print "scaleFonts", scale
    for ax in h.GetXaxis(),h.GetYaxis(),h.GetZaxis():
        ax.SetTitleSize(ax.GetTitleSize()*scale)
        ax.SetLabelSize(ax.GetLabelSize()*scale)

    for ax in h.GetYaxis(),:
        ax.SetTitleOffset(ax.GetTitleOffset()/scale)
        

# -----------------------------------------------------------------------------------------------------------
def colors(h,color):
    h.SetMarkerColor(color)
    h.SetLineColor(color)
    h.SetFillColor(color)

# -----------------------------------------------------------------------------------------------------------
def legopt(h,opt):
    h.legopt = opt

# -----------------------------------------------------------------------------------------------------------
def xrange(h,xmin,xmax):
    h.GetXaxis().SetRangeUser(xmin,xmax)
    
# -----------------------------------------------------------------------------------------------------------
def yrange(h,xmin,xmax):
    h.GetYaxis().SetRangeUser(xmin,xmax)

# -----------------------------------------------------------------------------------------------------------
def zrange(h,xmin,xmax):
    h.GetZaxis().SetRangeUser(xmin,xmax)

# -----------------------------------------------------------------------------------------------------------
def mvStatBox(h,prev=None,vert=-1,horiz=0.):
    ROOT.gPad.Update()
    st = h.FindObject('stats')
    st.SetLineColor(h.GetLineColor())
    st.SetTextColor(h.GetLineColor())

    if prev:
        shiftx = (prev.GetX2NDC() - st.GetX1NDC())*horiz
        shifty = (prev.GetY2NDC() - st.GetY1NDC())*vert

        st.SetX1NDC(st.GetX1NDC()+shiftx)
        st.SetX2NDC(st.GetX2NDC()+shiftx)

        st.SetY1NDC(st.GetY1NDC()+shifty)
        st.SetY2NDC(st.GetY2NDC()+shifty)

    ROOT.gPad.Update()
    return st

# -----------------------------------------------------------------------------------------------------------
def addCmsLumi(canv,period,pos,extraText=None):
    if extraText:
        ROOT.gROOT.ProcessLine("writeExtraText = true;")
        if type(extraText) == str or type(extraText) == unicode and extraText != "":
            ROOT.gROOT.ProcessLine('extraText = "%s";' % extraText)
    ROOT.CMS_lumi(canv,period,pos)


def addCatLabel(canv,cat=None,x1=0.19,y1=0.81,x2=0.26,y2=0.91):
    if not cat:
        cat = canv.GetName().rsplit("_",1)[-1]
    pt=ROOT.TPaveText(x1,y1,x2,y2,"nbNDC")    
    pt.SetFillStyle(0)
    pt.SetLineColor(ROOT.kWhite)
    pt.AddText(cat)
    canv.cd()
    pt.Draw("same")
    setattr(canv,"objs",getattr(canv,"objs",[]))
    canv.objs.append(pt)
    return canv


# -----------------------------------------------------------------------------------------------------------
def printIntegral(h,xmin=None,xmax=None):
    try:
        bmin=-1
        bmax=-1
        if xmin:
            bmin = h.FindBin(xmin)
        if xmax:
            bmax = h.FindBin(xmax)
                
        print("Integral %s(%s,%s): %2.4g" % (h.GetName(), str(xmin), str(xmax), h.Integral(bmin,bmax) ))
    except:
        pass

# -----------------------------------------------------------------------------------------------------------
def printMean(h,xmin=None,xmax=None):
    try:
        if xmin and xmax:
            first,last=h.GetXaxis().GetFirst(),h.GetXaxis().GetLast()
            h.GetXaxis().SetRangeUser(xmin,xmax)
        print("Mean %s(%s,%s): %2.4g" % (h.GetName(), str(xmin), str(xmax), h.GetMean() ))
        if xmin and xmax:
            h.GetXaxis().SetRange(first,second)
    except:
        pass
        
# -----------------------------------------------------------------------------------------------------------
def apply(h,modifs):
    for method in modifs:
        raw = method
        
        args = None
        ret = None
        if type(method) == tuple or type(method) == list:
            method, args = method
        if type(method) == unicode:
            method = str(method)

        if type(method) == str:
            if hasattr(h,method):
                method = getattr(h,method)
            else:
                method = globals()[method]

        exceptions = []
        try:
            if args == None:
                try:
                    ret = method(h)
                except Exception as e:
                    exceptions.append(e)
                    ret = method()
            else:
                if not ( type(args) == tuple or type(args) == list ):
                    args = [args]
                for i,a in enumerate(args):
                    if type(a) == unicode or type(a) == str:
                        if a.startswith("k") or a.startswith("my") and hasattr(ROOT,a):
                            try:
                                args[i] = getattr(ROOT,a)
                            except Exception as e:
                                print e
                try:
                    ret = method(h,*args)
                except Exception as e:
                    exceptions.append(e)
                    ret = method(*args)
                if type(ret) == type(h):
                    h = ret
        except Exception as e:
            exceptions.append(e)
            exc = "\n".join( str(e) for e in exceptions)
            raise Exception("Failed to apply %s to %s. Got following exceptions:\n%s" % ( str(raw), str(h), exc ))
        
    return h
