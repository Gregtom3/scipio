#include "BinningStructure.C"
#include "BinManager.C"
struct YAMLbinstruct {
    std::string name="";
    std::string parentDirectory="";
    int numDimensions=0;
    std::vector<std::string> dimensionNames;
    std::vector<std::vector<double>> binEdges;
    std::string injectName="";
    std::vector<std::string> injectsigFuncs;
    std::vector<std::string> injectbgFuncs;
};


int findIndexOfFile(const char * file, std::vector<YAMLbinstruct> binStructs){
    int index = -1;
    std::string fileName = file;
    int pos = fileName.find_last_of("/\\");
    fileName = fileName.substr(pos + 4);
    for (int i = 0; i < binStructs.size(); i++) {
        if (binStructs[i].name == fileName) {
            index = i;
            break;
        }
    }
    return index;
}

std::vector<YAMLbinstruct> get_structs(const char * inyaml){
    std::ifstream yamlFile(inyaml);
    std::vector<YAMLbinstruct> YAMLbinstructs;
    
    // Read line by line
    std::string line;
    YAMLbinstruct currentStructure;
    std::string injectName;
    std::vector<std::string> injectFuncs;
    while (std::getline(yamlFile, line)) {
        // Get the name of the binning structure
        if (line.find("name") != std::string::npos) {
            currentStructure.name = line.substr(line.find(":") + 2);
            continue;
        }
        
        // Get the parent directory
        if (line.find("parentDirectory") != std::string::npos) {
            currentStructure.parentDirectory = line.substr(line.find(":") + 2);
            continue;
        }
        
        // Get the number of dimensions
        if (line.find("numDimensions") != std::string::npos) {
            currentStructure.numDimensions = std::stoi(line.substr(line.find(":") + 2));
            continue;
        }
        
        // Get the dimension names
        if (line.find("dimensionNames") != std::string::npos) {
            line = line.substr(line.find("[") + 1, line.find("]") - line.find("[") - 1);
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            while (line.find(",") != std::string::npos) {
                currentStructure.dimensionNames.push_back(line.substr(0, line.find(",")));
                line = line.substr(line.find(",") + 1);
            }
            currentStructure.dimensionNames.push_back(line);
            continue;
        }
        
        // Get the bin edges
        if (line.find("binEdges") != std::string::npos) {
            std::getline(yamlFile, line);
            while (line.find("-") != std::string::npos) {
                std::vector<double> binEdge;
                line = line.substr(line.find("[") + 1, line.find("]") - line.find("[") - 1);
                while (line.find(",") != std::string::npos) {
                    binEdge.push_back(std::stod(line.substr(0, line.find(","))));
                    line = line.substr(line.find(",") + 1);
                }
                binEdge.push_back(std::stod(line));
                currentStructure.binEdges.push_back(binEdge);
                std::getline(yamlFile, line);
            }
        }
        
        // Get the name of the inject parameter
        if (line.find("injectLabel") != std::string::npos) {
            currentStructure.injectName = line.substr(line.find(":") + 2);
            continue;
        }
        cout << line << endl;
        // Get the inject functions for signal events
        if (line.find("inject_sigfuncs") != std::string::npos) {
            std::getline(yamlFile, line);
            while (line.find("-") != std::string::npos) {
                currentStructure.injectsigFuncs.push_back(line.substr(line.find("\"") + 1, line.find_last_of("\"") - line.find("\"") - 1));
                std::getline(yamlFile, line);
            }
        }
        // Get the inject functions for bg events
        if (line.find("inject_bgfuncs") != std::string::npos) {
            std::getline(yamlFile, line);
            while (line.find("-") != std::string::npos) {
                currentStructure.injectbgFuncs.push_back(line.substr(line.find("\"") + 1, line.find_last_of("\"") - line.find("\"") - 1));
                std::getline(yamlFile, line);
            }
            YAMLbinstructs.push_back(currentStructure);
            currentStructure = {};
        }
    }                                         
                                             
    yamlFile.close();
    
    // Print out the binning structures
    for (const auto& structure : YAMLbinstructs) {
        std::cout << "Name: " << structure.name << "\n"
                  << "Parent Directory: " << structure.parentDirectory << "\n"
                  << "Number of Dimensions: " << structure.numDimensions << "\n"
                  << "Dimension Names: ";
        for (const auto& name : structure.dimensionNames)
            std::cout << name << " ";
        std::cout << "\nBin Edges: ";
        for (const auto& edges : structure.binEdges) {
            std::cout << "[";
            for (const auto& edge : edges)
                std::cout << edge << " ";
            std::cout << "] ";
        }
        std::cout << "\nInject Name: " << structure.injectName << "\n" << "Inject SigFunctions: \n";
        for (const auto& func : structure.injectsigFuncs)
            std::cout << "\t - " << func << "\n";
        std::cout << "\nInject Name: " << structure.injectName << "\n" << "Inject BgFunctions: \n";
        for (const auto& func : structure.injectbgFuncs)
            std::cout << "\t - " << func << "\n";
        std::cout << "\n\n";
    }
    
    return YAMLbinstructs;
}

YAMLbinstruct get_struct(const char *inyaml, const char *file){ // Get the binstruct corresponding to the specifically binned file
    auto bs = get_structs(inyaml);
    return bs.at(findIndexOfFile(file,bs));
}

void ParseBinYaml(const char * input_data_wildcard = "",
                 const char * output_datadir = "",
                 const char * input_yaml="",
                 int isMC = 0,
                 int binNum = -1){
    
    
    // Create TChain for input_datadir
    TChain *inchain = new TChain("dihadron");
    inchain->Add(input_data_wildcard);
    
    std::vector<YAMLbinstruct> binStructs = get_structs(input_yaml);
    
    // for loop over all YAMLbinstructs
    int _binNum=0;
    for(YAMLbinstruct bs: binStructs){
        cout << "C" << endl;
        if(_binNum!=binNum && (binNum!=-1))
        {
            _binNum++; // If a specific binning is sought after, only run that one
            continue;
        }
        else{
            _binNum++;
        }
        // Check if the directory exists
        string output_subdir = (output_datadir+string("/")+bs.parentDirectory);
        try {
            gSystem->Exec(Form("mkdir %s",output_subdir.c_str()));
        }
        catch (std::exception &e) {
            cout << Form("Directory %s already exists...continuing...",output_subdir.c_str()) << endl;
        }
        // Create bins 
        map<string, vector<double>> bins;
        for(unsigned int i = 0 ; i < bs.dimensionNames.size() ; i++){
            bins[bs.dimensionNames.at(i)]=bs.binEdges.at(i);
        }
        
        cout << "Running " << bs.name << endl;
        // Use the BinningStructure.C skeleton to bin the dataset
        BinningStructure BS(bs.dimensionNames, bins);
	if(isMC==0)
	  BS.process_ttree(inchain,(output_subdir+"/"+string("nSidis_")+bs.name).c_str());
	else
	  BS.process_ttree(inchain,(output_subdir+"/"+string("MC_")+bs.name).c_str());
    }
    return;
    
}
