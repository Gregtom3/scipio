// This code was generated with the help of OpenAI https://beta.openai.com/

#include "BinningStructure.h"
// Method to convert bin structure
map<string, vector<double>> BinningStructure::convert_min_max_n(vector<string> bin_names, map<string, double> min_bin_vals, map<string, double> max_bin_vals, map<string, int> num_bins){
    map<string, std::vector<double>> bins;
    
    for (string key : bin_names) {
        int numbins = num_bins[key];

        double min = min_bin_vals[key];
        double max = max_bin_vals[key];
        double bin_width = (max - min) / numbins;

        std::vector<double> bin_edges;
        for (int i = 0; i <= numbins; i++) {
            bin_edges.push_back(min + i * bin_width);
        }
        bins[key] = bin_edges;
    }
    return bins; 
}

// Method to process a TChain
void BinningStructure::process_ttree(TChain *tree,const char * outfile, string version="") {
    // Get branches from tree
    int hel,run;
    int isGoodEventWithoutML;
    float x,Q2,Mx,z, Mgg, Mh, phi_h, phi_R0, phi_R1, th, prob_g1, prob_g2;
    tree->SetBranchAddress("run", &run);
    tree->SetBranchAddress("hel", &hel);
    tree->SetBranchAddress("isGoodEventWithoutML",&isGoodEventWithoutML);
    tree->SetBranchAddress("x", &x);
    tree->SetBranchAddress("Q2", &Q2);
    tree->SetBranchAddress("Mx", &Mx);
    tree->SetBranchAddress("z", &z);

    tree->SetBranchAddress("Mgg", &Mgg);
    tree->SetBranchAddress("Mh", &Mh);
    tree->SetBranchAddress("phi_h", &phi_h);
    tree->SetBranchAddress("phi_R0", &phi_R0);
    tree->SetBranchAddress("phi_R1", &phi_R1);
    tree->SetBranchAddress("th", &th);
    tree->SetBranchAddress("prob_g1", &prob_g1);
    tree->SetBranchAddress("prob_g2", &prob_g2);
    TFile *fout = new TFile(outfile,"RECREATE");
    // Loop over events in tree
    for (int i = 0; i < tree->GetEntries(); i++) {
      if(i%10000==0)
          cout << "Analyzed " << i << " of " << tree->GetEntries() << endl;
      tree->GetEntry(i);
      // Get multidimensional index for this event
      vector<int> index = get_index(tree);
      // If index is valid, store data in bin
      if (index.size() == bin_names.size()) {
          if(bin_data_map[index].bintree==0){//create TTree
              SetBinNames(bins,bin_data_map);
              bin_data_map[index].bintree = new TTree(bin_data_map[index].binName,bin_data_map[index].binName);
              bin_data_map[index].bintree->Branch("fgID", &bin_data_map[index].entries);
              bin_data_map[index].bintree->Branch("run", &run);
              bin_data_map[index].bintree->Branch("x",&x);
              bin_data_map[index].bintree->Branch("Q2",&Q2);
              bin_data_map[index].bintree->Branch("Mx",&Mx);
              bin_data_map[index].bintree->Branch("z",&z);
              bin_data_map[index].bintree->Branch("hel", &hel);
              bin_data_map[index].bintree->Branch("Mgg", &Mgg);
              bin_data_map[index].bintree->Branch("Mh", &Mh);
              bin_data_map[index].bintree->Branch("phi_h", &phi_h);
              bin_data_map[index].bintree->Branch("phi_R0", &phi_R0);
              bin_data_map[index].bintree->Branch("phi_R1", &phi_R1);
              bin_data_map[index].bintree->Branch("th", &th);
              bin_data_map[index].bintree->Branch("prob_g1", &prob_g1);
              bin_data_map[index].bintree->Branch("prob_g2", &prob_g2);
              bin_data_map[index].bintree->Branch("isGoodEventWithoutML",&isGoodEventWithoutML);
          }
            bin_data_map[index].bintree->Fill();
            bin_data_map[index].entries++;
      }
    }
    
    // Save all bins to output TTree
    for (map<vector<int>, BinData>::iterator it = bin_data_map.begin(); it != bin_data_map.end(); ++it) {
        (it->second).bintree->Write();
    }
    
    // Close output file
    fout->Close();
  }

//Function set bin names of filled substructures
void BinningStructure::SetBinNames(map<string, vector<double>> bins, map<vector<int>, BinData> &bin_data_map) {
    //Loop over all elements of bin_data_map
    for (map<vector<int>, BinData>::iterator it = bin_data_map.begin(); it != bin_data_map.end(); ++it) {
        TString binName="";
        //Loop over all elements of vector<int> in bin_data_map
        for (int i = 0; i < it->first.size(); i++) {
            //Loop over all elements of bins
            int idx = it->first.at(i);
            string key = bin_names.at(i);
            binName += Form("%s_%f_%f",key.c_str(),bins[key].at(idx),bins[key].at(idx+1));
            if(i!=it->first.size()-1)
                binName+="_";
        }
        it->second.binName=binName;
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
