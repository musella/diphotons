#!/bin/zsh

writeScript() {

    root -b<<EOF

.L ResolForSignalModel.cc++
runfits()    

.q
EOF

}

writeScript;
