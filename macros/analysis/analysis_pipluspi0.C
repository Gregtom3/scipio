#include "../../src/BinningStructure.C"
//#include "../../src/sPlotTools.C"

int analysis_pipluspi0(const char * input_file = "../process/test.root",
			  const char * output_file = ""){
  
  vector<string> bin_names={"x","Mh","z"};
  map<string, double> min_bin_vals = { {"x", 0} , {"Mh", 0} , {"z", 0} };
  map<string, double> max_bin_vals = { {"x", 1} , {"Mh", 2} , {"z", 1} };
  map<string, int> num_bins = { {"x" , 5} , {"Mh" , 2} , {"z" , 1} };

  BinningStructure b(bin_names, min_bin_vals, max_bin_vals, num_bins);
  
  TFile *infile = new TFile(input_file,"READ");
  TTree *intree = (TTree*)infile->Get("dihadron");
  
  b.process_ttree(intree);
  
  b.write_bin_data_map("out.root");
  //vector<BinData> bv = b.get_all_bins_data();
  //for(unsigned int i = 0 ; i < bv.size() ; i++){
  //    cout << bv.at(i).binName << endl;
  //}
  
  //fit_unbinned_splot_pipi0(bv.at(0));

  return 0;
}
