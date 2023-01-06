
int test(){
  float _polarization = 0.89;
  int L = 2;
  vector<string> char_vec;
  vector<string> str_vec;
  for (int l = 0; l <= L; l++)
    {
      for (int m = 1; m <= l; m++)
        {
            
	  string str = string(Form("%f",_polarization))+"*@hel[]*sin(" + to_string(m) + "*@phi_h[]-" + to_string(m) +"*@phi_R0[])";
	  str_vec.push_back(str);
        }
      for (int m = -l; m <= l; m++)
        {
	  string str = string(Form("%f",_polarization))+"*@hel[]*sin(" + to_string(1-m) +"*@phi_h[]+" + to_string(m) +"*@phi_R0[])";
	  str_vec.push_back(str);
        }
    }
    
  // Remove duplicate entries
  sort(str_vec.begin(), str_vec.end());
  str_vec.erase(unique(str_vec.begin(), str_vec.end()), str_vec.end());
    
  // Add the mod line to the front
  for(unsigned int i = 0 ; i < str_vec.size(); i++){
    str_vec.at(i)="mod" + to_string(i) + "=" + str_vec.at(i);
  }
    
  int cidx=0;
  for (char c = 'A'; cidx<str_vec.size(); c++) 
    {
      string str = "";
      str += c;
      char_vec.push_back(str);
      cidx++;
    }
  
  for(string v: str_vec){
    cout << v << endl;
  }
  return 0;
}
