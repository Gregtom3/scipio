// This code was generated with the help of OpenAI https://beta.openai.com/

#include "BinningStructure.h"
// Method to convert bin structure
map<string, vector<double>> BinningStructure::convert_min_max_n( map<string, double> min_bin_vals, map<string, double> max_bin_vals, map<string, int> num_bins ){
    map<string, std::vector<double>> bins;

    for (auto &pair : num_bins) {
        string key = pair.first;
        int num_bins = pair.second;

        double min = min_bin_vals[key];
        double max = max_bin_vals[key];
        double bin_width = (max - min) / num_bins;

        std::vector<double> bin_edges;
        for (int i = 0; i <= num_bins; i++) {
            bin_edges.push_back(min + i * bin_width);
        }
        bins[key] = bin_edges;
    }
    return bins; 
}

// Method to process a TTree
void BinningStructure::process_ttree(TTree* tree) {
    // Get branches from tree
    float Mgg, Mh, phi_h, phi_R0, phi_R1, th, prob_g1, prob_g2;
    tree->SetBranchAddress("Mgg", &Mgg);
    tree->SetBranchAddress("Mh", &Mh);
    tree->SetBranchAddress("phi_h", &phi_h);
    tree->SetBranchAddress("phi_R0", &phi_R0);
    tree->SetBranchAddress("phi_R1", &phi_R1);
    tree->SetBranchAddress("th", &th);
    tree->SetBranchAddress("prob_g1", &prob_g1);
    tree->SetBranchAddress("prob_g2", &prob_g2);
    // Loop over events in tree
    for (int i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);
      // Get multidimensional index for this event
      vector<int> index = get_index(tree);
      // If index is valid, store data in bin
      if (index.size() == bin_names.size()) {
        cout << i << " " << index.at(0) << " " << index.at(1) << " " << index.at(2) << " " << Mgg << endl;
    BinData bin_data = bin_data_map[index];
    // Append data to bin
    bin_data.Mgg.push_back(Mgg);
    bin_data.Mh.push_back(Mh);
    bin_data.phi_h.push_back(phi_h);
    bin_data.phi_R0.push_back(phi_R0);
    bin_data.phi_R1.push_back(phi_R1);
    bin_data.th.push_back(th);
    bin_data.prob_g1.push_back(prob_g1);
    bin_data.prob_g2.push_back(prob_g2);
    // Store bin in map
    bin_data_map[index] = bin_data;
      }
    }
  }

// Method to get index of bin for a given event
vector<int> BinningStructure::get_index(TTree* tree) {
    vector<int> index;
    for (unsigned int i = 0; i < bin_names.size(); i++) {
      string bin_name = bin_names[i];
      double bin_val = tree->GetLeaf(bin_name.c_str())->GetValue();
      int bin_idx = get_bin_index(bin_val, bin_name);
      if(bin_idx!=-1) // So long as the index is found (i.e. within a bin)
          index.push_back(bin_idx);
    }
    return index;
}

// Method to get index of bin for a given value
int BinningStructure::get_bin_index(double val, string bin_name) {
    
    int index = -1;
    for (auto it = bins.begin(); it != bins.end(); ++it) {
        if (it->first == bin_name) {
            vector<double>& vec = it->second;
            for (int i = 0; i < vec.size() - 1; ++i) {
                if (vec[i] <= val && val < vec[i+1]) {
                    index = i;
                    break;
                }
            }
        }
    }
    
    return index;
}

// Method to get BinData struct for a given bin
BinData BinningStructure::get_bin_data(vector<int> index) {
    return bin_data_map[index];
}

// Method to iterate over all bins and get corresponding arrays
vector<BinData> BinningStructure::get_all_bins_data() {
    vector<BinData> bins_data;
    for (auto it = bin_data_map.begin(); it != bin_data_map.end(); it++) {
      bins_data.push_back(it->second);
    }
    return bins_data;
}