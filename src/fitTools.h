using namespace std;
using namespace RooFit;

class fitTools{
    public:
    // Constructor
    fitTools(const char *, const char *, const char *);
    // Main methods
    void splot_pipluspi0(int, double, double, double);
    void sideband_pipluspi0(int, double, double, double,
                            double, double);
    
    
    
    
    
    pair<vector<string>, vector<string>> get_modulations(int);
    
   
    private:
        TFile *_infile;
        TTree *_intree;
        string _input_file;
        string _input_dir;
    
        // Internal methods for computing splot fits
        void unbinned_pi0(string,  double, double, double);
        void splot_modulations(double, double, double, int);
    
        // Internal methods for computing sideband fits
        void binned_pi0(double, double, double,
                        double, double);
        void sideband_modulations(string, double, double, double,
                                  double, double , int);
    
        void process_azi_FM(FitManager &, double, double, double, int);
    
};

