#include "../../src/ParseBinYaml.C"

// int binner(const char * input_data_wildcard = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/projects/pipluspi0_noresonance/catboost/postprocess/nSidis*.root",
//                  const char * output_datadir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/projects/pipluspi0_noresonance/catboost/postprocess_binned",
//                  const char * input_yaml="/work/clas12/users/gmat/scipio/macros/analysis/test.yaml",
//            int isMC = 0)
int binner(const char * input_data_wildcard = "",
                 const char * output_datadir = "",
                 const char * input_yaml="",
           int isMC = 0)
{
    
    ParseBinYaml(input_data_wildcard, output_datadir, input_yaml, isMC);
    return 0;
    
}