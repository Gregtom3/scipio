#include "sPlotTools.h"

/*void sPlotTools::fit_unbinned_splot_pipi0(BinData binData) {
    // ---------------------------------------------------------------
    
    //                        DIPHOTON FITTING
    
    // ---------------------------------------------------------------
    // Create RooRealVars for the observables
    RooRealVar hel("hel","hel",-2,2);
    RooRealVar phi_h("phi_h", "phi_h", 0, 2*TMath::Pi());
    RooRealVar phi_R("phi_R", "phi_R", 0, 2*TMath::Pi());
    RooRealVar Mdiphoton("Mdiphoton", "Mdiphoton", 0.08, 0.22);
    RooArgSet vars(Mdiphoton);
    // Create the RooDataSet
    std::vector<double> phi_h_vec = binData.phi_h;
    std::vector<double> phi_R_vec = binData.phi_R0;
    std::vector<double> Mdiphoton_vec = binData.Mgg;
    RooDataSet data("data", "data", vars);
    for (int i = 0; i < Mdiphoton_vec.size(); i++) {
      Mdiphoton.setVal(Mdiphoton_vec[i]);
      data.add(vars);
    }
    // Create the signal model
    RooRealVar mean("mean", "mean", 0.133, 0.08, 0.22);
    RooRealVar stddev("stddev", "stddev", 0.1, 0.001, 0.2);
    RooGaussian signal("signal", "signal", Mdiphoton, mean, stddev);
    // Create the background model
    RooRealVar a("a", "a", 0.1, -1., 1.);
    RooRealVar b("b", "b", 0.1, -1., 1.);
    RooPolynomial background("background", "background", Mdiphoton, RooArgSet(a, b));
    // Create the composite model
    RooRealVar Nsig("Nsig", "Nsig", 0, 1000);
    RooRealVar Nbkg("Nbkg", "Nbkg", 0, 1000);
    RooAddPdf model("model", "model", RooArgList(signal, background), RooArgList(Nsig, Nbkg));
    // Perform the fit
    model.fitTo(data, RooFit::Minimizer("Minuit2"));
    // Save diphoton fit parameters
    binData.parMap_diphoton = saveParamsAndErrors(model)
    // ---------------------------------------------------------------
    
    //                        SPLOT MODULATION FITTING
    
    // ---------------------------------------------------------------    
    // Perform the unbinned SPlot fit
    RooStats::SPlot* sData = new RooStats::SPlot("sData", "sData", data, &model, RooArgList(Nsig, Nbkg));
    // Create a weighted dataset
    RooRealVar sweight("sweight", "sweight", -100, 100);
    RooArgSet vars_sweight(phi_h,phi_R,sweight);
    RooDataSet data_sweight("data_sweight", "data_sweight", vars_sweight, WeightVar("sweight"));
    
    for (int i = 0; i < phi_h_vec.size(); i++) {
      phi_h.setVal(phi_h_vec[i]);
      phi_R.setVal(phi_R_vec[i]);
      sweight.setVal(sData->GetSWeight(i,"Nsig"));
      data_sweight.add(vars_sweight, sweight.getVal());
    }
    // Get the weights and save them to the TTree
    Double_t weight_sig = sData->GetYieldFromSWeight("Nsig");
    Double_t weight_bkg = sData->GetYieldFromSWeight("Nbkg");
    // Perform the unbinned fit to the azimuthal modulation
    RooRealVar A("A", "A", 0., -1., 1.);
    RooRealVar B("B", "B", 0., -1., 1.);
    RooRealVar C("C", "C", 0., -1., 1.);
    RooRealVar D("D", "D", 0., -1., 1.);
    RooRealVar E("E", "E", 0., -1., 1.);
    RooRealVar F("F", "F", 0., -1., 1.);
    RooRealVar G("G", "G", 0., -1., 1.);
    RooGenericPdf mod_model("mod_model", "1. + A*sin(2*phi_h-2*phi_R)+B*sin(phi_h-phi_R)+C*sin(-phi_h+2*phi_R)+D*sin(phi_R)+E*sin(phi_h)+F*sin(2*phi_h-phi_R)+G*sin(3*phi_h-2*phi_R)",RooArgList(A,B,C,D,E,F,G,phi_h,phi_R));
    mod_model.fitTo(data_sweight, SumW2Error(true));
    // Save splot fit parameters
    binData.parMap_7mod = saveParamsAndErrors(mod_model)
    // Clean up
    delete sData;
}
*/
std::map<std::string, double> sPlotTools::saveParamsAndErrors(RooAddPdf model) {
    // Create the std::map to store the parameters and errors
    std::map<std::string, double> fit_params;
    RooArgList params = model.getParameters(data);
    int n_params = params.getSize();
    
    for (int i = 0; i < n_params; i++) {
        RooRealVar* param = (RooRealVar*) params.at(i);
        std::string name = param->GetName();
        double value = param->getVal();
        double error = res->correlation(*param);
        fit_params.insert(std::pair<std::string, double>(name, value));
        fit_params.insert(std::pair<std::string, double>(name + "_error", error));
    }
    
    return fit_params;
}



void ReadinTFileTree(TFile *file, TTree *tree)
{
  // Read in the TFile and TTree
  tree = (TTree*)file->Get("tree");
  double phi_h, phi_R, Mdiphoton;
  tree->SetBranchAddress("phi_h", &phi_h);
  tree->SetBranchAddress("phi_R", &phi_R);
  tree->SetBranchAddress("Mdiphoton", &Mdiphoton);
  tree->GetEntry(0);
}

void PerformSPlotFit(TFile *file, TTree *tree, RooDataSet *data, RooRealVar *x, RooRealVar *mean, RooRealVar *stddev, RooRealVar *A, RooRealVar *hel, RooArgSet *varset, RooAddPdf *model, RooFitResult *fitres)
{
  // Read in the TFile and TTree
  ReadinTFileTree(file, tree);
  // Initialize RooRealVars
  x = new RooRealVar("Mdiphoton","Mdiphoton",0.08,0.22);
  mean = new RooRealVar("mean","mean of gaussian",0.133,0.08,0.22);
  stddev = new RooRealVar("stddev","stddev of gaussian",0.1,0.001,0.2);
  A = new RooRealVar("A","Amp of modulation",0.1,-2.,2.);
  hel = new RooRealVar("hel","hel of modulation",-1.);
  // Create the RooDataSet
  data = new RooDataSet("data","data",tree,*x);
  // Create the signal pdf
  RooGaussian gauss("gauss","gauss(x,mean,stddev)",*x,*mean,*stddev);
  // Create the background pdf
  RooPolynomial poly("poly","poly(x)",*x);
  // Add the signal and background pdfs
  varset = new RooArgSet(*x);
  model = new RooAddPdf("model","model",RooArgList(gauss,poly),RooArgList(*x));
  // Perform the sPlot fit to the Mdiphoton distribution
  fitres = model->fitTo(*data,Save(),Minos(kFALSE));
  // Calculate the sWeights
  RooStats::SPlot *sData = new RooStats::SPlot("sData","An SPlot",*data,model,RooArgList(*x));
  // Adding the sWeights to the TTree
  tree->Branch("sWeight",&sData->GetYieldFromSWeight());
}

void PerformAzimuthalFit(TFile *file, TTree *tree, RooRealVar *phi_h, RooRealVar *phi_R, RooRealVar *A, RooRealVar *hel, RooArgSet *varset, RooAddPdf *modAzimuthal, RooFitResult *fitres)
{
  // Read in the TFile and TTree
  ReadinTFileTree(file, tree);
  // Initialize RooRealVars
  phi_h = new RooRealVar("phi_h","phi_h",-M_PI,M_PI);
  phi_R = new RooRealVar("phi_R","phi_R",-M_PI,M_PI);
  A = new RooRealVar("A","Amp of modulation",0.1,-2.,2.);
  hel = new RooRealVar("hel","hel of modulation",-1.);
  // Create the azimuthal pdf
  RooFormulaVar f("f","1+hel*A*sin(phi_h-phi_R)",RooArgList(*hel,*A,*phi_h,*phi_R));
  // Add the pdf to the RooArgSet
  varset = new RooArgSet(*phi_h,*phi_R);
  modAzimuthal = new RooAddPdf("modAzimuthal","modAzimuthal",RooArgList(f),RooArgList(*varset));
  // Perform the azimuthal fit, weighted by the sWeights
  fitres = modAzimuthal->fitTo(*data,Weights(sData->GetYieldFromSWeight()),Save(),Minos(kFALSE));
}

// Initialize the TFile and TTree
  TFile *file = TFile::Open("example.root");
  TTree *tree = new TTree();
  // Initialize RooRealVars and RooDataSets
  RooRealVar *x, *mean, *stddev, *A, *hel;
  RooDataSet *data;
  // Initialize RooArgSets and RooAddPdfs
  RooArgSet *varset;
  RooAddPdf *model, *modAzimuthal;
  // Initialize RooFitResults
  RooFitResult *fitres;
  // Perform SPlot fit
  PerformSPlotFit(file, tree, data, x, mean, stddev, A, hel, varset, model, fitres);
  // Perform azimuthal fit
  PerformAzimuthalFit(file, tree, phi_h, phi_R, A, hel, varset, modAzimuthal, fitres);
  return 0;