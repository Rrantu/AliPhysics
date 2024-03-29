/***************************************************************************
               Modified by Dukhishyam Mallick- 01/04/2019 (dmallick@cern.ch)
               Modified by Dukhishyam Mallick- 12/04/2022 (dmallick@cern.ch)
	       //Based on AddAnalysisTaskRsnMini
	       //pPb specific settings from AddTaskKStarPPB.C
	       //
	       // Macro to configure the KStarPlusMinus analysis task
	       // It calls all configs desired by the user, by means
	       // of the boolean switches defined in the first lines.
	       // ---
	       // Inputs:
	       //  1) flag to know if running on MC or data
	       //  2) collision system, whether pp, pPb or PbPb
	       // --
	       // Returns:
	       //  kTRUE  --> initialization successful
	       //  kFALSE --> initialization failed (some config gave errors)
	       //
	       ****************************************************************************/

//enum ERsnCollType_t { kPP=0,      kPPb,      kPbPb};

#ifdef __CLING__
R__ADD_INCLUDE_PATH($ALICE_PHYSICS)
#include <PWGLF/RESONANCES/macros/mini/ConfigKStarPlusMinuspPbRun2_AOD.C>
#endif


//#include "ConfigKStarPlusMinuspPbRun2.C"
enum pairYCutSet { kPairDefault,    // USED ONLY FOR pA
    kNegative,       // USED ONLY FOR pA
    kCentral         // USED ONLY FOR pA
};

enum eventCutSet { kEvtDefault=0,
    kNoPileUpCut, //=1
    kDefaultVtx12,//=2
    kDefaultVtx8, //=3
    kDefaultVtx5, //=4
    kMCEvtDefault, //=5
    kSpecial1, //=6
    kSpecial2, //=7
    kNoEvtSel, //=8
    kSpecial3, //=9
    kSpecial4, //=10
    kSpecial5 //=11
};

enum eventMixConfig { kDisabled = -1,
    kMixDefault,     //=0 //10 events, Dvz = 1cm, DC = 10
    k5Evts,          //=1 //5 events, Dvz = 1cm, DC = 10
    k5Cent,          //=2 //10 events, Dvz = 1cm, DC = 5
};


AliRsnMiniAnalysisTask *AddTaskKStarPlusMinuspPbRun2_AOD
(
 Bool_t      isMC=kFALSE,
 Bool_t      isPP=kFALSE,
 Float_t     cutV = 10.0,
 Bool_t      isGT = 0,
 Int_t       evtCutSetID = 0,
 Int_t       pairCutSetID = 0,
 Int_t       mixingConfigID = 0,
 Int_t       aodFilterBit = 0,
 Bool_t      enableMonitor=kTRUE,
 TString     monitorOpt="pPb",
 Float_t     piPIDCut = 3.0,
 Float_t    nsigmaTOF = 3.0,
 Int_t       customQualityCutsID=1,
 AliRsnCutSetDaughterParticle::ERsnDaughterCutSet cutPiCandidate = AliRsnCutSetDaughterParticle::kTPCTOFpidphikstarpPb2016,
 Float_t     pi_k0s_PIDCut = 5.0,
 Float_t     massTol = 0.03,
 Float_t     massTolVeto = 0.0043,
 Int_t       tol_switch = 1,
 Double_t    tol_sigma = 6,
 Float_t     pLife = 20,
 Float_t     radiuslow = 0.5,
 Bool_t      Switch = kTRUE,
 Float_t     k0sDCA = 1000.0,
 Float_t     k0sCosPoinAn = 0.97,
 Float_t     k0sDaughDCA = 1.0,
 Int_t       NTPCcluster = 70,
 Float_t     maxDiffVzMix = 1.0,
 Float_t     maxDiffMultMix = 5.0,
 Float_t     maxDiffAngleMixDeg = 20.0,
 Int_t       aodN = 5,
 TString     outNameSuffix = "KStarPlusMinus_V0Mass_Pt",
 Float_t     DCAxy = 0.06,
 Bool_t      enableSys = kFALSE,
 Float_t     crossedRows = 70,
 Float_t     rowsbycluster = 0.8,
 Float_t     v0rapidity= 0.8,
 Int_t       Sys= 0
 )
{
    //-------------------------------------------
    // event cuts
    //-------------------------------------------
    UInt_t      triggerMask=AliVEvent::kINT7;
    Bool_t      rejectPileUp=kTRUE;
    Double_t    vtxZcut=10.0;//cm, default cut on vtx z
    Int_t       MultBins=aodFilterBit/100;
    //   cout<<"EVENTCUTID is    "<<evtCutSetID<<endl;
    if(evtCutSetID==eventCutSet::kDefaultVtx12) vtxZcut=12.0; //cm
    if(evtCutSetID==eventCutSet::kDefaultVtx8) vtxZcut=8.0; //cm
    if(evtCutSetID==eventCutSet::kDefaultVtx5) vtxZcut=5.0; //cm
    if(evtCutSetID==eventCutSet::kNoPileUpCut) rejectPileUp=kFALSE;
    
    
    if(isMC) rejectPileUp=kFALSE;
    
    //-------------------------------------------
    //mixing settings
    //-------------------------------------------
    
    Int_t       nmix = 10;
    if (mixingConfigID == eventMixConfig::kMixDefault) nmix = 10;
    if (mixingConfigID == eventMixConfig::k5Evts)      nmix = 5;
    if (mixingConfigID == eventMixConfig::k5Cent)      maxDiffMultMix = 5;
    
    //
    // -- INITIALIZATION ----------------------------------------------------------------------------
    // retrieve analysis manager
    //
    
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        ::Error("AddTaskKStarPlusMinus", "No analysis manager to connect to.");
        return NULL;
    }
    
    // create the task and configure
    TString taskName = Form("KStarPlusMinus%s%s", (isPP? "pp" : "pPb"), (isMC ? "MC" : "Data"));
    AliRsnMiniAnalysisTask* task = new AliRsnMiniAnalysisTask(taskName.Data(),isMC);
    //   task->UseESDTriggerMask(triggerMask);
    task->SelectCollisionCandidates(triggerMask);
    task->UseMultiplicity("AliMultSelection_V0A");
    
    // set event mixing options
    task->UseContinuousMix();
    //task->UseBinnedMix();
    task->SetNMix(nmix);
    task->SetMaxDiffVz(maxDiffVzMix);
    task->SetMaxDiffMult(maxDiffMultMix);
    ::Info("AddAnalysisTaskTOFKStar", Form("Event mixing configuration: \n events to mix = %i \n max diff. vtxZ = cm %5.3f \n max diff multi = %\5.3f",  nmix, maxDiffVzMix, maxDiffMultMix));
    
    mgr->AddTask(task);
    AliRsnCutPrimaryVertex *cutVertex = 0;
    cutVertex=new AliRsnCutPrimaryVertex("cutVertex",vtxZcut,0,kFALSE);
    AliRsnCutEventUtils* cutEventUtils=0;
    cutEventUtils=new AliRsnCutEventUtils("cutEventUtils",kTRUE,rejectPileUp);
    cutEventUtils->SetCheckAcceptedMultSelection();
    if(!isMC){ //assume pp data
        cutVertex->SetCheckPileUp(rejectPileUp);// set the check for pileup
        ::Info("AddAnalysisTaskTOFKStar", Form(":::::::::::::::::: Pile-up rejection mode: %s", (rejectPileUp)?"ON":"OFF"));
    }
    
     // define and fill cut set for event cut
    AliRsnCutSet* eventCuts=0;
    if(cutEventUtils || cutVertex){
        eventCuts=new AliRsnCutSet("eventCuts",AliRsnTarget::kEvent);
        
        if(cutEventUtils && cutVertex){
            eventCuts->AddCut(cutEventUtils);
            eventCuts->AddCut(cutVertex);
            eventCuts->SetCutScheme(Form("%s&%s",cutEventUtils->GetName(),cutVertex->GetName()));
        }else if(cutEventUtils && !cutVertex){
            eventCuts->AddCut(cutEventUtils);
            eventCuts->SetCutScheme(Form("%s",cutEventUtils->GetName()));
        }else if(!cutEventUtils && cutVertex){
            eventCuts->AddCut(cutVertex);
            eventCuts->SetCutScheme(Form("%s",cutVertex->GetName()));
        }
        
        task->SetEventCuts(eventCuts);
    }
    
    
    // -- EVENT-ONLY COMPUTATIONS -------------------------------------------------------------------
    //vertex
    Int_t vtxID=task->CreateValue(AliRsnMiniValue::kVz,kFALSE);
    AliRsnMiniOutput* outVtx=task->CreateOutput("eventVtx","HIST","EVENT");
    outVtx->AddAxis(vtxID,240,-12.0,12.0);
    
    //multiplicity
    Int_t multID=task->CreateValue(AliRsnMiniValue::kMult,kFALSE);
    AliRsnMiniOutput* outMult=task->CreateOutput("eventMult","HIST","EVENT");
    if(isPP)
    outMult->AddAxis(multID,400,0.5,400.5);
    else outMult->AddAxis(multID,100,0.,100.);
    
    TH2F* hvz=new TH2F("hVzVsCent","",100,0.,100., 240,-12.0,12.0);
    task->SetEventQAHist("vz",hvz);//plugs this histogram into the fHAEventVz data member
    
    TH2F* hmc=new TH2F("MultiVsCent","", 100,0.,100., 400,0.5,400.5);
    hmc->GetYaxis()->SetTitle("QUALITY");
    task->SetEventQAHist("multicent",hmc);//plugs this histogram into the fHAEventMultiCent data member
    
    //
    // -- PAIR CUTS (common to all resonances) ------------------------------------------------------
    Double_t    minYlab =  -0.465;
    Double_t    maxYlab =  0.035;
    
    AliRsnCutMiniPair* cutY=new AliRsnCutMiniPair("cutRapidity", AliRsnCutMiniPair::kRapidityRange);
    cutY->SetRangeD(-0.465,0.035);
    
    AliRsnCutMiniPair* cutV0=new AliRsnCutMiniPair("cutV0", AliRsnCutMiniPair::kContainsV0Daughter);
    
    AliRsnCutSet* PairCutsSame=new AliRsnCutSet("PairCutsSame",AliRsnTarget::kMother);
    PairCutsSame->AddCut(cutY);
    PairCutsSame->AddCut(cutV0);
    PairCutsSame->SetCutScheme(TString::Format("%s&(!%s)",cutY->GetName(),cutV0->GetName()).Data());
    //note the use of the ! operator in this cut scheme
    
    AliRsnCutSet* PairCutsMix=new AliRsnCutSet("PairCutsMix",AliRsnTarget::kMother);
    PairCutsMix->AddCut(cutY);
    PairCutsMix->SetCutScheme(cutY->GetName());


    #if !defined (__CINT__) || defined (__CLING__)

    if (!ConfigKStarPlusMinuspPbRun2_AOD(task, isMC, isPP, isGT, piPIDCut,nsigmaTOF,customQualityCutsID, cutPiCandidate, pi_k0s_PIDCut, aodFilterBit, enableMonitor, monitorOpt.Data(), massTol, massTolVeto, tol_switch, tol_sigma, pLife, radiuslow, Switch, k0sDCA, k0sCosPoinAn, k0sDaughDCA, NTPCcluster, "", PairCutsSame, PairCutsMix, DCAxy, enableSys, crossedRows, rowsbycluster, v0rapidity, Sys)) return 0x0;

 #else

    gROOT->LoadMacro("$ALICE_PHYSICS/PWGLF/RESONANCES/macros/mini/ConfigKStarPlusMinuspPbRun2_AOD.C");

    //    gROOT->LoadMacro("ConfigKStarPlusMinuspPbRun2.C");
  
 
    if (isMC) {
        Printf("========================== MC analysis - PID cuts not used");
    } else
        Printf("========================== DATA analysis - PID cuts used");
    
   if (!ConfigKStarPlusMinuspPbRun2_AOD(task, isMC, isPP, isGT, piPIDCut,nsigmaTOF,customQualityCutsID, cutPiCandidate, pi_k0s_PIDCut, aodFilterBit, enableMonitor, monitorOpt.Data(), massTol, massTolVeto, tol_switch, tol_sigma, pLife, radiuslow, Switch, k0sDCA, k0sCosPoinAn, k0sDaughDCA, NTPCcluster, "", PairCutsSame, PairCutsMix, DCAxy, enableSys, crossedRows, rowsbycluster, v0rapidity, Sys)) return 0x0;
  #endif
    // -- CONTAINERS -------------------------------------------------------------------------------//
    TString outputFileName = AliAnalysisManager::GetCommonFileName();
    //  outputFileName += ":Rsn";
    Printf("AddTaskKStarPlusMinus - Set OutputFileName : \n %s\n", outputFileName.Data() );
    
    
    AliAnalysisDataContainer *output = mgr->CreateContainer(Form("RsnOut_%s_%.1f_%.1f_%.1f_%.2f_%.3f_%.f_%.f_%.f_%.1f_%.2f_%.1f_%.3f_%.1f_%.2f", outNameSuffix.Data(),piPIDCut,nsigmaTOF,customQualityCutsID,pi_k0s_PIDCut,massTol,massTolVeto,pLife,radiuslow, k0sDCA,k0sCosPoinAn,k0sDaughDCA, DCAxy, Sys), TList::Class(), AliAnalysisManager::kOutputContainer, outputFileName);
    
    mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
    mgr->ConnectOutput(task, 1, output);
    
    return task;
}
