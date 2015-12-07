#! /bin/sh    

root -l -b <<EOF
.L checksFormatting.cc++

checksFormatting("mcCommon/output.root",1,0,0,"EBHighR9","kinGenIso","001","750")
checksFormatting("mcCommon/output.root",1,0,0,"EBLowR9", "kinGenIso","001","750")
checksFormatting("mcCommon/output.root",1,0,0,"EEHighR9","kinGenIso","001","750")
checksFormatting("mcCommon/output.root",1,0,0,"EELowR9", "kinGenIso","001","750")

checksFormatting("mcCommon/output.root",1,0,0,"EBHighR9","kinNonGenIso","001","750")
checksFormatting("mcCommon/output.root",1,0,0,"EBLowR9", "kinNonGenIso","001","750")
checksFormatting("mcCommon/output.root",1,0,0,"EEHighR9","kinNonGenIso","001","750")
checksFormatting("mcCommon/output.root",1,0,0,"EELowR9", "kinNonGenIso","001","750")

checksFormatting("mcCommon/output.root",0,1,0,"EBHighR9");
checksFormatting("mcCommon/output.root",0,1,0,"EBLowR9");
checksFormatting("mcCommon/output.root",0,1,0,"EEHighR9");
checksFormatting("mcCommon/output.root",0,1,0,"EELowR9");

checksFormatting("dataCommon/output.root",0,0,1,"EBHighR9");
checksFormatting("dataCommon/output.root",0,0,1,"EBLowR9");
checksFormatting("dataCommon/output.root",0,0,1,"EEHighR9");
checksFormatting("dataCommon/output.root",0,0,1,"EELowR9");

.q

EOF 

echo "done"
