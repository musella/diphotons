#! /usr/bin/env python

import os
import sys
import re
import time
import commands
import optparse
import datetime

def main():
#######################################
### usage
#######################################
    usage = '''usage: %prog [opts] --cfg cmssw.py dataset doPUreweighting(0/1) sampleIndex PUweightsFile x-section kFactor '''

    parser = optparse.OptionParser(usage=usage)
    now = datetime.datetime.now()
    defaultoutputdir='job_'+str(now.year)+str(now.month)+str(now.day)+"_"+str(now.hour)+str(now.minute)+str(now.second)

    parser.add_option('-q', '--queue', action='store', dest='queue', help='run in batch in queue specified as option (default -q 8nh)', default='8nh')
    parser.add_option('-n', '--nfileperjob', action='store', dest='nfileperjob', help='split the jobs with n files read/batch job' , default=1, type='int')
    parser.add_option('-p', '--prefix', action='store', dest='prefix', help='the prefix to be added to the output' , default=defaultoutputdir)
    parser.add_option('-a', '--application', action='store', dest='application', help='the executable to be run' , default='cmsRun')
    parser.add_option('-d', '--download', action='store', dest='download', help='download the output on a local computer' , default='pccmsrm')
    parser.add_option('-c', '--create', action='store_true',dest='create', help='create only the jobs, do not submit them' , default=False)
    parser.add_option('-t', '--testnjobs', action='store', dest='testnjobs', help='submit only the first n jobs' , default=1000000, type='int')
    parser.add_option('-N', '--neventsjob', action='store', dest='neventsjob', help='split the jobs with n events / batch job' , default=-1, type='int')
    parser.add_option('-T', '--eventsperfile', action='store', dest='eventsperfile', help='number of events per input file' , default=-1, type='int')
    parser.add_option('--eos', action='store', dest='eos', help='copy the output in the specified EOS path' , default='')
    parser.add_option('--cfg', action='store', dest='cfg', help='the cfg to be run' , default='pippo_cfg.py')
    (opt, args) = parser.parse_args()

    if len(args) != 6:
        print usage
        sys.exit(1)

    dataset = args[0]      
    doPUreweighting = args[1]
    sampleIndex = args[2]
    PUweights = args[3]
    xsection = args[4]
    kfactor = args[5]

    inputlist="lists_Spring15v1/50ns/MC/"+dataset+".list"
    inputweights="lists_Spring15v1/50ns/MC/"+dataset+".weight"
    #inputlist="lists_Spring15v1/50ns/data/"+dataset+".list"
    #inputweights="lists_Spring15v1/50ns/data/"+dataset+".weight"

    output = dataset

    print "the outputs will be in the directory: "+opt.prefix

    if opt.download=='pccmsrm':
        diskoutputdir = "/cmsrm/pc29_2/crovelli/data/Exo/" 
    else: diskoutputdir = ''
    diskoutputmain = diskoutputdir+"/"+opt.prefix+"/"+output

    os.system("mkdir -p "+opt.prefix+"/"+output)
    os.system("mkdir -p "+opt.prefix+"/"+output+"/log/")
    os.system("mkdir -p "+opt.prefix+"/"+output+"/src/")
    os.system("mkdir -p "+opt.prefix+"/"+output+"/cfg/")
    outputroot = diskoutputmain+"/root/"

    if (diskoutputdir != "none" and opt.download=='pccmsrm'):
        os.system("ssh -o BatchMode=yes -o StrictHostKeyChecking=no pccmsrm29 mkdir -p "+diskoutputmain)   


    #look for the current directory
    #######################################
    pwd = os.environ['PWD']
    scramarch = os.environ['SCRAM_ARCH']
    #######################################
    inputListfile=open(inputlist)
    inputfiles = inputListfile.readlines()
    inputWeightsfile=open(inputweights)
    inputweights = inputWeightsfile.readlines()
    ijob=0

    while (len(inputfiles) > 0):
        L = []
        for line in range(min(opt.nfileperjob,len(inputfiles))):
            ntpfile = inputfiles.pop()
            ntpfile = ntpfile.rstrip('\n')
            ntpfile = re.sub(r'/eos/cms','',ntpfile.rstrip())     
            if ntpfile != '':
                L.append("\'"+ntpfile+"\',\n")

        LW = []
        for line in range(min(opt.nfileperjob,len(inputweights))):
            ntpfile = inputweights.pop()
            ntpfile = ntpfile.rstrip('\n')
            if ntpfile != '':
                LW.append("\'"+ntpfile+"\',\n")

        firstEvent = 1
        while (firstEvent < opt.eventsperfile or opt.eventsperfile == -1):
            lastEvent = firstEvent+opt.neventsjob

            # prepare the cfg
            icfgfilename = pwd+"/"+opt.prefix+"/"+output+"/cfg/cmssw"+str(ijob)+"_cfg.py"
            icfgfile = open(icfgfilename,'w')
            icfgfile.write('import sys\n')
            cfgfile=open(opt.cfg,'r')
            stringtoreplace = ''.join(L)
            stringtoreplace = stringtoreplace[:-2] # remove the "," and the end of line for the last input
            stringtoreplace = 'fileNames = cms.untracked.vstring('+stringtoreplace+')\n#'
            stringtoreplaceW = ''.join(LW)
            stringtoreplaceW = stringtoreplaceW[:-3]
            stringtoreplaceW = stringtoreplaceW[1:]
            print stringtoreplaceW
            if (opt.eventsperfile == -1): maxEventsString = 'process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )#'
            else: maxEventsString = 'process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32('+str(opt.neventsjob)+') )#'
            puRewString = 'dopureweight = cms.untracked.int32('+str(doPUreweighting)+')'
            sampleIndexString = 'sampleIndex  = cms.untracked.int32('+str(sampleIndex)+')'
            puWfileString = 'puWFileName  = cms.string("'+PUweights+'")'
            xsecString = 'xsec = cms.untracked.double('+str(xsection)+')'
            kfacString = 'kfac = cms.untracked.double('+str(kfactor)+')'
            sdsString = 'sumDataset = cms.untracked.double('+stringtoreplaceW+')'
            outputFileName = output+'_'+str(ijob)+'.root'
            for line in cfgfile:
                line = re.sub(r'fileNames = cms.untracked.vstring',stringtoreplace, line.rstrip())
                line = re.sub(r'fileName = cms.untracked.string','fileName = cms.untracked.string(sys.argv[2]),#', line.rstrip())
                line = re.sub(r'process.maxEvents = cms.untracked.PSet', maxEventsString, line.rstrip())
                line = re.sub(r'dopureweight = PU',puRewString,line.rstrip())
                line = re.sub(r'sampleIndex  = SI',sampleIndexString,line.rstrip())
                line = re.sub(r'puWFileName  = weights',puWfileString,line.rstrip())
                line = re.sub(r'xsec         = XS',xsecString,line.rstrip())
                line = re.sub(r'kfac         = KF',kfacString,line.rstrip())
                line = re.sub(r'sumDataset   = SDS',sdsString,line.rstrip())
                line = re.sub(r'OUTPUT',outputFileName,line.rstrip())
                icfgfile.write(line+'\n')

            if (opt.eventsperfile > -1): icfgfile.write('process.source.skipEvents=cms.untracked.uint32('+str(firstEvent-1)+')\n')

            # prepare the script to run
            outputname = opt.prefix+"/"+output+"/src/submit_"+str(ijob)+".src"
            outputfile = open(outputname,'w')
            outputfile.write('#!/bin/bash\n')
            outputfile.write('export SCRAM_ARCH='+scramarch+'\n')
            outputfile.write('cd '+pwd+'\n')
            outputfile.write('eval `scramv1 runtime -sh`\n')
            outputfile.write('cd $WORKDIR\n')
            outputfile.write(opt.application+' '+icfgfilename+' \n')
            if(opt.download=='pccmsrm'): outputfile.write('ls *.root | xargs -i scp -o BatchMode=yes -o StrictHostKeyChecking=no {} pccmsrm29:'+diskoutputmain+'/{}\n')
            if(opt.eos!=''): outputfile.write('ls *.root | grep -v histProbFunction | xargs -i xrdcp {} root://eoscms/'+opt.eos+'/\n')
            outputfile.close
            logfile = opt.prefix+"/"+output+"/log/"+output+"_"+str(ijob)+".log"
            os.system("echo bsub -q "+opt.queue+" -o "+logfile+" source "+pwd+"/"+outputname)
            if(opt.create==False):
                os.system("bsub -q "+opt.queue+" -o "+logfile+" source "+pwd+"/"+outputname)
            ijob = ijob+1
            if(ijob==opt.testnjobs): break
            if (opt.eventsperfile == -1): break
            else: firstEvent = lastEvent

if __name__ == "__main__":
    main()
