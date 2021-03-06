#ifndef MERGEREALNOISE_HH
#define MERGEREALNOISE_HH 1


#include <stdLibs.hh>
#include <random>

#include "tqrealroot.h"
#include "loweroot.h"
#include "softtrgroot.h"
#include "mcinfo.h"

#include "MergeRealNoise.hh"

class MergeRealNoise {
  public : 
    MergeRealNoise(int seed);
    ~MergeRealNoise();


    void Clear();
    void Combine();
    void DefineHitThreshold();
    void AppendNoiseData();
    void ReadInputTQ();
    void SortAppendedTQ();

    inline void SetNoiseEntry(Int_t entry) { fNoiseEntry = entry; }
    inline void SetTimeCut(Float_t tcut) { fTimeCut = tcut; }
    inline void SetTimeMax(Float_t tmax) { fTimeMax = tmax; }
    inline void SetTimeWindow(Float_t twin) { fTimeWindow = twin; }
    inline void SetPMTDeadTime(Float_t tdead) { fPMTDeadTime = tdead; }
    inline void SetStartEvent(int ievt) { fNoiseEvent = ievt; }

    Double_t MakeRandom() { return rndm(engine); }

    Int_t   GetNoiseEntry()   { return fNoiseEntry; }
    Float_t GetTimeCut()      { return fTimeCut; }
    Float_t GetTimeMax()      { return fTimeMax; }
    Float_t GetTimeWindow()   { return fTimeWindow; }
    Float_t GetiPMTDeadTime() { return fPMTDeadTime; }
    

    std::mt19937 engine;
    std::uniform_real_distribution<> rndm;

  protected :
    TChain *fNoiseChain;
    TQReal *fNoiseTQI;
    TQReal *fNoiseTQA;
    Header *fNoiseHEAD;

    
    Header *fInHEAD;
    TQReal *fInTQI;
    LoweInfo *fInLOWE; 
    MCInfo *fInMCINFO;
    Int_t is_signal[100000];

    Header *fOutHEAD;
    TQReal *fOutTQI;

    TFile *outFile;
    TFile *inFile;

    TTree *outTree;
    TTree *inTree;


  private : 
    Int_t   ProcessEvent;
    Int_t   fNoiseEvent;
    Int_t   nReadNoiseEvent;

    Int_t   fNoiseEntry; // Number of entry for the noise run. 
    Float_t fTimeWindow; // fTimeMax - fTimeCut = TimeWindow
    Float_t fTimeOffset; // Randomly determined start-timing for dark noise data
    Float_t fTimeCut; // cut time from start time 
    Float_t fTimeMax; // Maximum time for dark noise
    Float_t fPMTDeadTime; // PMT dead time after hit

    Float_t fLeftEdge;  // Lower edge of nhits
    Float_t fRightEdge; // Higher edge of nhits
//    Int_t fRandomSeed;

    Int_t nHit_App;
    Int_t nHit_Save;
    std::vector<float> vT;
    std::vector<float> vQ;
    std::vector<int> vCableID;
    std::vector<int> vIsSignal;
    std::vector<int> vHitFlag;
    Int_t ftqreal_version; 
    Int_t fqbconst_version; 
    Int_t ftqmap_version;   
    Int_t fpgain_version;   
    Int_t fit0xsk;          

    std::vector<float> vSortedT;
    std::vector<float> vSortedQ;
    std::vector<int> vSortedCableID;
    std::vector<int> vSortedIsSignal;
    std::vector<int> vSortedHitFlag;
};
#endif
