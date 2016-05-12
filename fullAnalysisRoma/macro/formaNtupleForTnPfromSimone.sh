#! /bin/sh    

root -l -b <<EOF
.L tnpTreeFormatFromPasquale.cc++

tnpTreeFormatFromPasquale("dataCommon76x_ele_0T/output_Data.root","EBHighR9")
tnpTreeFormatFromPasquale("dataCommon76x_ele_0T/output_Data.root","EBLowR9")
tnpTreeFormatFromPasquale("dataCommon76x_ele_0T/output_Data.root","EEHighR9")
tnpTreeFormatFromPasquale("dataCommon76x_ele_0T/output_Data.root","EELowR9")


.q

EOF 

echo "done"
