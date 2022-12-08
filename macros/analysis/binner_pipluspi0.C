#include "../../src/BinningStructure.C"
//#include "../../src/sPlotTools.C"

int binner_pipluspi0(const char * input_dir = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/pipluspi0_catboost/nSidis*.root",
			  const char * output_file = "out.root"){
  
  vector<string> bin_names={"x","Mh","z"};
  map<string, double> min_bin_vals = { {"x", 0} , {"Mh", 0} , {"z", 0} };
  map<string, double> max_bin_vals = { {"x", 1} , {"Mh", 2} , {"z", 1} };
  map<string, int> num_bins = { {"x" , 5} , {"Mh" , 2} , {"z" , 1} };

  BinningStructure b(bin_names, min_bin_vals, max_bin_vals, num_bins);
  
  TChain *inchain = new TChain("dihadron");
  inchain->Add(input_dir);
  b.process_ttree(inchain,output_file);
  
  //vector<BinData> bv = b.get_all_bins_data();
  //for(unsigned int i = 0 ; i < bv.size() ; i++){
  //    cout << bv.at(i).binName << endl;
  //}
  
  //fit_unbinned_splot_pipi0(bv.at(0));

  return 0;
}
