#include "../../src/fitTools.C"

int bruana_pipluspi0(const char * input_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/catboost/binned_pipluspi0/testNd",
           int L = 2,
           double threshold = 0.9,
           double Mggmin = 0.07,
           double Mggmax = 0.22)
{
  fitTools ftools(input_dir);
  ftools.splot_pipluspi0(L,
		      threshold,
		      Mggmin,
		      Mggmax);
  
    return 0;
}
