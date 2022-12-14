using namespace std;
using namespace RooFit;

class sPlotTools{
    public:
    // Constructor
    sPlotTools(const char *);
  
    void splot_pipluspi0(int, double, double, double);
    
    pair<vector<string>, vector<string>> get_modulations(int);
    
   
    private:
        TFile *_infile;
        string _input_dir;
        string _input_file;
};

