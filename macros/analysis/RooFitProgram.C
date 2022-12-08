

using namespace RooFit;
void RooFitProgram(){
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);  
  // Threshold
  double threshold=0.9;

  RooRealVar rrv_threshold("threshold","threshold",threshold);
  
  // TFile containing many TTrees
  TFile *file = new TFile("out.root","UPDATE");
  TIter iter(file->GetListOfKeys());
  TKey *key;

  while ((key = (TKey*)iter.Next())) {
    if (strstr(key->GetClassName(), "TTree") && !strstr(key->GetName(), "sweight")) {
      // Load the TTree
      TTree *tree = (TTree*)key->ReadObj();

      //Create RooRealVars for Mgg, prob_g1, and prob_g2
      RooRealVar Mgg("Mgg","Mgg",0,1);
      RooRealVar prob_g1("prob_g1","prob_g1",0,1);
      RooRealVar prob_g2("prob_g2","prob_g2",0,1);

      //Create RooRealVars for hel, phi_h, and phi_R
      RooRealVar hel("hel","hel",-1,1);
      RooRealVar phi_h("phi_h","phi_h",-3.14159,3.14159);
      RooRealVar phi_R("phi_R0","phi_R0",-3.14159,3.14159);
  
  
      //Create RooDataSet and fill it with events with Mgg, prob_g1, and prob_g2 over the threshold
      RooDataSet data("data","data",RooArgSet(Mgg));
      //Create RooDataSet and fill it with events with hel, phi_h, phi_R, prob_g1, prob_g2, and both sweights
      RooDataSet data2("data2","data2",RooArgSet(hel,phi_h,phi_R));

      for(int i=0;i<tree->GetEntries();i++){
	tree->GetEntry(i);
	Mgg = tree->GetLeaf("Mgg")->GetValue();
	prob_g1 = tree->GetLeaf("prob_g1")->GetValue();
	prob_g2 = tree->GetLeaf("prob_g2")->GetValue();
	hel = tree->GetLeaf("hel")->GetValue();
	phi_h = tree->GetLeaf("phi_h")->GetValue();
	phi_R = tree->GetLeaf("phi_R0")->GetValue();

	if(prob_g1.getVal()>threshold && prob_g2.getVal()>threshold){
	  data.add(RooArgSet(Mgg));
	  data2.add(RooArgSet(hel,phi_h,phi_R));
	}

      }
      //Create RooGaussian for signal
      RooRealVar mean("mean","mean",0.133,0.128,0.14);
      RooRealVar sigma("sigma","sigma",0.1,0.01,0.2);
      RooGaussian signal("signal","signal",Mgg,mean,sigma);

      //Create RooPolynomial for background
      RooRealVar *a0 = new RooRealVar("a0","a0",0,-1,1);
      RooRealVar *a1 = new RooRealVar("a1","a1",0,-1,1);
      RooRealVar *a2 = new RooRealVar("a2","a2",0,-1,1);
      RooRealVar *a3 = new RooRealVar("a3","a3",0,-1,1);
      RooArgList *coeffList = new RooArgList(*a0,*a1,*a2,*a3);
      RooPolynomial background("background","background",Mgg,*coeffList);

      //Create RooAddPdf of signal and background
      RooRealVar sigFrac("sigFrac","sigFrac",0.5,0.,1e7);
      RooRealVar bgFrac("bgFrac","bgFrac",0.5,0.,1e7);
      RooAddPdf model("model","model",RooArgList(signal,background),RooArgList(sigFrac,bgFrac));

      //Perform SPlot fit
      RooFitResult *fitresult = model.fitTo(data,RooFit::Save(),"q");
      RooStats::SPlot sData("sData","sData",data,&model,RooArgList(sigFrac,bgFrac));
      //  std::cout << "Fit params and errors: " << sData.GetFitParams() << std::endl;
      //Save sWeights in TTree
      Double_t sWeightSignal;
      Double_t sWeightBackground;
  
      Double_t sWeightSignalArr[data.numEntries()];
      Double_t sWeightBackgroundArr[data.numEntries()];

      for (int i = 0; i < data.numEntries(); i++){
	sWeightSignalArr[i] = sData.GetSWeight(i,"sigFrac");
	sWeightBackgroundArr[i] = sData.GetSWeight(i,"bgFrac");
      }

      TTree *tree_new = tree->CloneTree(0);
      tree_new->Branch("sWeightSignal",&sWeightSignal,"sWeightSignal/D");
      tree_new->Branch("sWeightBackground",&sWeightBackground,"sWeightBackground/D");
      int k = 0;
      for (int i = 0 ; i < tree->GetEntries() ; i++){

	sWeightSignal=-999;
	sWeightBackground=-999;
	tree->GetEntry(i);

	prob_g1 = tree_new->GetLeaf("prob_g1")->GetValue();
	prob_g2 = tree_new->GetLeaf("prob_g2")->GetValue();
    
	if(prob_g1.getVal()>threshold && prob_g2.getVal()>threshold){
	  sWeightSignal=sWeightSignalArr[k];
	  sWeightBackground=sWeightBackgroundArr[k];
	  tree_new->Fill();
	  k++;
	}
    

	if(k==data.numEntries())
	  break;
      }
      //Create RooFit PDF
      RooRealVar A("A", "A", 0., -1., 1.);
      RooRealVar B("B", "B", 0., -1., 1.);
      RooRealVar C("C", "C", 0., -1., 1.);
      RooRealVar D("D", "D", 0., -1., 1.);
      RooRealVar E("E", "E", 0., -1., 1.);
      RooRealVar F("F", "F", 0., -1., 1.);
      RooRealVar G("G", "G", 0., -1., 1.);
      RooGenericPdf mod_model("mod_model", "1. + A*sin(2*phi_h-2*phi_R0)+B*sin(phi_h-phi_R0)+C*sin(-phi_h+2*phi_R0)+D*sin(phi_R0)+E*sin(phi_h)+F*sin(2*phi_h-phi_R0)+G*sin(3*phi_h-2*phi_R0)",RooArgList(A,B,C,D,E,F,G,phi_h,phi_R));

      //Perform 2D unbinned fit with weighting by signal sWeight
      RooFitResult *fitresult2 = mod_model.fitTo(data2,RooFit::Save(),RooFit::WeightVar("sWeightSignal"));

      // Write Trees and fitresults
      fitresult->SetName(Form("%s_diphoton_fitresult",tree->GetName()));
      fitresult2->SetName(Form("%s_7mod_fitresult",tree->GetName()));
      tree_new->SetName(Form("%s_sweight",tree->GetName()));  

      fitresult->Write(fitresult->GetName(),TObject::kOverwrite);
      fitresult2->Write(fitresult2->GetName(),TObject::kOverwrite);
      tree_new->Write(tree_new->GetName(),TObject::kOverwrite);

    }
    
  }
  
  rrv_threshold.Write("threshold",TObject::kOverwrite);
  file->Close();
  return;



  
  
}

