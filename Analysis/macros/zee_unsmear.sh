#!/bin/bash

#pustr="7.735e-06,1.575e-05,5.06e-05,8.869e-05,0.0001304,0.0001819,0.0002892,0.0008933,0.001639,0.003119,0.006672,0.0127,0.02062,0.0294,0.03836,0.04763,0.05549,0.06052,0.06272,0.06291,0.06194,0.06035,0.05835,0.05585,0.05272,0.04889,0.04452,0.03985,0.03509,0.03035,0.02571,0.02123,0.01703,0.01326,0.009987,0.007273,0.005112,0.003462,0.002256,0.001413,0.0008511,0.0004931,0.0002751,0.0001482,7.72e-05,3.902e-05,1.919e-05,9.231e-06,4.382e-06,2.091e-06,1.042e-06,5.737e-07,3.702e-07,2.823e-07,2.43e-07,2.229e-07,2.099e-07,1.991e-07,1.885e-07,1.776e-07,1.663e-07,1.547e-07,1.43e-07,1.312e-07,1.196e-07,1.083e-07,9.738e-08,8.696e-08,7.713e-08,6.794e-08,5.944e-08,5.165e-08,4.458e-08,3.821e-08,3.253e-08,2.751e-08,2.31e-08,1.927e-08,1.596e-08,1.313e-08,1.073e-08,8.712e-09,7.024e-09,5.624e-09,4.472e-09,3.533e-09,2.771e-09,2.159e-09,1.671e-09,1.284e-09,9.804e-10,7.433e-10,5.598e-10,4.187e-10,3.11e-10,2.295e-10,1.681e-10,1.224e-10,8.845e-11,6.35e-11"

./auto_plotter.py --selection cic \
                  --file  /afs/cern.ch/user/s/spigazzi/work/public/Xgg/Spring16/double_ele_spring16v2_sync_MC_05JulyEcorr_cert/output.root --move cicNoChIsoGenIso:cicNoChIso,cicNoChIsoNonGenIso:cicNoChIso,cicGenIso:cic,cicNonGenIso:cic \
                  --cut "1" \
                  --process "*DY*" \
                  --histograms "nvtx>>nvtx(101,0.5,100.5);rho>>rho(20,0,50);mass*sqrt((leadInitialEnergy*subLeadInitialEnergy)/(leadEnergy*subLeadEnergy))>>lowmass(560,60,200);leadSigmaIeIe>>leadSigmaIeIe(320,0,3.2e-2);subleadSigmaIeIe>>subleadSigmaIeIe(320,0,3.2e-2);leadR9>>leadR9(110,0,1.1);subleadR9>>subleadR9(110,0,1.1)" \
                  --output output_mc.root
