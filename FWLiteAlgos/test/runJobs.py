#!/usr/bin/env python
import os
from subprocess import call, Popen, PIPE
from copy import copy

# Load command line option utilities
from flashgg.MetaData.optpars_utils import *
from flashgg.MetaData.parallel import *

# -------------------------------------------------------------------------------
def shell_expand(string):
    if string:
        return os.path.expanduser( os.path.expandvars(string) )
    return None


# -------------------------------------------------------------------------------
def dumpCfg(options):
    cfg = {}
    for key,val in options.__dict__.iteritems():
        if key != "__opt__" and key != "dumpCfg":
            cfg[key] = val

    return json.dumps( cfg,indent=4)

def wait(parallel):
    for i in range(parallel.njobs):
        print "Finished jobs: %d. Total jobs: %d" % (i, parallel.njobs)
        job, jobargs, ret = parallel.returned.get()
        print "finished: %s %s" % ( job, " ".join(jobargs) )
        for line in ret[1].split("\n"):
            print line
    

# Command line options
parser = OptionParser(option_list=[
        make_option("--processes", action="callback", callback=Load(), type="string", dest="processes",
                    default={}, help="List of datasets to be analyzed"),
        make_option("--load",  # special option to load whole configuaration from JSON
                    action="callback",callback=Load(),dest="__opts__",
                    type="string",
                    help="load JSON file with configuration",metavar="CONFIG.json"
                    ),
        make_option("-n","--njobs",dest="njobs",type="int",default=0,
                    help="number of jobs to run"),
        make_option("-q","--queue",dest="queue",type="string",default=None,
                    help="LSF queue to use. default: %default"),
        make_option("-o","--output",dest="output",type="string",
                    default="output.root", help="output file name. default: %default"),
        make_option("-d","--outputDir",dest="outputDir",type="string",
                    default=None, help="output folder. default: %default"),
        make_option("--dumpCfg",
                    action="store_true",
                    default=False,
                    help="dump configuaration and exit. default: %default"),
        make_option("-v","--verbose",
                    action="store_true", dest="verbose",
                    default=False,
                    help="default: %default"),
        make_option("-N","--ncpu",dest="ncpu", type="int",default=cpu_count()),
        make_option("-H","--hadd",dest="hadd",default=False, action="store_true",
                    help="hadd output files when all jobs are finished."
                    ),
        make_option("-D","--hadd-dateset",dest="hadd_dataset",default=False, action="store_true",
                    help="hadd output per dataset when all jobs are finished."
                    ),
        make_option("-P","--hadd-process",dest="hadd_process",default=False, action="store_true",
                    help="hadd output per process when all jobs are finished."
                    ),
        make_option("--dry-run",dest="dry_run",default=False, action="store_true",
                    help="do not actually run the jobs."
                    ),
        ]
                      )

# parse the command line
(options, args) = parser.parse_args()

outputPfx = options.output.replace(".root","")
jobName = "runJobs"

if options.outputDir:
    if not os.path.exists(options.outputDir):
        os.mkdir(options.outputDir)
    outputPfx = "%s/%s" % ( options.outputDir, outputPfx )
    jobName   = "%s/%s" % ( options.outputDir, jobName )

if options.dumpCfg:
    print ( dumpCfg(cfg) )
    exit(0)

parallel = Parallel(options.ncpu,lsfQueue=options.queue,lsfJobName=jobName)

outfiles = []
doutfiles = {}
poutfiles = {}

for name,datasets in options.processes.iteritems():
    poutfiles[name] = ( "%s_%s.root" % ( outputPfx,name), [] )

    for dset in datasets:
        job = args[0]
        jobargs = copy(args[1:])
        outfile = "%s_%s.root" % ( outputPfx, dset.lstrip("/").replace("/","_") )
        doutfiles[dset] = ( str(outfile),[] )
        jobargs.extend( ["dataset=%s" % dset, "outputFile=%s" % outfile ] )
        print "running: %s %s" % ( job, " ".join(jobargs) )
        if options.njobs != 0:
            print  "splitting in %d jobs" % options.njobs
            for ijob in range(options.njobs):
                iargs = jobargs+["nJobs=%d jobId=%d" % (options.njobs, ijob)]
                ret = parallel.run("python %s" % job,iargs+["dryRun=1"],interactive=True)[2][0]
                if ret != 0:
                    continue
                if not options.dry_run:
                    parallel.run(job,iargs)
                outfiles.append( outfile.replace(".root","_%d.root" % ijob) )
                doutfiles[dset][1].append( outfiles[-1] )
                poutfiles[name][1].append( outfiles[-1] )
        else:
            if not options.dry_run:
                parallel.run(job,jobargs)
            outfiles.append( outfile )
            poutfiles[name][1].append( outfiles[-1] )
        
if not options.dry_run:
    wait(parallel)        

if options.hadd:
    print "All jobs finished. Merging output."
    p = Parallel(options.ncpu)
    hadd = "hadd -f "
    if options.hadd_process:
        for proc,out in poutfiles.iteritems():
            outfile,outfiles = out
            p.run("%s %s" % (hadd, outfile), outfiles )
    if options.hadd_dataset:
        if options.hadd_process:
            hadd += " -T"
        for dset,out in doutfiles.iteritems():
            outfile,outfiles = out
            p.run("%s %s" % (hadd,outfile), outfiles) 
    if not (options.hadd_process or options.hadd_dataset):
        p.run("%s %s.root" % (hadd,outputPfx), outfiles)
    
    wait(p)

