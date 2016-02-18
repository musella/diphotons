#!/bin/zsh

writeScript() {

    root -b<<EOF

.L HighMorphCompiled.cc++
runfits("001")    

.q
EOF

}

writeScript;
