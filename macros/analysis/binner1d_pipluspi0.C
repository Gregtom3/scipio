#include "../../src/BinningStructure.C"

int binner1d_pipluspi0(const char * input_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/catboost/postprocess_pipluspi0/nSidis*.root",
                     string output_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/catboost/binned_pipluspi0/",
                     string output_subdir = "test1d"){
    
  // Check if the directory exists
  output_subdir = (output_dir+output_subdir);
  try {
    gSystem->Exec(Form("mkdir %s",output_subdir.c_str()));
  }
  catch (std::exception &e) {
    printf("Directory already exists");
  }
    

    
  TChain *inchain = new TChain("dihadron");
  inchain->Add(input_dir);
    

  map<string, vector<double>> xbins;
  xbins["x"]=std::vector<double>{0,0.1,0.13,0.16,0.19,0.235,0.3,0.5,1};
  BinningStructure xb(vector<string>{"x"}, xbins);
  xb.process_ttree(inchain,(output_subdir+"/x_binned.root").c_str());
    
  map<string, vector<double>> Mhbins;
  Mhbins["Mh"]=std::vector<double>{0 ,0.3 ,0.44285714 ,0.58571429 ,0.72857143 ,0.87142857 ,1.01428571, 1.15714286 ,1.3 ,2};
  BinningStructure Mhb(vector<string>{"Mh"}, Mhbins);
  Mhb.process_ttree(inchain,(output_subdir+"/Mh_binned.root").c_str());
    
  map<string, vector<double>> zbins;
  zbins["z"]=std::vector<double>{0 ,0.35, 0.43, 0.49 ,0.550 ,0.620 ,0.700 ,0.8 ,1};
  BinningStructure zb(vector<string>{"z"}, zbins);
  zb.process_ttree(inchain,(output_subdir+"/z_binned.root").c_str());
  ///////////////////////////////////////////////////////////////////////////////////////

  return 0;
}
