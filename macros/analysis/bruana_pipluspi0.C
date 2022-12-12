pair<vector<string>, vector<string>> get_modulations(int L = 2){
    vector<string> char_vec;
    vector<string> str_vec;
    for (int l = 0; l <= L; l++)
    {
        for (int m = 1; m <= l; m++)
        {
            
            string str = "@Pol[]*@hel[]*sin(" + to_string(m) + "*@phi_h[]-" + to_string(m) +"*@phi_R0[])";
            str_vec.push_back(str);
        }
        for (int m = -l; m <= l; m++)
        {
            string str = "@Pol[]*@hel[]*sin(" + to_string(1-m) +"*@phi_h[]+" + to_string(m) +"*@phi_R0[])";
            str_vec.push_back(str);
        }
    }
    
    // Remove duplicate entries
    sort(str_vec.begin(), str_vec.end());
    str_vec.erase(unique(str_vec.begin(), str_vec.end()), str_vec.end());
    
    // Add the mod line to the front
    for(unsigned int i = 0 ; i < str_vec.size(); i++){
        str_vec.at(i)="mod" + to_string(i) + "=" + str_vec.at(i);
    }
    
    int cidx=0;
    for (char c = 'A'; cidx<str_vec.size(); c++) 
    {
        string str = "";
        str += c;
        char_vec.push_back(str);
        cidx++;
    }
    
    return make_pair(char_vec, str_vec);
    
}

int bruana_pipluspi0(const char * input_file = "out.root",
           const char * outdir = "bru_out/",
           double threshold = 0.9,
           double Mggmin = 0.07,
           double Mggmax = 0.22)
{
    auto mods = get_modulations(2);
    vector<string> char_vec = mods.first;
    vector<string> str_vec = mods.second;

    // TFile containing many TTrees
    TFile *file = new TFile(input_file,"UPDATE");
    TIter iter(file->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)iter.Next())) {
    if (strstr(key->GetClassName(), "TTree")) {
        // Load the TTree
        TTree *tree = (TTree*)key->ReadObj();
        cout << "\n\n----------------------------------------------------\n\n";
        cout << "      sPlot on TTree " << tree->GetName() << "\n\n";
        cout << "----------------------------------------------------\n\n";
        
        sPlot RF;
        RF.SetUp().SetOutDir(Form("bru_out/%s/",tree->GetName()));
        ///////////////////////////////Load Variables
        RF.SetUp().LoadVariable(Form("Mgg[%f,%f]",Mggmin,Mggmax));//should be same name as variable in tree
        RF.SetUp().LoadAuxVar("prob_g1[0,10]");
        RF.SetUp().LoadAuxVar("prob_g2[0,10]");

        RF.SetUp().SetIDBranchName("fgID");

        /////////////////////////////Make Model Signal
        RF.SetUp().FactoryPDF("Gaussian::Signal( Mgg, mean[0.131,0.129,0.14], sigma[0.1,0.0001,0.2] )");
        RF.SetUp().LoadSpeciesPDF("Signal",1);

        ////////////////////////////////Additional background
        RF.SetUp().FactoryPDF("Chebychev::BG(Mgg,{a0[-0.1,-1,1],a1[0.1,-1,1]})");
        RF.SetUp().LoadSpeciesPDF("BG",1);

        RF.SetUp().AddCut(Form("prob_g1>%f",threshold));
        RF.SetUp().AddCut(Form("prob_g2>%f",threshold));
        ///////////////////////////Load Data
        RF.LoadData(tree->GetName(),input_file);

        //Run the fit here
        Here::Go(&RF);
        TCanvas *c = new TCanvas();

        RF.DrawWeighted("Mgg>>(100,0,0.25)","Signal");
        //compare to true signal
    
        c->SaveAs(Form("bru_out/%s/quickDiphotonSig.png",tree->GetName()));

        
        
        RF.DeleteWeightedTree();
        
        /////////////////////////////Create azimuthal modulation fit manager
        FitManager FM;
        FM.SetUp().SetOutDir(Form("bru_obs/%s/",tree->GetName()));
        ///////////////////////////////Load Variables
        FM.SetUp().LoadVariable("phi_h[-3.14159265,3.14159265]");
        FM.SetUp().LoadVariable("phi_R0[-3.14159265,3.14159265]");
        FM.SetUp().LoadCategory("hel[Polp=1,Polm=-1]");
        FM.SetUp().LoadAuxVar("prob_g1[0,10]");
        FM.SetUp().LoadAuxVar("prob_g2[0,10]");
        FM.SetUp().SetIDBranchName("fgID");
        FM.SetUp().AddCut(Form("prob_g1>%f",threshold));
        FM.SetUp().AddCut(Form("prob_g2>%f",threshold));
        ///////////////////////////////Load parameters
        for (string cc: char_vec)
            FM.SetUp().LoadParameter(Form("%s[0.0,-1,1]",cc.c_str()));
        FM.SetUp().LoadParameter("Pol[0.88,0,1]");
        ///////////////////////////////Load formulas
        for (string ss: str_vec)
            FM.SetUp().LoadFormula(ss);
        ///////////////////////////////Load PDF
        string pdf = "RooComponentsPDF::AziFit(1,{phi_h,phi_R0,hel,Pol},=";
        for(unsigned int i = 0 ; i < str_vec.size() ; i++){
            pdf+=char_vec.at(i);
            pdf+=";mod";
            pdf+=to_string(i);
            if(i==str_vec.size()-1)
                pdf+=")";
            else
                pdf+=":";
        }
        
        FM.SetUp().FactoryPDF(pdf);
        FM.SetUp().LoadSpeciesPDF("AziFit",1);

        FM.LoadData(tree->GetName(),input_file);
        FM.Data().LoadWeights("Signal",Form("bru_out/%s/Tweights.root",tree->GetName()));
        
        Here::Go(&FM);
                    
        }
    }
    return 0;
}