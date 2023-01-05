// void ReadTTrees(const char * input_file){
//     TFile *fin = new TFile(input_file,"READ");
//     TIter iter(fin->GetListOfKeys());
//     TKey *key;
//     while ((key = (TKey*)iter.Next())) {
//         if (strstr(key->GetClassName(), "TTree")) {
//             TTree *tree = (TTree*)key->ReadObj();
//             cout << tree->GetName() << " ";
//         }
//     }
// }

void ReadTTrees(const char * input_file){
    TFile *fin = new TFile(input_file,"READ");
    TIter iter(fin->GetListOfKeys());
    TKey *key;
    set<string> ttrees; //Create a set to store the TTree names
    while ((key = (TKey*)iter.Next())) {
        if (strstr(key->GetClassName(), "TTree")) {
            TTree *tree = (TTree*)key->ReadObj();
            if (ttrees.find(tree->GetName()) == ttrees.end()) { //Check if the TTree name is already in the set
                ttrees.insert(tree->GetName()); //Insert the TTree name into the set
                cout << tree->GetName() << " ";
            }
        }
    }
}