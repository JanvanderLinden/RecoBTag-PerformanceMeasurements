# RecoBTag-PerformanceMeasurements

## Software setup for Fragmentation studies

```
cmsrel CMSSW_10_6_20 
cd CMSSW_10_6_20/src
cmsenv

for bash
export CMSSW_GIT_REFERENCE=/cvmfs/cms.cern.ch/cmssw.git.daily

git cms-init

git clone -b 10_6_X_fragmentationStudies https://github.com/JanvanderLinden/RecoBTag-PerformanceMeasurements.git RecoBTag/PerformanceMeasurements

git clone -b master_106X https://gitlab.cern.ch/vanderli/BFragmentationAnalyzer.git TopQuarkAnalysis/BFragmentationAnalyzer

scram b -j 12

```

The ntuplizer can be run and configured through 
```
RecoBTag/PerformanceMeasurements/test/runBTagAnalyzer_cfg.py
```

To get the fragmentation weights and variables, for example run
```
cmsRun runBTagAnalyzer_cfg.py defaults=2016_UltraLegacy_APV maxEvents=500 reportEvery=1 groups="EventInfo,Fragmentation,Quarks,Hadrons,PV,JetInfo,JetDeepFlavour,JetDeepCSV,JetSV"
```

NOTE1: due to the structure of the preliminary JECs, there are 2 different "defaults" sets; 

> **_preVFP_**: ```defaults=2016_UltraLegacy_APV```<br/>
> **_postVFP_**: ```defaults=2018_UltraLegacy_nonAPV```<br/>

This means in your crab configuration file, you will have to check which file you are running on, and pick the correct default set accordingly. As an example, one might do something like:

```
For MC samples: 
...
config.Data.inputDataset = /*/*RunIISummer20UL16*/MINIAODSIM
...

For collision data samples:
...
config.Data.inputDataset = /BTagMu*/Run2016*-21Feb2020_UL2016*/MINIAOD
...

...
if "HIPM" not in config.Data.inputDataset and "APV" not in config.Data.inputDataset: 
	config.JobType.pyCfgParams = [defaults=2016_UltraLegacy_nonAPV,...]
else: 
	config.JobType.pyCfgParams = [defaults=2016_UltraLegacy_APV, ...]
...
```

NOTE2: The preliminary JECs are only available for AK4PFCHS jets, and therefore you **_can not have any FatJet observables listed in you varGroup!_**. Otherwise the BTA will automatically assume you are running over FatJets and it will use the JECs included in the global tag, rather than in the local SQLite .db files!



To run the tests for integrating changes run:

```
cd RecoBTag/PerformanceMeasurements/test/
./run_tests.sh
```
The content of the output ntuple is by default empty and has to be configured according to your needs. The ```store*Variables``` options have been removed.
The new variable configuration can be customized in the file ```RecoBTag/PerformanceMeasurements/python/varGroups_cfi.py```.
New variables need also to be added (apart from adding them in the code) in ```RecoBTag/PerformanceMeasurements/python/variables_cfi.py```
