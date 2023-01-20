#include "BinManager.h"

void BinManager::process(TChain * tree){
    // Get number of BinningStructures
    int Nmax = binStructures.size();
    
    // Get branches from tree
    int hel,run;
    int isGoodEventWithoutML;
    int MCmatch; // True if all important particles had a Monte Carlo match
    float x,Q2,Mx,y,z, Mgg, Mh, phi_h, phi_R0, phi_R1, th, prob_g1, prob_g2;
    float truex,trueQ2,trueMx,truez,trueMgg,trueMh,truephi_h,truephi_R0,truephi_R1,trueth;
    int chi2CutPiPlus, truepid_gamma1, truepid_gamma2, truepid_pion, trueparentpid_gamma1, trueparentpid_gamma2, trueparentpid_pion;
    tree->SetBranchAddress("run", &run);
    tree->SetBranchAddress("hel", &hel);
    tree->SetBranchAddress("MCmatch", &MCmatch);
    tree->SetBranchAddress("isGoodEventWithoutML",&isGoodEventWithoutML);
    tree->SetBranchAddress("chi2CutPiPlus", &chi2CutPiPlus);
    tree->SetBranchAddress("x", &x);
    tree->SetBranchAddress("y", &y);
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
    tree->SetBranchAddress("truex", &truex);
    tree->SetBranchAddress("trueQ2", &trueQ2);
    tree->SetBranchAddress("trueMx", &trueMx);
    tree->SetBranchAddress("truez", &truez);
    tree->SetBranchAddress("trueMgg", &trueMgg);
    tree->SetBranchAddress("trueMh", &trueMh);
    tree->SetBranchAddress("truephi_h", &truephi_h);
    tree->SetBranchAddress("truephi_R0", &truephi_R0);
    tree->SetBranchAddress("truephi_R1", &truephi_R1);
    tree->SetBranchAddress("trueth", &trueth);
    tree->SetBranchAddress("truepid_gamma1", &truepid_gamma1);
    tree->SetBranchAddress("truepid_gamma2", &truepid_gamma2);
    tree->SetBranchAddress("truepid_pion", &truepid_pion);
    tree->SetBranchAddress("trueparentpid_gamma1", &trueparentpid_gamma1);
    tree->SetBranchAddress("trueparentpid_gamma2", &trueparentpid_gamma2);
    tree->SetBranchAddress("trueparentpid_pion", &trueparentpid_pion);
    
    for (int i = 0; i < tree->GetEntries(); i++) {
      if(i%10000==0)
          cout << "Analyzed " << i << " of " << tree->GetEntries() << endl;
      tree->GetEntry(i);
      for(int j = 0 ; j < Nmax; j++) // Loop over BinningStructures
      {
          // Get multidimensional index for this event
          vector<int> index = binStructures.at(j).get_index(tree);
          // If index is valid, store data in bin
          if (index.size() == binStructures.at(j).bin_names.size()) {
              if(binStructures.at(j).bin_data_map[index].bintree==0){//create TTree
                  binStructures.at(j).SetBinNames(binStructures.at(j).bins,binStructures.at(j).bin_data_map);
                  binStructures.at(j).bin_data_map[index].bintree = new TTree(binStructures.at(j).bin_data_map[index].binName,binStructures.at(j).bin_data_map[index].binName);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("fgID", &binStructures.at(j).bin_data_map[index].entries);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("MCmatch",&MCmatch);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("run", &run);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("x",&x);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("y",&y);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("Q2",&Q2);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("Mx",&Mx);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("z",&z);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("hel", &hel);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("Mgg", &Mgg);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("Mh", &Mh);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("phi_h", &phi_h);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("phi_R0", &phi_R0);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("phi_R1", &phi_R1);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("th", &th);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("prob_g1", &prob_g1);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("prob_g2", &prob_g2);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("isGoodEventWithoutML",&isGoodEventWithoutML);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("truex",&truex);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("trueQ2",&trueQ2);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("trueMx",&trueMx);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("truez",&truez);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("trueMgg",&trueMgg);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("trueMh",&trueMh);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("truephi_h",&truephi_h);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("truephi_R0",&truephi_R0);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("truephi_R1",&truephi_R1);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("trueth",&trueth);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("chi2CutPiPlus",&chi2CutPiPlus);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("truepid_gamma1",&truepid_gamma1);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("truepid_gamma2",&truepid_gamma2);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("truepid_pion",&truepid_pion);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("trueparentpid_gamma1",&trueparentpid_gamma1);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("trueparentpid_gamma2",&trueparentpid_gamma2);
                  binStructures.at(j).bin_data_map[index].bintree->Branch("trueparentpid_pion",&trueparentpid_pion);
              }
                binStructures.at(j).bin_data_map[index].bintree->Fill();
                binStructures.at(j).bin_data_map[index].entries++;
          }
        }
    }
    
    for(int j = 0 ; j < Nmax ; j++){
        TFile *fout = new TFile(outfiles.at(j).c_str(),"RECREATE");
        BinningStructure BS = binStructures.at(j);
        // Save all bins to output TTree
        for (map<vector<int>, BinData>::iterator it = BS.bin_data_map.begin(); it != BS.bin_data_map.end(); ++it) {
            (it->second).bintree->Write();
            //cout << (it->second).bintree->GetEntries() << endl;
        }
        fout->Close();
    }
}