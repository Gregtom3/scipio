class Kinematics {
 public:
  static double xF(TLorentzVector, TLorentzVector, TLorentzVector, double);
  static double phi_h(TLorentzVector,TLorentzVector,TLorentzVector,TLorentzVector);
  static double phi_R(TLorentzVector,TLorentzVector,TLorentzVector,TLorentzVector, int);
  static double com_th(TLorentzVector, TLorentzVector);
  static double z(TLorentzVector, TLorentzVector, TLorentzVector);
};
