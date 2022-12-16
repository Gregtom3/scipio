#include "sPlotTools.h"

sPlotTools::sPlotTools(const char * input_dir){
    _input_dir = string(input_dir);
    TSystemDirectory dir(input_dir,input_dir);
    TList *files = dir.GetListOfFiles();
    if(files){
        TSystemFile *file;
        TString fname;
        TIter next(files);
        while((file=(TSystemFile*)next())){
            fname=file->GetName();
            if(fname.EndsWith(".root")){
                _input_files.push_back(string(input_dir)+"/"+string(fname));
            }
        }
    }
}

void sPlotTools::splot_pipluspi0(int L, double threshold, double Mggmin, double Mggmax){
    
    auto mods = get_modulations(L);
    vector<string> char_vec = mods.first;
    vector<string> str_vec = mods.second;
    for(string _input_file: _input_files){
        cout << "\n\n\n\n\n Analyzing file: " << _input_file << "\n\n\n\n\n";
        _infile = new TFile(_input_file.c_str(),"READ");
        // TFile containing many TTrees
        TIter iter(_infile->GetListOfKeys());
        TKey *key;

        while ((key = (TKey*)iter.Next())) {
        if (strstr(key->GetClassName(), "TTree")) {
            // Load the TTree
            TTree *tree = (TTree*)key->ReadObj();
            cout << "\n\n----------------------------------------------------\n\n";
            cout << "      sPlot on TTree " << tree->GetName() << "\n\n";
            cout << "----------------------------------------------------\n\n";

            sPlot RF;
            RF.SetUp().SetOutDir(Form("%s/bru_out/%s/",_input_dir.c_str(),tree->GetName()));
            ///////////////////////////////Load Variables
            RF.SetUp().LoadVariable(Form("Mgg[%f,%f]",Mggmin,Mggmax));//should be same name as variable in tree
            RF.SetUp().LoadAuxVar("prob_g1[0,10]");
            RF.SetUp().LoadAuxVar("prob_g2[0,10]");

            RF.SetUp().SetIDBranchName("fgID");
            int k = 0;
            /////////////////////////////Make Model Signal
            RF.SetUp().FactoryPDF("Gaussian::Signal( Mgg, mean[0.131,0.129,0.14], sigma[0.1,0.0001,0.2] )");
            RF.SetUp().LoadSpeciesPDF("Signal",1);
            ////////////////////////////////Additional background
            RF.SetUp().FactoryPDF("Chebychev::BG(Mgg,{a0[-0.1,-1,1],a1[0.1,-1,1]})");
            RF.SetUp().LoadSpeciesPDF("BG",1);

            RF.SetUp().AddCut(Form("prob_g1>%f",threshold));
            RF.SetUp().AddCut(Form("prob_g2>%f",threshold));
            ///////////////////////////Load Data
            RF.LoadData(tree->GetName(),_input_file.c_str());
            //Run the fit here
            Here::Go(&RF);
            TCanvas *c = new TCanvas();
            RF.DrawWeighted("Mgg>>(100,0,0.25)","Signal");
            //compare to true signal
            c->SaveAs(Form("%s/bru_out/%s/quickDiphotonSig.png",_input_dir.c_str(),tree->GetName()));


            RF.DeleteWeightedTree();
            /////////////////////////////Create azimuthal modulation fit manager
            FitManager FM;
            FM.SetUp().SetOutDir(Form("%s/bru_obs/%s/",_input_dir.c_str(),tree->GetName()));
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
            //FM.SetUp().LoadParameter("Pol[0.88]");
            ///////////////////////////////Load formulas
            for (string ss: str_vec)
                FM.SetUp().LoadFormula(ss);
            ///////////////////////////////Load PDF
            string pdf = "RooComponentsPDF::AziFit(1,{phi_h,phi_R0,hel},=";
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
            FM.LoadData(tree->GetName(),Form("%s/bru_out/%s/DataWeightedTree.root",_input_dir.c_str(),tree->GetName()));
            FM.Data().LoadWeights("Signal",Form("%s/bru_out/%s/Tweights.root",_input_dir.c_str(),tree->GetName()));
            Here::Go(&FM);

            }
        }
    }
}
pair<vector<string>, vector<string>> sPlotTools::get_modulations(int L){
    
    vector<string> char_vec;
    vector<string> str_vec;
    for (int l = 0; l <= L; l++)
    {
        for (int m = 1; m <= l; m++)
        {
            
            string str = "0.8692*@hel[]*sin(" + to_string(m) + "*@phi_h[]-" + to_string(m) +"*@phi_R0[])";
            str_vec.push_back(str);
        }
        for (int m = -l; m <= l; m++)
        {
            string str = "0.8692*@hel[]*sin(" + to_string(1-m) +"*@phi_h[]+" + to_string(m) +"*@phi_R0[])";
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