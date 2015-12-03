from flashgg.MetaData.JobConfig import JobConfig

customize = JobConfig(metaDataSrc="diphotons",
                      crossSections=["$CMSSW_BASE/src/flashgg/MetaData/data/cross_sections.json",
                                     "$CMSSW_BASE/src/diphotons/MetaData/data/cross_sections.json",
                                     "$CMSSW_BASE/src/diphotons/MetaData/data/cross_sections_gen_grid.json"])

