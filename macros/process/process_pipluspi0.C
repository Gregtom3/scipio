#include "../../src/Kinematics.C"

int process_pipluspi0(const char * input_file = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/postprocess_catboost/MC_3051_0.root",
		      const char * output_file = "test.root",
		      const float beamE = 10.604,
		      const int maxEvents = 999999999){

  // Input tfile and tree
  TFile *infile = new TFile(input_file);
  TTree *intree = (TTree*)infile->Get("RawEvents");
  
  //declare all necessary variables
  float x, Q2, W;
  int nPart=100;
  float px[nPart], py[nPart], pz[nPart], E[nPart], theta[nPart], eta[nPart], phi[nPart];
  vector<float> * catboost_weight = 0;
  int pid[nPart];
  //link the TBranches to the variables
  intree->SetBranchAddress("x",&x);
  intree->SetBranchAddress("Q2",&Q2);
  intree->SetBranchAddress("W",&W);
  intree->SetBranchAddress("nPart",&nPart);
  intree->SetBranchAddress("px",px);
  intree->SetBranchAddress("py",py);
  intree->SetBranchAddress("pz",pz);
  intree->SetBranchAddress("E",E);
  intree->SetBranchAddress("pid",pid);
  intree->SetBranchAddress("theta",theta);
  intree->SetBranchAddress("eta",eta);
  intree->SetBranchAddress("phi",phi);
  intree->SetBranchAddress("catboost_weight",&catboost_weight);
  
  // Create a TFile and TTree
  TFile* outfile = new TFile(output_file,"RECREATE");
  TTree* outtree = new TTree("dihadron","dihadron");

  // Declare branch variables
  Float_t Mgg, Mh, phi_h, phi_R0, phi_R1, th, zpiplus, zpi0, xFpiplus, xFpi0, prob_g1, prob_g2, z, xF;

  // Create branches
  outtree->Branch("x", &x, "x/F");
  outtree->Branch("Q2", &Q2, "Q2/F");
  outtree->Branch("W", &W, "W/F");
  outtree->Branch("Mgg", &Mgg, "Mgg/F");
  outtree->Branch("Mh", &Mh, "Mh/F");
  outtree->Branch("phi_h", &phi_h, "phi_h/F");
  outtree->Branch("phi_R0", &phi_R0, "phi_R0/F");
  outtree->Branch("phi_R1", &phi_R1, "phi_R1/F");
  outtree->Branch("th", &th, "th/F");
  outtree->Branch("zpiplus", &zpiplus, "zpiplus/F");
  outtree->Branch("zpi0", &zpi0, "zpi0/F");
  outtree->Branch("z", &z, "z/F");
  outtree->Branch("xFpiplus", &xFpiplus, "xFpiplus/F");
  outtree->Branch("xFpi0", &xFpi0, "xFpi0/F");
  outtree->Branch("xF", &xF, "xF/F");
  outtree->Branch("prob_g1", &prob_g1, "prob_g1/F");
  outtree->Branch("prob_g2", &prob_g2, "prob_g2/F");

  // Kinematics Object
  Kinematics kin;
  
  // Initial particles
  TLorentzVector init_electron(0,0,sqrt(beamE*beamE-0.000511*0.000511),beamE);
  TLorentzVector init_target(0,0,0,0.938272);
  
  //loop over the tree entries 
  for (Int_t i=0; i<intree->GetEntries(); i++){
    intree->GetEntry(i);
    
    // Find electron
    TLorentzVector electron;
    for (Int_t j=0; j<nPart; j++){
      if(pid[j]==11)
	electron.SetPxPyPzE(px[j],py[j],pz[j],E[j]);
    }
    if(electron.E()==0){
      cout << "ERROR: Lost an electron?" << endl;
      continue; // no electron found
    }
    TLorentzVector q = init_electron-electron;
    
    //loop over all particles in the event
    for (Int_t j=0; j<nPart; j++){
      TLorentzVector photon1;
      TLorentzVector photon2;
      if (pid[j]==22){ //if this particle is a photon
	//fill the 4-momentum of the second photon
	photon1.SetPxPyPzE(px[j],py[j],pz[j],E[j]);
	prob_g1=catboost_weight->at(j); // catboost weight
	// loop over all particles again
	for (Int_t k=j+1; k<nPart;k++){
	  if(pid[k]!=22)continue; // if this particle is not a photon, continue
	  photon2.SetPxPyPzE(px[k],py[k],pz[k],E[k]);
	  prob_g2=catboost_weight->at(k); // catboost weight
	  TLorentzVector diphoton = photon1 + photon2;
	  // loop over all particles again
	  for (Int_t m=0;m<nPart;m++){
	    if(pid[m]!=211)continue; //if this particle is not a pi+, continue
	    //fill the 4-momentum of the pion
	    TLorentzVector pion(px[m],py[m],pz[m],E[m]);	    
	    // form dihadron
	    TLorentzVector dihadron = pion + diphoton;

	    // fill results
	    Mgg = diphoton.M();
	    Mh = dihadron.M();
	    phi_h = kin.phi_h(q,electron,diphoton,pion);
	    phi_R0 = kin.phi_R(q,electron,diphoton,pion,0);
	    phi_R1 = kin.phi_R(q,electron,diphoton,pion,1);
	    th     = kin.com_th(diphoton,pion);
	    xFpiplus = kin.xF(q,pion,init_target,W);
	    xFpi0 = kin.xF(q,diphoton,init_target,W);
	    xF     = kin.xF(q,dihadron,init_target,W);
	    zpiplus = kin.z(init_target,pion,q);
	    zpi0 = kin.z(init_target,diphoton,q);
	    z = zpiplus+zpi0;
	    
	    // fill tree
	    outtree->Fill();
	  }
	}
      }
    }	
  }

  // write the outtree
  outtree->Write();
  //close the files
  infile->Close();
  outfile->Close();
  return 0;
}
