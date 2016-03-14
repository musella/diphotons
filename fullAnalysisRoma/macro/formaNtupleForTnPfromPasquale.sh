#! /bin/sh    

root -l -b <<EOF
.L tnpTreeFormatFromPasquale.cc++

tnpTreeFormatFromPasquale("dataCommon76x_ele_v4/output.root","EBHighR9")
tnpTreeFormatFromPasquale("dataCommon76x_ele_v4/output.root","EBLowR9")
tnpTreeFormatFromPasquale("dataCommon76x_ele_v4/output.root","EEHighR9")
tnpTreeFormatFromPasquale("dataCommon76x_ele_v4/output.root","EELowR9")


.q

EOF 

echo "done"
