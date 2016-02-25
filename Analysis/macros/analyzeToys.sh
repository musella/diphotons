./hadd_toys.sh $1
./bkg_bias.py --analyze-bias --bias-files $1/toys.root --bias-labels 200_10000 -O $1 --no-use-custom

