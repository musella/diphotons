#!/bin/env python

import ROOT

import sys

iname = sys.argv[1]
oname = iname.replace(".root","_split.root")

fin = ROOT.TFile(iname)
testTree = fin.Get("TestTree")

fout = ROOT.TFile(oname,"recreate")
print "promptTree"
promptTree = testTree.CopyTree("classID == 0")

print "fakesTree"
fakesTree = testTree.CopyTree("classID == 1")

promptTree.Write("promptTree")
fakesTree.Write("fakesTree")

fout.Close()

