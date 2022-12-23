void ReadTTrees(const char * input_file){
    TFile *fin = new TFile(input_file,"READ");
    TIter iter(fin->GetListOfKeys());
    TKey *key;

    cout << "\n" << endl;
    while ((key = (TKey*)iter.Next())) {
        if (strstr(key->GetClassName(), "TTree")) {
            TTree *tree = (TTree*)key->ReadObj();
            cout << tree->GetName() << " ";
        }
    }
}