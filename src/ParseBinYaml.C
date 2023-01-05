#include "BinningStructure.C"

struct YAMLbinstruct {
    std::string name;
    std::string parentDirectory;
    int numDimensions;
    std::vector<std::string> dimensionNames;
    std::vector<std::vector<double>> binEdges;
};

std::vector<YAMLbinstruct> get_structs(const char * inyaml){
    std::ifstream yamlFile(inyaml);
    std::vector<YAMLbinstruct> YAMLbinstructs;
    
    // Read line by line
    std::string line;
    YAMLbinstruct currentStructure;
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
        std::cout << "\n\n";
    }
    
    return YAMLbinstructs;
}

void ParseBinYaml(const char * input_data_wildcard = "",
                 const char * output_datadir = "",
                 const char * input_yaml="",
                 int isMC = 0,
                 string version = ""){
    
    // Create TChain for input_datadir
    TChain *inchain = new TChain("dihadron");
    inchain->Add(input_data_wildcard);
    
    std::vector<YAMLbinstruct> binStructs = get_structs(input_yaml);
    
    // for loop over all YAMLbinstructs
    for(YAMLbinstruct bs: binStructs){
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
        
        // Use the BinningStructure.C skeleton to bin the dataset
        BinningStructure BS(bs.dimensionNames, bins);
        if(isMC==0)
            BS.process_ttree(inchain,(output_subdir+"/"+string("nSidis_")+bs.name).c_str(),"");
        else
            BS.process_ttree(inchain,(output_subdir+"/"+string("MC_")+bs.name).c_str(),"");
//         if(isMC==0){
//             if (version == ""){
//                 BS.process_ttree(inchain,(output_subdir+"/"+string("nSidis_")+bs.name).c_str(),"");
//             }
//             else if (version == "Fall2018_inbending"){
//                 BS.process_ttree(inchain,(output_subdir+"/"+string("nSidis_Fall2018_inbending_")+bs.name).c_str(),"Fall2018_inbending");
//             }
//             else if (version == "Fall2018_outbending"){
//                 BS.process_ttree(inchain,(output_subdir+"/"+string("nSidis_Fall2018_outbending_")+bs.name).c_str(),"Fall2018_outbending");
//             }
//             else if (version == "Spring2019_inbending"){
//                 BS.process_ttree(inchain,(output_subdir+"/"+string("nSidis_Spring2019_inbending_")+bs.name).c_str(),"Spring2019_inbending");
//             }
//         }
//         else{
//             if (version == "") {
//                 BS.process_ttree(inchain,(output_subdir+"/"+string("MC_")+bs.name).c_str(),"");
//             } else if (version == "MC_inbending") {
//                 BS.process_ttree(inchain,(output_subdir+"/"+string("MC_inbending_")+bs.name).c_str(),"MC_inbending");
//             } else if (version == "MC_outbending") {
//                 BS.process_ttree(inchain,(output_subdir+"/"+string("MC_outbending_")+bs.name).c_str(),"MC_outbending");
//             }
//         }
    }
    
    return;
    
}