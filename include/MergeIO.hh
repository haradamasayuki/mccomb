#ifndef MERGEIO_HH
#define MERGEIO_HH 1


#include "SKLibs.hh"
#include "ROOTLibs.hh"

#include "MergeIO.hh"
#include "MergeRealNoise.hh"

class MergeIO : public MergeRealNoise 
{
  public : 
    MergeIO(const char *inputfile, const char *outputfile, int run_number, int seed);
    ~MergeIO();

    void Initialize();
    void CreateOutputFile();
    void SKInitialize();
    void OpenMCFile();

    void SetRealNoiseFile();
    void SetT2KDummyDataFile();
    void SetRandomWideDataFile();

    void ReadFile();
    void ReadEvent();
    void WriteOutput();

    bool CheckFileExist(const std::string& str);

    inline void SetNoiseFileType(std::string str) {fNoiseDataType = str;}   

  protected : 
    const char* fInputFileName;
    const char* fOutputFileName;

    std::string fNoiseDataType;
    Int_t fRunNumber;
    Int_t lun;



};
#endif
