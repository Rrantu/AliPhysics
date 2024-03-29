/////////////////////////////////////////////////////////////////////////////////////////////
//
// AddTask* macro for flow analysis
// Creates a Flow Event task and adds it to the analysis manager.
// Sets the cuts using the correction framework (CORRFW) classes.
// Also creates Flow Analysis tasks and connects them to the output of the flow event task.
//
/////////////////////////////////////////////////////////////////////////////////////////////

AliAnalysisTaskFlowEvent *AddTaskFlowOnMC(TString fileNameBase="AnalysisResults",
					  Double_t etaMax = 1.5,
					  Double_t ptMin  = 0.05,
					  Double_t ptMax  = 20.0,
					  Int_t chargePOI = 0,
					  Int_t pdgPOI = 4122,
					  Int_t harmonic  = 2,
					  Double_t gImpactParameterMin = 0.0,
					  Double_t gImpactParameterMax = 100.0,
					  Int_t gRefMultMin = 0,
					  Int_t gRefMultMax = 100,
					  Bool_t MCEP     = kTRUE,
					  Bool_t SP       = kTRUE,
					  Bool_t GFC      = kFALSE,
					  Bool_t QC       = kTRUE,
					  Bool_t FQD      = kFALSE,
					  Bool_t LYZ1SUM  = kFALSE,
					  Bool_t LYZ1PROD = kFALSE,
					  Bool_t LYZ2SUM  = kFALSE,
					  Bool_t LYZ2PROD = kFALSE,
					  Bool_t LYZEP    = kFALSE,
					  Bool_t MH       = kFALSE,
					  Bool_t NL       = kFALSE,
					  Int_t side      = 0,
					  Bool_t kUseAfterBurner = kFALSE) {	  
  // Define the range for eta subevents (for SP method)
  Double_t minA = -etaMax;
  Double_t maxA = -0.5;
  Double_t minB = 0.5;
  Double_t maxB = etaMax;

  // AFTERBURNER
  //Bool_t useAfterBurner=kFALSE;
  Double_t v1=0.0;
  Double_t v2=0.0;
  Double_t v3=0.0;
  Double_t v4=0.0;
  Int_t numberOfTrackClones=0; //non-flow

  // Define a range of the detector to exclude
  Bool_t ExcludeRegion = kFALSE;
  Double_t excludeEtaMin = -0.;
  Double_t excludeEtaMax = 0.;
  Double_t excludePhiMin = 0.;
  Double_t excludePhiMax = 0.;

  // use physics selection class
  Bool_t  UsePhysicsSelection = kFALSE;

  // QA
  Bool_t runQAtask=kFALSE;
  Bool_t FillQAntuple=kFALSE;
  Bool_t DoQAcorrelations=kFALSE;

  //Methods
  Bool_t METHODS[] = {SP,LYZ1SUM,LYZ1PROD,LYZ2SUM,LYZ2PROD,LYZEP,GFC,QC,FQD,MCEP,MH,NL};

  // Boolean to use/not use weights for the Q vector
  Bool_t WEIGHTS[] = {kFALSE,kFALSE,kFALSE}; //Phi, v'(pt), v'(eta)

  // SETTING THE CUTS
  //---------Data selection----------
  //kMC, kGlobal, kTPCstandalone, kESD_SPDtracklet
  AliFlowTrackCuts::trackParameterType rptype = AliFlowTrackCuts::kMC;
  AliFlowTrackCuts::trackParameterType poitype = AliFlowTrackCuts::kMC;

  //---------Parameter mixing--------
  //kPure - no mixing, kTrackWithMCkine, kTrackWithMCPID, kTrackWithMCpt
  AliFlowTrackCuts::trackParameterMix rpmix = AliFlowTrackCuts::kTrackWithMCkine;
  AliFlowTrackCuts::trackParameterMix poimix = AliFlowTrackCuts::kTrackWithMCkine;


  const char* rptypestr = AliFlowTrackCuts::GetParamTypeName(rptype);
  const char* poitypestr = AliFlowTrackCuts::GetParamTypeName(poitype);

  //===========================================================================
  // EVENTS CUTS:
  AliFlowEventCuts* cutsEvent = new AliFlowEventCuts("event cuts");
  cutsEvent->SetImpactParameterRange(gImpactParameterMin,gImpactParameterMax);
  cutsEvent->SetRefMultRange(gRefMultMin,gRefMultMax);
  cutsEvent->SetQA(kFALSE);
  
  // Ref mult TRACK CUTS:
  AliFlowTrackCuts* cutsRefMult = new AliFlowTrackCuts("MCRefMult");
  cutsRefMult->SetParamType(rptype);
  cutsRefMult->SetParamMix(rpmix);
  cutsRefMult->SetPtRange(ptMin,ptMax);
  cutsRefMult->SetEtaRange(-etaMax,etaMax);
  cutsRefMult->SetRequireCharge(kTRUE);
  cutsRefMult->SetQA(kFALSE);
  cutsEvent->SetRefMultCuts(cutsRefMult);

  // RP TRACK CUTS:
  AliFlowTrackCuts* cutsRP = new AliFlowTrackCuts("MCRP");
  cutsRP->SetParamType(rptype);
  cutsRP->SetParamMix(rpmix);
  cutsRP->SetPtRange(ptMin,ptMax);
  cutsRP->SetEtaRange(-etaMax,etaMax);
  cutsRP->SetQA(kFALSE);

  // POI TRACK CUTS:
  AliFlowTrackCuts* cutsPOI = new AliFlowTrackCuts("MCPOI");
  cutsPOI->SetParamType(poitype);
  cutsPOI->SetParamMix(poimix);
  cutsPOI->SetPtRange(ptMin,ptMax);
  cutsPOI->SetEtaRange(-etaMax,etaMax);
  cutsPOI->SetMCPID(pdgPOI);

  // side A
  if(side < 0)
    cutsPOI->SetEtaRange(-etaMax,-0.5);

  // side C
  else if(side > 0)
  cutsPOI->SetEtaRange(0.5,etaMax);

  // both sides
  else
    cutsPOI->SetEtaRange(-etaMax,etaMax);
  if(chargePOI != 0)
    cutsPOI->SetCharge(chargePOI);
  cutsPOI->SetQA(kFALSE);

  TString outputSlotName("");
  outputSlotName+=Form("V%i_",harmonic);
  outputSlotName+=cutsRP->GetName();
  outputSlotName+="_";
  outputSlotName+=cutsPOI->GetName();

  TString fileName(fileNameBase);
  fileName.Append(".root");

  Bool_t useWeights  = WEIGHTS[0] || WEIGHTS[1] || WEIGHTS[2];
  if (useWeights) cout<<"Weights are used"<<endl;
  else cout<<"Weights are not used"<<endl;
  
  // Get the pointer to the existing analysis manager via the static access method.
  //==============================================================================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTaskFlowEvent", "No analysis manager to connect to.");
    return NULL;
  }
  
  // Check the analysis type using the event handlers connected to the analysis
  // manager. The availability of MC handler can also be checked here.
  //==============================================================================
  if (!mgr->GetInputEventHandler()) {
    ::Error("AddTaskFlowEvent", "This task requires an input event handler");
    return NULL;
  }  

  // Open external input files
  //===========================================================================
  //weights: 
  TFile *weightsFile = NULL;
  TList *weightsList = NULL;

  if(useWeights) {
    //open the file with the weights:
    weightsFile = TFile::Open("weights.root","READ");
    if(weightsFile) {
      //access the list which holds the histos with weigths:
      weightsList = (TList*)weightsFile->Get("weights");
    }
    else {
      cout<<" WARNING: the file <weights.root> with weights from the previous run was not available."<<endl;
      break;
    } 
  }
  
  //LYZ2
  if (LYZ2SUM || LYZ2PROD) {
    //read the outputfile of the first run
    TString outputFileName = "AnalysisResults1.root";
    TString pwd(gSystem->pwd());
    pwd+="/";
    pwd+=outputFileName.Data();
    TFile *outputFile = NULL;
    if(gSystem->AccessPathName(pwd.Data(),kFileExists)) {
      cout<<"WARNING: You do not have an output file:"<<endl;
      cout<<"         "<<pwd.Data()<<endl;
      exit(0);
    } else { outputFile = TFile::Open(pwd.Data(),"READ");}
    
    if (LYZ2SUM){  
      // read the output directory from LYZ1SUM 
      TString inputFileNameLYZ2SUM = "outputLYZ1SUManalysis" ;
      inputFileNameLYZ2SUM += rptypestr;
      cout<<"The input directory is "<<inputFileNameLYZ2SUM.Data()<<endl;
      TFile* fInputFileLYZ2SUM = (TFile*)outputFile->FindObjectAny(inputFileNameLYZ2SUM.Data());
      if(!fInputFileLYZ2SUM || fInputFileLYZ2SUM->IsZombie()) { 
	cerr << " ERROR: To run LYZ2SUM you need the output file from LYZ1SUM. This file is not there! Please run LYZ1SUM first." << endl ; 
	break;
      }
      else {
	TList* fInputListLYZ2SUM = (TList*)fInputFileLYZ2SUM->Get("LYZ1SUM");
	if (!fInputListLYZ2SUM) {cout<<"list is NULL pointer!"<<endl;}
      }
      cout<<"LYZ2SUM input file/list read..."<<endl;
    }

    if (LYZ2PROD){  
      // read the output directory from LYZ1PROD 
      TString inputFileNameLYZ2PROD = "outputLYZ1PRODanalysis" ;
      inputFileNameLYZ2PROD += rptypestr;
      cout<<"The input directory is "<<inputFileNameLYZ2PROD.Data()<<endl;
      TFile* fInputFileLYZ2PROD = (TFile*)outputFile->FindObjectAny(inputFileNameLYZ2PROD.Data());
      if(!fInputFileLYZ2PROD || fInputFileLYZ2PROD->IsZombie()) { 
	cerr << " ERROR: To run LYZ2PROD you need the output file from LYZ1PROD. This file is not there! Please run LYZ1PROD first." << endl ; 
	break;
      }
      else {
	TList* fInputListLYZ2PROD = (TList*)fInputFileLYZ2PROD->Get("LYZ1PROD");
	if (!fInputListLYZ2PROD) {cout<<"list is NULL pointer!"<<endl;}
      }
      cout<<"LYZ2PROD input file/list read..."<<endl;
    }
  }

  if (LYZEP) {
    //read the outputfile of the second run
    TString outputFileName = "AnalysisResults2.root";
    TString pwd(gSystem->pwd());
    pwd+="/";
    pwd+=outputFileName.Data();
    TFile *outputFile = NULL;
    if(gSystem->AccessPathName(pwd.Data(),kFileExists)) {
      cout<<"WARNING: You do not have an output file:"<<endl;
      cout<<"         "<<pwd.Data()<<endl;
      exit(0);
    } else {
      outputFile = TFile::Open(pwd.Data(),"READ");
    }
    
    // read the output file from LYZ2SUM
    TString inputFileNameLYZEP = "outputLYZ2SUManalysis" ;
    inputFileNameLYZEP += rptypestr;
    cout<<"The input file is "<<inputFileNameLYZEP.Data()<<endl;
    TFile* fInputFileLYZEP = (TFile*)outputFile->FindObjectAny(inputFileNameLYZEP.Data());
    if(!fInputFileLYZEP || fInputFileLYZEP->IsZombie()) { 
      cerr << " ERROR: To run LYZEP you need the output file from LYZ2SUM. This file is not there! Please run LYZ2SUM first." << endl ; 
      break;
    }
    else {
      TList* fInputListLYZEP = (TList*)fInputFileLYZEP->Get("LYZ2SUM");
      if (!fInputListLYZEP) {cout<<"list is NULL pointer!"<<endl;}
    }
    cout<<"LYZEP input file/list read..."<<endl;
  }
  
  
  // Create the FMD task and add it to the manager
  //===========================================================================
  if (rptypestr == "FMD") {
    AliFMDAnalysisTaskSE *taskfmd = NULL;
    if (rptypestr == "FMD") {
      taskfmd = new AliFMDAnalysisTaskSE("TaskFMD");
      mgr->AddTask(taskfmd);
      
      AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();
      pars->Init();
      pars->SetProcessPrimary(kTRUE); //for MC only
      pars->SetProcessHits(kFALSE);
      
      //pars->SetRealData(kTRUE); //for real data
      //pars->SetProcessPrimary(kFALSE); //for real data
    }
  }
  
  // Create the flow event task, add it to the manager.
  //===========================================================================
  AliAnalysisTaskFlowEvent *taskFE = NULL;

  if(kUseAfterBurner) { 
    taskFE = new AliAnalysisTaskFlowEvent(Form("TaskFlowEvent %s",outputSlotName.Data()),"",kFALSE,1);
    taskFE->SetFlow(v1,v2,v3,v4); 
    taskFE->SetNonFlowNumberOfTrackClones(numberOfTrackClones);
    taskFE->SetAfterburnerOn();
  }
  else {taskFE = new AliAnalysisTaskFlowEvent(Form("TaskFlowEvent %s",outputSlotName.Data()),"",kFALSE); }
  if (ExcludeRegion) {
    taskFE->DefineDeadZone(excludeEtaMin, excludeEtaMax, excludePhiMin, excludePhiMax); 
  }
  taskFE->SetSubeventEtaRange(minA, maxA, minB, maxB);
  if (UsePhysicsSelection) {
    taskFE->SelectCollisionCandidates(AliVEvent::kMB);
    cout<<"Using Physics Selection"<<endl;
  }
  mgr->AddTask(taskFE);
  
  // Pass cuts for RPs and POIs to the task:
  taskFE->SetCutsEvent(cutsEvent);
  taskFE->SetCutsRP(cutsRP);
  taskFE->SetCutsPOI(cutsPOI);

  // Create the analysis tasks, add them to the manager.
  //===========================================================================
  if (SP){
    AliAnalysisTaskScalarProduct *taskSP = new AliAnalysisTaskScalarProduct(Form("TaskScalarProduct %s",outputSlotName.Data()),WEIGHTS[0]);
    taskSP->SetRelDiffMsub(1.0);
    taskSP->SetApplyCorrectionForNUA(kTRUE);
    taskSP->SetHarmonic(harmonic);
    if(side < 0)   taskSP->SetTotalQvector("Qb");
    else     taskSP->SetTotalQvector("Qa");
    mgr->AddTask(taskSP);
  }
  if (LYZ1SUM){
    AliAnalysisTaskLeeYangZeros *taskLYZ1SUM = new AliAnalysisTaskLeeYangZeros(Form("TaskLeeYangZerosSUM %s",outputSlotName.Data()),kTRUE);
    taskLYZ1SUM->SetFirstRunLYZ(kTRUE);
    taskLYZ1SUM->SetUseSumLYZ(kTRUE);
    mgr->AddTask(taskLYZ1SUM);
  }
  if (LYZ1PROD){
    AliAnalysisTaskLeeYangZeros *taskLYZ1PROD = new AliAnalysisTaskLeeYangZeros(Form("TaskLeeYangZerosPROD %s",outputSlotName.Data()),kTRUE);
    taskLYZ1PROD->SetFirstRunLYZ(kTRUE);
    taskLYZ1PROD->SetUseSumLYZ(kFALSE);
    mgr->AddTask(taskLYZ1PROD);
  }
  if (LYZ2SUM){
    AliAnalysisTaskLeeYangZeros *taskLYZ2SUM = new AliAnalysisTaskLeeYangZeros(Form("TaskLeeYangZerosSUM %s",outputSlotName.Data()),kFALSE);
    taskLYZ2SUM->SetFirstRunLYZ(kFALSE);
    taskLYZ2SUM->SetUseSumLYZ(kTRUE);
    mgr->AddTask(taskLYZ2SUM);
  }
  if (LYZ2PROD){
    AliAnalysisTaskLeeYangZeros *taskLYZ2PROD = new AliAnalysisTaskLeeYangZeros(Form("TaskLeeYangZerosPROD %s",outputSlotName.Data()),kFALSE);
    taskLYZ2PROD->SetFirstRunLYZ(kFALSE);
    taskLYZ2PROD->SetUseSumLYZ(kFALSE);
    mgr->AddTask(taskLYZ2PROD);
  }
  if (LYZEP){
    AliAnalysisTaskLYZEventPlane *taskLYZEP = new AliAnalysisTaskLYZEventPlane(Form("TaskLYZEventPlane %s",outputSlotName.Data()));
    mgr->AddTask(taskLYZEP);
  }
  if (GFC){
    AliAnalysisTaskCumulants *taskGFC = new AliAnalysisTaskCumulants(Form("TaskCumulants %s",outputSlotName.Data()),useWeights);
    taskGFC->SetUsePhiWeights(WEIGHTS[0]); 
    taskGFC->SetUsePtWeights(WEIGHTS[1]);
    taskGFC->SetUseEtaWeights(WEIGHTS[2]); 
    mgr->AddTask(taskGFC);
  }
  if (QC){
    AliAnalysisTaskQCumulants *taskQC = new AliAnalysisTaskQCumulants(Form("TaskQCumulants %s",outputSlotName.Data()),useWeights);
    //taskQC->SetMultiplicityIs(AliFlowCommonConstants::kRP); 
    taskQC->SetMultiplicityIs(AliFlowCommonConstants::kExternal); 
    //taskQC->SetFillProfilesVsMUsingWeights(kFALSE);
    //taskQC->SetUseQvectorTerms(kTRUE);
    taskQC->SetUsePhiWeights(WEIGHTS[0]); 
    taskQC->SetUsePtWeights(WEIGHTS[1]);
    taskQC->SetUseEtaWeights(WEIGHTS[2]); 
    taskQC->SetCalculateCumulantsVsM(kTRUE);
    taskQC->SetnBinsMult(10000);
    taskQC->SetMinMult(0.);
    taskQC->SetMaxMult(10000.);
    taskQC->SetHarmonic(harmonic);
    taskQC->SetApplyCorrectionForNUA(kFALSE);
    taskQC->SetFillMultipleControlHistograms(kFALSE);     
    mgr->AddTask(taskQC);
  }
  if (FQD){
    AliAnalysisTaskFittingQDistribution *taskFQD = new AliAnalysisTaskFittingQDistribution(Form("TaskFittingQDistribution %s",outputSlotName.Data()),kFALSE);
    taskFQD->SetUsePhiWeights(WEIGHTS[0]); 
    taskFQD->SetqMin(0.);
    taskFQD->SetqMax(1000.);
    taskFQD->SetqNbins(10000);
    mgr->AddTask(taskFQD);
  }
  if (MCEP){
    AliAnalysisTaskMCEventPlane *taskMCEP = new AliAnalysisTaskMCEventPlane(Form("TaskMCEventPlane %s",outputSlotName.Data()));
    taskMCEP->SetHarmonic(harmonic);
    mgr->AddTask(taskMCEP);
  }
  if (MH){
    AliAnalysisTaskMixedHarmonics *taskMH = new AliAnalysisTaskMixedHarmonics(Form("TaskMixedHarmonics %s",outputSlotName.Data()),useWeights);
    taskMH->SetHarmonic(1); // n in cos[n(phi1+phi2-2phi3)] and cos[n(psi1+psi2-2phi3)]
    taskMH->SetNoOfMultipicityBins(10000);
    taskMH->SetMultipicityBinWidth(1.);
    taskMH->SetMinMultiplicity(1.);
    taskMH->SetCorrectForDetectorEffects(kTRUE);
    taskMH->SetEvaluateDifferential3pCorrelator(kTRUE); // evaluate <<cos[n(psi1+psi2-2phi3)]>> (Remark: two nested loops)    
    if(chargePOI == 0)
      taskMH->SetOppositeChargesPOI(kTRUE);    
    else
      taskMH->SetOppositeChargesPOI(kFALSE); // POIs psi1 and psi2 in cos[n(psi1+psi2-2phi3)] will have opposite charges  
    mgr->AddTask(taskMH);
  }  
  if (NL){
    AliAnalysisTaskNestedLoops *taskNL = new AliAnalysisTaskNestedLoops(Form("TaskNestedLoops %s",outputSlotName.Data()),useWeights);
    taskNL->SetHarmonic(1); // n in cos[n(phi1+phi2-2phi3)] and cos[n(psi1+psi2-2phi3)]
    taskNL->SetEvaluateNestedLoopsForRAD(kTRUE); // RAD = Relative Angle Distribution
    taskNL->SetEvaluateNestedLoopsForMH(kTRUE); // evalaute <<cos[n(phi1+phi2-2phi3)]>> (Remark: three nested loops)   
    taskNL->SetEvaluateDifferential3pCorrelator(kFALSE); // evaluate <<cos[n(psi1+psi2-2phi3)]>>  (Remark: three nested loops)   
    taskNL->SetOppositeChargesPOI(kFALSE); // POIs psi1 and psi2 in cos[n(psi1+psi2-2phi3)] will have opposite charges  
    mgr->AddTask(taskNL);
  }

  // Create the output container for the data produced by the task
  // Connect to the input and output containers
  //===========================================================================
  AliAnalysisDataContainer *cinput1 = mgr->GetCommonInputContainer();
  
  if (rptypestr == "FMD") {
    AliAnalysisDataContainer *coutputFMD = 
      mgr->CreateContainer(Form("BackgroundCorrected %s",outputSlotName.Data()), TList::Class(), AliAnalysisManager::kExchangeContainer);
    //input and output taskFMD     
    mgr->ConnectInput(taskfmd, 0, cinput1);
    mgr->ConnectOutput(taskfmd, 1, coutputFMD);
    //input into taskFE
    mgr->ConnectInput(taskFE,1,coutputFMD);
  }
  
  AliAnalysisDataContainer *coutputFE = 
  mgr->CreateContainer(Form("FlowEventSimple %s",outputSlotName.Data()),AliFlowEventSimple::Class(),AliAnalysisManager::kExchangeContainer);
  mgr->ConnectInput(taskFE,0,cinput1); 
  mgr->ConnectOutput(taskFE,1,coutputFE);
 
  if (taskFE->GetQAOn())
  {
    TString outputQA = fileName;
    outputQA += ":QA";
    AliAnalysisDataContainer* coutputFEQA = 
    mgr->CreateContainer(Form("QA %s",outputSlotName.Data()), TList::Class(),AliAnalysisManager::kOutputContainer,outputQA);
    mgr->ConnectOutput(taskFE,2,coutputFEQA);
  }

  // Create the output containers for the data produced by the analysis tasks
  // Connect to the input and output containers
  //===========================================================================
  if (useWeights) {    
    AliAnalysisDataContainer *cinputWeights = mgr->CreateContainer(Form("Weights %s",outputSlotName.Data()),
								   TList::Class(),AliAnalysisManager::kInputContainer); 
  }

  if(SP) {
    TString outputSP = fileName;
    outputSP += ":outputSPanalysis";
    outputSP+= rptypestr;
    AliAnalysisDataContainer *coutputSP = mgr->CreateContainer(Form("SP%s",outputSlotName.Data()), 
							       TList::Class(),AliAnalysisManager::kOutputContainer,outputSP); 
    mgr->ConnectInput(taskSP,0,coutputFE); 
    mgr->ConnectOutput(taskSP,1,coutputSP); 
    if (WEIGHTS[0]) {
      mgr->ConnectInput(taskSP,1,cinputWeights);
      cinputWeights->SetData(weightsList);
    }
  }
  if(LYZ1SUM) {
    TString outputLYZ1SUM = fileName;
    outputLYZ1SUM += ":outputLYZ1SUManalysis";
    outputLYZ1SUM+= rptypestr;
    AliAnalysisDataContainer *coutputLYZ1SUM = mgr->CreateContainer(Form("LYZ1SUM%s",outputSlotName.Data()), 
								    TList::Class(),AliAnalysisManager::kOutputContainer,outputLYZ1SUM); 
    mgr->ConnectInput(taskLYZ1SUM,0,coutputFE);
    mgr->ConnectOutput(taskLYZ1SUM,1,coutputLYZ1SUM);
  }
  if(LYZ1PROD) {
    TString outputLYZ1PROD = fileName;
    outputLYZ1PROD += ":outputLYZ1PRODanalysis";
    outputLYZ1PROD+= rptypestr;
    AliAnalysisDataContainer *coutputLYZ1PROD = mgr->CreateContainer(Form("LYZ1PROD%s",outputSlotName.Data()), 
								     TList::Class(),AliAnalysisManager::kOutputContainer,outputLYZ1PROD); 
    mgr->ConnectInput(taskLYZ1PROD,0,coutputFE); 
    mgr->ConnectOutput(taskLYZ1PROD,1,coutputLYZ1PROD);
  }
  if(LYZ2SUM) {
    AliAnalysisDataContainer *cinputLYZ2SUM = mgr->CreateContainer(Form("LYZ2SUMin%s",outputSlotName.Data()),
								   TList::Class(),AliAnalysisManager::kInputContainer);
    TString outputLYZ2SUM = fileName;
    outputLYZ2SUM += ":outputLYZ2SUManalysis";
    outputLYZ2SUM+= rptypestr;
    
    AliAnalysisDataContainer *coutputLYZ2SUM = mgr->CreateContainer(Form("LYZ2SUM%s",outputSlotName.Data()), 
								    TList::Class(),AliAnalysisManager::kOutputContainer,outputLYZ2SUM); 
    mgr->ConnectInput(taskLYZ2SUM,0,coutputFE); 
    mgr->ConnectInput(taskLYZ2SUM,1,cinputLYZ2SUM);
    mgr->ConnectOutput(taskLYZ2SUM,1,coutputLYZ2SUM); 
    cinputLYZ2SUM->SetData(fInputListLYZ2SUM);
  }
  if(LYZ2PROD) {
    AliAnalysisDataContainer *cinputLYZ2PROD = mgr->CreateContainer(Form("LYZ2PRODin%s",outputSlotName.Data()),
								    TList::Class(),AliAnalysisManager::kInputContainer);
    TString outputLYZ2PROD = fileName;
    outputLYZ2PROD += ":outputLYZ2PRODanalysis";
    outputLYZ2PROD+= rptypestr;
    
    AliAnalysisDataContainer *coutputLYZ2PROD = mgr->CreateContainer(Form("LYZ2PROD%s",outputSlotName.Data()), 
								     TList::Class(),AliAnalysisManager::kOutputContainer,outputLYZ2PROD); 
    mgr->ConnectInput(taskLYZ2PROD,0,coutputFE); 
    mgr->ConnectInput(taskLYZ2PROD,1,cinputLYZ2PROD);
    mgr->ConnectOutput(taskLYZ2PROD,1,coutputLYZ2PROD); 
    cinputLYZ2PROD->SetData(fInputListLYZ2PROD);
  }
  if(LYZEP) {
    AliAnalysisDataContainer *cinputLYZEP = mgr->CreateContainer(Form("LYZEPin%s",outputSlotName.Data()),
								 TList::Class(),AliAnalysisManager::kInputContainer);
    TString outputLYZEP = fileName;
    outputLYZEP += ":outputLYZEPanalysis";
    outputLYZEP+= rptypestr;
    
    AliAnalysisDataContainer *coutputLYZEP = mgr->CreateContainer(Form("LYZEP%s",outputSlotName.Data()), 
								  TList::Class(),AliAnalysisManager::kOutputContainer,outputLYZEP); 
    mgr->ConnectInput(taskLYZEP,0,coutputFE); 
    mgr->ConnectInput(taskLYZEP,1,cinputLYZEP);
    mgr->ConnectOutput(taskLYZEP,1,coutputLYZEP); 
    cinputLYZEP->SetData(fInputListLYZEP);
  }
  if(GFC) {
    TString outputGFC = fileName;
    outputGFC += ":outputGFCanalysis";
    outputGFC+= rptypestr;
    
    AliAnalysisDataContainer *coutputGFC = mgr->CreateContainer(Form("GFC%s",outputSlotName.Data()), 
								TList::Class(),AliAnalysisManager::kOutputContainer,outputGFC); 
    mgr->ConnectInput(taskGFC,0,coutputFE); 
    mgr->ConnectOutput(taskGFC,1,coutputGFC);
    if (useWeights) {
      mgr->ConnectInput(taskGFC,1,cinputWeights);
      cinputWeights->SetData(weightsList);
    } 
  }
  if(QC) {
    TString outputQC = fileName;
    outputQC += ":outputQCanalysis";
    outputQC+= rptypestr;

    AliAnalysisDataContainer *coutputQC = mgr->CreateContainer(Form("QC%s",outputSlotName.Data()), 
							       TList::Class(),AliAnalysisManager::kOutputContainer,outputQC); 
    mgr->ConnectInput(taskQC,0,coutputFE); 
    mgr->ConnectOutput(taskQC,1,coutputQC);
    if (useWeights) {
      mgr->ConnectInput(taskQC,1,cinputWeights);
      cinputWeights->SetData(weightsList);
    }
  }
  if(FQD) {
    TString outputFQD = fileName;
    outputFQD += ":outputFQDanalysis";
    outputFQD+= rptypestr;
    
    AliAnalysisDataContainer *coutputFQD = mgr->CreateContainer(Form("FQD%s",outputSlotName.Data()), 
								TList::Class(),AliAnalysisManager::kOutputContainer,outputFQD); 
    mgr->ConnectInput(taskFQD,0,coutputFE); 
    mgr->ConnectOutput(taskFQD,1,coutputFQD);
    if(useWeights) {
      mgr->ConnectInput(taskFQD,1,cinputWeights);
      cinputWeights->SetData(weightsList);
    } 
  }
  if(MCEP) {
    TString outputMCEP = fileName;
    outputMCEP += ":outputMCEPanalysis";
    outputMCEP+= rptypestr;
    
    AliAnalysisDataContainer *coutputMCEP = mgr->CreateContainer(Form("MCEP%s",outputSlotName.Data()), 
								 TList::Class(),AliAnalysisManager::kOutputContainer,outputMCEP); 
    mgr->ConnectInput(taskMCEP,0,coutputFE);
    mgr->ConnectOutput(taskMCEP,1,coutputMCEP); 
  }
  if(MH) {
    TString outputMH = fileName;
    outputMH += ":outputMHanalysis";
    outputMH += rptypestr;
        
    AliAnalysisDataContainer *coutputMH = mgr->CreateContainer(Form("MH%s",outputSlotName.Data()), 
							       TList::Class(),AliAnalysisManager::kOutputContainer,outputMH); 
    mgr->ConnectInput(taskMH,0,coutputFE); 
    mgr->ConnectOutput(taskMH,1,coutputMH); 
    //if (useWeights) {
    //  mgr->ConnectInput(taskMH,1,cinputWeights);
    //  cinputWeights->SetData(weightsList);
    //} 
  }
  if(NL) {
    TString outputNL = fileName;
    outputNL += ":outputNLanalysis";
    outputNL += rptypestr;

    AliAnalysisDataContainer *coutputNL = mgr->CreateContainer(Form("NL%s",outputSlotName.Data()), 
							       TList::Class(),AliAnalysisManager::kOutputContainer,outputNL); 
    mgr->ConnectInput(taskNL,0,coutputFE);
    mgr->ConnectOutput(taskNL,1,coutputNL);
    //if (useWeights) {
    //  mgr->ConnectInput(taskNL,1,cinputWeights);
    //  cinputWeights->SetData(weightsList);
    //} 
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  if (runQAtask)
  {
    AliAnalysisTaskQAflow* taskQAflow = new AliAnalysisTaskQAflow(Form("TaskQAflow %s",outputSlotName.Data()));
    taskQAflow->SetEventCuts(cutsEvent);
    taskQAflow->SetTrackCuts(cutsRP);
    taskQAflow->SetFillNTuple(FillQAntuple);
    taskQAflow->SetDoCorrelations(DoQAcorrelations);
    mgr->AddTask(taskQAflow);
    
    Printf("outputSlotName %s",outputSlotName.Data());
    TString taskQAoutputFileName(fileNameBase);
    taskQAoutputFileName.Append("_QA.root");
    AliAnalysisDataContainer* coutputQAtask = mgr->CreateContainer(Form("flowQA %s",outputSlotName.Data()),
                                              TObjArray::Class(),
                                              AliAnalysisManager::kOutputContainer,
                                              taskQAoutputFileName);
    AliAnalysisDataContainer* coutputQAtaskTree = mgr->CreateContainer(Form("flowQAntuple %s",outputSlotName.Data()),
                                              TNtuple::Class(),
                                              AliAnalysisManager::kOutputContainer,
                                              taskQAoutputFileName);
    mgr->ConnectInput(taskQAflow,0,mgr->GetCommonInputContainer());
    mgr->ConnectInput(taskQAflow,1,coutputFE);
    mgr->ConnectOutput(taskQAflow,1,coutputQAtask);
    if (FillQAntuple) mgr->ConnectOutput(taskQAflow,2,coutputQAtaskTree);
  }

  return taskFE;
}






