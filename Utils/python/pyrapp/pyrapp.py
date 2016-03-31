from optparse import OptionParser, OptionGroup, make_option
import sys
import json
import os
import subprocess, shlex

from optpars_utils import ScratchAppend, Load
from fnmatch import fnmatch

from pprint import pprint

# -------------------------------------------------------------------------------
def shell_expand(string):
    if string:
        return os.path.expanduser( os.path.expandvars(string) )
    return None

# -------------------------------------------------------------------------------
def shell_args(cmd):
    return [ shell_expand(a) for a in shlex.split(cmd) ]
                
# -----------------------------------------------------------------------------
class PyRApp(object):
    
    def __init__(self,option_list,option_groups=[],defaults=None):
        self.objs_ = []
        self.canvs_ = []

        self.files_ = {}

        opt_list=[
            make_option("-I","--interactive",
                        action="store_true", dest="interactive",
                        default=sys.flags.interactive,
                        help="default: %default", metavar=""
                        ),
            make_option("-b","--non-interactive",
                        action="store_false", dest="interactive",
                        help="default: %default", metavar=""
                        ),
            make_option("-S","--save",
                        action="store_true", dest="save",
                        default=False,
                        help="default: %default", metavar=""
                        ),
            make_option("--saveas",
                        action="callback", dest="saveas", type="string", callback=ScratchAppend(),
                        default=["convert_png","root"],
                        help="default: %default", metavar=""
                        ),
            make_option("--savebw",
                        action="store_true", dest="savebw", default=False,
                        help="default: %default",
                        ),
            make_option("--dumpcfg",
                        action="store_true", dest="dumpcfg", default=False,
                        help="default: %default",
                        ),
            make_option("-O","--outdir",
                        action="store", type="string", dest="outdir", default=None,
                        help="default: %default",
                        ),
            make_option("--load",
                        action="callback", dest="__opts__", type="string", callback=Load(), metavar="JSON",
                        help="default: %default"
                        ),
            make_option("--load-scratch",
                        action="callback", dest="__opts__", type="string", callback=Load(scratch=True), metavar="JSON",
                        help="default: %default"
                        ),
            make_option("--styles",
                        action="callback", dest="styles", type="string", callback=Load(), metavar="JSON",
                        default={},
                        help="default: %default"
                        ),
            make_option("-v","--verbose",
                        action="store_true", dest="verbose",
                        default=False,
                        help="default: %default"
                        )
            ] ##  + option_list
        
        ## parser = OptionParser(option_list=opt_list)
        parser = OptionParser()

        opt_groups = [ ("PyRApp Options", opt_list ) ] + option_groups
        if len(option_list) > 0:
            opt_groups.append( ("User Options", option_list) )
        
        for name,opts in opt_groups:
            gr = OptionGroup(parser,name)
            for opt in opts:
                gr.add_option(opt)
            parser.add_option_group(gr)
            
        (self.options, self.args) = parser.parse_args()

        if self.options.dumpcfg:
            self.options.dumpcfg = False
            print ( json.dumps( self.options.__dict__,indent=4,sort_keys=True) )  
            sys.exit(0)
            
        if self.options.verbose:
            print ( json.dumps( self.options.__dict__,indent=4,sort_keys=True) )
            
        if not self.options.interactive:
            sys.argv.append("-b")
            if self.options.outdir:
                self.options.save = True
        if not self.options.outdir:
            self.options.outdir = os.getcwd()
        else:
            try:
                os.mkdir(self.options.outdir)
            except:
                pass
            
        
        global ROOT, style_utils
        import ROOT
        import style_utils
        
    def run(self):
        self.__call__(self.options,self.args)
        if self.options.save:
            self.save()

    def autosave(self,clear=False):
        if self.options.save:
            self.save(clear)
        
    def save(self,clear=False):
        for c in self.canvs_:
            if not c: continue
            ## print c
            c.Modified()
            for fmt in self.options.saveas:
                ## print fmt
                if fmt == "convert_png":
                    c.SaveAs("%s/tmp_%s.eps" % ( self.options.outdir, c.GetName() ) )
                    cmd = "convert -format png %s/tmp_%s.eps %s/%s.png" % ( self.options.outdir, c.GetName(), self.options.outdir, c.GetName() )
                    ## print cmd
                    subprocess.Popen( shell_args(cmd) )
                    ## subprocess.Popen( shell_args("rm %s/tmp_%s.eps" % ( self.options.outdir, c.GetName() ))  )
                else:
                    c.SaveAs("%s/%s.%s" % ( self.options.outdir, c.GetName(), fmt ) )            
            if self.options.savebw:
                ## print "gray scale"
                c.SetGrayscale(True)
                for fmt in self.options.saveas:
                    if not fmt in ["C","root"]:
                        c.SaveAs("%s/%s_bw.%s" % ( self.options.outdir, c.GetName(), fmt ) )
                c.SetGrayscale(False)
            ## print "ok"
        if clear:
            for c in self.canvs_:
                del c
            self.canvs_ = []

    def keep(self,objs,format=False):
        if type(objs) == list:
            for obj in objs:
                self.keep(obj,format)
            return
        
        try:
            if objs.IsA().InheritsFrom("TCanvas"):
                self.canvs_.append(objs)
            else:
                self.objs_.append(objs)
        except:
            self.objs_.append(objs)
        try:
            if objs.IsA().InheritsFrom("TFile"):
                key = "%s::%s" % (os.path.abspath(objs.GetName()), self.normalizeTFileOptions(objs.GetOption()))
                self.files_[key] = objs
        except:
            pass
        if format:
            self.format(objs,self.options.styles)
            ### for key,st in self.options.styles.iteritems():
            ###     ## print objs.GetName(),key
            ###     if fnmatch(objs.GetName(),key) or objs.GetName() == key:
            ###         style_utils.apply(objs,st)
            ### 
            ### for key,st in self.options.styles.iteritems():
            ###     if "!%s" % objs.GetName() == key:
            ###         style_utils.apply(objs,st)

    def format(self,objs,styles):
        for key,st in styles.iteritems():
            if fnmatch(objs.GetName(),key) or objs.GetName() == key:
                style_utils.apply(objs,st)

        for key,st in styles.iteritems():
            if "!%s" % objs.GetName() == key:
                style_utils.apply(objs,st)
        
        
    def log(self,what,level=-1):
        if level < self.options.verbose:
            print( what )
            
    def getStyle(self,key):
        if key in self.options.styles:
            return self.options.styles[key]
        return None

    def setStyle(self,key,style,replace=False):
        if key in self.options.styles and not replace:
            self.options.styles[key].extend( style )
        else:
            self.options.styles[key] = style

    def normalizeTFileOptions(self,option):
        ## FIXME reordering etc
        return str(option).lower()
    
    def open(self,name,option="",folder=None):
        tdir = None
        fname = name
        if folder and not fname.startswith("/"):
            if not os.path.exists(folder):
                try:
                    os.mkdir(folder)
                except:
                    pass
            fname = "%s/%s" % ( folder, name )
        if ".root/" in name:
            fname, tdir = name.split(".root/")
            fname += ".root"
        print fname
        if not fname.endswith(".root"):
            return open(fname,option)
        option = self.normalizeTFileOptions(option)
        key = "%s::%s" % (os.path.abspath(fname), option)
        if not key in self.files_:
            self.files_[key] = ROOT.TFile.Open(os.path.abspath(fname),option)
        if tdir:
            return self.files_[key].Get(tdir)
        return self.files_[key]
        
# -----------------------------------------------------------------------------------------------------------
class Test(PyRApp):

    def __init__(self):
        super(Test,self).__init__(option_list=[
            make_option("-t","--test",
                        action="store", dest="test", type="int",
                        default=None,
                        help="default: %default", metavar=""
                        ),
            make_option("-l","--loadmap",
                        action="callback", dest="loadmap", type="string", callback=Load(),
                        default={}
                        ),
            ])
        

    def __call__(self,options,args):

        print self.__dict__
        print ROOT

        options.styles["h"]=[ ("SetLineColor","kRed"), ("SetFillColor","kOrange") ]
        
        hist = ROOT.TH1F("h","h",101,-5.05,5.05)

        hist.FillRandom("gaus",1000)
        hist.Fit("gaus")

        canv = ROOT.TCanvas("canv","canv")
        self.keep([hist,canv],format=True)
        
        canv.cd()
        
        style_utils.apply( hist.GetListOfFunctions().At(0), [("SetLineWidth",2),("SetLineColor","kBlack")] )
        hist.Draw()

        
# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    test = Test()
    test.run()
    
    
