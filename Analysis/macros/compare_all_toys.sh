./bkg_bias.py --analyze-bias \
   --bias-files toys_mcV2bias_1k_pow/toys.root       --bias-labels pow \
   --bias-files toys_mcV2bias_1k_moddijet/toys.root  --bias-labels moddijet \
   --bias-files toys_mcV2bias_1k_expow2/toys.root    --bias-labels expow2 \
   --bias-files toys_mcV2bias_1k_invpow/toys.root    --bias-labels invpow \
   --bias-files toys_mcV2bias_1k_invpowlin/toys.root --bias-labels invpowlin \
   --bias-files toys_mcV2bias_1k_sumexp3/toys.root   --bias-labels sumexp3 \
   --no-use-custom \
   -O compare_all_mcV2bias

