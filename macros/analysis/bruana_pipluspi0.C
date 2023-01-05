#include "../../src/fitTools.C"

int bruana_pipluspi0(const char *input_dir = "",
                    const char * input_file = "",
                    const char * input_tree = "",
           int L = 2,
           double threshold = 0.9,
           double Mggmin = 0.07,
           double Mggmax = 0.22,
           double smin   = 0.22,
           double smax   = 0.4,
           int isMC = 0,
           string version="")
{
    fitTools ftools(input_dir,input_file,input_tree,isMC,version,threshold);
    if(!ftools.isGoodBin()){
        cout << "TTree has too few entries, skipping..." << endl;
        return -1;
    }
    ftools.splot_pipluspi0(L,threshold,Mggmin,Mggmax);
    ftools.sideband_pipluspi0(L,threshold,Mggmin,Mggmax,smin,smax);

    return 0;
}
