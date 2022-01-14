import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import sys
import os

from CRABClient.UserUtilities import config as crab_config
from CRABAPI.RawCommand import crabCommand
import Utilities.General.cmssw_das_client as das_client

import optparse
from toolbox import mkdir

parser = optparse.OptionParser()
parser.add_option("-o", "--output", dest = "outdir",
    help = "output directory for crab configs")
parser.add_option("-s","--storage", dest = "storage", default = "T2_DE_DESY",
    help = "output storage (e.g. T2_DE_DESY)")
parser.add_option("-u","--cmsUser", dest = "cmsUser", default = "vanderli",
    help = "cms username for submit")
parser.add_option("-c","--config", dest = "config",
    help = "config to run via crab")
parser.add_option("-e","--era", dest = "dataEra",
    help = "era for determining global tag etc",
    choices = ["UL18","UL17","UL16preVFP","UL16postVFP"])
parser.add_option("-t","--tag", dest = "tag",
    help = "tag for dataset in output directory (make sure it doesnt exist yet")
parser.add_option("--submit",dest = "submit",default = False, action = "store_true",
    help = "submit crab jobs directly")
(opts, args) = parser.parse_args()

if opts.config is None:
    sys.exit("need to specify input config to run via crab")
if not os.path.exists(opts.config):
    sys.exit("config file does not exist")
if opts.outdir is None:
    sys.exit("need to specify output directory for crab configs")
if opts.tag is None:
    sys.exit("need to specify tag for output dataset")

# setting output directory
opts.outdir = mkdir(opts.outdir, versioning = True)


def writeCrabConfig(inputDataset, opts):
    sampleName = inputDataset.split("/")[1]

    config = crab_config()
    config.General.requestName = "fragBTA_{tag}_{sampleName}".format(
        tag = opts.tag, sampleName = sampleName)
    config.General.workArea = "crabJobs"
    
    config.JobType.pluginName  = "Analysis"
    config.JobType.psetName    = os.path.abspath(opts.config)
    config.JobType.outputFiles = ["tree.root"]

    triggerSelection = ""
    if "TTTo2L2Nu" in sampleName:
        triggerSelection = "TTbar"
    elif "QCD" in sampleName:
        triggerSelection = "QCD"
    else: sys.exit("no valid triggerSelection")

    # Data_94X, Data_94X_2016, MC_Fall17MiniAODv2, MC_Summer16MiniAODv3, MC_Summer19UL18MiniAODv2'
    defaults = ""
    if opts.dataEra == "UL18":
        defaults = "2018_UltraLegacy"
    elif opts.dataEra == "UL17":
        defaults = "2017_UltraLegacy"
    elif opts.dataEra == "UL16preVFP":
        defaults = "2016_UltraLegacy_APV"
    elif opts.dataEra == "UL16postVFP":
        defaults = "defaults=2018_UltraLegacy_nonAPV"
    else:
        sys.exit("no valid era")
        

    config.JobType.pyCfgParams = [
        "defaults={}".format(defaults),
        "groups=\"EventInfo,Fragmentation,Hadrons,JetFragInfo,JetDeepFlavour,JetDeepCSV,JetSV\"",
        "fragTriggerSet={}".format(triggerSelection),
        "outFilename=tree.root",
        ]

   
    config.JobType.maxMemoryMB             = 2000
    config.JobType.sendPythonFolder        = True
    config.JobType.allowUndistributedCMSSW = True
    
    config.Data.publication      = False
    config.Data.inputDataset     = inputDataset
    config.Data.inputDBS         = "global"
    config.Data.splitting        = "FileBased"
    config.Data.unitsPerJob      = 1
    config.Data.outputDatasetTag = opts.tag

    outPath = "/store/user/{cmsUser}/btv/".format(
        cmsUser = opts.cmsUser)
    config.Data.outLFNDirBase = outPath

    
    config.User.voGroup     = "dcms"
    config.Site.storageSite = opts.storage

    cfgpath = os.path.join(opts.outdir, "crab_{}.py".format(sampleName))
    with open(cfgpath, "w") as cfg:
        cfg.write(str(config))
    print("\twrote config {}".format(cfgpath))

    if opts.submit:
        os.chdir(opts.outdir)
        jobInfo = crabCommand("submit", config = config)

    target = ""
    if opts.storage == "T2_DE_DESY":
        target = "/pnfs/desy.de/cms/tier2"
    pnfsPath = os.path.join(target, outPath[1:], sampleName, opts.tag)
    return pnfsPath

def getDataSets(daspath):
    dasquery = das_client.get_data(
        "dataset={} instance=prod/global".format(daspath))
    
    datasets = []
    for sample in dasquery["data"]:
        if len(sample["dataset"]) == 1:
            dataset = sample["dataset"][0]["name"]
            if not len(dataset.split("/")) == 4:
                print("weird dataset name {}".format(dataset))
                continue

            datasets.append(dataset)

    datasets = list(set(datasets))
    print("database entries:")
    for d in datasets: print("\t"+d)
    return datasets

outputDatasets = []
for daspath in args:
    print("searching samples from {}".format(daspath))
    datasets = getDataSets(daspath)
    print("\nwriting configs ...")
    for dataset in datasets:
        path = writeCrabConfig(dataset, opts)
        outputDatasets.append(path)
    print("="*30)

sampleFile = os.path.join(opts.outdir, "samples.txt")
with open(sampleFile, "w") as f:
    f.write("\n".join(outputDatasets))

