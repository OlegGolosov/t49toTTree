#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <TFile.h>
#include <TMath.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include <TDatabasePDG.h>

#include "T49Run.h"
#include "T49EventRoot.h"
#include "T49EventMCRoot.h"
#include "T49VetoRoot.h"
#include "T49VertexRoot.h"
#include "T49VertexMCRoot.h"
#include "T49ParticleRoot.h"
#include "T49ParticleMCRoot.h"
#include "T49Vertex.h"
#include "T49Ring.h"

#include "productionMap.h"
#include "t49toTTree.h"

T49Run *run;

using namespace std;

void InitTree(TTree *tree);
void ReadEvent ();
void ClearEvent ();
void ReadMCEvent ();
int T49_to_DT(string inputFileList="list", string outputFileName = "tree.root", string productionTag="01D");

int main(int argc, char *argv[])
{
  TStopwatch timer;
  timer.Reset();
  timer.Start();

  switch (argc)
  {
    case 1:
    return T49_to_DT();
    case 2:
    return T49_to_DT(argv[1]);
    case 3:
    return T49_to_DT(argv[1], argv[2]);
    break;
    case 4:
    return T49_to_DT(argv[1], argv[2], argv[3]);
    break;
    break;
    default:
    cout << "Specify input filelist, output file name and production tag!" << endl;
    cout << argv[0] << " inputFileList outputFileName productionTag" << endl << endl;
    return 0;
  }

  timer.Stop();
  printf("Real time: %f\n",timer.RealTime());
  printf("CPU time: %f\n",timer.CpuTime());
  return 1;
}

int T49_to_DT(string inputFileList, string outputFileName, string productionTag)
{
  if (prodMap.count(productionTag) == 0)
    {
    cout << "There is no such production tag as " << productionTag << endl;
//    return 0;
    }

  string runType = prodMap [productionTag];
  if(runType.find("central")<runType.size())
    trigT4=false, trigT2=true;
  else
    trigT4=true, trigT2=false;

  cout << "inputFileList: " << inputFileList << endl;
  cout << "outputFileName: " << outputFileName << endl;

  cout << runType << endl;

  run = new T49Run();
  run->SetVerbose(0);

  ifstream inputFileListStream (inputFileList);
  if (!inputFileListStream) cout << inputFileList << " not found!\n";

  TFile* outputFile = new TFile((Char_t*) outputFileName.c_str(),"recreate");
  auto tree= new TTree("t","na49 data");
  InitTree(tree);
  string inputFileName;
  while (getline(inputFileListStream, inputFileName))
  {
    cout << "Opening file: " << inputFileName.c_str() << endl;
    if(!run -> Open ((Char_t*) (inputFileName).c_str())) continue;
    if (runType.find ("VENUS") != string::npos)
        cout << "isSimulation = true!!!\n";
    else 
      isSimulation=false;

    if (runType.find ("40") != string::npos)
    {
        cout << "Veto calibration for 40A GeV\n";
        beamPz=41;
        run->SetEvetoCal (3); // set 1 for other energies?, 0 for no time-dependent calibration
    }
    else if (runType.find ("160") != string::npos)
    {
        cout << "Veto calibration for 160A GeV\n"; // set 1 for other energies?, 0 for no time-dependent calibration
        beamPz=159;
        run->SetEvetoCal (2);
    }
    else
    {
        cout << "Veto calibration for energies other than 40A and 160A GeV\n"; // set 1 for other energies?, 0 for no time-dependent calibration
        run->SetEvetoCal (1);
    }
    run->SetEvetoCal (1); // patch
    run -> SetRunType ((Char_t*) (runType.c_str ()));
    auto event = (T49EventRoot*) run -> GetNextEvent ();
    run -> SetRunNumber (event -> GetNRun ());
    run -> CheckEvetoCal ();
    cout << "Run number: " << run -> GetRunNumber () << endl;
    cout << "Centrality of first event: " << event -> GetCentrality () << endl << endl;

    int nEvents = run -> GetMaxEvents();
    for (int nEvent = 0; nEvent < nEvents; nEvent++)
    {
        cout << "\rEvent " << nEvent;
        ClearEvent ();
        ReadEvent ();
        if (isSimulation) ReadMCEvent ();
        tree -> Fill ();
        run -> GetNextEvent ();
    }
    cout << endl;
    run -> Close ();
  }
  std::vector<std::string> toRemove;
  if(isSimulation)
    toRemove=recOnlyBranchNames;
  else if(!isSimulation)
    toRemove=simOnlyBranchNames;
  for(auto &name:toRemove) // watch out: for some reeason this produces corrupted MC trees
  {
    auto leaf=tree->GetListOfLeaves()->FindObject(name.c_str()); 
    auto branch=tree->GetListOfBranches()->FindObject(name.c_str());
    if(leaf) tree->GetListOfLeaves()->Remove(leaf);
    if(branch) tree->GetListOfBranches()->Remove(branch);
  }
  outputFile -> cd ();
  tree -> Write ();
  outputFile -> Close ();

  return 1;
}

void InitTree(TTree* tree) 
{
  //event
  tree->Branch("isSimulation", &isSimulation, "isSimulation/O");
  tree->Branch("targetIn", &targetIn, "targetIn/O");
  tree->Branch("runId", &runId, "runId/I");
  tree->Branch("eventId", &eventId, "eventId/I");
  tree->Branch("vtxNtrFit", &vtxNtrFit, "vtxNtrFit/I");
  tree->Branch("vtxNdf", &vtxNdf, "vtxNdf/I");
  tree->Branch("vtxChi2", &vtxChi2, "vtxChi2/F");
  tree->Branch("vtxX", &vtxX, "vtxX/F");
  tree->Branch("vtxY", &vtxY, "vtxY/F");
  tree->Branch("vtxZ", &vtxZ, "vtxZ/F");
  tree->Branch("centrality49", &centrality49, "centrality49/F");
  tree->Branch("vetoEcal", &vetoEcal, "vetoEcal/F");
  tree->Branch("vtxFitPerfect", &vtxFitPerfect, "vtxFitPerfect/O");
  //track
  tree->Branch("nTracks", &nTracks);
  tree->Branch("trPt", &trPt, "trPt[nTracks]/F");
  tree->Branch("trEta", &trEta, "trEta[nTracks]/F");
  tree->Branch("trPhi", &trPhi, "trPhi[nTracks]/F");
  tree->Branch("trDcaX", &trDcaX, "trDcaX[nTracks]/F");
  tree->Branch("trDcaY", &trDcaY, "trDcaY[nTracks]/F");
  tree->Branch("trDcaZ", &trDcaZ, "trDcaZ[nTracks]/F");
  tree->Branch("trChi2", &trChi2, "trChi2[nTracks]/F"); 
  tree->Branch("trNdf", &trNdf, "trNdf[nTracks]/I"); 
  tree->Branch("trCharge", &trCharge, "trCharge[nTracks]/I"); 
  tree->Branch("trNclustVTPC1", &trNclustVTPC1, "trNclustVTPC1[nTracks]/I"); 
  tree->Branch("trNclustVTPC2", &trNclustVTPC2, "trNclustVTPC2[nTracks]/I"); 
  tree->Branch("trNclustMTPC", &trNclustMTPC, "trNclustMTPC[nTracks]/I"); 
  tree->Branch("trNclust", &trNclust, "trNclust[nTracks]/I"); 
  tree->Branch("trNclustPotVTPC1", &trNclustPotVTPC1, "trNclustPotVTPC1[nTracks]/I"); 
  tree->Branch("trNclustPotVTPC2", &trNclustPotVTPC2, "trNclustPotVTPC2[nTracks]/I"); 
  tree->Branch("trNclustPotMTPC", &trNclustPotMTPC, "trNclustPotMTPC[nTracks]/I"); 
  tree->Branch("trNclustPot", &trNclustPot, "trNclustPot[nTracks]/I"); 
  tree->Branch("trNdEdxClustVTPC1", &trNdEdxClustVTPC1, "trNdEdxClustVTPC1[nTracks]/I"); 
  tree->Branch("trNdEdxClustVTPC2", &trNdEdxClustVTPC2, "trNdEdxClustVTPC2[nTracks]/I"); 
  tree->Branch("trNdEdxClustMTPC", &trNdEdxClustMTPC, "trNdEdxClustMTPC[nTracks]/I"); 
  tree->Branch("trNdEdxClust", &trNdEdxClust, "trNdEdxClust[nTracks]/I"); 
  tree->Branch("trdEdxVTPC1", &trdEdxVTPC1, "trdEdxVTPC1[nTracks]/F"); 
  tree->Branch("trdEdxVTPC2", &trdEdxVTPC2, "trdEdxVTPC2[nTracks]/F"); 
  tree->Branch("trdEdxMTPC", &trdEdxMTPC, "trdEdxMTPC[nTracks]/F"); 
  tree->Branch("trdEdx", &trdEdx, "trdEdx[nTracks]/F"); 
  tree->Branch("trdEdxSigma", &trdEdxSigma, "trdEdxSigma[nTracks]/F"); 
  tree->Branch("trTofT", &trTofT, "trTofT[nTracks]/F");
  tree->Branch("trTofL", &trTofL, "trTofL[nTracks]/F");
  tree->Branch("trTofM2", &trTofM2, "trTofM2[nTracks]/F");
  //psd
  tree->Branch("psdPosX", &psdPosX, "psdPosX/F"); 
  tree->Branch("psdPosY", &psdPosY, "psdPosY/F"); 
  tree->Branch("psdPosZ", &psdPosZ, "psdPosZ/F"); 
  tree->Branch("psdNmod", &psdNmod, "psdNmod/I"); 
  tree->Branch("psdModId", &psdModId, "psdModId[psdNmod]/I"); 
  tree->Branch("psdModX", &psdModX, "psdModX[psdNmod]/F"); 
  tree->Branch("psdModY", &psdModY, "psdModY[psdNmod]/F"); 
  tree->Branch("psdModZ", &psdModZ, "psdModZ[psdNmod]/F"); 
  tree->Branch("psdModE", &psdModE, "psdModE[psdNmod]/F"); 
  //beam
  tree->Branch("bpd1x", &bpd1x, "bpd1x/F");
  tree->Branch("bpd1y", &bpd1y, "bpd1y/F");
  tree->Branch("bpd2x", &bpd2x, "bpd2x/F");
  tree->Branch("bpd2y", &bpd2y, "bpd2y/F");
  tree->Branch("bpd3x", &bpd3x, "bpd3x/F");
  tree->Branch("bpd3y", &bpd3y, "bpd3y/F");
  tree->Branch("bpd3xExtr", &bpd3xExtr, "bpd3xExtr/F");
  tree->Branch("bpd3yExtr", &bpd3yExtr, "bpd3yExtr/F");
  tree->Branch("beamStatus", &beamStatus, "beamStatus/I");
  tree->Branch("bpd1good", &bpd1good, "bpd1good/O");
  tree->Branch("bpd2good", &bpd2good, "bpd2good/O");
  tree->Branch("bpd3good", &bpd3good, "bpd3good/O");
  tree->Branch("bpdFitGood", &bpdFitGood, "bpdFitGood/O");
  tree->Branch("bpdOutlier", &bpdOutlier, "bpdOutlier/O");
  tree->Branch("beamPx", &beamPx, "beamPx/F");
  tree->Branch("beamPy", &beamPy, "beamPy/F");
  tree->Branch("beamPz", &beamPz, "beamPz/F");
  //triggers
  tree->Branch("nTimesS1_1", &nTimesS1_1, "nTimesS1_1/I");
  tree->Branch("nTimesT4", &nTimesT4, "nTimesT4/I");
  tree->Branch("trigT1", &trigT1, "trigT1/O");
  tree->Branch("trigT2", &trigT2, "trigT2/O");
  tree->Branch("trigT3", &trigT3, "trigT3/O");
  tree->Branch("trigT4", &trigT4, "trigT4/O");
  tree->Branch("adcS1", &adcS1, "adcS1/F");
  tree->Branch("adcS2", &adcS2, "adcS2/F");
  tree->Branch("adcS3", &adcS3, "adcS3/F");
  tree->Branch("adcV1", &adcV1, "adcV1/F");
  tree->Branch("adcV1p", &adcV1p, "adcV1p/F");
  tree->Branch("adcPSD", &adcPSD, "adcPSD/F");
  tree->Branch("timeS1_1", &timeS1_1, "timeS1_1/I");
  tree->Branch("timeT4", &timeT4, "timeT4/I");
  // sim
  tree->Branch("nSim", &nSim, "nSim/I");
  tree->Branch("psiRP", &psiRP, "psiRP/F");
  tree->Branch("b", &b, "b/F");
  tree->Branch("simVtxX", &simVtxX, "simVtxX/F");
  tree->Branch("simVtxY", &simVtxY, "simVtxY/F");
  tree->Branch("simVtxZ", &simVtxZ, "simVtxZ/F");
  tree->Branch("simIsSpect", &simIsSpect, "simIsSpect[nSim]/O");
  tree->Branch("simNhitsVTPC1", &simNhitsVTPC1, "simNhitsVTPC1[nSim]/I");
  tree->Branch("simNhitsVTPC2", &simNhitsVTPC2, "simNhitsVTPC2[nSim]/I");
  tree->Branch("simNhitsMTPC", &simNhitsMTPC, "simNhitsMTPC[nSim]/I");
  tree->Branch("simPdg", &simPdg, "simPdg[nSim]/I");
  tree->Branch("simMotherId", &simMotherId, "simMotherId[nSim]/I");
  tree->Branch("trSimIndex", &trSimIndex, "trSimIndex[nTracks]/I");
  tree->Branch("trSimNcommonPoints", &trSimNcommonPoints, "trSimNcommonPoints[nTracks]/I");
  tree->Branch("simPt", &simPt, "simPt[nSim]/F");
  tree->Branch("simEta", &simEta, "simEta[nSim]/F");
  tree->Branch("simPhi", &simPhi, "simPhi[nSim]/F");
  tree->Branch("psdModEsim", &psdModEsim, "psdModEsim[psdNmod]/F");
}

void ClearEvent() {
  nTracks=0;
  nSim=0;
  targetIn=true;
  // clearing indexes
//  fVertexTrackIndexMap.clear();
//  fVertexTrackIndexMapRev.clear();
//  fTrackVertexTrackIndexMap.clear();
//  fMCTrackIndexMap.clear();
//  fMCTrackIndexMapRev.clear();
}

void ReadEvent ()
{
    auto event = (T49EventRoot*) run -> GetEvent ();

    runId=event->GetNRun();
    eventId=event->GetNEvent();
    beamPx=0, beamPy=0;
    vtxX=event->GetVertexX();
    vtxY=event->GetVertexY();
    vtxZ=event->GetVertexZ();
    auto vertex = (T49VertexRoot*)event -> GetPrimaryVertex();
    vtxNtrFit=vertex->GetNTrkFit();
    vtxChi2=vertex->GetPchi2();
    vtxFitPerfect=(vertex->GetIflag()==0);
    psdPosX=0.;
    psdPosY=0.;
    psdPosZ=1800.;
    vetoEcal=event->GetTDCalEveto();
    centrality49=event->GetCentrality();

    //fTriggerMask =(event->GetTriggerMask()); // Trigger Mask
    nTimesS1_1=event->GetWfaNbeam();
    timeS1_1=event->GetWfaBeamTime();
    nTimesT4=event->GetWfaNinter();
    timeT4=event->GetWfaInterTime();
    auto beam=(T49BeamRoot*)event->GetBeam();
    adcS1=beam->GetAdcS1();
    adcS2=beam->GetAdcS2();
    adcS3=beam->GetAdcS3();

    psdNmod=0;
    float vetoModuleAngle [4] = {1.25, 1.75, 0.25, 0.75};
    auto vetoModuleCenter=25*sqrt(2);
    auto veto = (T49VetoRoot*)event->GetVeto();
    for (unsigned int iVeto=0; iVeto<4; ++iVeto)
    {
        psdModX[iVeto]=vetoModuleCenter*cos(vetoModuleAngle[iVeto]*TMath::Pi());
        psdModY[iVeto]=vetoModuleCenter*sin(vetoModuleAngle[iVeto]*TMath::Pi());
        psdModZ[iVeto]=2600.;
        psdModE[iVeto]=veto->GetADChadron(iVeto); // +veto->GetADCphoton(iVeto);
        if(psdModE[iVeto]<0.)psdModE[iVeto]=0;
        psdModId[iVeto]=iVeto+1;
        psdNmod++;
    }

    auto ring = (T49RingRoot *)event->GetRing();
    float ring_middle_radius[10]={30.5,36,42.5,50.2,59.4,70.3,83,98.1,116,137};

//    for (unsigned int iRing=0; iRing<240; ++iRing)
//    {
//        fDTEvent -> AddPSDModule(0);
////        fDTEvent -> GetLastPSDModule() -> SetPosition( cos( (iRing+0.5)*TMath::Pi()/12 ), sin( (iRing+0.5)*TMath::Pi()/12 ), 1800 );
//        fDTEvent -> GetLastPSDModule() -> SetPosition(-ring_middle_radius[iRing%10]*sin(((iRing/10)+0.5)*TMath::Pi()/12),
//                                                       ring_middle_radius[iRing%10]*cos(((iRing/10)+0.5)*TMath::Pi()/12), 1800);
//        fDTEvent -> GetLastPSDModule() -> SetEnergy(ring->GetADChadron(iRing));
//    }

    for (unsigned int iRing=0; iRing<240; ++iRing)
    {
        int modId = 4 + iRing / 10 + iRing % 10 * 24;
        psdModX[modId]=-ring_middle_radius [iRing%10] * sin (((iRing/10)+0.5) * TMath::Pi() / 12.);
        psdModY[modId]=ring_middle_radius [iRing%10] * cos (((iRing/10)+0.5) * TMath::Pi() / 12.);
        psdModZ[modId]=1800;
        psdModE[modId]=ring->GetADChadron(iRing); //+ring->GetADCphoton(iRing);
        if(psdModE[modId]<0.)psdModE[modId] = 0;
        psdModId[modId]=modId+1;
        psdNmod++;
    }

    auto particles = (TObjArray*) event -> GetPrimaryParticles();
    TIter particles_iter(particles);
    for (int iTrack=0; auto particle = (T49ParticleRoot*) particles_iter.Next(); iTrack++)
    {
        trPt[iTrack]=particle->GetPt();
        trEta[iTrack]=particle->GetEta();
        trPhi[iTrack]=particle->GetPhi();
        if(trPhi[iTrack]>2*TMath::Pi()) trPhi[iTrack]-=2*TMath::Pi();

        trDcaX[iTrack]=particle->GetBx();
        trDcaY[iTrack]=particle->GetBy();
        trDcaZ[iTrack]=0;
        trChi2[iTrack]=particle->GetPchi2();
        trNdf[iTrack]=1;
        trCharge[iTrack]=particle->GetCharge();

        trNclustVTPC1[iTrack]=particle->GetNFitPoint(0);
        trNclustVTPC2[iTrack]=particle->GetNFitPoint(1);
        trNclustMTPC[iTrack]=particle->GetNFitPoint(2);
        trNclust[iTrack]=particle->GetNFitPoint();

        trNclustPotVTPC1[iTrack]=particle->GetNMaxPoint(0);
        trNclustPotVTPC2[iTrack]=particle->GetNMaxPoint(1);
        trNclustPotMTPC[iTrack]=particle->GetNMaxPoint(2);
        trNclustPot[iTrack]=particle->GetNMaxPoint();

        trdEdxVTPC1[iTrack]=0.001 * particle->GetTmeanCharge(0);
        trdEdxVTPC2[iTrack]=0.001 * particle->GetTmeanCharge(1);
        trdEdxMTPC[iTrack]=0.001 * particle->GetTmeanCharge(2);
        trdEdx[iTrack]=0.001 * particle->GetTmeanCharge();

        trNdEdxClustVTPC1[iTrack]=particle-> GetNDedxPoint(0);
        trNdEdxClustVTPC2[iTrack]=particle-> GetNDedxPoint(1);
        trNdEdxClustMTPC[iTrack]=particle-> GetNDedxPoint(2);
        trNdEdxClust[iTrack]=particle-> GetNDedxPoint();

        trTofT[iTrack]=0;
        trTofL[iTrack]=particle->GetTofPathl();
        if(trTofL[iTrack]>0)
          trTofM2[iTrack]=particle->GetTofMass2();
        else
          trTofM2[iTrack]=-999;

        /*
        fPrimaryParticles_fIdDet[k] =(particle->GetIdDet());
              for(unsigned int j=0;j<3;j++)
              {
                fPrimaryParticles_fXFirst[k][j]=(particle->GetXFirst(j));
                fPrimaryParticles_fYFirst[k][j]=(particle->GetYFirst(j));
                fPrimaryParticles_fZFirst[k][j]=(particle->GetZFirst(j));
                fPrimaryParticles_fXLast[k][j] =(particle->GetXLast(j));
                fPrimaryParticles_fYLast[k][j] =(particle->GetYLast(j));
                fPrimaryParticles_fZLast[k][j] =(particle->GetZLast(j));
              }
          fPrimaryParticles_fXFirst[k][3]=(particle->GetXFirst());
          fPrimaryParticles_fYFirst[k][3]=(particle->GetYFirst());
          fPrimaryParticles_fZFirst[k][3]=(particle->GetZFirst());
          fPrimaryParticles_fXLast[k][3] =(particle->GetXLast());
          fPrimaryParticles_fYLast[k][3] =(particle->GetYLast());
          fPrimaryParticles_fZLast[k][3] =(particle->GetZLast());
          fPrimaryParticles_fLabel[k]=(particle->GetLabel());
          fPrimaryParticles_fTofIflag[k]= (particle->GetTofIflag());
          fPrimaryParticles_fTofId[k]=(particle->GetTofId());
          fPrimaryParticles_fIflag[k]=(particle->GetIflag());
          fPrimaryParticles_fSigPx[k]=(particle->GetSigPx());
          fPrimaryParticles_fSigPy[k]=(particle->GetSigPy());
          */
        nTracks++;
    }
}

void ReadMCEvent ()
{
    auto MCevent = (T49EventMCRoot*) run -> GetEvent ();
    auto MCvertex = (T49VertexMCRoot*) (MCevent -> GetVertices () -> At (0));
    simVtxX=MCvertex->GetX();
    simVtxY=MCvertex->GetY();
    simVtxZ=MCvertex->GetZ();
    eventId=run -> GetRunNumber();

    auto MCparticles = (TObjArray*) MCevent -> GetMCParticles();
    TIter MCparticles_iter (MCparticles);
    int localIndex=0;
    for (int iTrack=0; auto MCparticle = (T49ParticleMCRoot*) MCparticles_iter.Next(); iTrack++)
    {
        if (MCparticle -> GetStartVertex() != 0) continue;
        simPt[localIndex]=MCparticle -> GetPt();
        simEta[localIndex]=MCparticle -> GetEta();
        simPhi[localIndex]=MCparticle -> GetPhi();
        simPdg[localIndex]=TDatabasePDG::Instance()->ConvertGeant3ToPdg(MCparticle -> GetPid());
        simNhitsVTPC1[localIndex]=MCparticle->GetNPoint(0);
        simNhitsVTPC2[localIndex]=MCparticle->GetNPoint(1);
        simNhitsMTPC[localIndex]=MCparticle->GetNPoint(2);
        simMotherId[localIndex]=-1;

        for (int iMatch = 0; iMatch < MCparticle -> GetNPriMatched (); iMatch++)
        {
            auto matchedIndex=MCparticle -> GetPriMatched (iMatch);
            trSimIndex[matchedIndex]=localIndex; 
            trSimNcommonPoints[matchedIndex] = MCparticle -> GetNPriMatchPoint (iMatch);
            auto trackPurity = (double) MCparticle -> GetNPriMatchPoint (iMatch) / MCparticle -> GetNPoint ();
        }
        localIndex++;
    }
}
