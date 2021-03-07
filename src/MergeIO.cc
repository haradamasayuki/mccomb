
#include <skroot.h>
#undef MAXHWSK
#include <skheadC.h>
#include <sktqC.h>
#include <sys/stat.h>

#include <SKLibs.hh>
#include <ROOTLibs.hh>
#include <MergeIO.hh>
#include <MergeRealNoise.hh>


MergeIO::MergeIO(const char *inputfile, const char *outputfile, int run_number, int seed)
  :MergeRealNoise(seed), fInputFileName(inputfile), fOutputFileName(outputfile),
  fNoiseDataType("T2K") ,fRunNumber(run_number), fFileNumber(0), lun(10)
{
  Initialize();
  CreateOutputFile();
}


MergeIO::~MergeIO()
{
  cout<<" Destructor : Writing output "<<endl;
  WriteOutput();
  //delete inTree;
  //delete outTree;
}

void MergeIO::Initialize()
{
//  SetTimeMax(535.e3);
//  SetPMTDeadTime(900.);
//  SetTimeWindow(GetTimeMax() - GetTimeCut());

  SKInitialize();
  OpenMCFile();
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
  if (fNoiseDataType == "T2K") {
    SetT2KDummyDataFile();
  }
  else if (fNoiseDataType == "WideRandom") {
    SetRandomWideDataFile();
  }
  else if (fNoiseDataType == "AmBe") {
    //not implemented yet
  }
  SetStartEvent(fFileNumber*3000.);
}

void MergeIO::SetT2KDummyDataFile()
{
  //Register Input RootFile
  //Tentatively only SK6
  string DirName = "/disk01/calib/usr/han/dummy/sk6/";
  //string DirName = "/disk02/calib3/auto/random_wide/root/";

	fNoiseChain = new TChain("data");

  for (int i = -75;i<75;i++) {
    string sRun = std::to_string(fRunNumber-i);
    if (sRun.size()==5) sRun = "0"+sRun;

    string RunDir = DirName + sRun;
    if (CheckDirectoryExist(RunDir)) {
      string FileName = DirName + sRun + "/t2k_run" + sRun + "*.root";
      fNoiseChain->Add(FileName.c_str());
      cout<<"Noise data File   : "<<FileName<<endl;
    }
  }
	fNoiseTQI = new TQReal;
	fNoiseTQA = new TQReal;
	fNoiseHEAD = new Header;
	fNoiseChain->SetBranchAddress("TQREAL", &fNoiseTQI);
	fNoiseChain->SetBranchAddress("TQAREAL", &fNoiseTQA);
	fNoiseChain->SetBranchAddress("HEADER", &fNoiseHEAD);

  MergeRealNoise::DefineHitThreshold();

  MergeRealNoise::SetNoiseEntry(fNoiseChain->GetEntries());
  std::cout<<"Noise Event :  "<<MergeRealNoise::GetNoiseEntry()<<std::endl;

	if (fNoiseChain->GetEntries() == 0) {
		cout << "no random data for time period  : "<< endl;
		exit(42);
	}
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
		cout << "no random data for time period  : "<< endl;
		exit(42);
	}
}


void MergeIO::ReadFile()
{
  Int_t nevent = inTree->GetEntries();
  cout<<" Input Event : "<<nevent<<endl;
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

bool MergeIO::CheckDirectoryExist(const std::string& str)
{
  struct stat st;
  bool bExist = 0;
  if (!stat(str.c_str(), &st) ) bExist = 1;
  else if (stat(str.c_str(), &st) ) bExist = 0;
  return bExist;
}
