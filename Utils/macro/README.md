## Quick instructions to run the category optimization code

```cmsrel <YOUR_PREFERRED_VERSION>
cd <YOUR_PREFERRED_VERSION>
git clone https://github.com/musella/diphotons.git

scram b -j 4 diphotons/Utils diphotons/RooUtils

cmsenv

cd diphotons/Utils/macro

./categoryOptimizationMultiDim.py --load <config_file> --infile <input_file_name> --outdir <output_www_folder>
```

