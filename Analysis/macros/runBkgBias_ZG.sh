#bash runAllFits_ZG.sh fit_v0
python bkg_bias.py --throw-toys --throw-from-model --n-toys=1000 --components data --models $1 --fit-name fit_v0 --store-new-only --read-ws EventYields_v0_eth74X_fit_v0_final_ws.root -o toys_mcV14bias_1k_$1/toys_input.root -O ./toys_mcV14bias_1k_$1/ --observable 'mass[230,200,2500]'
./submit_toys_ZG.sh 8nm toys_mcV14bias_1k_$1/toys_input.root toys_mcV14bias_1k_$1 1000 10 --fit-range 200,2500 --observable 'mass[230,200,2500]'
