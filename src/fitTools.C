#include "fitTools.h"

fitTools::fitTools(const char * input_dir, const char * input_file, const char * input_tree){
    _input_dir  = string(input_dir);
    _input_file = string(input_file);
    _infile = new TFile(input_file,"READ");
    _intree = (TTree*)_infile->Get(input_tree);
    
      
    cout << "\n\n----------------------------------------------------\n\n";
    cout << "      TFile: " << _infile->GetName() << "\n";
    cout << "      TTree: " << _intree->GetName() << "\n";
    cout << "      Entries: " << _intree->GetEntries() << "\n";
    cout << "----------------------------------------------------\n\n";

}


void fitTools::sideband_pipluspi0(int L, double threshold, double Mggmin, double Mggmax,
                                  double smin, double smax){
    
    /////////////////////////////Perform binned diphoton fit w/ sideband
    binned_pi0(threshold, Mggmin, Mggmax, smin, smax);

    /////////////////////////////Perform azimuthal modulation fits
    sideband_modulations(_input_file, threshold, Mggmin, Mggmax, smin, smax, L );

}


void fitTools::splot_pipluspi0(int L, double threshold, double Mggmin, double Mggmax){
    
    /////////////////////////////Perform unbinned diphoton fit w/ sPlot
    unbinned_pi0(_input_file, threshold, Mggmin, Mggmax);

    /////////////////////////////Perform azimuthal modulation fits
    splot_modulations(threshold, Mggmin, Mggmax, L);
}
        
void fitTools::unbinned_pi0(string _input_file, double threshold, double Mggmin, double Mggmax){
    
    sPlot RF;
    RF.SetUp().SetOutDir(Form("%s/bru_out/%s/",_input_dir.c_str(),_intree->GetName()));
    ///////////////////////////////Load Variables
    RF.SetUp().LoadVariable(Form("Mgg[%f,%f]",Mggmin,Mggmax));//should be same name as variable in tree
    RF.SetUp().LoadAuxVar("prob_g1[0,10]");
    RF.SetUp().LoadAuxVar("prob_g2[0,10]");
    RF.SetUp().LoadAuxVar("isGoodEventWithoutML[0,1]");

    RF.SetUp().SetIDBranchName("fgID");
    int k = 0;
    /////////////////////////////Make Model Signal
    RF.SetUp().FactoryPDF("Gaussian::Signal( Mgg, mean[0.131,0.129,0.14], sigma[0.1,0.0001,0.04] )");
    RF.SetUp().LoadSpeciesPDF("Signal",1);
    ////////////////////////////////Additional background
    RF.SetUp().FactoryPDF("Chebychev::BG(Mgg,{a0[-0.1,-1,1],a1[0.1,-1,1]})");
    RF.SetUp().LoadSpeciesPDF("BG",1);
    if(threshold!=1){ // Use ML
        RF.SetUp().AddCut(Form("prob_g1>%f",threshold));
        RF.SetUp().AddCut(Form("prob_g2>%f",threshold));
    }
    else              // Don't use ML
        RF.SetUp().AddCut("isGoodEventWithoutML==1");
    ///////////////////////////Load Data
    RF.LoadData(_intree->GetName(),_input_file.c_str());
    //Run the fit here
    Here::Go(&RF);
    TCanvas *c = new TCanvas();
    RF.DrawWeighted("Mgg>>(100,0,0.25)","Signal");
    //compare to true signal
    c->SaveAs(Form("%s/bru_out/%s/quickDiphotonSig.png",_input_dir.c_str(),_intree->GetName()));


    RF.DeleteWeightedTree();
    RF.Reset();
    
}

void fitTools::binned_pi0(double threshold, double Mggmin, double Mggmax,
                                  double smin, double smax){
    
    ///////////////////////////// Create directory for saving output
    gSystem->Exec(Form("mkdir -p %s/sdbnd_out/%s/",_input_dir.c_str(),_intree->GetName()));
    
    ///////////////////////////// Create histogram 
    TH1F *h = new TH1F("Mdiphoton",";Mdiphoton [GeV];",100,Mggmin,smax);
    
    ///////////////////////////// Draw into the histogram
    if(threshold!=1) // Use ML
        _intree->Draw("Mgg>>Mdiphoton",Form("prob_g1>%f&&prob_g2>%f&&(hel==1||hel==-1)",threshold,threshold),"goff");
    else             // Don't use ML
        _intree->Draw("Mgg>>Mdiphoton","isGoodEventWithoutML==1&&(hel==1||hel==-1)","goff");
    ///////////////////////////// Scale the histogram
    TH1F *hnorm = (TH1F*)h->Clone();
    hnorm->SetName("Mdiphoton_normed");
    hnorm->Scale(1/hnorm->Integral());
    
    ///////////////////////////// Find the minimum and maximum filled bin for bin range
    double xmin = 0;
    double xmax = smax;
    for(int i = 1 ; i <= h->GetNbinsX(); i++){
        float content = h->GetBinContent(i);
        if(content>0 && xmin==0){
            xmin=h->GetBinLowEdge(i);
        }
        if(content==0 && h->GetBinCenter(i)>Mggmax){
            xmax = h->GetBinLowEdge(i+1);
            break;
        }
    }
    
    ///////////////////////////// Create fit object 
    TF1 * f_sdbnd = new TF1("f_sdbnd","gaus(0)+pol2(3)",xmin,xmax);
    f_sdbnd->SetParameter(1,0.134);
    f_sdbnd->SetParLimits(0,0.001,100);
    f_sdbnd->SetParLimits(1,0.129,0.14);
    f_sdbnd->SetParLimits(2,0.007,0.04);
    
    ///////////////////////////// Perform fit
    hnorm->Fit(f_sdbnd,"R");
    
    ///////////////////////////// Calculate purity
    TF1 * sig = new TF1("sig","gaus(0)",Mggmin,Mggmax);
    TF1 * bg = new TF1("bg","pol2(0)",Mggmin,Mggmax);
    for(int j = 0 ; j < 3 ; j++){
        sig->SetParameter(j,f_sdbnd->GetParameter(j));
    }
    for(int k = 0 ; k < 3 ; k++){
        bg->SetParameter(k,f_sdbnd->GetParameter(k));
    }

    double u = 1-(bg->Integral(Mggmin,Mggmax)/sig->Integral(Mggmin,Mggmax));
    
    ///////////////////////////// Create TFile for saving output
    TFile *sdbnd_out = new TFile(Form("%s/sdbnd_out/%s/sideband.root",_input_dir.c_str(),_intree->GetName()),"RECREATE");
    TVectorD purity(1);
    purity[0]=u;
    TVectorD sigregion(2);
    sigregion[0]=Mggmin;
    sigregion[1]=Mggmax;
    sdbnd_out->WriteObject(&purity,"purity");
    sdbnd_out->WriteObject(&sigregion,"signal_region");
    f_sdbnd->Write();
    hnorm->Write();
    h->Write();
    sdbnd_out->Close();
    delete hnorm;
    delete h;
    delete f_sdbnd;
    delete sig;
    delete bg;
    hnorm=NULL;
    h=NULL;
    f_sdbnd=NULL;
    sig=NULL;
    bg=NULL;
}

void fitTools::process_azi_FM(FitManager &FM, double threshold, double min, double max, int L){
    
    auto mods = get_modulations(L);
    vector<string> char_vec = mods.first;
    vector<string> str_vec = mods.second;
    
    ///////////////////////////////Load Variables
    FM.SetUp().LoadVariable("phi_h[-3.14159265,3.14159265]");
    FM.SetUp().LoadVariable("phi_R0[-3.14159265,3.14159265]");
    FM.SetUp().LoadCategory("hel[Polp=1,Polm=-1]");
    FM.SetUp().LoadAuxVar("prob_g1[0,10]");
    FM.SetUp().LoadAuxVar("prob_g2[0,10]");
    FM.SetUp().LoadAuxVar("isGoodEventWithoutML[0,1]");
    FM.SetUp().LoadAuxVar("Mgg[0,10]");
    FM.SetUp().SetIDBranchName("fgID");
    if(threshold!=1){ // use ML
        FM.SetUp().AddCut(Form("prob_g1>%f",threshold));
        FM.SetUp().AddCut(Form("prob_g2>%f",threshold));
    }
    else // Don't use ML
        FM.SetUp().AddCut("isGoodEventWithoutML==1");
    FM.SetUp().AddCut(Form("Mgg>%f",min));
    FM.SetUp().AddCut(Form("Mgg<%f",max));
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
    
    return ;
}

void fitTools::splot_modulations(double threshold, double Mggmin, double Mggmax, int L){

    /////////////////////////////Create azimuthal modulation fit manager
    FitManager FM;
    FM.SetUp().SetOutDir(Form("%s/bru_obs/%s/",_input_dir.c_str(),_intree->GetName()));
    process_azi_FM(FM,threshold,Mggmin,Mggmax,L);
    
    FM.LoadData(_intree->GetName(),Form("%s/bru_out/%s/DataWeightedTree.root",_input_dir.c_str(),_intree->GetName()));
    FM.Data().LoadWeights("Signal",Form("%s/bru_out/%s/Tweights.root",_input_dir.c_str(),_intree->GetName()));
    Here::Go(&FM);
    FM.Reset();
}

void fitTools::sideband_modulations(string _input_file, double threshold, double Mggmin, double Mggmax,
                                    double smin, double smax, int L){
    
    ///////////////////////////// Create directory for saving output
    gSystem->Exec(Form("mkdir -p %s/sdbnd_obs/%s/",_input_dir.c_str(),_intree->GetName()));
    
    /////////////////////////////Create azimuthal modulation fit manager
    /////////////////////////////Do this for both the sigbg and bg region (diphoton and sdbnd)
    FitManager FM_sigbg;
    FM_sigbg.SetUp().SetOutDir(Form("%s/sdbnd_obs/%s/sigbg",_input_dir.c_str(),_intree->GetName()));
    process_azi_FM(FM_sigbg,threshold,Mggmin,Mggmax,L);
    FM_sigbg.LoadData(_intree->GetName(),_input_file.c_str());
    Here::Go(&FM_sigbg);
    FM_sigbg.Reset();
    
    FitManager FM_bg;
    FM_bg.SetUp().SetOutDir(Form("%s/sdbnd_obs/%s/bg",_input_dir.c_str(),_intree->GetName()));
    process_azi_FM(FM_bg,threshold, smin, smax, L);
    FM_bg.LoadData(_intree->GetName(),_input_file.c_str());
    Here::Go(&FM_bg);
    FM_bg.Reset();
    
    
}












pair<vector<string>, vector<string>> fitTools::get_modulations(int L){
    
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
