#ifndef FragTriggerSelection_h
#define FragTriggerSelection_h

#include <map>
#include <vector>
#include <iostream>
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

class FragTriggerSelection {
    public:
        FragTriggerSelection(const edm::ParameterSet&, edm::ConsumesCollector&&);
        ~FragTriggerSelection();

        void LoadTriggers(const edm::Event&, const edm::EventSetup&);
        bool IsTriggered();
        std::string GetBranchName(std::string);
        std::string GetTriggerName(std::string) const;
        int GetTriggerValue(std::string);
        int GetPrescaleValue(std::string);
    private:
        edm::EDGetTokenT<edm::TriggerResults>         triggerBitsToken_;
        edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescalesToken_;

        edm::Handle<edm::TriggerResults>         triggerBits;
        edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;

        bool noTriggers = false;
        std::string triggerSetup;
        std::vector<std::string> triggerList;
        std::map<std::string, bool> triggers;
        std::map<std::string, int>  prescales;

        //EventInfoBranches e;
};

#endif

