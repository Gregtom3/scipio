#include "../../src/BinningStructure.C"

int binner2d_pipluspi0(const char * input_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/catboost/postprocess_pipluspi0/nSidis*.root",
                     string output_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/catboost/binned_pipluspi0/",
                     string output_subdir = "test2d"){
    
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
  xbins["Mh"]=std::vector<double>{0,0.5,0.9,2};
  BinningStructure xb(vector<string>{"x","Mh"}, xbins);
  xb.process_ttree(inchain,(output_subdir+"/x_binned.root").c_str());
    
    
  map<string, vector<double>> zbins;
  zbins["z"]=std::vector<double>{0 ,0.35, 0.43, 0.49 ,0.550 ,0.620 ,0.700 ,0.8 ,1};
  zbins["Mh"]=std::vector<double>{0,0.5,0.9,2};
  BinningStructure zb(vector<string>{"z","Mh"}, zbins);
  zb.process_ttree(inchain,(output_subdir+"/z_binned.root").c_str());
  ///////////////////////////////////////////////////////////////////////////////////////

  return 0;
}
