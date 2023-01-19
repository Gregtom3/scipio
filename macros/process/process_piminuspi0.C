#include "../../src/Kinematics.C"


bool piplus_chi2_cut(int pid, float p, float chi2,int isStrict=0){
  bool passChargedPionChi2=false;
  if(pid==211||pid==-211){
    // Determine pion charge dependent C value
    float C = 0.0;
    (pid==211 ? C=0.88 : C=0.93);
    // 2 different pion chi2pid regions
    // standard
    // strict
    if(isStrict==0){
      if(p<2.44)
	passChargedPionChi2=chi2<C*3;
      else
	passChargedPionChi2=chi2<C*(0.00869 + 14.98587 * exp(-p/1.18236) + 1.81751 * exp(-p/4.86394));
    } 
    else if(isStrict==1){
      if(p<2.44)
	passChargedPionChi2=chi2<C*3;
      else if(p<4.6)
	passChargedPionChi2=chi2< C * (0.00869 + 14.98587 * exp(-p/1.18236) + 1.81751 * exp(-p/4.86394));
      else
	passChargedPionChi2=chi2< C * (-1.14099 + 24.14992 * exp(-p/1.36554) + 2.66876 * exp(-p/6.80552));

    }
  }
  return passChargedPionChi2;
}
int process_piminuspi0(
		              const char * input_file = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/projects/pipluspi0_prelim/catboost/MLoutput/nSidis_5032.root",
                      const char * output_file = "/volatile/clas12/users/gmat/clas12analysis.sidis.data/rga/ML/projects/pipluspi0_prelim/catboost/postprocess/nSidis_5032.root",
		      const float beamE = 10.604){

  // Input tfile and tree
  TFile *infile = new TFile(input_file);
  TTree *intree = (TTree*)infile->Get("PostProcessedEvents");
  
  //declare all necessary variables
  float x, Q2, W, Pol,y;
  float truex, trueQ2, trueW, truey;
  int hel,run;
  int nPart=100;
  float px[nPart], py[nPart], pz[nPart], E[nPart], vz[nPart], chi2[nPart], theta[nPart], eta[nPart], phi[nPart], catboost_weight[nPart];
  float truepx[nPart] , truepy[nPart] , truepz[nPart], trueE[nPart], truetheta[nPart], trueeta[nPart], truephi[nPart];
  int pid[nPart] ,parentID[nPart],parentPID[nPart];
  int truepid[nPart];
  //link the TBranches to the variables
  intree->SetBranchAddress("run",&run);
  intree->SetBranchAddress("Pol",&Pol);
  intree->SetBranchAddress("hel",&hel);
  intree->SetBranchAddress("MCmatch_parent_id", &parentID);
  intree->SetBranchAddress("MCmatch_parent_pid", &parentPID);
  intree->SetBranchAddress("x",&x);
  intree->SetBranchAddress("Q2",&Q2);
  intree->SetBranchAddress("W",&W); 
  intree->SetBranchAddress("y",&y); 
  intree->SetBranchAddress("nPart",&nPart);
  intree->SetBranchAddress("px",px);
  intree->SetBranchAddress("py",py);
  intree->SetBranchAddress("pz",pz);
  intree->SetBranchAddress("E",E);
  intree->SetBranchAddress("vz",vz);
  intree->SetBranchAddress("chi2",chi2);
  intree->SetBranchAddress("pid",pid);
  intree->SetBranchAddress("theta",theta);
  intree->SetBranchAddress("eta",eta);
  intree->SetBranchAddress("phi",phi);
  intree->SetBranchAddress("truex",&truex);
  intree->SetBranchAddress("truey",&truey);
  intree->SetBranchAddress("trueQ2",&trueQ2);
  intree->SetBranchAddress("trueW",&trueW);
  intree->SetBranchAddress("trueE",&trueE);
  intree->SetBranchAddress("truepx",&truepx);
  intree->SetBranchAddress("truepy",&truepy);
  intree->SetBranchAddress("truepz",&truepz);
  intree->SetBranchAddress("truetheta",&truetheta);
  intree->SetBranchAddress("trueeta",&trueeta);
  intree->SetBranchAddress("truephi",&truephi);
  intree->SetBranchAddress("truepid",truepid);
  intree->SetBranchAddress("catboost_weight",catboost_weight);
  
  // Create a TFile and TTree
  TFile* outfile = new TFile(output_file,"RECREATE");
  TTree* outtree = new TTree("dihadron","dihadron");

  // Declare branch variables
  Float_t Mgg, Mh, phi_h, phi_R0, phi_R1, th, zpiplus, zpi0, xFpiplus, xFpi0, prob_g1, prob_g2, z, xF, Mx;
  Float_t trueMgg, trueMh, truephi_h, truephi_R0, truephi_R1, trueth, truezpiplus, truezpi0, truexFpiplus, truexFpi0, truez, truexF, trueMx;
  int truePi0=-1;
  int halftruePi0=-1;
  int isGoodEventWithoutML = 0;
  int chi2CutPiPlus = 0;
  int truepid_gamma1 = 0;
  int truepid_gamma2 = 0;
  int truepid_pion   = 0;
  int trueparentpid_gamma1 = 0;
  int trueparentpid_gamma2 = 0;
  int trueparentpid_pion   = 0;
  Float_t eE, eth, ephi;
  Float_t g1E, g2E, piE;
  Float_t g1th, g2th, pith;
  Float_t g1phi, g2phi, piphi;  
  Float_t trueeE, trueeth, trueephi;
  Float_t trueg1E, trueg2E, truepiE;
  Float_t trueg1th, trueg2th, truepith;
  Float_t trueg1phi, trueg2phi, truepiphi;
  
  int MCmatch = 0; // True if all important particles had a Monte Carlo match
  // Create branches
  outtree->Branch("run", &run, "run/I");
  outtree->Branch("Pol", &Pol, "Pol/F");
  outtree->Branch("hel", &hel, "hel/I");
  outtree->Branch("MCmatch", &MCmatch, "MCmatch/I");
  outtree->Branch("truePi0", &truePi0, "truePi0/I");
  outtree->Branch("halftruePi0", &halftruePi0, "halftruePi0/I");
  outtree->Branch("x", &x, "x/F");
  outtree->Branch("Q2", &Q2, "Q2/F");
  outtree->Branch("W", &W, "W/F");
  outtree->Branch("y", &y, "y/F");
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
  outtree->Branch("Mx", &Mx, "Mx/F");
  outtree->Branch("truex", &truex, "truex/F");
  outtree->Branch("trueQ2", &trueQ2, "trueQ2/F");
  outtree->Branch("trueW", &trueW, "trueW/F");
  outtree->Branch("truey", &truey, "truey/F");
  outtree->Branch("trueMgg", &trueMgg, "trueMgg/F");
  outtree->Branch("trueMh", &trueMh, "trueMh/F");
  outtree->Branch("truephi_h", &truephi_h, "truephi_h/F");
  outtree->Branch("truephi_R0", &truephi_R0, "truephi_R0/F");
  outtree->Branch("truephi_R1", &truephi_R1, "truephi_R1/F");
  outtree->Branch("trueth", &trueth, "trueth/F");
  outtree->Branch("truezpiplus", &truezpiplus, "truezpiplus/F");
  outtree->Branch("truezpi0", &truezpi0, "truezpi0/F");
  outtree->Branch("truez", &truez, "truez/F");
  outtree->Branch("truexFpiplus", &truexFpiplus, "truexFpiplus/F");
  outtree->Branch("truexFpi0", &truexFpi0, "truexFpi0/F");
  outtree->Branch("truexF", &truexF, "truexF/F");
  outtree->Branch("trueMx", &trueMx, "trueMx/F");
  outtree->Branch("prob_g1", &prob_g1, "prob_g1/F");
  outtree->Branch("prob_g2", &prob_g2, "prob_g2/F");
  outtree->Branch("trueparentpid_gamma1",&trueparentpid_gamma1,"trueparentpid_gamma1/I");
  outtree->Branch("trueparentpid_gamma2",&trueparentpid_gamma2,"trueparentpid_gamma2/I");
  outtree->Branch("trueparentpid_pion",&trueparentpid_pion,"trueparentpid_pion/I");
  outtree->Branch("truepid_gamma1",&truepid_gamma1,"truepid_gamma1/I");
  outtree->Branch("truepid_gamma2",&truepid_gamma2,"truepid_gamma2/I");
  outtree->Branch("truepid_pion",&truepid_pion,"truepid_pion/I");
  outtree->Branch("chi2CutPiPlus", &chi2CutPiPlus, "chi2CutPiPlus/I");
  outtree->Branch("isGoodEventWithoutML", &isGoodEventWithoutML, "isGoodEventWithoutML/I");
  outtree->Branch("eE",&eE,"eE/F");
  outtree->Branch("g1E",&g1E,"g1E/F");
  outtree->Branch("g2E",&g2E,"g2E/F");
  outtree->Branch("piE",&piE,"piE/F");
  outtree->Branch("eth",&eth,"eth/F");
  outtree->Branch("g1th",&g1th,"g1th/F");
  outtree->Branch("g2th",&g2th,"g2th/F");
  outtree->Branch("pith",&pith,"pith/F");
  outtree->Branch("ephi",&ephi,"ephi/F");
  outtree->Branch("g1phi",&g1phi,"g1phi/F");
  outtree->Branch("g2phi",&g2phi,"g2phi/F");
  outtree->Branch("piphi",&piphi,"piphi/F");
  outtree->Branch("trueeE",&trueeE,"trueeE/F");
  outtree->Branch("trueg1E",&trueg1E,"trueg1E/F");
  outtree->Branch("trueg2E",&trueg2E,"trueg2E/F");
  outtree->Branch("truepiE",&truepiE,"truepiE/F");
  outtree->Branch("trueeth",&trueeth,"trueeth/F");
  outtree->Branch("trueg1th",&trueg1th,"trueg1th/F");
  outtree->Branch("trueg2th",&trueg2th,"trueg2th/F");
  outtree->Branch("truepith",&truepith,"truepith/F");
  outtree->Branch("trueephi",&trueephi,"trueephi/F");
  outtree->Branch("trueg1phi",&trueg1phi,"trueg1phi/F");
  outtree->Branch("trueg2phi",&trueg2phi,"trueg2phi/F");
  outtree->Branch("truepiphi",&truepiphi,"truepiphi/F");
  // Kinematics Object
  Kinematics kin;
  
  // Initial particles
  TLorentzVector init_electron(0,0,sqrt(beamE*beamE-0.000511*0.000511),beamE);
  TLorentzVector init_target(0,0,0,0.938272);
  //loop over the tree entries 
  for (Int_t i=0; i<intree->GetEntries(); i++){
    intree->GetEntry(i);
    // Find electron
    TLorentzVector electron,trueelectron;
    double ebeam=0.0;
    double vz_e = 0;
    for (Int_t j=0; j<nPart; j++){
      if(pid[j]==11){
	electron.SetPxPyPzE(px[j],py[j],pz[j],E[j]);
    trueelectron.SetPxPyPzE(truepx[j],truepy[j],truepz[j],trueE[j]);
	vz_e=vz[j];
      }
    }
    if(electron.E()==0){
      cout << "ERROR: Lost an electron?" << endl;
      continue; // no electron found
    }
    TLorentzVector q = init_electron-electron;
    TLorentzVector trueq = init_electron-trueelectron;
    //loop over all particles in the event
    for (Int_t j=0; j<nPart; j++){
      TLorentzVector photon1, truephoton1;
      TLorentzVector photon2, truephoton2;
      if (pid[j]==22){ //if this particle is a photon
	
	//fill the 4-momentum of the second photon
	photon1.SetPxPyPzE(px[j],py[j],pz[j],E[j]);
    truephoton1.SetPxPyPzE(truepx[j],truepy[j],truepz[j],trueE[j]);
    truepid_gamma1 = truepid[j];
    trueparentpid_gamma1 = parentPID[j];
	prob_g1=catboost_weight[j]; // catboost weight
	// loop over all particles again
	for (Int_t k=j+1; k<nPart;k++){
	  if(pid[k]==22){ // if this particle is a photon
	    photon2.SetPxPyPzE(px[k],py[k],pz[k],E[k]);
        truephoton2.SetPxPyPzE(truepx[k],truepy[k],truepz[k],trueE[k]);
        truepid_gamma2 = truepid[k];
        trueparentpid_gamma2 = parentPID[k];
	    prob_g2=catboost_weight[k]; // catboost weight
	    TLorentzVector diphoton = photon1 + photon2;
        TLorentzVector truediphoton = truephoton1+truephoton2;
	    // loop over all particles again
	    for (Int_t m=0;m<nPart;m++){
	      if(pid[m]!=-211)continue; //if this particle is not a pi-, continue
	      //fill the 4-momentum of the pion
	      TLorentzVector pion(px[m],py[m],pz[m],E[m]);
          TLorentzVector truepion(truepx[m],truepy[m],truepz[m],trueE[m]);
          truepid_pion = truepid[m];
          trueparentpid_pion = parentPID[m];
	      // form dihadron
	      TLorentzVector dihadron = pion + diphoton;
	      TLorentzVector truedihadron = truepion + truediphoton;
	      // fill results
	      Mgg = diphoton.M();
	      Mh = dihadron.M();
	      phi_h = kin.phi_h(q,init_electron,pion,diphoton);
	      phi_R0 = kin.phi_R(q,init_electron,pion,diphoton,0);
	      phi_R1 = kin.phi_R(q,init_electron,pion,diphoton,1);
	      th     = kin.com_th(diphoton,pion);
	      xFpiplus = kin.xF(q,pion,init_target,W);
	      xFpi0 = kin.xF(q,diphoton,init_target,W);
	      xF     = kin.xF(q,dihadron,init_target,W);
	      zpiplus = kin.z(init_target,pion,q);
	      zpi0 = kin.z(init_target,diphoton,q);
	      z = zpiplus+zpi0;
	      Mx = (init_electron+init_target-electron-dihadron).M();
            
            
          eE = electron.E();
          eth = electron.Theta();
          ephi = electron.Phi();
          g1E = photon1.E();
          g1th = photon1.Theta();
          g1phi = photon1.Phi();
          g2E = photon2.E();
          g2th = photon2.Theta();
          g2phi = photon2.Phi();
          piE = pion.E();
          pith = pion.Theta();
          piphi = pion.Phi();
        
            
          if(piplus_chi2_cut(211, pion.P(), chi2[m], 0))
              chi2CutPiPlus=1;
          else{
              chi2CutPiPlus=0;
              continue; // Just don't even consider pi+ why fail their Chi2 pid check
          }
          // fill true results
          trueMgg = truediphoton.M();
          trueMh = truedihadron.M();
	      truephi_h = kin.phi_h(trueq,trueelectron,truepion,truediphoton);
	      truephi_R0 = kin.phi_R(trueq,trueelectron,truepion,truediphoton,0);
	      truephi_R1 = kin.phi_R(trueq,trueelectron,truepion,truediphoton,1);
	      trueth     = kin.com_th(truediphoton,truepion);
	      truexFpiplus = kin.xF(trueq,truepion,init_target,trueW);
	      truexFpi0 = kin.xF(trueq,truediphoton,init_target,trueW);
	      truexF     = kin.xF(trueq,truedihadron,init_target,trueW);
	      truezpiplus = kin.z(init_target,truepion,trueq);
	      truezpi0 = kin.z(init_target,truediphoton,trueq);
	      truez = truezpiplus+truezpi0;
	      trueMx = (init_electron+init_target-trueelectron-truedihadron).M();
          truePi0=((parentID[j]==parentID[k] && parentPID[j]==111 && parentPID[k]==111))?1:0;
	      halftruePi0=((parentID[j]!=parentID[k] && parentPID[j]==111 ^ parentPID[k]==111))?1:0; // '^' --> XOR
            
          trueeE = trueelectron.E();
            trueeth = trueelectron.Theta();
            trueephi = trueelectron.Phi();
            trueg1E = truephoton1.E();
            trueg1th = truephoton1.Theta();
            trueg1phi = truephoton1.Phi();
            trueg2E = truephoton2.E();
            trueg2th = truephoton2.Theta();
            trueg2phi = truephoton2.Phi();
            truepiE = truepion.E();
            truepith = truepion.Theta();
            truepiphi = truepion.Phi();
	      // Determine if the standard, non ML cuts would've been satisfied
	      if(z < 0.95 &&
		 photon1.E() > 0.6 && photon2.E() > 0.6 &&
		 abs(vz_e-vz[m])<20 && 
		 xFpiplus>0 && xFpi0 > 0 &&
		 piplus_chi2_cut(-211, pion.P(), chi2[m], 0) &&
		 pion.P() > 1.25)
		isGoodEventWithoutML=1;
	      else
		isGoodEventWithoutML=0;
          // Determine if the MC match all particles
          if(trueelectron.E()>0 && truephoton1.E()>0 && truephoton2.E()>0 && truepion.E() >0)
              MCmatch=1;
          else
              MCmatch=0;
	      // fill tree
          
	      outtree->Fill();
	    }
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
