using namespace std;

class BinManager{
    private:
        // Vector of BinningStructures
        vector<BinningStructure> binStructures;
        // Vector of outfiles for each structure
        vector<string> outfiles;
    
    public:
        // Constructor
        BinManager(){}
    
        // Load in BinStructure
        void load(BinningStructure BS, string outfile){
            binStructures.push_back(BS);
            outfiles.push_back(outfile);
        }
    
        // Method to process the TChain
        void process(TChain *tree);
    
};