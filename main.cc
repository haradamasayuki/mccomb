#include <stdLibs.hh>
#include <iostream>
#include <string>

#include "SKLibs.hh"
#include "ROOTLibs.hh"

#include <MergeArgOptions.hh>
#include <MergeIO.hh>
#include <MergeRealNoise.hh>


using namespace std;


//Int_t	MA_QBCONST = qbconst_ver / 1000;
//Int_t MI_QBCONST = qbconst_ver%1000;
//Int_t	MA_TQMAP   = tqmap_ver / 1000;
//Int_t MI_TQMAP   = tqmap_ver%1000;
//Int_t	MA_PGAIN   = pgain_ver / 1000;
//Int_t MI_PGAIN   = pgain_ver%1000;
//
//
//Int_t tqreal_ver  = 2;                             // copied from tqrawsk.F
//Int_t qbconst_ver = MA_QBCONST*1000 + MI_QBCONST;  // copied from tqrawsk.F
//Int_t tqmap_ver   = MA_TQMAP*1000 + MI_TQMAP; // copied from tqrawsk.F
//Int_t pgain_ver   = MA_PGAIN*1000 + MI_PGAIN;      // copied from tqrawsk.F


void DoProcess(MergeIO *mrg, MergeArgOptions &options);
//****************************************************************************//
int main(int argc, char **argv)
{

  MergeArgOptions options(argc, argv);

	//if (argc < 4)
	//{
	//	cout << "Usage: " << argv[0] << " fname_in fname_out run_number (seed)" << endl;
	//	exit(1);
	//}

  const std::string  &optInput    = options.GetOption("-in");
  if(optInput.empty()) {
    cerr<<"!!!!!!!!!!!!!"<<endl;
    std::exit(1);
  }
  const std::string  &optOutput    = options.GetOption("-out");
  if ( optOutput.empty() ) { 
    cerr<<"!!!!!!!!!!!!!"<<endl;
    std::exit(1);
    //optOutput = "./out.root";
  }
  const std::string  &optRunNum   = options.GetOption("-run");
  if ( optRunNum.empty()) {
    cerr<<"!!!!!!!!!!!!!"<<endl;
    std::exit(1);
  }
  //if(!optInput.empty()&&!optOutput.empty()&&!optRunNum.empty()) {
  TString  inputName(optInput);
  TString  outputName(optOutput); 
  Int_t    nrun = std::stoi(optRunNum);
  //}
  
	//TString fname_in = argv[1];
	//TString fname_out = argv[2];
	//Int_t nrun = atoi(argv[3]);


  Int_t seed = 0;
  time_t StartTime;
  srand(time(&StartTime));
  seed = rand();
  const std::string &optSeed = options.GetOption("-seed");
  if(!optSeed.empty()) seed = stoi(optSeed);
  //if ( argc>=4) seed = atoi(argv[4]);

  MergeIO *mrg = new MergeIO(inputName.Data(), outputName.Data(), nrun, seed);

  const std::string &optNoiseFileType    = options.GetOption("-NOISEFILETYPE");
  if (!optNoiseFileType.empty()) mrg->SetNoiseFileType(optNoiseFileType);
  
  const std::string &optInputFileNum    = options.GetOption("-FILENUMBER");
  if (!optInputFileNum.empty()) mrg->SetInputFileNumber(std::stoi(optInputFileNum));
  
  mrg->SetRealNoiseFile();
  DoProcess(mrg, options);
  delete mrg;

	return 0;
}
//****************************************************************************//
//****************************************************************************//
void DoProcess(MergeIO *mrg, MergeArgOptions &options)
{
  const std::string &foptTimeMax     = options.GetOption("-TMAX");
  const std::string &foptTimeCut     = options.GetOption("-TCUT");
  const std::string &foptPMTDeadTime = options.GetOption("-TDEAD");
  if (!foptTimeMax.empty())      mrg->SetTimeMax(std::stof(foptTimeMax));
  if (!foptTimeCut.empty())      mrg->SetTimeCut(std::stof(foptTimeCut));
  if (!foptPMTDeadTime.empty())  mrg->SetPMTDeadTime(std::stof(foptPMTDeadTime));
  mrg->SetTimeWindow(mrg->GetTimeMax() - mrg->GetTimeCut());

  mrg->ReadFile();
}


