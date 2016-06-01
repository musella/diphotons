#! /bin/sh

root -l -b <<EOF
.L signalModelGenonlyFormatting.cc++    

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","001","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","001","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","005","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","005","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","007","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","007","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","01","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","01","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","015","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","015","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","02","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","02","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","025","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","025","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","03","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","03","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","035","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","035","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","04","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","04","750","EE")  

signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","06","750","EB")  
signalModelGenonlyFormat("dataCommonGenOnlyV2/output.root","genGenIso","06","750","EE")  

.q

EOF

echo "done"    