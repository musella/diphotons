#!/usr/bin/env python 

import ROOT
import sys

from math import sqrt

fin = ROOT.TFile.Open(sys.argv[1])
fin.ls()

## for comp in "mctruth_pp", "mctruth_pf", "mctruth_ff", "data":
## for comp in "mc", "data":
for comp in ["data"]:
	if comp != "data":
		scl = 2.4
	else:
		scl =1.
	for cat,cut in ("EBEB",230), ("EBEE",320):
	## for cat,cut in ("EBEB0T",230), ("EBEE0T",320):
		tree = fin.Get("tree_%s_cic2_%s" % (comp,cat) )
		## tree = fin.Get("tree_%s_cic0T_%s" % (comp,cat) )
		sums_250 = {}
		sums_500 = {}
		for iev in range(tree.GetEntries()):
			tree.GetEntry(iev)
			mass = tree.mass
			if mass <= cut: continue
			wei = tree.weight*scl
			if not wei in sums_250:
				sums_250[wei] = 1.
			else:
				sums_250[wei] += 1.
			if mass>500.:
				if not wei in sums_500:
					sums_500[wei] = 1.
				else:
					sums_500[wei] += 1.
		sumw_250  = 0.
		sumw2_250 = 0.
		for wei,num in sums_250.iteritems():
			sumw_250  += num*wei
			sumw2_250 += num*wei*wei

		sumw_500  = 0.
		sumw2_500 = 0.
		for wei,num in sums_500.iteritems():
			sumw_500  += num*wei
			sumw2_500 += num*wei*wei

			
		print comp, cat, ("%1.4g" % sumw_250), "+-", ("%1.3g" % sqrt(sumw2_250))
		print comp, cat, ("%1.4g" % sumw_500), "+-", ("%1.3g" % sqrt(sumw2_500))
			
