#!/bin/zsh

writeScript() {

    root -b<<EOF

.L HighMassGGFitterCompiled.cc++ 
runfits(1500,"01")

.q
EOF

}

writeScript;
