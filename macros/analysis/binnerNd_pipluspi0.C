#include "../../src/BinningStructure.C"

int binnerNd_pipluspi0(const char * input_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/catboost/postprocess_pipluspi0/nSidis*.root",
                     string output_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/catboost/binned_pipluspi0/",
                     string output_subdir = "testNd"){
    
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
    
  // Multidimensional binning
  vector<string> bin_names={"x","Mh","z"};
  ///////////////////////////////////////////////////////////////////////////////////////
  map<string, double> min_bin_vals = { {"x", 0} , {"Mh", 0} , {"z", 0} };
  map<string, double> max_bin_vals = { {"x", 1} , {"Mh", 2} , {"z", 1} };
  map<string, int> num_bins = { {"x" , 5} , {"Mh" , 2} , {"z" , 1} };
  ///////////////////////////////////////////////////////////////////////////////////////
  BinningStructure b(bin_names, min_bin_vals, max_bin_vals, num_bins);
  b.process_ttree(inchain,(output_subdir+"/binned.root").c_str());
  ///////////////////////////////////////////////////////////////////////////////////////

  return 0;
}
