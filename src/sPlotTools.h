using namespace std;
using namespace RooFit;

class sPlotTools{
    public:
    // Constructor
    sPlotTools(){}
  
    // Perform unbinned Mdiphoton fit and then sWeighted fit of PiPi0 asymmetries
    void fit_unbinned_splot_pipi0(BinData);
        
    std::map<std::string, double> saveParamsAndErrors(RooAddPdf);
};

