using namespace std;

// Struct to store data for each bin
struct BinData {
  vector<double> Mgg;
  vector<double> Mh;
  vector<double> phi_h;  
  vector<double> phi_R0;
  vector<double> phi_R1;
  vector<double> th;
  vector<double> prob_g1;
  vector<double> prob_g2;
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
    this->bins = convert_min_max_n(min_bin_vals, max_bin_vals,num_bins);
  }
    
  // Constructor
  BinningStructure(vector<string> bin_names, map<string, vector<double>> bins) {
    this->bin_names = bin_names;
    this->bins = bins;
  }
    
    // Method to convert bin structure
    map<string, vector<double>> convert_min_max_n(map<string,double>, map<string,double>, map<string,int>);
    
    // Method to process a TTree
    void process_ttree(TTree* tree);
    
    // Method to get index of bin for a given event
    vector<int> get_index(TTree* tree);
    
    // Method to get index of bin for a given value
    int get_bin_index(double val, string bin_name);
    
    // Method to get BinData struct for a given bin
    BinData get_bin_data(vector<int> index);
    
    // Method to iterate over all bins and get corresponding arrays
    vector<BinData> get_all_bins_data();
};