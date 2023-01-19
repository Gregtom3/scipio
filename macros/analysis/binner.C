#include "../../src/ParseBinYaml.C"

int binner(const char * input_data_wildcard = "",
                 const char * output_datadir = "",
                 const char * input_yaml="",
           int isMC = 0,
           int binNum = -1)
{
    
    ParseBinYaml(input_data_wildcard, output_datadir, input_yaml, isMC, binNum);
    return 0;
    
}