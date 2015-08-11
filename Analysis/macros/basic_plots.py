#!/bin/env python

from diphotons.Utils.pyrapp import *
from optparse import OptionParser, make_option
from copy import deepcopy as copy
import os

# -----------------------------------------------------------------------------------------------------------
class BasicPlot(PlotApp):

    def __init__(self):
        super(BasicPlot,self).__init__(option_list=[
                make_option("--selections",action="callback", dest="selections", type="string", callback=optpars_utils.Load(),
                            default=[])
                
            ])
        

    def replaceIn(self, template, replacements):
        ret = []
        print template
        for name, title, style, subprocs in template:
            rsubprocs = {}
            for key,val in subprocs.iteritems():
                rsubprocs[ key % replacements ] = val
            ret.append( (name % replacements, title % replacements, style, rsubprocs ) )
        return ret
                        
    def __call__(self,options,args):
        
        data_template = copy(options.data)
        bkg_template = copy(options.bkg)
        sig_template = copy(options.sig)
        plots_template = copy(options.plots)
        outdir = copy(options.outdir)
        ## for sel,out in options.selections:
        for selection in options.selections:
            if len(selection) == 2:
                sel,out = selection
                plotsel = None
            elif len(selection) == 3:
                sel,out,plotsel = selection
                
            replacements = { "sel" : sel }
            if data_template:
                options.data = self.replaceIn(data_template,replacements)
            if bkg_template:
                options.bkg = self.replaceIn(bkg_template,replacements)
            if sig_template:
                options.sig = self.replaceIn(sig_template,replacements)
            output = os.path.join(outdir,out % replacements)
            
            if plotsel:
                theplots = [ plot for plot in plots_template if plot[0] in plotsel ]
            else:
                theplots = plots_template

            options.outdir = output
            options.plots = theplots
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
    app = BasicPlot()
    app.run()
