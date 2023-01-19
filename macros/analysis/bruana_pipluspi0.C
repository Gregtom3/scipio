#include "../../src/ParseBinYaml.C"
#include "../../src/fitTools.C"


int bruana_pipluspi0(//const char *input_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/projects/piminuspi0_prelim/catboost/postprocess_binned/test1d",
                    //const char * input_file = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/projects/piminuspi0_prelim/catboost/postprocess_binned/test1d/MC_x_binned.root",
                    //const char * input_tree = "x_0.130000_0.160000",
            const char *input_dir = "",
            const char * input_file = "",
            const char * input_tree = "",
           int L = 2,
           double threshold = 0.9,
           double Mggmin = 0.07,
           double Mggmax = 0.22,
           double smin   = 0.22,
           double smax   = 0.4,
           int isMC = 1,
           string version="all",
           const char * inject_yaml = "/work/clas12/users/gmat/scipio/projects/piminuspi0_prelim/catboost/Inject.yaml")
{
    
    YAMLbinstruct binStruct = get_struct(inject_yaml, input_file);
    
    fitTools ftools(input_dir,input_file,input_tree,isMC,L,version,threshold,binStruct);
    
    if(!ftools.isGoodBin()){
        cout << "TTree has too few entries, skipping..." << endl;
        return -1;
    }
    ftools.splot_pipluspi0(L,threshold,Mggmin,Mggmax);
    ftools.sideband_pipluspi0(L,threshold,Mggmin,Mggmax,smin,smax);
    
    /////////////////////////////Delete now unneeded TTree if injection took place
    if(isMC==1)
    {
        gSystem->Exec(Form("rm %s",Form("%s_%s_%s_%s.root",input_file,input_tree,binStruct.injectName.c_str(),version.c_str())));
    }
    
    return 0;
}
