#include <map>
#include <string>
  
  //event variables
  int runId, eventId, nTracks, vtxNdf, vtxNtrFit;
  float vtxX, vtxY, vtxZ, vtxChi2, centrality49;
  bool isSimulation, vtxFitPerfect, targetIn;
  //track variables
  static const uint maxNtr=2000;
  float trPt[maxNtr], trEta[maxNtr], trPhi[maxNtr], trDcaX[maxNtr], trDcaY[maxNtr], trDcaZ[maxNtr], trChi2[maxNtr], trdEdx[maxNtr], trdEdxVTPC1[maxNtr], trdEdxVTPC2[maxNtr], trdEdxMTPC[maxNtr], trTofT[maxNtr], trTofL[maxNtr], trTofM2[maxNtr], trdEdxSigma[maxNtr];
  int trNdf[maxNtr], trCharge[maxNtr], trNclustVTPC1[maxNtr], trNclustVTPC2[maxNtr], trNclust[maxNtr], trNclustMTPC[maxNtr], trNclustPotVTPC1[maxNtr], trNclustPotVTPC2[maxNtr], trNclustPotMTPC[maxNtr], trNclustPot[maxNtr], trNdEdxClustVTPC1[maxNtr], trNdEdxClustVTPC2[maxNtr], trNdEdxClustMTPC[maxNtr], trNdEdxClust[maxNtr];
  int trNclustPotVTPC1_tr[maxNtr], trNclustPotVTPC2_tr[maxNtr], trNclustPotMTPC_tr[maxNtr], trNclustPot_tr[maxNtr];
  //psd variables
  static const uint maxNpsd=1000;
  int psdNmod, psdModId[maxNpsd];
  float psdPosX, psdPosY, psdPosZ, psdModX[maxNpsd], psdModY[maxNpsd], psdModZ[maxNpsd], psdModE[maxNpsd];
  float vetoEcal;
  //trigger variables
  static const int maxNtriggerHits=1000;
  int nTimesS1_1, nTimesT4, timeS1_1, timeT4;
  float adcS1, adcS2, adcS3, adcV1, adcV1p, adcPSD;
  //beam variables
  float bpd1x, bpd1y, bpd2x, bpd2y, bpd3x, bpd3y, bpd3xExtr, bpd3yExtr, beamPx, beamPy, beamPz;
  int beamStatus;
  bool bpd1good, bpd2good, bpd3good, bpdFitGood, bpdOutlier;
//  bool trigS1, trigS2, trigS3, trigV1, trigV1p, trigPSD; 
  bool trigT1, trigT2, trigT3, trigT4;
  //MC event variables
  int nSim;
//  int nPsdPrim;
  float psiRP, b, simVtxX, simVtxY, simVtxZ;
  //MC track variables
  static const uint maxNsim=5000;
  bool simIsSpect[maxNsim];
  int simNhitsVTPC1[maxNsim], simNhitsVTPC2[maxNsim], simNhitsMTPC[maxNsim], simPdg[maxNsim], simMotherId[maxNsim], trSimIndex[maxNtr], trSimNcommonPoints[maxNtr];
  float simPt[maxNsim], simEta[maxNsim], simPhi[maxNsim], psdModEsim[maxNpsd];
//  int psdPrimPdg[maxNsim];
//  float psdPrimX[maxNsim], psdPrimY[maxNsim], psdPrimZ[maxNsim], psdPrimPx[maxNsim], psdPrimPy[maxNsim], psdPrimPz[maxNsim], psdPrimE[maxNsim];
  std::vector<std::string> recOnlyBranchNames={
    "nTimesS1_1", "timeS1_1", "nTimesT4", "timeT4",
    "adcS1", "adcS2", "adcS3", "adcV1", "adcV1p", "adcPSD",
    "trigT1", "trigT2", "trigT3", "trigT4",
    "beamStatus", "bpd1good", "bpd2good", "bpd3good", "bpdFitGood", "bpdOutlier",
    "bpd1x", "bpd1y", "bpd2x", "bpd2y", "bpd3x", "bpd3y", "bpd3xExtr", "bpd3yExtr",
    "trTofT", "trTofL", "trTofM2", 
    "trdEdx", "trdEdxVTPC1", "trdEdxVTPC2", "trdEdxMTPC", "trdEdxSigma", 
    "trNdEdxClustVTPC1", "trNdEdxClustVTPC2", "trNdEdxClustMTPC", "trNdEdxClust"
  };
  std::vector<std::string> simOnlyBranchNames={
    "simIsSpect",
    "nSim", 
//    "nPsdPrim",
    "psiRP", "b", "simVtxX", "simVtxY", "simVtxZ",
    "simNhitsVTPC1", "simNhitsVTPC2", "simNhitsMTPC", "simPdg", "simMotherId", 
    "trSimIndex", "trSimNcommonPoints", 
    "simPt", "simEta", "simPhi", "psdModEsim", 
//    "psdPrimPdg", "psdPrimX", "psdPrimY", "psdPrimZ", "psdPrimPx", "psdPrimPy", "psdPrimPz", "psdPrimE"
  };
