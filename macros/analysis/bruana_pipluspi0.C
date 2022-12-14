#include "../../src/sPlotTools.C"

int bruana_pipluspi0(const char * input_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/binned_catboost/test",
           int L = 2,
           double threshold = 0.9,
           double Mggmin = 0.07,
           double Mggmax = 0.22)
{
    sPlotTools spt(input_dir);
    spt.splot_pipluspi0(L,
                        threshold,
                        Mggmin,s
                        Mggmax);
    
    return 0;
}