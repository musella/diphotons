{

  if (gSystem->Getenv("CMSSW_VERSION")) {

    gSystem->Load("libFWCoreFWLite.so");
    AutoLibraryLoader::enable();

    TString path = gSystem->GetIncludePath();
    path += "-I. -I$ROOTSYS/src -I$ROOFITSYS/include/";
    gSystem->SetIncludePath(path.Data());
    
    TString str = gSystem->GetMakeSharedLib();
    if (str.Contains("-m32")==0 && str.Contains("-m64")==0) {
      str.ReplaceAll("g++", "g++ -m32");
      gSystem->SetMakeSharedLib(str);
    }
  }

  gSystem->Load("libHiggsAnalysisCombinedLimit.so");
}
