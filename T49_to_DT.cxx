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
#include <TLorentzVector.h>

#include "T49Run.h"
#include "T49EventRoot.h"
#include "T49VetoRoot.h"
#include "T49VertexRoot.h"
#include "T49ParticleRoot.h"
#include "T49Vertex.h"
#include "T49Ring.h"

#include "DataTreeEvent.h"
#include "productionMap.h"

using namespace std;

//string input_path = "/afs/cern.ch/work/o/ogolosov/public/NA49/DST";
//string input_path = "/eos/user/o/ogolosov/NA49_data/DST";
//string output_path = "/afs/cern.ch/work/o/ogolosov/public/NA49/DT";
string input_path = "/home/ogolosov/Desktop/Analysis/NA49/DST";
string output_path = "/home/ogolosov/Desktop/Analysis/NA49/DT";

int T49_to_DT(string productionTag, int maxFileNumber = 10000)
{
  TStopwatch timer;
  timer.Reset();
  timer.Start();

  string runType;
  if (prodMap.count(productionTag) == 0)
    {
    cout << "There is no such production tag as " << productionTag << endl;
    return 0;
    }

  input_path = input_path + "/" + productionTag;
  output_path = output_path + "/" + productionTag;
  string command = "mkdir -p " + output_path;
  gSystem -> Exec (command.c_str ());
  cout << "input path: " << input_path << endl;
  cout << "output path: " << output_path << endl;

  runType = prodMap [productionTag];
  cout << runType << endl;

  string inputFileList = input_path + "/list.txt";

  TTree* myTree  = new TTree("DataTree", "NA49 Pb-Pb DataTree");
  DataTreeEvent *fDTEvent = new DataTreeEvent();
  myTree -> Branch("DTEvent", "DataTree", fDTEvent);

  T49Run *run = new T49Run();
  run->SetVerbose(0);

  T49EventRoot *event;
  T49ParticleRoot *particle;
  T49RingRoot *ring;
  T49VertexRoot *vertex;
  T49VetoRoot *veto;
  TObjArray *particles;
  bool runNumberIsSet = false;
  int fileNumber = 0;
  int nEvent = 0;

  ifstream inputFileListStream (inputFileList);
  if (!inputFileListStream) cout << "list.txt not found!\n";
  string inputFileName;
  while (getline(inputFileListStream, inputFileName) && fileNumber < maxFileNumber)
  {
  	string outputFileName = inputFileName.substr(1,inputFileName.size());
  	outputFileName = output_path + "/DT" + outputFileName;
  	inputFileName = input_path + "/" + inputFileName;

    TFile* outputFile = new TFile((Char_t*) outputFileName.c_str(),"recreate");
    TTree* tree = myTree->CloneTree();
    fileNumber++;
    cout << "Opening file " << fileNumber << ": " << inputFileName.c_str() << endl;
    if(!run -> Open((Char_t*) (inputFileName).c_str())) continue;
    runNumberIsSet = false;
    while( event = (T49EventRoot*)run -> GetNextEvent() )
    {
        if (runNumberIsSet==false)
        {
          if (runType.find ("40") != string::npos)
          {
              cout << "Veto calibration for 40A GeV\n";
              run->SetEvetoCal(1);
          }
          else
          {
              cout << "Veto calibration for 160A GeV\n";
              run->SetEvetoCal(2);
          }
          run->SetRunType((Char_t*)(runType.c_str ()));
          run->SetRunNumber(event->GetNRun());
          run->CheckEvetoCal();
          runNumberIsSet = true;
          cout << "Run number: " << run->GetRunNumber() << endl;
          cout << "Centrality of first event: " << event->GetCentrality() << endl << endl;
        }

        cout << "\rEvent " << nEvent++;

        fDTEvent -> ClearEvent();

        fDTEvent -> SetRunId( event->GetNRun() );
        fDTEvent -> SetEventId( event->GetNEvent() );

        vertex = ((T49VertexRoot*)event -> GetPrimaryVertex());

        fDTEvent -> SetVertexPosition(event->GetVertexX(), event->GetVertexY(), event->GetVertexZ(), EnumVertexType::kReconstructedVertex);
        fDTEvent -> SetVertexQuality( vertex->GetPchi2() );

//      fTriggerMask =(event->GetTriggerMask()); // Trigger Mask
//      fDate=(event->GetDate()); // Event date
//      fTime =(event->GetTime()); // Event time
//      fWfaNbeam=(event->GetWfaNbeam()); // returns the number of the beam particles (including the interacting one) arriving in the ±25µs around the interaction time
//      fWfaNinter=(event->GetWfaNinter()); // returns the number of the interacting particles (including the one that triggered the event) arriving in the ±25µs around the interaction time
//      fWfaBeamTime=(event->GetWfaBeamTime() ); // returns the array containing the times in nanoseconds for all beam particles in the ±25µs around the interaction time. The length of this array is given by the method above
//      fWfaInterTime=(event->GetWfaInterTime()); //returns the array containing the times in nanoseconds for all interacting particles in the ±25µs around the interaction time. The length of this array is given by the method above

        fDTEvent->SetPsdPosition(0., 0., 0.);
        fDTEvent->SetPsdEnergy(event->GetEveto());

    veto = (T49VetoRoot*)event->GetVeto();
    for (unsigned int iVeto=0; iVeto<4; ++iVeto)
    {
        fDTEvent -> AddPSDModule(0);
        fDTEvent -> GetLastPSDModule() -> SetPosition( cos( (iVeto+0.5)*TMath::Pi()/2 ), sin( (iVeto+0.5)*TMath::Pi()/2 ), 2500 );
        fDTEvent -> GetLastPSDModule() -> SetEnergy(veto->GetADChadron(iVeto));
    }

    ring = (T49RingRoot *)event->GetRing();

    double ring_middle_radius[10]={30.5,36,42.5,50.2,59.4,70.3,83,98.1,116,137};

    for (unsigned int iRing=0; iRing<240; ++iRing)
    {
        fDTEvent -> AddPSDModule(0);
//        fDTEvent -> GetLastPSDModule() -> SetPosition( cos( (iRing+0.5)*TMath::Pi()/12 ), sin( (iRing+0.5)*TMath::Pi()/12 ), 1800 );
        fDTEvent -> GetLastPSDModule() -> SetPosition(-ring_middle_radius[iRing%10]*sin(((iRing/10)+0.5)*TMath::Pi()/12),
                                                       ring_middle_radius[iRing%10]*cos(((iRing/10)+0.5)*TMath::Pi()/12), 1800);
        fDTEvent -> GetLastPSDModule() -> SetEnergy(ring->GetADChadron(iRing));
    }

      int iTrack=0;
      particles = (TObjArray*)event -> GetPrimaryParticles();
      TIter particles_iter(particles);

      TLorentzVector TrackPar;

      while( particle = (T49ParticleRoot*) particles_iter.Next())
      {
        fDTEvent -> AddTrack();
        fDTEvent -> GetLastTrack() -> SetId(iTrack);
        TrackPar.SetPtEtaPhiM (particle->GetPt(), particle->GetEta(), particle->GetPhi(), 0.14);
        fDTEvent -> GetLastTrack() -> SetMomentum(TrackPar);

        fDTEvent -> GetLastTrack() -> SetDCA(particle->GetBx(), particle->GetBy(), 0);
        fDTEvent -> GetLastTrack() -> SetChi2(particle->GetPchi2());
        fDTEvent -> GetLastTrack() -> SetNDF(1);
        fDTEvent -> GetLastTrack() -> SetCharge(particle->GetCharge());

        fDTEvent -> GetLastTrack() -> SetNumberOfHits(particle->GetNPoint(0), EnumTPC::kVTPC1);
        fDTEvent -> GetLastTrack() -> SetNumberOfHits(particle->GetNPoint(1), EnumTPC::kVTPC2);
        fDTEvent -> GetLastTrack() -> SetNumberOfHits(particle->GetNPoint(2), EnumTPC::kMTPC);
        fDTEvent -> GetLastTrack() -> SetNumberOfHits(particle->GetNPoint(), EnumTPC::kTPCAll);

        fDTEvent -> GetLastTrack() -> SetNumberOfHitsPotential(particle->GetNMaxPoint(0), EnumTPC::kVTPC1);
        fDTEvent -> GetLastTrack() -> SetNumberOfHitsPotential(particle->GetNMaxPoint(1), EnumTPC::kVTPC2);
        fDTEvent -> GetLastTrack() -> SetNumberOfHitsPotential(particle->GetNMaxPoint(2), EnumTPC::kMTPC);
        fDTEvent -> GetLastTrack() -> SetNumberOfHitsPotential(particle->GetNMaxPoint(), EnumTPC::kTPCAll);

        fDTEvent -> GetLastTrack() -> SetNumberOfHitsFit(particle->GetNFitPoint(0), EnumTPC::kVTPC1);
        fDTEvent -> GetLastTrack() -> SetNumberOfHitsFit(particle->GetNFitPoint(1), EnumTPC::kVTPC2);
        fDTEvent -> GetLastTrack() -> SetNumberOfHitsFit(particle->GetNFitPoint(2), EnumTPC::kMTPC);
        fDTEvent -> GetLastTrack() -> SetNumberOfHitsFit(particle->GetNFitPoint(), EnumTPC::kTPCAll);

        fDTEvent -> GetLastTrack() -> SetdEdx(particle->GetTmeanCharge(0), EnumTPC::kVTPC1);
        fDTEvent -> GetLastTrack() -> SetdEdx(particle->GetTmeanCharge(1), EnumTPC::kVTPC2);
        fDTEvent -> GetLastTrack() -> SetdEdx(particle->GetTmeanCharge(2), EnumTPC::kMTPC);
        fDTEvent -> GetLastTrack() -> SetdEdx(particle->GetTmeanCharge(), EnumTPC::kTPCAll);

        fDTEvent -> GetLastTrack() -> SetNumberOfdEdxClusters(particle-> GetNDedxPoint(0), EnumTPC::kVTPC1);
        fDTEvent -> GetLastTrack() -> SetNumberOfdEdxClusters(particle-> GetNDedxPoint(1), EnumTPC::kVTPC2);
        fDTEvent -> GetLastTrack() -> SetNumberOfdEdxClusters(particle-> GetNDedxPoint(2), EnumTPC::kMTPC);
        fDTEvent -> GetLastTrack() -> SetNumberOfdEdxClusters(particle-> GetNDedxPoint(), EnumTPC::kTPCAll);

        fDTEvent -> AddTOFHit();
        fDTEvent -> GetLastTOFHit() -> SetPathLength( particle->GetTofPathl() );
        fDTEvent -> GetLastTOFHit() -> SetSquaredMass( particle->GetTofMass2() );
        fDTEvent -> GetLastTOFHit() -> SetSquaredMassError( particle->GetTofSigMass2() );
        fDTEvent -> GetLastTOFHit() -> AddRecoTrackId( iTrack );
        fDTEvent -> GetLastTOFHit() -> SetCharge( particle->GetTofCharge() );
        fDTEvent -> GetLastTOFHit() -> SetPosition( particle->GetTofX(), particle->GetTofY(), 500);

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
          iTrack++;
        }
        tree -> Fill();
    }

    outputFile->cd();
  	tree ->Write();
 	outputFile->Close();
    run -> Close();
     cout << endl;
  }

  timer.Stop();
  printf("Real time: %f\n",timer.RealTime());
  printf("CPU time: %f\n",timer.CpuTime());
  return 1;
}

int main(int argc, char *argv[])
{
  switch (argc)
  {
    case 2:
    return T49_to_DT(argv[1]);
    break;
    case 3:
    return T49_to_DT(argv[1], atoi(argv[2]));
    break;
    default:
    cout << "Specify input filelist!" << endl;
    cout<<argv[0]<<"  PRODUCTION_TAG  [N_FILES]"<<endl << endl;
    return 0;
  }
  return 1;
}
