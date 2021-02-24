
#include <stdLibs.hh>
#include "SKLibs.hh"
#include "ROOTLibs.hh"

#include <MergeRealNoise.hh>

using namespace std;

MergeRealNoise::MergeRealNoise(int seed)
//:PMTXYZ(geopmt_.xyzpm), C_WATER(21.5833), 
:engine(seed), rndm(0,1), fTimeWindow(535.e3), fTimeCut(0.), fTimeMax(535.e3), fPMTDeadTime(900.) 
{
}
//****************************************************************************//
MergeRealNoise::~MergeRealNoise()
{}

//****************************************************************************//
/*
void AddT2Krun( int nfile)
{
	int timebin = -1;
	int t2k = 1; //tentative
	if (t2k){
		// Find run time bin and the corresponding T2K runs to use
		// Info in a text file
		FILE *fruns = fopen("/home/elhedri/SK2p2MeV/mc/generate/ibd_signal/run_info_t2k.txt", "r");
		int run, bin, dum;
		int startbin = -1, oldbin = -1;
		int count = 0;
		// Fill run table and find time bin for current run
		while(!feof(fruns)){
			fscanf(fruns, "%d %d %d\n", &run, &bin, &dum);
			if (bin != oldbin){
				if (timebin != -1) break;
				startbin = count;
				oldbin = bin;
			}
			if (run >= nfile){
				timebin = bin;
				//cout<<nfile<<"  "<<run<<"  "<<bin<<"  "<<dum<<"  "<<timebin<<endl;
			}
			pair<int,int> p(run,dum);
			runs.insert(p);
			count++;
		}
		// Remove everything not in the right time bin
		// (for the first bin, nothing to remove)
		// So at the end we have a hash table that, for a given run, tells us
		// whether it is dummy or real bin
		if (startbin > 0){
			for(int q = 0; q < startbin; q++){
				runs.erase(runs.begin());
			}
		}
		fclose(fruns);
		cout << "Time bin is " << timebin << endl;

		// Load T2K data for the run period we want
		//dummy trigger chain
		cout << "Loading T2K events..." << endl;
		for(map<int,int>::iterator iter = runs.begin(); iter != runs.end(); iter++){
			int irun = iter->first;
			char inname[500];
			//cout<<irun<<"  "<<iter->second<<endl;
			for (int k=0; k<9; k++){
				//If the specified run is exist, the data are added for chain.
				sprintf(inname, "/disk01/usr5/rakutsu/t2k/Neutron/work/dummy/out/inbtwRun%d_%d/t2k.0%d*.root", k, k+1, irun);
				t2kch->Add(inname);
			}
		}
	}
}
*/

//****************************************************************************//
/*
//(koshio)
void AddSKrun( int nfile)
{
  char inname[500];
  //sprintf(inname, "/disk01/usr3/koshio/sk5/calib/200326/random_wide.0%d.root", nfile);
  // Load SK random trigger data for the run period we want
  //dummy trigger chain
  cout << "Loading SK events..." << endl;
  sprintf(inname, "/disk01/usr3/koshio/sk5/calib/200326/random_wide.085169.root");
  t2kch->Add(inname);
  sprintf(inname, "/disk01/usr3/koshio/sk5/calib/200326/random_wide.085170.root");
  t2kch->Add(inname);
  sprintf(inname, "/disk01/usr3/koshio/sk5/calib/200326/random_wide.085171.root");
  t2kch->Add(inname);
}
*/

void MergeRealNoise::Clear()
{
  vT.clear();
  vQ.clear();
  vCableID.clear();
  vIsSignal.clear();
  vHitFlag.clear();

  vSortedT.clear();
  vSortedQ.clear();
  vSortedCableID.clear();
  vSortedIsSignal.clear();
  vSortedHitFlag.clear();
  nHit_App = 0;
  nHit_Save = 0;
}


void MergeRealNoise::Combine()
{

//  //2 dummy trigger events per MC event, hence nread%2 conditional
//  if(nread%2 == 0) {
//    //read dummy trigger
//    t2kch->GetEntry(entry);
//    entry++;
//    //require hit cuts to cut out cosmic ray muons etc
//    while (TQI->nhits > 75000 || TQI->nhits < 30000) {
//      t2kch->GetEntry(entry);
//      entry++;
//    }
//  }

//  nread++;
//  if (nread%1000 == 0) cout << "Read " << nread << " events. " << endl;

  //random trigger timing goes from -500us < t < 500us, therefore I want the offset from 0 that my dummy trigger hits start at. As I use 1 dummy trigger event for 2 MC events, the offset is different depending on nread%2.
//  if (nread%2 ==0)
//  {
//    toffset = TQI->T[0];
//  }
//  else
//  {
//    toffset = TQI->T[TQI->nhits-1] - twind; //twind is total timewindow required, e.g. 535-18us.
//  }


  fOutTQI->Clear();

//this is used to check for PMT deadtimes.
  float PreHitTime[MAXPM+1] = {0.};
  for (int i = 0; i<MAXPM+1; i++) PreHitTime[i] = -9999999.;

//Dead time treatment
  for (Int_t i = 0; i<nHit_App;i++) {
    if (vSortedT[i] - PreHitTime[vSortedCableID[i]] < fPMTDeadTime) continue;//cutting for pmt dead time.
    if (PreHitTime[vSortedCableID[i]] < vSortedT[i]) PreHitTime[vSortedCableID[i]] = vSortedT[i];
    if (vSortedT[i] > fTimeMax) continue;

    fOutTQI->T.push_back(vSortedT[i]);
    fOutTQI->Q.push_back(vSortedQ[i]);
    fOutTQI->cables.push_back(vSortedCableID[i] + (vSortedHitFlag[i] << 16));
    nHit_Save++;
  }
  fOutTQI->nhits = nHit_Save;
  fOutTQI->it0xsk = fInTQI->it0xsk;

  return;
}


void MergeRealNoise::ReadInputTQ()
{

  ftqreal_version  = fInTQI->tqreal_version;  // added in ver.2
  fqbconst_version = fInTQI->qbconst_version; 
  ftqmap_version   = fInTQI->tqmap_version;   
  fpgain_version   = fInTQI->pgain_version;   
  fit0xsk          = fInTQI->it0xsk;          

  for( int i=0; i<fInTQI->nhits; i++) {
    Int_t cbl = fInTQI->cables[i]&0x0000FFFF;
    if(cbl<1||cbl>MAXPM) continue;//t2k data contained impossible cableID

    vHitFlag.push_back(fInTQI->cables[i]>>16);
    vCableID.push_back(fInTQI->cables[i]&0x0000FFFF);
    vIsSignal.push_back(1);
    vT.push_back(fInTQI->T[i]);
    vQ.push_back(fInTQI->Q[i]);
    nHit_App++;
  }
}


void MergeRealNoise::AppendNoiseData()
{
  fNoiseChain->GetEntry((int)GetNoiseEntry()*MakeRandom());

  Float_t fTMin, fTMax;//tentative
  
  fTMin = fNoiseTQI->T[0] + (fNoiseTQI->T[fNoiseTQI->nhits-1] - fTimeWindow)*MakeRandom();//fTMin is randomised b/w -500 ~ 500 us.
  fTMax = fTMin + fTimeWindow; 
  fTimeOffset = fTMin;

  for ( int i=0; i<fNoiseTQI->nhits; i++) {
    Int_t cbl = fNoiseTQI->cables[i]&0x0000FFFF;
    if (cbl<1||cbl>MAXPM) continue;//dummy data contained impossible cableID (OD or others)

    if (fNoiseTQI->T[i]<fTMin||fNoiseTQI->T[i]>fTMax) continue;//only selected within [range] hit

    vHitFlag.push_back(fNoiseTQI->cables[i]>>16);
    vCableID.push_back(cbl);
    vIsSignal.push_back(0);
    vT.push_back(fNoiseTQI->T[i]-fTimeOffset +fTimeCut);
    vQ.push_back(fNoiseTQI->Q[i]);
    nHit_App++;
  }
}


void MergeRealNoise::SortAppendedTQ()
{
  int nhits = static_cast<int>(nHit_App);
  int sortedIndex[nhits];

  // Sort: early hit first
  TMath::Sort(nhits, vT.data(), sortedIndex, false);

  // Save hit info, sorted in (T - ToF)
  for (int iHit = 0; iHit < nhits; iHit++) {
    vSortedCableID.push_back(  vCableID[ sortedIndex[iHit] ]   );
    vSortedHitFlag.push_back(  vHitFlag[ sortedIndex[iHit] ]   );
    vSortedIsSignal.push_back( vIsSignal[ sortedIndex[iHit] ]  );
    vSortedT.push_back(        vT[ sortedIndex[iHit] ]         );
    vSortedQ.push_back(        vQ[ sortedIndex[iHit] ]         );
  }
}


