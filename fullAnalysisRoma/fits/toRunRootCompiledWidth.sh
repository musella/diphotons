#!/bin/zsh

writeScript() {

    root -b<<EOF

.L widthForSignalModel.cc++
runfits("02")    

.q
EOF

}

writeScript;
