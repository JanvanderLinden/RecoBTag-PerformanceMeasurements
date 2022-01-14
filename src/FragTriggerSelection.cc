#include "RecoBTag/PerformanceMeasurements/interface/FragTriggerSelection.h"

FragTriggerSelection::FragTriggerSelection(const edm::ParameterSet& iConfig, edm::ConsumesCollector&& iC)
{
    std::cout << "initializing FragTriggerSelection" << std::endl;
    triggerSetup = iConfig.getParameter<std::string>("TriggerSetup");
    triggerList  = iConfig.getParameter<std::vector<std::string> >(triggerSetup);
    for(unsigned int i = 0; i < triggerList.size(); ++i)
        std::cout << triggerList.at(i) << std::endl;

    noTriggers = false;
    if(triggerList.size() == 1 && triggerList.at(0) == "None") 
    {
        cout << "Initialized without FragTriggerSelections" << endl;
        noTriggers = true;
    }

    triggerBitsToken_      = iC.consumes<edm::TriggerResults>(
        iConfig.getParameter<edm::InputTag>("triggerTable"));
    triggerPrescalesToken_ = iC.consumes<pat::PackedTriggerPrescales>(
        iConfig.getParameter<edm::InputTag>("triggerPrescales"));
}


FragTriggerSelection::~FragTriggerSelection() {}


std::string FragTriggerSelection::GetBranchName(std::string triggerName)
{
    std::string branchName = triggerName;
    int ast = branchName.find("*");
    if (ast == int(branchName.length() -1))
    {
        branchName.pop_back();
        branchName+="X";
    }
    return branchName;
}

std::string FragTriggerSelection::GetTriggerName(std::string triggerName) const
{
    int ast = triggerName.find("*");
    if (ast == int(std::string::npos))
    {
        if (triggers.find(triggerName) == triggers.end())
        {
            //cout << "Trigger " << triggerName << " does not exist" << endl;
            return "";
        }
        return triggerName;
    }
    else if (ast == int(triggerName.length() - 1))
    {
        triggerName.pop_back();
        bool foundTrigger = false;
        std::string fullTriggerName;
        for (auto it = triggers.begin(); it != triggers.end(); it++)
        {
            if(it->first.find(triggerName) != std::string::npos)
            {
                foundTrigger = true;
                fullTriggerName = it->first;
                //cout<<"Found trigger "<<fullTriggerName<<endl;
                if(it->second) break;
            }
        }
        if(!foundTrigger)
        {
            //cout << "Trigger " << triggerName << "* not found in trigger list" << endl;
            return "";
        }
        return fullTriggerName;
    }
    else
    {
        //cout << "Invalid Trigger name " << triggerName << endl;
        return "";
    }
}

void FragTriggerSelection::LoadTriggers(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    if(noTriggers) return;
    iEvent.getByToken(triggerBitsToken_,      triggerBits);
    iEvent.getByToken(triggerPrescalesToken_, triggerPrescales);   

    triggers.clear();
    prescales.clear();
    // fill trigger bits
    const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);
    for(unsigned int i = 0; i < triggerBits->size(); ++i)
    {
        triggers[names.triggerName(i)]  = triggerBits->accept(i);
        prescales[names.triggerName(i)] = triggerPrescales->getPrescaleForIndex(i);
    }
}

bool FragTriggerSelection::IsTriggered()
{   
    if(noTriggers) return true;

    // cout << "## CHECKING TRIGGERS ##" << endl;
    for(unsigned int i = 0; i < triggerList.size(); ++i)
    {
        std::string triggerName = GetTriggerName(triggerList.at(i));
        if(triggerName.empty()) continue;
        if(triggers[triggerName])
            return true;
    }
    return false;
}

int FragTriggerSelection::GetTriggerValue(std::string name)
{
    return triggers[name];
}
int FragTriggerSelection::GetPrescaleValue(std::string name)
{
    return prescales[name];
}

