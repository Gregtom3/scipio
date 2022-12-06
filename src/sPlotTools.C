using namespace std;
using namespace RooFit;
void fit_unbinned_splot_pipi0(BinData binData) {
    // Create RooRealVars for the observables
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
    cout << weight_sig << " " << weight_bkg << endl;
    // Perform the unbinned fit to the azimuthal modulation
    RooRealVar A("A", "A", 0., -1., 1.);
    RooGenericPdf mod_model("mod_model", "1. + A*sin(phi_h-phi_R)",RooArgList(A,phi_h,phi_R));
    mod_model.fitTo(data_sweight, SumW2Error(true));
    // Clean up
    delete sData;
}