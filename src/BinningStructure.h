using namespace std;

// Struct to store data for each bin
struct BinData {
  TString binName;
  TTree *bintree = 0;
  int entries=0;
};

class BinningStructure {
private:
  // Vector of bin names
  vector<string> bin_names;
  // Map of bin name and bin edges
  map<string, vector<double>> bins;
  // Map of BinData structs
  map<vector<int>, BinData> bin_data_map;
  
public:
  // Constructor
  BinningStructure(vector<string> bin_names, map<string, double> min_bin_vals, map<string, double> max_bin_vals, map<string, int> num_bins) {
    this->bin_names = bin_names;
    this->bins = convert_min_max_n(bin_names ,min_bin_vals, max_bin_vals,num_bins);
    
  }
    
  // Constructor
  BinningStructure(vector<string> bin_names, map<string, vector<double>> bins) {
    this->bin_names = bin_names;
    this->bins = bins;
  }

    // Method to convert bin structure
    map<string, vector<double>> convert_min_max_n(vector<string>, map<string,double>, map<string,double>, map<string,int>);
    
    // Method to process a TTree
    void process_ttree(TChain *, const char *, string);
    
    // Method to set the bin names
    void SetBinNames(map<string, vector<double>>, map<vector<int>, BinData>&);
    
    // Method to get index of bin for a given event
    vector<int> get_index(TTree* tree);
    
    // Method to get index of bin for a given value
    int get_bin_index(double val, string bin_name);
    
    // Method to get BinData struct for a given bin
    BinData get_bin_data(vector<int> index);
    
    // Method to iterate over all bins and get corresponding arrays
    vector<BinData> get_all_bins_data();
};