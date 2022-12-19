using namespace std;
using namespace RooFit;

class fitTools{
    public:
    // Constructor
    fitTools(const char *);
    // Main methods
    void splot_pipluspi0(int, double, double, double);
    void sideband_pipluspi0(int, double, double, double,
                            double, double);
    
    
    
    
    
    pair<vector<string>, vector<string>> get_modulations(int);
    
   
    private:
        TFile *_infile;
        string _input_dir;
        vector<string> _input_files;
    
        // Internal methods for computing splot fits
        void unbinned_pi0(TTree *, string,  double, double, double);
        void splot_modulations(TTree *, double, double, double, int);
    
        // Internal methods for computing sideband fits
        void binned_pi0(TTree *, double, double, double,
                        double, double);
        void sideband_modulations(TTree *, string, double, double, double,
                                  double, double , int);
    
        void process_azi_FM(FitManager &, double, double, double, int);
};

