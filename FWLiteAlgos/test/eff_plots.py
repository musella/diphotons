#!/bin/env python

from pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os

# -----------------------------------------------------------------------------------------------------------
class EffPlot(PlotApp):

    def __init__(self):
        super(EffPlot,self).__init__(option_list=[
                make_option("--ratios",action="callback", dest="ratios", type="string", callback=optpars_utils.Load(),
                            default=[])
                
            ])
        

    def replaceIn(self, template, replacements):
        ret = []
        for name, title, style, subprocs in template:
            rsubprocs = {}
            for key,val in subprocs.iteritems():
                rsubprocs[ key % replacements ] = val
            ret.append( (name % replacements, title % replacements, style, rsubprocs ) )
        return ret
                        
    def __call__(self,options,args):
        
        data_template = copy(options.data)
        bkg_template = copy(options.bkg)
        outdir = copy(options.outdir)
        for den,dentit,num,numtit,out in options.ratios:
            replacements = { "num" : num, "numtit" : numtit, "den" : den, "dentit" : dentit }
            data = self.replaceIn(data_template,replacements)
            bkg = self.replaceIn(bkg_template,replacements)
            output = os.path.join(outdir,out % replacements)
            
            options.outdir = output
            options.data = data
            options.bkg  = bkg
            try:
                os.mkdir(options.outdir)
            except:
                pass
            
            self.data_ = None
            self.sig_  = None
            self.bkg_  = None

            PlotApp.__call__(self,options,args)
            self.autosave(True)

# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app = EffPlot()
    app.run()
