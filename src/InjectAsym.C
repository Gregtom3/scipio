
// Get Psi_i's for the A_LU
std::vector<TF2> get_mods(int L){
    
    int mod = 0;
    std::vector<TF2> mods;
    std::vector<string> str_vec;
    
    // Get strings of all the functions of the azimuthals
    // Useful for removing duplicates later
    for (int l = 0; l <= L; l++)
    {
        for (int m = 1; m <= l; m++)
        {
            string func = Form("sin(%d*x - %d*y)",m,m);
            str_vec.push_back(func);
            
        }
        for (int m = -l; m <= l; m++)
        {
            string func = Form("sin(%d*x + %d*y)",1-m,m);
            str_vec.push_back(func);
        }
    }
    
    // Remove duplicate entries
    sort(str_vec.begin(), str_vec.end());
    str_vec.erase(unique(str_vec.begin(), str_vec.end()), str_vec.end());
    
    // Fill the TF2 vector
    for(unsigned int i = 0 ; i < str_vec.size(); i++){
        mods.push_back(TF2(Form("mod%d",mod),str_vec.at(i).c_str(),-3.15,3.15,-3.15,3.15));
        mod++;
    }
    return mods;
}

// Get the A_LUs
std::vector<TF2> get_ALUs_sig(YAMLbinstruct bs){
    int mod = 0;
    std::vector<TF2> ALUs;
    for(std::string ALU_func: bs.injectsigFuncs){
        ALUs.push_back(TF2(Form("ALUsig%d",mod),ALU_func.c_str(),-100,100,-100,100));
        mod++;
    }
    return ALUs;
}

std::vector<TF2> get_ALUs_bg(YAMLbinstruct bs){
    int mod = 0;
    std::vector<TF2> ALUs;
    for(std::string ALU_func: bs.injectbgFuncs){
        ALUs.push_back(TF2(Form("ALUbg%d",mod),ALU_func.c_str(),-100,100,-100,100));
        mod++;
    }
    return ALUs;
}

// Inject the TTree with a helicity determined by probabilities
void injectTree(const char * input_file, const char * input_tree, int L, YAMLbinstruct bs, string version){
    TFile * f = new TFile(input_file,"READ");
    std::vector<TF2> mods = get_mods(L);
    std::vector<TF2> ALUs_sig = get_ALUs_sig(bs);
    std::vector<TF2> ALUs_bg = get_ALUs_bg(bs);
    // Get the binNames from the binStruct
    std::vector<string> binNames = bs.dimensionNames;
    // Open the TTree
    TTree *ttmp = (TTree*)f->Get(input_tree);
    
    // Dimension of the binning
    int dim = bs.numDimensions;
    // Set relevant branch addresses
    int hel;
    int MCmatch;
    int truePi0;
    int halftruePi0;
    int truepid_pion;
    float x;
    float y=0;
    float phih;
    float phiR;
    
    ttmp->SetBranchAddress("hel",&hel);
    ttmp->SetBranchAddress("truePi0",&truePi0);
    ttmp->SetBranchAddress("halftruePi0",&halftruePi0);
    ttmp->SetBranchAddress("truepid_pion",&truepid_pion);
    ttmp->SetBranchAddress("MCmatch",&MCmatch);
    ttmp->SetBranchAddress("truephi_h",&phih);
    ttmp->SetBranchAddress("truephi_R0",&phiR);
    ttmp->SetBranchAddress(binNames.at(0).c_str(),&x);
    if(dim>1){
        ttmp->SetBranchAddress(binNames.at(1).c_str(),&y);
    }
    // Create temporary tfile
    TFile *ftmp = new TFile(Form("%s_%s_%s_%s.root",input_file,input_tree,bs.injectName.c_str(),version.c_str()),"RECREATE");
    // Clone TTree
    TTree *t = ttmp->CloneTree(0);
    t->SetName(TString(t->GetName())+TString("_")+TString(bs.injectName.c_str()));
    // Loop through all events
    for ( int i = 0 ; i < ttmp->GetEntries()-1 ; i++){
    //for ( int i = 0 ; i < 10000 ; i++){
        ttmp->GetEntry(i);
        if(i%10000==0)
            cout << i << " " << ttmp->GetEntries()-1 << endl;
        
        // If not all Reco Particles had a reco match, skip event
        if(MCmatch!=1)
            continue;
        
        // If the event wasn't a pure pipi0 (ex: the two gammas came from separate pi0's)
        // use bg asymmetry
        float sum=0.0;
        if(truePi0==1&&abs(truepid_pion)==211){
            for (unsigned int j = 0 ; j < mods.size(); j++){
                sum+=ALUs_sig.at(j).Eval(x,y)*mods.at(j).Eval(phih,phiR);
            }
        }
        else
        {
            for (unsigned int j = 0 ; j < mods.size(); j++){
                sum+=ALUs_bg.at(j).Eval(x,y)*mods.at(j).Eval(phih,phiR);
            }
        }
        
        // Calculate the probability of positive helicity
        float prob = (1.0+sum)/2.0; // 0 < prob < 1 for small asymmetries
        
        // Set helicity
        if(gRandom->Uniform()<prob)
            hel=1;
        else
            hel=-1;
        t->Fill();
    }
    t->Write();
    ftmp->Close();
    f->Close();
}

