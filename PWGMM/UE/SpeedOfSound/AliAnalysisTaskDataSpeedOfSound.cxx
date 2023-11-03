/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.
 *                                                                        *
 * Authors: Omar Vazquez (omar.vazquez.rueda@cern.ch)                     *
 **************************************************************************/

/* This source code yields the histograms for the measurement of the
 * speed of sound using very central Pb-Pb collisions.
 */

class TTree;

class AliPPVsMultUtils;
class AliESDtrackCuts;

#include <AliAnalysisFilter.h>
#include <AliESDVertex.h>
#include <AliHeader.h>
#include <AliMultiplicity.h>
#include <TBits.h>
#include <TDirectory.h>
#include <TMath.h>
#include <TRandom.h>
#include <TTree.h>

#include <iostream>
#include <vector>

#include "AliAnalysisManager.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisTaskESDfilter.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisUtils.h"
#include "AliCentrality.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliESDUtils.h"
#include "AliESDVZERO.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"
// #include "AliEventCuts.h"
#include "AliGenCocktailEventHeader.h"
#include "AliGenEventHeader.h"
#include "AliInputEventHandler.h"
#include "AliLog.h"
#include "AliMCEvent.h"
#include "AliMCEventHandler.h"
// #include "AliMCParticle.h"
#include "AliMultEstimator.h"
#include "AliMultInput.h"
#include "AliMultSelection.h"
#include "AliMultVariable.h"
#include "AliMultiplicity.h"
#include "AliOADBContainer.h"
#include "AliOADBMultSelection.h"
#include "AliPPVsMultUtils.h"
#include "AliStack.h"
#include "AliVEvent.h"
#include "AliVTrack.h"
#include "AliVVertex.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THnSparse.h"
#include "TLegend.h"
#include "TList.h"
#include "TMath.h"
#include "TParticle.h"
#include "TProfile.h"
#include "TVector3.h"

using std::cout;
using std::endl;
using std::vector;

static constexpr int v0m_Nbins{1};
static constexpr double v0m_bins[v0m_Nbins + 1] = {0.0, 5.0};
static constexpr double uc_v0m_bins_high[v0m_Nbins] = {5.0};
static constexpr double uc_v0m_bins_low[v0m_Nbins] = {0.0};
static const char* uc_v0m_bins_name[v0m_Nbins] = {"0_5"};

#include "AliAnalysisTaskDataSpeedOfSound.h"

class AliAnalysisTaskDataSpeedOfSound;  // your analysis class

ClassImp(AliAnalysisTaskDataSpeedOfSound)  // classimp: necessary for root

    AliAnalysisTaskDataSpeedOfSound::AliAnalysisTaskDataSpeedOfSound()
    : AliAnalysisTaskSE(),
      fESD(0),
      fEventCuts(0x0),
      fMCStack(0),
      fMC(0),
      fUseZDC(false),
      fUseMC(kFALSE),
      fIsTPConly(kTRUE),
      fTrigger(AliVEvent::kCentral),
      fTrackFilter(0x0),
      fTrackFilterwoDCA(0x0),
      fOutputList(0),
      fEtaCut(0.8),
      fEtaMin(-0.8),
      fEtaMax(0.8),
      fPtMin(0.15),
      fV0Mmin(0.0),
      fV0Mmax(100.0),
      fHMCut(10.0),
      ftrackmult08(0),
      fv0mpercentile(0),
      fv0mamplitude(0),
      fdcaxy(-999),
      fdcaz(-999),
      fMultSelection(0x0),
      hNchvsV0M(0),
      hNchvsV0MAmp(0),
      hV0MvsV0MAmp(0),
      pV0MAmpChannel(0),
      hV0MAmplitude(0),
      hV0Mmult(0),
      pPtvsNch(0),
      pPtEtaNegvsNchEtaPos(0),
      pPtEtaPosvsNchEtaNeg(0),
      pPtvsV0MAmp(0),
      hPtvsNchvsV0MAmp(0),
      hNchEtaPosvsNchEtaNeg(0),
      hPtEtaNegvsNchEtaPosvsV0MAmp(0),
      hPtEtaPosvsNchEtaNegvsV0MAmp(0),
      hTrueVtxZ(0),
      hTrueNchvsTrueV0MAmp(0),
      hRecNchvsRecV0MAmp(0),
      hNchResponse(0),
      hPtTruePrivsV0M(0),
      hPtRecPrivsV0M(0),
      hRecPtvsRecNchvsRecV0MAmp(0),
      hNchEtaPosvsNchEtaNeg_MCRec(0),
      hPtEtaNegvsNchEtaPosvsV0MAmp_MCRec(0),
      hPtEtaPosvsNchEtaNegvsV0MAmp_MCRec(0),
      hTruePtvsTrueNchvsTrueV0MAmp(0),
      hNchEtaPosvsNchEtaNeg_MCTrue(0),
      hPtEtaNegvsNchEtaPosvsV0MAmp_MCTrue(0),
      hPtEtaPosvsNchEtaNegvsV0MAmp_MCTrue(0),
      hAllpTRec(0),
      hAllpTTrue(0),
      hPripTRec(0),
      hPripTTrue(0),
      hTrueNchHM(0),
      hTrueNchHMWithTrigger(0),
      hTrueNchHMWithEventCuts(0),
      hTrueNchHMWithVtxSel(0),
      hZNCvsZNA(0),
      hZPCvsZPA(0),
      hZEM(0),
      hZNvsNch(0),
      hZAvsNchHM(0),
      hZCvsNchHM(0),
      hZNvsNchHM(0),
      hZNvsV0MAmp(0),
      hZNvsV0MAmpHM(0),
      hPtvsNchvsZAHM(0),
      hPtvsNchvsZCHM(0),
      hPtvsNchvsZNHM(0),
      pPtvsZA(0),
      pPtvsZC(0),
      pPtvsZN(0) {
  for (int i = 0; i < v0m_Nbins; ++i) {
    hDCAxyPri[i] = 0;
    hDCAxyWeDe[i] = 0;
    hDCAxyMaIn[i] = 0;
    hDCAxyData[i] = 0;
  }
}
//_____________________________________________________________________________
AliAnalysisTaskDataSpeedOfSound::AliAnalysisTaskDataSpeedOfSound(
    const char* name)
    : AliAnalysisTaskSE(name),
      fESD(0),
      fEventCuts(0x0),
      fMCStack(0),
      fMC(0),
      fUseZDC(false),
      fUseMC(kFALSE),
      fIsTPConly(kTRUE),
      fTrigger(AliVEvent::kCentral),
      fTrackFilter(0x0),
      fTrackFilterwoDCA(0x0),
      fOutputList(0),
      fEtaCut(0.8),
      fEtaMin(-0.8),
      fEtaMax(0.8),
      fPtMin(0.15),
      fV0Mmin(0.0),
      fV0Mmax(100.0),
      fHMCut(10.0),
      ftrackmult08(0),
      fv0mpercentile(0),
      fv0mamplitude(0),
      fdcaxy(-999),
      fdcaz(-999),
      fMultSelection(0x0),
      hNchvsV0M(0),
      hNchvsV0MAmp(0),
      hV0MvsV0MAmp(0),
      pV0MAmpChannel(0),
      hV0MAmplitude(0),
      hV0Mmult(0),
      pPtvsNch(0),
      pPtEtaNegvsNchEtaPos(0),
      pPtEtaPosvsNchEtaNeg(0),
      pPtvsV0MAmp(0),
      hPtvsNchvsV0MAmp(0),
      hNchEtaPosvsNchEtaNeg(0),
      hPtEtaNegvsNchEtaPosvsV0MAmp(0),
      hPtEtaPosvsNchEtaNegvsV0MAmp(0),
      hTrueVtxZ(0),
      hTrueNchvsTrueV0MAmp(0),
      hRecNchvsRecV0MAmp(0),
      hNchResponse(0),
      hPtTruePrivsV0M(0),
      hPtRecPrivsV0M(0),
      hRecPtvsRecNchvsRecV0MAmp(0),
      hNchEtaPosvsNchEtaNeg_MCRec(0),
      hPtEtaNegvsNchEtaPosvsV0MAmp_MCRec(0),
      hPtEtaPosvsNchEtaNegvsV0MAmp_MCRec(0),
      hTruePtvsTrueNchvsTrueV0MAmp(0),
      hNchEtaPosvsNchEtaNeg_MCTrue(0),
      hPtEtaNegvsNchEtaPosvsV0MAmp_MCTrue(0),
      hPtEtaPosvsNchEtaNegvsV0MAmp_MCTrue(0),
      hAllpTRec(0),
      hAllpTTrue(0),
      hPripTRec(0),
      hPripTTrue(0),
      hTrueNchHM(0),
      hTrueNchHMWithTrigger(0),
      hTrueNchHMWithEventCuts(0),
      hTrueNchHMWithVtxSel(0),
      hZNCvsZNA(0),
      hZPCvsZPA(0),
      hZEM(0),
      hZNvsNch(0),
      hZAvsNchHM(0),
      hZCvsNchHM(0),
      hZNvsNchHM(0),
      hZNvsV0MAmp(0),
      hZNvsV0MAmpHM(0),
      hPtvsNchvsZAHM(0),
      hPtvsNchvsZCHM(0),
      hPtvsNchvsZNHM(0),
      pPtvsZA(0),
      pPtvsZC(0),
      pPtvsZN(0) {
  for (int i = 0; i < v0m_Nbins; ++i) {
    hDCAxyPri[i] = 0;
    hDCAxyWeDe[i] = 0;
    hDCAxyMaIn[i] = 0;
    hDCAxyData[i] = 0;
  }
  DefineInput(0, TChain::Class());  // define the input of the analysis: in this
                                    // case you take a 'chain' of events
  // this chain is created by the analysis manager, so no need to worry about
  // it, does its work automatically
  DefineOutput(1, TList::Class());  // define the ouptut of the analysis: in
                                    // this case it's a list of histograms
}
//_____________________________________________________________________________
AliAnalysisTaskDataSpeedOfSound::~AliAnalysisTaskDataSpeedOfSound() {
  // destructor
  if (fOutputList) {
    delete fOutputList;  // at the end of your task, it is deleted from memory
                         // by calling this function
    fOutputList = 0x0;
  }
}
//_____________________________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::UserCreateOutputObjects() {
  if (!fTrackFilter) {
    if (fIsTPConly)  // Default option
    {
      fTrackFilter = new AliAnalysisFilter("trackFilterTPConly");
      AliESDtrackCuts* fCuts2 = AliESDtrackCuts::GetStandardTPCOnlyTrackCuts();
      fCuts2->SetRequireTPCRefit(kTRUE);
      fCuts2->SetRequireITSRefit(kTRUE);
      fCuts2->SetEtaRange(-0.8, 0.8);
      fTrackFilter->AddCuts(fCuts2);
    } else {  // Same as in the Nch vs. mult in pp, p-Pb and Pb-Pb
      std::cout << "Selecting non-TPConly cuts" << std::endl;
      fTrackFilter = new AliAnalysisFilter("trackFilter2015");
      AliESDtrackCuts* fCuts2_1 = new AliESDtrackCuts();
      fCuts2_1->SetMaxFractionSharedTPCClusters(0.4);                //
      fCuts2_1->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);  //
      fCuts2_1->SetCutGeoNcrNcl(3., 130., 1.5, 0.85, 0.7);           //
      fCuts2_1->SetMaxChi2PerClusterTPC(4);                          //
      fCuts2_1->SetAcceptKinkDaughters(kFALSE);                      //
      fCuts2_1->SetRequireTPCRefit(kTRUE);                           //
      fCuts2_1->SetRequireITSRefit(kTRUE);                           //
      fCuts2_1->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
                                         AliESDtrackCuts::kAny);    //
      fCuts2_1->SetMaxDCAToVertexXYPtDep("0.0182+0.0350/pt^1.01");  //
      fCuts2_1->SetMaxChi2TPCConstrainedGlobal(36);                 //
      fCuts2_1->SetMaxDCAToVertexZ(2);                              //
      fCuts2_1->SetDCAToVertex2D(kFALSE);                           //
      fCuts2_1->SetRequireSigmaToVertex(kFALSE);                    //
      fCuts2_1->SetMaxChi2PerClusterITS(36);                        //
      fCuts2_1->SetEtaRange(-0.8, 0.8);
      fTrackFilter->AddCuts(fCuts2_1);
    }
  }

  // track cuts to find contamination via DCA distribution
  if (!fTrackFilterwoDCA) {
    fTrackFilterwoDCA = new AliAnalysisFilter("trackFilter2015");
    AliESDtrackCuts* fCuts3 = new AliESDtrackCuts();
    fCuts3->SetMaxFractionSharedTPCClusters(0.4);                //
    fCuts3->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);  //
    fCuts3->SetCutGeoNcrNcl(3., 130., 1.5, 0.85, 0.7);           //
    fCuts3->SetMaxChi2PerClusterTPC(4);                          //
    fCuts3->SetAcceptKinkDaughters(kFALSE);                      //
    fCuts3->SetRequireTPCRefit(kTRUE);                           //
    fCuts3->SetRequireITSRefit(kTRUE);                           //
    fCuts3->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
                                     AliESDtrackCuts::kAny);  //
    // fCuts3->SetMaxDCAToVertexXYPtDep("0.0182+0.0350/pt^1.01");//
    // fCuts3->SetMaxChi2TPCConstrainedGlobal(36);//
    fCuts3->SetMaxDCAToVertexZ(2);            //
    fCuts3->SetDCAToVertex2D(kFALSE);         //
    fCuts3->SetRequireSigmaToVertex(kFALSE);  //
    fCuts3->SetMaxChi2PerClusterITS(36);      //
    fCuts3->SetEtaRange(-0.8, 0.8);
    fTrackFilterwoDCA->AddCuts(fCuts3);
  }

  // create output objects
  OpenFile(1);
  fOutputList = new TList();

  fOutputList->SetOwner(kTRUE);

  constexpr int pt_Nbins{190};
  double pt_bins[pt_Nbins + 1] = {0};
  for (int i = 0; i <= pt_Nbins; ++i) {
    pt_bins[i] = 0.15 + (i * 0.08);
  }
  // constexpr int pt_Nbins{41};
  // constexpr double pt_bins[pt_Nbins + 1] = {
  //     0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45, 0.5,  0.6, 0.7,  0.8,
  //     0.9,  1.0,  1.25, 1.5,  2.0,  2.5,  3.0,  3.5,  4.0, 4.5,  5.0,
  //     5.5,  6.0,  6.5,  7.0,  7.5,  8.0,  8.5,  9.0,  9.5, 10.0, 11.0,
  //     12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0};

  constexpr int nch_Nbins{84};
  double nch_bins[nch_Nbins + 1] = {0};
  for (int i = 0; i <= nch_Nbins; ++i) {
    nch_bins[i] = 30.0 * i;
  }

  constexpr double v0mAmp_width{25.0};
  constexpr int v0mAmp_Nbins{1720};
  double v0mAmp_bins[v0mAmp_Nbins + 1] = {0};
  for (int i = 0; i <= v0mAmp_Nbins; ++i) {
    v0mAmp_bins[i] = 0.0 + i * v0mAmp_width;
  }

  constexpr int v0mAmp_Nbins_true{800};
  double v0mAmp_bins_true[v0mAmp_Nbins_true + 1] = {0};
  for (int i = 0; i <= v0mAmp_Nbins_true; ++i) {
    v0mAmp_bins_true[i] = 0.0 + i * v0mAmp_width;
  }

  constexpr int dcaxy_Nbins{100};
  double dcaxy_bins[dcaxy_Nbins + 1] = {0};
  for (int i = 0; i <= dcaxy_Nbins; ++i) {
    dcaxy_bins[i] = -3.0 + (0.06 * i);
  }

  constexpr int v0m_Nbins080{6};
  constexpr double v0m_bins080[v0m_Nbins080 + 1] = {0.0,  1.0,  5.0, 10.0,
                                                    20.0, 50.0, 80.0};

  hV0Mmult =
      new TH1F("hV0Mmult", ";V0M (%);Entries", v0m_Nbins080, v0m_bins080);
  fOutputList->Add(hV0Mmult);

  //! This is used in data
  hNchvsV0M = new TH2D("hNchvsV0M", ";#it{N}_{ch}; V0M (%)", nch_Nbins,
                       nch_bins, v0m_Nbins080, v0m_bins080);

  hNchvsV0MAmp = new TH2D("hNchvsV0MAmp", ";#it{N}_{ch}; V0M Amp", nch_Nbins,
                          nch_bins, v0mAmp_Nbins, v0mAmp_bins);

  hV0MvsV0MAmp = new TH2D("hV0MvsV0MAmp", ";V0M Ampl; V0M (%)", v0mAmp_Nbins,
                          v0mAmp_bins, v0m_Nbins080, v0m_bins080);

  pV0MAmpChannel =
      new TProfile("pV0MAmpChannel", ";V0 Channel; Amplitude;", 64, -0.5, 63.5);

  hV0MAmplitude =
      new TH1D("hV0MAmp", ";V0M Amplitude; Entries", v0mAmp_Nbins, v0mAmp_bins);

  pPtvsNch = new TProfile("pPtvsNch",
                          "; #it{N}_{ch}^{rec}; #LT#it{p}_{T}#GT GeV/#it{c}",
                          nch_Nbins, nch_bins);
  pPtEtaNegvsNchEtaPos =
      new TProfile("pPtEtaNegvsNchEtaPos",
                   "; #it{N}_{ch} (0#leq#eta#leq0.8); #it{p}_{T} (GeV/#it{c})"
                   "(-0.8#leq#eta<0)",
                   nch_Nbins, nch_bins);
  pPtEtaPosvsNchEtaNeg =
      new TProfile("pPtEtaPosvsNchEtaNeg",
                   "; #it{N}_{ch} (-0.8#leq#eta<0); #it{p}_{T} (GeV/#it{c})"
                   "(0#geq#eta#leq0.8)",
                   nch_Nbins, nch_bins);
  pPtvsV0MAmp = new TProfile("pPtvsV0MAmp",
                             "; V0M Amplitude; #LT#it{p}_{T}#GT GeV/#it{c}",
                             v0mAmp_Nbins, v0mAmp_bins);

  hPtvsNchvsV0MAmp = new TH3D(
      "hPtvsNchvsV0MAmp", ";V0M Amp; #it{N}_{ch}^{rec}; #it{p}_{T} GeV/#it{c}",
      v0mAmp_Nbins, v0mAmp_bins, nch_Nbins, nch_bins, pt_Nbins, pt_bins);

  hNchEtaPosvsNchEtaNeg =
      new TH2D("hNchEtaPosvsNchEtaNeg",
               ";#it{N}_{ch} (0#leq#eta#leq0.8); #it{N}_{ch} (-0.8#leq#eta<0)",
               nch_Nbins, nch_bins, nch_Nbins, nch_bins);

  hPtEtaNegvsNchEtaPosvsV0MAmp = new TH3D(
      "hPtEtaNegvsNchEtaPosvsV0MAmp",
      "; V0M Amplitude; #it{N}_{ch} (0#leq#eta#leq0.8); #it{p}_{T} (GeV/#it{c})"
      "(-0.8#leq#eta<0)",
      v0mAmp_Nbins, v0mAmp_bins, nch_Nbins, nch_bins, pt_Nbins, pt_bins);

  hPtEtaPosvsNchEtaNegvsV0MAmp = new TH3D(
      "hPtEtaPosvsNchEtaNegvsV0MAmp",
      "; V0M Amplitude; #it{N}_{ch} (-0.8#leq#eta<0); #it{p}_{T} (GeV/#it{c})"
      "(0#geq#eta#leq0.8)",
      v0mAmp_Nbins, v0mAmp_bins, nch_Nbins, nch_bins, pt_Nbins, pt_bins);

  const int zdcN_Nbins{800};
  double zdcN_bins[zdcN_Nbins + 1] = {0.0};
  for (int i = 0; i <= zdcN_Nbins; ++i) {
    zdcN_bins[i] = 0.25 * i;
  }

  const int zdcN_sumNbins{1600};
  double zdcN_sumbins[zdcN_sumNbins + 1] = {0.0};
  for (int i = 0; i <= zdcN_sumNbins; ++i) {
    zdcN_sumbins[i] = 0.25 * i;
  }

  const int zdcP_Nbins{400};
  double zdcP_bins[zdcP_Nbins + 1] = {0.0};
  for (int i = 0; i <= zdcP_Nbins; ++i) {
    zdcP_bins[i] = 0.25 * i;
  }

  const int zdcP_sumNbins{800};
  double zdcP_sumbins[zdcP_sumNbins + 1] = {0.0};
  for (int i = 0; i <= zdcP_sumNbins; ++i) {
    zdcP_sumbins[i] = 0.25 * i;
  }

  hZNCvsZNA = new TH2D("hZNCvsZNA", ";ZNC signal (TeV); ZNA Signal (TeV)",
                       zdcN_Nbins, zdcN_bins, zdcN_Nbins, zdcN_bins);
  hZPCvsZPA = new TH2D("hZPCvsZPA", ";ZPC signal (TeV); ZPA Signal (TeV)",
                       zdcP_Nbins, zdcP_bins, zdcP_Nbins, zdcP_bins);
  hZEM = new TH2D("hZEM", ";ZEM1 signal (GeV); ZEM2 signal (GeV)", 200, 0, 1400,
                  200, 0, 1400);
  hZNvsNch = new TH2D("hZNvsNch", ";#it{N}_{ch}(|#eta|<0.8); ZN signal (TeV)",
                      nch_Nbins, nch_bins, zdcN_sumNbins, zdcN_sumbins);
  hZAvsNchHM =
      new TH2D("hZAvsNchHM", ";#it{N}_{ch}(|#eta|<0.8); ZA signal (TeV)",
               nch_Nbins, nch_bins, zdcN_Nbins, zdcN_bins);
  hZCvsNchHM =
      new TH2D("hZCvsNchHM", ";#it{N}_{ch}(|#eta|<0.8); ZC signal (TeV)",
               nch_Nbins, nch_bins, zdcN_Nbins, zdcN_bins);
  hZNvsNchHM =
      new TH2D("hZNvsNchHM", ";#it{N}_{ch}(|#eta|<0.8); ZN signal (TeV)",
               nch_Nbins, nch_bins, zdcN_sumNbins, zdcN_sumbins);
  hZNvsV0MAmp =
      new TH2D("hZNvsV0MAmp", ";ZN signal (TeV); V0M Amplitude", zdcN_sumNbins,
               zdcN_sumbins, v0mAmp_Nbins, v0mAmp_bins);
  hZNvsV0MAmpHM =
      new TH2D("hZNvsV0MAmpHM", ";ZN signal (TeV); V0M Amplitude",
               zdcN_sumNbins, zdcN_sumbins, v0mAmp_Nbins, v0mAmp_bins);
  hPtvsNchvsZAHM =
      new TH3D("hPtvsNchvsZAHM",
               ";#it{N}_{ch}^{rec}; ZA signal (TeV); #it{p}_{T} GeV/#it{c}",
               nch_Nbins, nch_bins, zdcN_Nbins, zdcN_bins, pt_Nbins, pt_bins);
  hPtvsNchvsZCHM =
      new TH3D("hPtvsNchvsZCHM",
               ";#it{N}_{ch}^{rec}; ZC signal (TeV); #it{p}_{T} GeV/#it{c}",
               nch_Nbins, nch_bins, zdcN_Nbins, zdcN_bins, pt_Nbins, pt_bins);
  hPtvsNchvsZNHM = new TH3D(
      "hPtvsNchvsZNHM",
      ";#it{N}_{ch}^{rec}; ZN signal (TeV); #it{p}_{T} GeV/#it{c}", nch_Nbins,
      nch_bins, zdcN_sumNbins, zdcN_sumbins, pt_Nbins, pt_bins);
  pPtvsZA = new TProfile(
      "pPtvsZA", "HM events;ZA signal (TeV); #LT#it{p}_{T}#GT GeV/#it{c}",
      zdcN_Nbins, zdcN_bins);
  pPtvsZC = new TProfile(
      "pPtvsZC", "HM events;ZC signal (TeV); #LT#it{p}_{T}#GT GeV/#it{c}",
      zdcN_Nbins, zdcN_bins);
  pPtvsZN = new TProfile(
      "pPtvsZN", "HM events;ZN signal (TeV); #LT#it{p}_{T}#GT GeV/#it{c}",
      zdcN_sumNbins, zdcN_sumbins);

  if (fUseZDC) {
    fOutputList->Add(hZNCvsZNA);
    fOutputList->Add(hZPCvsZPA);
    // fOutputList->Add(hZEM);
    fOutputList->Add(hZNvsNch);
    fOutputList->Add(hZAvsNchHM);
    fOutputList->Add(hZCvsNchHM);
    fOutputList->Add(hZNvsNchHM);
    fOutputList->Add(hZNvsV0MAmp);
    fOutputList->Add(hZNvsV0MAmpHM);
    fOutputList->Add(hPtvsNchvsZAHM);
    fOutputList->Add(hPtvsNchvsZCHM);
    fOutputList->Add(hPtvsNchvsZNHM);
    fOutputList->Add(pPtvsZA);
    fOutputList->Add(pPtvsZC);
    fOutputList->Add(pPtvsZN);
  }

  if (!fUseMC && !fUseZDC) {
    fOutputList->Add(hNchvsV0M);
    fOutputList->Add(hNchvsV0MAmp);
    fOutputList->Add(hV0MvsV0MAmp);
    fOutputList->Add(pV0MAmpChannel);
    fOutputList->Add(hV0MAmplitude);
    fOutputList->Add(pPtvsNch);
    fOutputList->Add(pPtEtaNegvsNchEtaPos);
    fOutputList->Add(pPtEtaPosvsNchEtaNeg);
    fOutputList->Add(pPtvsV0MAmp);
    fOutputList->Add(hPtvsNchvsV0MAmp);
    fOutputList->Add(hNchEtaPosvsNchEtaNeg);
    fOutputList->Add(hPtEtaNegvsNchEtaPosvsV0MAmp);
    fOutputList->Add(hPtEtaPosvsNchEtaNegvsV0MAmp);
  }

  hTrueVtxZ =
      new TH1F("hTrueVtxZ", ";z-vertex position;Entries", 200, -10.0, 10.0);

  hNchResponse =
      new TH2D("hNchResponse", ";#it{N}_{ch}^{rec}; #it{N}_{ch}^{true};",
               nch_Nbins, nch_bins, nch_Nbins, nch_bins);

  hTrueNchvsTrueV0MAmp = new TH3F(
      "hTrueNchvsTrueV0MAmpvsV0M",
      "; V0M Amplitude;#it{N}_{ch}^{true}; V0M (%)", v0mAmp_Nbins_true,
      v0mAmp_bins_true, nch_Nbins, nch_bins, v0m_Nbins080, v0m_bins080);

  hRecNchvsRecV0MAmp =
      new TH3F("hRecNchvsRecV0MAmpvsV0M",
               "; V0M Amplitude;#it{N}_{ch}^{rec};V0M (%)", v0mAmp_Nbins,
               v0mAmp_bins, nch_Nbins, nch_bins, v0m_Nbins080, v0m_bins080);

  hAllpTRec = new TH2F("hAllpTRec", ";V0M (%);#it{p}_{T}^{rec} GeV/#it{c}",
                       v0m_Nbins, v0m_bins, pt_Nbins, pt_bins);

  hAllpTTrue = new TH2F("hAllpTTrue", ";V0M (%);#it{p}_{T}^{true} GeV/#it{c}",
                        v0m_Nbins, v0m_bins, pt_Nbins, pt_bins);

  hPripTRec = new TH2F("hPripTRec", ";V0M (%);#it{p}_{T}^{rec} GeV/#it{c}",
                       v0m_Nbins, v0m_bins, pt_Nbins, pt_bins);

  hPripTTrue = new TH2F("hPripTTrue", ";V0M (%);#it{p}_{T}^{rec} GeV/#it{c}",
                        v0m_Nbins, v0m_bins, pt_Nbins, pt_bins);

  hTrueNchHM = new TH2F("hTrueNchHM", ";#it{N}_{ch}^{true}; V0M (%)", nch_Nbins,
                        nch_bins, v0m_Nbins, v0m_bins);

  hTrueNchHMWithTrigger =
      new TH2F("hTrueNchHMWithTrigger", ";#it{N}_{ch}^{true}; V0M (%)",
               nch_Nbins, nch_bins, v0m_Nbins, v0m_bins);

  hTrueNchHMWithEventCuts =
      new TH2F("hTrueNchHMWithEventCuts", ";#it{N}_{ch}^{true}; V0M (%)",
               nch_Nbins, nch_bins, v0m_Nbins, v0m_bins);

  hTrueNchHMWithVtxSel =
      new TH2F("hTrueNchHMWithVtxSel", ";#it{N}_{ch}^{true}; V0M (%)",
               nch_Nbins, nch_bins, v0m_Nbins, v0m_bins);

  hRecPtvsRecNchvsRecV0MAmp = new TH3D(
      "hRecPtvsRecNchvsRecV0MAmp",
      ";V0M Amplitude;#it{N}_{ch}^{rec};#it{p}_{T} GeV/#it{c}", v0mAmp_Nbins,
      v0mAmp_bins, nch_Nbins, nch_bins, pt_Nbins, pt_bins);

  hNchEtaPosvsNchEtaNeg_MCRec =
      new TH2D("hNchEtaPosvsNchEtaNeg_MCRec",
               ";#it{N}_{ch} (0#leq#eta#leq0.8); #it{N}_{ch} (-0.8#leq#eta<0)",
               nch_Nbins, nch_bins, nch_Nbins, nch_bins);

  hPtEtaNegvsNchEtaPosvsV0MAmp_MCRec = new TH3D(
      "hPtEtaNegvsNchEtaPosvsV0MAmp_MCRec",
      "; V0M Amplitude; #it{N}_{ch} (0#leq#eta#leq0.8); #it{p}_{T} "
      "(GeV/#it{c}) (-0.8#leq#eta<0)",
      v0mAmp_Nbins, v0mAmp_bins, nch_Nbins, nch_bins, pt_Nbins, pt_bins);

  hPtEtaPosvsNchEtaNegvsV0MAmp_MCRec = new TH3D(
      "hPtEtaPosvsNchEtaNegvsV0MAmp_MCRec",
      "; V0M Amplitude; #it{N}_{ch} (-0.8#leq#eta<0); #it{p}_{T} (GeV/#it{c}) "
      "(0#geq#eta#leq0.8)",
      v0mAmp_Nbins, v0mAmp_bins, nch_Nbins, nch_bins, pt_Nbins, pt_bins);

  hTruePtvsTrueNchvsTrueV0MAmp =
      new TH3D("hTruePtvsTrueNchvsTrueV0MAmp",
               ";V0M Amplitude;#it{N}_{ch}^{rec}; #it{p}_{T} GeV/#it{c}",
               v0mAmp_Nbins_true, v0mAmp_bins_true, nch_Nbins, nch_bins,
               pt_Nbins, pt_bins);

  hNchEtaPosvsNchEtaNeg_MCTrue =
      new TH2D("hNchEtaPosvsNchEtaNeg_MCTrue",
               ";#it{N}_{ch} (0#leq#eta#leq0.8); #it{N}_{ch} (-0.8#leq#eta<0)",
               nch_Nbins, nch_bins, nch_Nbins, nch_bins);

  hPtEtaNegvsNchEtaPosvsV0MAmp_MCTrue =
      new TH3D("hPtEtaNegvsNchEtaPosvsV0MAmp_MCTrue",
               "; V0M Amplitude; #it{N}_{ch} (0#leq#eta#leq0.8); #it{p}_{T} "
               "(GeV/#it{c}) (-0.8#leq#eta<0)",
               v0mAmp_Nbins_true, v0mAmp_bins_true, nch_Nbins, nch_bins,
               pt_Nbins, pt_bins);

  hPtEtaPosvsNchEtaNegvsV0MAmp_MCTrue =
      new TH3D("hPtEtaPosvsNchEtaNegvsV0MAmp_MCTrue",
               "; V0M Amplitude; #it{N}_{ch} (-0.8#leq#eta<0); #it{p}_{T} "
               "(GeV/#it{c}) (0#geq#eta#leq0.8)",
               v0mAmp_Nbins_true, v0mAmp_bins_true, nch_Nbins, nch_bins,
               pt_Nbins, pt_bins);

  hPtTruePrivsV0M =
      new TH2D("hPtTruePrivsV0M", "; V0M (%); #it{p}_{T}^{true} GeV/#it{c}",
               v0m_Nbins, v0m_bins, pt_Nbins, pt_bins);

  hPtRecPrivsV0M =
      new TH2D("hPtRecPrivsV0M", "; V0M (%); #it{p}_{T}^{rec} GeV/#it{c}",
               v0m_Nbins, v0m_bins, pt_Nbins, pt_bins);

  for (int i = 0; i < v0m_Nbins; ++i) {
    hDCAxyPri[i] = new TH2F(Form("hDCAxyPri_%s", uc_v0m_bins_name[i]),
                            ";DCA_{xy} (cm);#it{p}_{T} GeV/#it{c}", dcaxy_Nbins,
                            dcaxy_bins, pt_Nbins, pt_bins);
    hDCAxyWeDe[i] = new TH2F(Form("hDCAxyWeDe_%s", uc_v0m_bins_name[i]),
                             ";DCA_{xy} (cm);#it{p}_{T} GeV/#it{c}",
                             dcaxy_Nbins, dcaxy_bins, pt_Nbins, pt_bins);
    hDCAxyMaIn[i] = new TH2F(Form("hDCAxyMaIn_%s", uc_v0m_bins_name[i]),
                             ";DCA_{xy} (cm);#it{p}_{T} GeV/#it{c}",
                             dcaxy_Nbins, dcaxy_bins, pt_Nbins, pt_bins);
    hDCAxyData[i] = new TH2F(Form("hDCAxyData_%s", uc_v0m_bins_name[i]),
                             ";DCA_{xy} (cm);#it{p}_{T} GeV/#it{c}",
                             dcaxy_Nbins, dcaxy_bins, pt_Nbins, pt_bins);
  }

  if (fUseMC) {
    fOutputList->Add(hTrueVtxZ);
    fOutputList->Add(hNchResponse);
    fOutputList->Add(hPtTruePrivsV0M);
    fOutputList->Add(hPtRecPrivsV0M);
    fOutputList->Add(hTrueNchvsTrueV0MAmp);
    fOutputList->Add(hRecNchvsRecV0MAmp);
    fOutputList->Add(hAllpTRec);
    fOutputList->Add(hAllpTTrue);
    fOutputList->Add(hPripTRec);
    fOutputList->Add(hPripTTrue);
    fOutputList->Add(hTrueNchHM);
    fOutputList->Add(hTrueNchHMWithTrigger);
    fOutputList->Add(hTrueNchHMWithEventCuts);
    fOutputList->Add(hTrueNchHMWithVtxSel);
    fOutputList->Add(hRecPtvsRecNchvsRecV0MAmp);
    fOutputList->Add(hNchEtaPosvsNchEtaNeg_MCRec);
    fOutputList->Add(hPtEtaNegvsNchEtaPosvsV0MAmp_MCRec);
    fOutputList->Add(hPtEtaPosvsNchEtaNegvsV0MAmp_MCRec);
    fOutputList->Add(hTruePtvsTrueNchvsTrueV0MAmp);
    fOutputList->Add(hNchEtaPosvsNchEtaNeg_MCTrue);
    fOutputList->Add(hPtEtaNegvsNchEtaPosvsV0MAmp_MCTrue);
    fOutputList->Add(hPtEtaPosvsNchEtaNegvsV0MAmp_MCTrue);

    for (int i = 0; i < v0m_Nbins; ++i) {
      fOutputList->Add(hDCAxyPri[i]);
      fOutputList->Add(hDCAxyWeDe[i]);
      fOutputList->Add(hDCAxyMaIn[i]);
    }
  }

  if (!fUseZDC) {
    for (int i = 0; i < v0m_Nbins; ++i) {
      fOutputList->Add(hDCAxyData[i]);
    }
  }

  fEventCuts.AddQAplotsToList(fOutputList);
  PostData(1, fOutputList);  // postdata will notify the analysis manager of
                             // changes / updates to the
}
//_____________________________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::UserExec(Option_t*) {
  AliVEvent* event = InputEvent();
  if (!event) {
    Error("UserExec", "Could not retrieve event");
    return;
  }

  fESD = dynamic_cast<AliESDEvent*>(event);

  if (!fESD) {
    Printf("%s:%d ESDEvent not found in Input Manager", (char*)__FILE__,
           __LINE__);
    this->Dump();
    return;
  }

  if (fUseMC) {
    //      E S D
    fMC = dynamic_cast<AliMCEvent*>(MCEvent());
    if (!fMC) {
      Printf("%s:%d MCEvent not found in Input Manager", (char*)__FILE__,
             __LINE__);
      this->Dump();
      return;
    }
    fMCStack = fMC->Stack();
  }

  double random_number = -1.0;
  bool fill_corrections{false};
  gRandom->SetSeed(0);
  random_number = gRandom->Uniform(0.0, 1.0);
  // if random_number < 0.5 --> Multiplicity Distributions
  // if random_number >= 0.5 --> Detector Response & Corrections
  if (random_number >= 0.5) {
    fill_corrections = true;
  }

  ftrackmult08 = -999.0;
  fv0mpercentile = -999.0;
  fv0mamplitude = -999.0;

  fMultSelection = (AliMultSelection*)fESD->FindListObject("MultSelection");
  fv0mpercentile = fMultSelection->GetMultiplicityPercentile("V0M");
  // fv0mamplitude = fMultSelection->GetEstimator("V0M")->GetValue();
  ftrackmult08 = AliESDtrackCuts::GetReferenceMultiplicity(
      fESD, AliESDtrackCuts::kTrackletsITSTPC, 0.8);

  //! Analyze only the 0--80 % V0M range
  if (!(fv0mpercentile >= fV0Mmin && fv0mpercentile < 80.0)) {
    return;
  }

  bool isGoodVtxPosMC{false};
  int true_nch{0};
  int true_nch_v0{0};
  int true_nch_neg_eta{0};
  int true_nch_pos_eta{0};
  int rec_nch{0};
  int rec_nch_neg_eta{0};
  int rec_nch_pos_eta{0};
  std::vector<float> vec_true_pt{};
  std::vector<float> vec_true_pt_neg_eta{};
  std::vector<float> vec_true_pt_pos_eta{};
  std::vector<float> vec_rec_pt{};
  std::vector<float> vec_rec_pt_neg_eta{};
  std::vector<float> vec_rec_pt_pos_eta{};

  if (fUseMC) {
    AliHeader* headerMC = fMC->Header();
    AliGenEventHeader* genHeader = headerMC->GenEventHeader();
    TArrayF vtxMC(3);  // primary vertex  MC
    vtxMC[0] = 9999;
    vtxMC[1] = 9999;
    vtxMC[2] = 9999;  // initialize with dummy
    if (genHeader) {
      genHeader->PrimaryVertex(vtxMC);
    }
    if (TMath::Abs(vtxMC[2]) <= 10.0) {
      isGoodVtxPosMC = true;
    }
    // cout << "Vtx_z = " << TMath::Abs(vtxMC[2]) << '\n';
    float vtx_z{999};
    vtx_z = vtxMC[2];
    if (!isGoodVtxPosMC) {
      return;
    }

    hTrueVtxZ->Fill(vtx_z);

    AnalyzeMCevent(true_nch, true_nch_v0, true_nch_neg_eta, true_nch_pos_eta,
                   vec_true_pt, vec_true_pt_neg_eta, vec_true_pt_pos_eta);
    // printf("true_nch=%d true_nch_v0=%d\n", true_nch, true_nch_v0);
    if (true_nch <= 0 || true_nch_v0 <= 0) {
      return;
    }
    // Before trigger selection
    if (!fill_corrections) {
      TrueMultiplicityDistributions(true_nch, true_nch_v0, true_nch_neg_eta,
                                    true_nch_pos_eta, vec_true_pt,
                                    vec_true_pt_neg_eta, vec_true_pt_pos_eta);
    }

    if (fill_corrections) {
      hTrueNchHM->Fill(true_nch, fv0mpercentile);
    }
  }

  // Trigger selection
  // bool isINT7selected = fSelectMask & AliVEvent::kINT7;
  // if (!isINT7selected) {
  //   return;
  // }

  //! Trigger selection
  bool isEventTriggered{false};
  UInt_t fSelectMask = fInputHandler->IsEventSelected();
  isEventTriggered = fSelectMask & fTrigger;
  if (!isEventTriggered) {
    return;
  }

  if (fUseMC && fill_corrections) {
    hTrueNchHMWithTrigger->Fill(true_nch, fv0mpercentile);
  }

  // Good events
  if (!fEventCuts.AcceptEvent(event)) {
    PostData(1, fOutputList);
    return;
  }

  if (fUseMC && fill_corrections) {
    hTrueNchHMWithEventCuts->Fill(true_nch, fv0mpercentile);
  }

  // Good vertex
  bool hasRecVertex = false;
  hasRecVertex = HasRecVertex();
  if (!hasRecVertex) {
    return;
  }

  if (fUseMC && fill_corrections) {
    hTrueNchHMWithVtxSel->Fill(true_nch, fv0mpercentile);
  }

  //! Get calibrated V0 amplitude
  GetCalibratedV0Amplitude();

  AnalyzeRecEvent(rec_nch, rec_nch_neg_eta, rec_nch_pos_eta, vec_rec_pt,
                  vec_rec_pt_neg_eta, vec_rec_pt_pos_eta);

  //! DCAxy templates MC and Data
  DCAxyDistributions();
  hV0Mmult->Fill(fv0mpercentile);

  //! Data Multiplicity distributions
  MultiplicityDistributions(rec_nch, rec_nch_neg_eta, rec_nch_pos_eta,
                            vec_rec_pt, vec_rec_pt_neg_eta, vec_rec_pt_pos_eta);
  if (fUseZDC) {
    ZDC(rec_nch, vec_rec_pt);
  }
  if (fUseMC && isGoodVtxPosMC) {
    if (!fill_corrections) {
      RecMultiplicityDistributions(rec_nch, rec_nch_neg_eta, rec_nch_pos_eta,
                                   vec_rec_pt, vec_rec_pt_neg_eta,
                                   vec_rec_pt_pos_eta);
    } else {
      DetectorResponse(true_nch, rec_nch);
      TrackingEfficiency();
    }
  }

  PostData(1, fOutputList);
}

//______________________________________________________________________________

void AliAnalysisTaskDataSpeedOfSound::Terminate(Option_t*) {}

//______________________________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::ZDC(
    const int& rec_nch, const std::vector<float>& vec_rec_pt) const {
  AliESDZDC* esdZDC = fESD->GetESDZDC();
  if (!esdZDC) {
    return;
  }

  double zc = -1.0;
  double za = -1.0;
  double zn = -1.0;
  zc = esdZDC->GetZDCN1Energy();
  za = esdZDC->GetZDCN2Energy();
  zc = zc * 0.001;
  za = za * 0.001;
  zn = zc + za;

  hZNCvsZNA->Fill(zc, za);
  hZPCvsZPA->Fill(esdZDC->GetZDCP1Energy() * 0.001,
                  esdZDC->GetZDCP2Energy() * 0.001);
  hZEM->Fill(esdZDC->GetZDCEMEnergy(0), esdZDC->GetZDCEMEnergy(1));
  hZNvsNch->Fill(rec_nch, zn);
  hZNvsV0MAmp->Fill(zn, fv0mamplitude);

  if (fv0mpercentile <= fHMCut) {
    hZAvsNchHM->Fill(rec_nch, za);
    hZCvsNchHM->Fill(rec_nch, zc);
    hZNvsNchHM->Fill(rec_nch, zn);
    hZNvsV0MAmpHM->Fill(zn, fv0mamplitude);
    for (auto pt : vec_rec_pt) {
      hPtvsNchvsZCHM->Fill(rec_nch, zc, pt);
      hPtvsNchvsZAHM->Fill(rec_nch, za, pt);
      hPtvsNchvsZNHM->Fill(rec_nch, zn, pt);
      pPtvsZA->Fill(za, pt);
      pPtvsZC->Fill(zc, pt);
      pPtvsZN->Fill(zn, pt);
    }
  }
}

//______________________________________________________________________________

void AliAnalysisTaskDataSpeedOfSound::GetCalibratedV0Amplitude() {
  float mV0M{0.0};
  for (int i = 0; i < 64; i++) {
    mV0M += fESD->GetVZEROEqMultiplicity(i);
    pV0MAmpChannel->Fill(i, fESD->GetVZEROEqMultiplicity(i));
  }
  fv0mamplitude = mV0M;
}
//______________________________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::MultiplicityDistributions(
    const int& rec_nch, const int& rec_nch_neg_eta, const int& rec_nch_pos_eta,
    const std::vector<float>& vec_rec_pt,
    const std::vector<float>& vec_rec_pt_neg_eta,
    const std::vector<float>& vec_rec_pt_pos_eta) const {
  unsigned long n_rec_nch = (unsigned long)rec_nch;
  if (n_rec_nch != vec_rec_pt.size()) {
    cout << "Different rec_nch and elements in vec_rec_pt"
         << "nch= " << rec_nch << "nch from vector= " << vec_rec_pt.size()
         << '\n';
  }

  hV0MAmplitude->Fill(fv0mamplitude);
  hNchvsV0M->Fill(rec_nch, fv0mpercentile);
  hNchvsV0MAmp->Fill(rec_nch, fv0mamplitude);
  hV0MvsV0MAmp->Fill(fv0mamplitude, fv0mpercentile);

  for (auto pt : vec_rec_pt) {
    pPtvsNch->Fill(rec_nch, pt);
    pPtvsV0MAmp->Fill(fv0mamplitude, pt);
    hPtvsNchvsV0MAmp->Fill(fv0mamplitude, rec_nch, pt);
  }

  hNchEtaPosvsNchEtaNeg->Fill(rec_nch_pos_eta, rec_nch_neg_eta);

  for (auto pt : vec_rec_pt_neg_eta) {
    hPtEtaNegvsNchEtaPosvsV0MAmp->Fill(fv0mamplitude, rec_nch_pos_eta, pt);
    pPtEtaNegvsNchEtaPos->Fill(rec_nch_pos_eta, pt);
  }

  for (auto pt : vec_rec_pt_pos_eta) {
    hPtEtaPosvsNchEtaNegvsV0MAmp->Fill(fv0mamplitude, rec_nch_neg_eta, pt);
    pPtEtaPosvsNchEtaNeg->Fill(rec_nch_neg_eta, pt);
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::AnalyzeRecEvent(
    int& rec_nch, int& rec_nch_neg_eta, int& rec_nch_pos_eta,
    std::vector<float>& vec_rec_pt, std::vector<float>& vec_rec_pt_neg_eta,
    std::vector<float>& vec_rec_pt_pos_eta) const {
  rec_nch = 0;
  rec_nch_neg_eta = 0;
  rec_nch_pos_eta = 0;
  vec_rec_pt.clear();
  vec_rec_pt_neg_eta.clear();
  vec_rec_pt_pos_eta.clear();
  const int n_tracks{fESD->GetNumberOfTracks()};
  for (int i = 0; i < n_tracks; ++i) {
    AliESDtrack* track = static_cast<AliESDtrack*>(fESD->GetTrack(i));
    if (!track) {
      continue;
    }
    if (!fTrackFilter->IsSelected(track)) {
      continue;
    }
    if (track->Pt() < fPtMin) {
      continue;
    }
    if (track->Charge() == 0) {
      continue;
    }
    if (TMath::Abs(track->Eta()) > fEtaCut) {
      continue;
    }
    rec_nch++;
    vec_rec_pt.push_back(track->Pt());

    if (fEtaMin <= track->Eta() && track->Eta() < 0.0) {
      rec_nch_neg_eta++;
      vec_rec_pt_neg_eta.push_back(track->Pt());
    }
    if (track->Eta() >= 0.0 && track->Eta() <= fEtaMax) {
      rec_nch_pos_eta++;
      vec_rec_pt_pos_eta.push_back(track->Pt());
    }
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::DCAxyDistributions() const {
  int index{-1};
  for (int i = 0; i < v0m_Nbins; ++i) {
    if (fv0mpercentile >= uc_v0m_bins_low[i] &&
        fv0mpercentile < uc_v0m_bins_high[i]) {
      index = i;
      break;
    }
  }

  if (index < 0) {
    return;
  }

  const int n_tracks{fESD->GetNumberOfTracks()};
  if (fUseMC) {
    for (int i = 0; i < n_tracks; ++i) {
      AliESDtrack* track = static_cast<AliESDtrack*>(fESD->GetTrack(i));
      if (!track) {
        continue;
      }
      if (!fTrackFilterwoDCA->IsSelected(track)) {
        continue;
      }
      if (track->Pt() < fPtMin) {
        continue;
      }
      if (TMath::Abs(track->Eta()) > fEtaCut) {
        continue;
      }
      if (track->Charge() == 0) {
        continue;
      }

      float dcaxy = -999;
      float dcaz = -999;
      track->GetImpactParameters(dcaxy, dcaz);

      int label = -1;
      label = TMath::Abs(track->GetLabel());
      // TParticle* particle = fMC->GetTrack(label)->Particle();
      // if (!particle) {
      //   continue;
      // }
      if (fMC->IsPhysicalPrimary(label)) {
        hDCAxyPri[index]->Fill(dcaxy, track->Pt());
      } else if (fMC->IsSecondaryFromWeakDecay(label)) {
        hDCAxyWeDe[index]->Fill(dcaxy, track->Pt());
      } else if (fMC->IsSecondaryFromMaterial(label)) {
        hDCAxyMaIn[index]->Fill(dcaxy, track->Pt());
      } else {
        continue;
      }
    }
  }
  for (int i = 0; i < n_tracks; ++i) {
    AliESDtrack* track = static_cast<AliESDtrack*>(fESD->GetTrack(i));
    if (!track) {
      continue;
    }
    if (!fTrackFilterwoDCA->IsSelected(track)) {
      continue;
    }
    if (track->Pt() < fPtMin) {
      continue;
    }
    if (TMath::Abs(track->Eta()) > fEtaCut) {
      continue;
    }
    if (track->Charge() == 0) {
      continue;
    }

    float dcaxy = -999;
    float dcaz = -999;
    track->GetImpactParameters(dcaxy, dcaz);
    hDCAxyData[index]->Fill(dcaxy, track->Pt());
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::TrackingEfficiency() const {
  const int n_tracks{fESD->GetNumberOfTracks()};
  for (int i = 0; i < n_tracks; ++i) {
    AliESDtrack* track = static_cast<AliESDtrack*>(fESD->GetTrack(i));
    if (!track) {
      continue;
    }
    if (!fTrackFilter->IsSelected(track)) {
      continue;
    }
    if (track->Pt() < fPtMin) {
      continue;
    }
    if (TMath::Abs(track->Eta()) > fEtaCut) {
      continue;
    }
    if (track->Charge() == 0) {
      continue;
    }

    int label = -1;
    label = TMath::Abs(track->GetLabel());
    TParticle* particle = fMC->GetTrack(label)->Particle();
    if (!particle) {
      continue;
    }
    hAllpTRec->Fill(fv0mpercentile, track->Pt());
    hAllpTTrue->Fill(fv0mpercentile, particle->Pt());
    if (fMC->IsPhysicalPrimary(label)) {
      hPtRecPrivsV0M->Fill(fv0mpercentile, track->Pt());
      hPripTRec->Fill(fv0mpercentile, track->Pt());
      hPripTTrue->Fill(fv0mpercentile, particle->Pt());
    } else {
      continue;
    }
  }

  const int n_particles{fMC->GetNumberOfTracks()};
  for (int i = 0; i < n_particles; ++i) {
    AliMCParticle* particle = (AliMCParticle*)fMC->GetTrack(i);
    if (!particle) {
      continue;
    }
    if (AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(i, fMC)) {
      continue;
    }
    if (TMath::Abs(particle->Eta()) > fEtaCut) {
      continue;
    }
    if (particle->Pt() < fPtMin) {
      continue;
    }
    if (particle->Charge() == 0) {
      continue;
    }
    if (fMC->IsPhysicalPrimary(i)) {
      hPtTruePrivsV0M->Fill(fv0mpercentile, particle->Pt());
    } else {
      continue;
    }
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::AnalyzeMCevent(
    int& true_nch, int& true_nch_v0, int& true_nch_neg_eta,
    int& true_nch_pos_eta, std::vector<float>& vec_true_pt,
    std::vector<float>& vec_true_pt_neg_eta,
    std::vector<float>& vec_true_pt_pos_eta) const {
  true_nch = 0;
  true_nch_v0 = 0;
  true_nch_neg_eta = 0;
  true_nch_pos_eta = 0;
  vec_true_pt.clear();
  vec_true_pt_neg_eta.clear();
  vec_true_pt_pos_eta.clear();
  const int n_particles{fMC->GetNumberOfTracks()};
  for (int i = 0; i < n_particles; ++i) {
    AliMCParticle* particle = (AliMCParticle*)fMC->GetTrack(i);
    if (!particle) {
      continue;
    }
    if (AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(i, fMC)) {
      continue;
    }
    if (particle->Charge() == 0.0) {
      continue;
    }
    if ((2.8 < particle->Eta() && particle->Eta() < 5.1) ||
        (-3.7 < particle->Eta() && particle->Eta() < -1.7)) {
      if (fMC->IsPhysicalPrimary(i)) {
        true_nch_v0++;
      }
    }
    if (particle->Pt() < fPtMin) {
      continue;
    }
    if (TMath::Abs(particle->Eta()) > fEtaCut) {
      continue;
    }
    if (fMC->IsPhysicalPrimary(i)) {
      true_nch++;
      vec_true_pt.push_back(particle->Pt());

      if (fEtaMin <= particle->Eta() && particle->Eta() < 0.0) {
        true_nch_neg_eta++;
        vec_true_pt_neg_eta.push_back(particle->Pt());
      }
      if (particle->Eta() >= 0.0 && particle->Eta() <= fEtaMax) {
        true_nch_pos_eta++;
        vec_true_pt_pos_eta.push_back(particle->Pt());
      }
    }
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::DetectorResponse(
    const int& true_nch, const int& rec_nch) const {
  hNchResponse->Fill(rec_nch, true_nch);
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::RecMultiplicityDistributions(
    const int& rec_nch, const int& rec_nch_neg_eta, const int& rec_nch_pos_eta,
    const std::vector<float>& vec_rec_pt,
    const std::vector<float>& vec_rec_pt_neg_eta,
    const std::vector<float>& vec_rec_pt_pos_eta) const {
  unsigned long n_rec_nch = (unsigned long)rec_nch;
  if (n_rec_nch != vec_rec_pt.size()) {
    cout << "Different rec_nch and elements in vec_rec_pt"
         << "nch= " << rec_nch << "nch from vector= " << vec_rec_pt.size()
         << '\n';
  }

  hRecNchvsRecV0MAmp->Fill(fv0mamplitude, rec_nch, fv0mpercentile);

  for (auto pt : vec_rec_pt) {
    hRecPtvsRecNchvsRecV0MAmp->Fill(fv0mamplitude, rec_nch, pt);
  }

  hNchEtaPosvsNchEtaNeg_MCRec->Fill(rec_nch_pos_eta, rec_nch_neg_eta);

  for (auto pt : vec_rec_pt_neg_eta) {
    hPtEtaNegvsNchEtaPosvsV0MAmp_MCRec->Fill(fv0mamplitude, rec_nch_pos_eta,
                                             pt);
  }

  for (auto pt : vec_rec_pt_pos_eta) {
    hPtEtaPosvsNchEtaNegvsV0MAmp_MCRec->Fill(fv0mamplitude, rec_nch_neg_eta,
                                             pt);
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::TrueMultiplicityDistributions(
    const int& true_nch, const int& true_nch_v0, const int& true_nch_neg_eta,
    const int& true_nch_pos_eta, const std::vector<float>& vec_true_pt,
    const std::vector<float>& vec_true_pt_neg_eta,
    const std::vector<float>& vec_true_pt_pos_eta) const {
  unsigned long n_true_nch = (unsigned long)true_nch;
  if (n_true_nch != vec_true_pt.size()) {
    cout << "Different true_nch and elements in vec_true_pt"
         << "nch= " << true_nch << "nch from vector= " << vec_true_pt.size()
         << '\n';
  }

  hTrueNchvsTrueV0MAmp->Fill(true_nch_v0, true_nch, fv0mpercentile);
  for (auto pt : vec_true_pt) {
    hTruePtvsTrueNchvsTrueV0MAmp->Fill(true_nch_v0, true_nch, pt);
  }

  hNchEtaPosvsNchEtaNeg_MCTrue->Fill(true_nch_pos_eta, true_nch_neg_eta);

  for (auto pt : vec_true_pt_neg_eta) {
    hPtEtaNegvsNchEtaPosvsV0MAmp_MCTrue->Fill(true_nch_v0, true_nch_pos_eta,
                                              pt);
  }

  for (auto pt : vec_true_pt_pos_eta) {
    hPtEtaPosvsNchEtaNegvsV0MAmp_MCTrue->Fill(true_nch_v0, true_nch_neg_eta,
                                              pt);
  }
}
//____________________________________________________________
Bool_t AliAnalysisTaskDataSpeedOfSound::HasRecVertex() {
  float fMaxDeltaSpdTrackAbsolute = 0.5f;
  float fMaxDeltaSpdTrackNsigmaSPD = 1.e14f;
  float fMaxDeltaSpdTrackNsigmaTrack = 1.e14;
  float fMaxResolutionSPDvertex = 0.25f;
  float fMaxDispersionSPDvertex = 1.e14f;

  Bool_t fRequireTrackVertex = true;
  unsigned long fFlag;
  fFlag = BIT(AliEventCuts::kNoCuts);

  const AliVVertex* vtTrc = fESD->GetPrimaryVertex();
  bool isTrackV = true;
  if (vtTrc->IsFromVertexer3D() || vtTrc->IsFromVertexerZ()) isTrackV = false;
  const AliVVertex* vtSPD = fESD->GetPrimaryVertexSPD();

  if (vtSPD->GetNContributors() > 0) fFlag |= BIT(AliEventCuts::kVertexSPD);

  if (vtTrc->GetNContributors() > 1 && isTrackV)
    fFlag |= BIT(AliEventCuts::kVertexTracks);

  if (((fFlag & BIT(AliEventCuts::kVertexTracks)) || !fRequireTrackVertex) &&
      (fFlag & BIT(AliEventCuts::kVertexSPD)))
    fFlag |= BIT(AliEventCuts::kVertex);

  const AliVVertex*& vtx =
      bool(fFlag & BIT(AliEventCuts::kVertexTracks)) ? vtTrc : vtSPD;
  AliVVertex* fPrimaryVertex = const_cast<AliVVertex*>(vtx);
  if (!fPrimaryVertex) return kFALSE;

  /// Vertex quality cuts
  double covTrc[6], covSPD[6];
  vtTrc->GetCovarianceMatrix(covTrc);
  vtSPD->GetCovarianceMatrix(covSPD);
  double dz = bool(fFlag & AliEventCuts::kVertexSPD) &&
                      bool(fFlag & AliEventCuts::kVertexTracks)
                  ? vtTrc->GetZ() - vtSPD->GetZ()
                  : 0.;  /// If one of the two vertices is not available this
                         /// cut is always passed.
  double errTot = TMath::Sqrt(covTrc[5] + covSPD[5]);
  double errTrc =
      bool(fFlag & AliEventCuts::kVertexTracks) ? TMath::Sqrt(covTrc[5]) : 1.;
  double nsigTot = TMath::Abs(dz) / errTot, nsigTrc = TMath::Abs(dz) / errTrc;
  /// vertex dispersion for run1, only for ESD, AOD code to be added here
  const AliESDVertex* vtSPDESD = dynamic_cast<const AliESDVertex*>(vtSPD);
  double vtSPDdispersion = vtSPDESD ? vtSPDESD->GetDispersion() : 0;
  if ((TMath::Abs(dz) <= fMaxDeltaSpdTrackAbsolute &&
       nsigTot <= fMaxDeltaSpdTrackNsigmaSPD &&
       nsigTrc <=
           fMaxDeltaSpdTrackNsigmaTrack) &&  // discrepancy track-SPD vertex
      (!vtSPD->IsFromVertexerZ() ||
       TMath::Sqrt(covSPD[5]) <= fMaxResolutionSPDvertex) &&
      (!vtSPD->IsFromVertexerZ() ||
       vtSPDdispersion <= fMaxDispersionSPDvertex)  /// vertex dispersion cut
                                                    /// for run1, only for ESD
      )  // quality cut on vertexer SPD z
    fFlag |= BIT(AliEventCuts::kVertexQuality);

  Bool_t hasVtx = (TESTBIT(fFlag, AliEventCuts::kVertex)) &&
                  (TESTBIT(fFlag, AliEventCuts::kVertexQuality));

  return hasVtx;
}
