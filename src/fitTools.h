using namespace std;
using namespace RooFit;

class fitTools{
    public:
    // Constructor
    fitTools(const char *);
  
    void splot_pipluspi0(int, double, double, double);
    
    pair<vector<string>, vector<string>> get_modulations(int);
    
   
    private:
        TFile *_infile;
        string _input_dir;
        vector<string> _input_files;
};

