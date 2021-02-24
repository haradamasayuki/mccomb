
#include <skroot.h>
#undef MAXHWSK
#include <skheadC.h>
#include <sktqC.h>

#include <SKLibs.hh>
#include <ROOTLibs.hh>
#include <MergeIO.hh>
#include <MergeRealNoise.hh>


MergeIO::MergeIO(const char *inputfile, const char *outputfile, int run_number, int seed)
  :MergeRealNoise(seed), fInputFileName(inputfile), fOutputFileName(outputfile), fRunNumber(run_number), lun(10)
{
  Initialize();
  CreateOutputFile();
}


MergeIO::~MergeIO()
{
  WriteOutput();
  delete inTree;
  delete outTree;
}

void MergeIO::Initialize()
{
//  SetTimeCut(0.);
//  SetTimeMax(535.e3);
//  SetPMTDeadTime(900.);
//  SetTimeWindow(GetTimeMax() - GetTimeCut());

  SKInitialize();
  OpenMCFile();
  SetRealNoiseFile();
}


void MergeIO::CreateOutputFile()
{

  outFile = new TFile(fOutputFileName, "recreate");
  if(inTree) outTree = (TTree*) inTree->CloneTree(0);
	if(outFile) outTree->SetDirectory(outFile);
	fOutHEAD = new Header;
	fOutTQI = new TQReal; // TQ (in-gate)
	outTree->Branch("issignal",  is_signal, "issignal[100000]/I");//MC hits
	outTree->SetBranchAddress("TQREAL",&fOutTQI);
}


void MergeIO::SKInitialize()
{
  // Set SK options and SK geometry
  const char* skoptn = "31,30,26,25";
  skoptn_(skoptn, strlen(skoptn));

  skheadg_.sk_geometry = 5;
  geoset_();

  //kzinit_();
}


void MergeIO::OpenMCFile()
{
  //Register Input RootFile
  fInHEAD = new Header;
  fInTQI  = new TQReal; // TQ (in-gate)
  fInLOWE = new LoweInfo; 
  fInMCINFO = new MCInfo; 

  inFile = new TFile(fInputFileName);
  inTree = (TTree*)inFile->Get("data");

  inTree->SetBranchAddress("HEADER", &fInHEAD);
  inTree->SetBranchAddress("TQREAL", &fInTQI);
//  inTree->SetBranchAddress("TQAREAL", &fInTQA);
}

void MergeIO::SetRealNoiseFile()
{
  //Tentative
  SetRandomWideDataFile();

}

void MergeIO::SetT2KDummyDataFile()
{
}

void MergeIO::SetRandomWideDataFile()
{
  //Register Input RootFile
  string DirName = "/disk02/calib3/auto/random_wide/root/";
  string sRun = std::to_string(fRunNumber);
  if (sRun.size()==5) sRun = "0"+sRun;
  else if (sRun.size()==6) {}
  else {};

  string sRunHeader = sRun.substr(0,4);
  DirName = DirName + sRunHeader + "/random_wide."+sRun+".root";

	fNoiseChain = new TChain("data");
  //fNoiseChain->Add(FilePath.c_str());
  fNoiseChain->Add(DirName.c_str());
	//AddSKrun(nfile); //(koshio)

	fNoiseTQI = new TQReal;
	fNoiseTQA = new TQReal;
	fNoiseHEAD = new Header;
	fNoiseChain->SetBranchAddress("TQREAL", &fNoiseTQI);
	fNoiseChain->SetBranchAddress("TQAREAL", &fNoiseTQA);
	fNoiseChain->SetBranchAddress("HEADER", &fNoiseHEAD);

  MergeRealNoise::SetNoiseEntry(fNoiseChain->GetEntries());

	if (fNoiseChain->GetEntries() == 0){
		//cout << "no random data for time period  : "<<nfile<< endl;
		exit(42);
	}
}


void MergeIO::ReadFile()
{
  Int_t nevent = inTree->GetEntries();
	for(Int_t iev = 0;iev<nevent;iev++) {
    if(iev%100 == 0) cout<<iev<<endl;
		inTree->GetEntry(iev);
    ReadEvent();
  }
}


void MergeIO::ReadEvent() {

  Clear();
  ReadInputTQ();
  AppendNoiseData();
  SortAppendedTQ();

  Combine();

  //SetOutPut();
  outTree->Fill();
}


void MergeIO::WriteOutput()
{
  outFile->cd();
  outTree->Write();
  outFile->Close();
}

bool MergeIO::CheckFileExist(const std::string& str)
{
    std::ifstream ifs(str);
      return ifs.is_open();
}
