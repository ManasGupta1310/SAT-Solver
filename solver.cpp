#include <iostream>
#include<bits/stdc++.h>
#include <fstream>

using namespace std;

// 1 for Satisfied
// 0 for Unsatisfied
// -1 for Completed
// 2 for Normal

class Formula {
    public:
    vector<int> literals;     // vector to store the value assigned to each variable
    // -1 unassigned
    // 0 True
    // 1 False
    vector<int> freq; // vector to store the number of occurrences of each literal

    vector<int> signs; // vector to stor different occurences of literals with different signs

    
    vector<vector<int>> clauses; //vector to store clauses
    // If literal is positive store it in form of 2n else 2n+1 if literal is negative 
    
    Formula() {}
    Formula(const Formula &f) {
        literals = f.literals;
        clauses = f.clauses;
        freq = f.freq;
        signs = f.signs;
    }
};

class SAT_Solver {
    private:
        Formula formula;                // Formula to be the initial input
        int literal_count;              // Number of Variables in the formula
        int clause_count;               // Number of Clauses in the formula
        int uni_prop(Formula &);        // Implements the unit propagation
        int Algorithm(Formula);         // Implements the DPLL algorithm in the clauses
        int transform(Formula &, int);   // Gives a value to literal in every clause
        void result(Formula &, int);    // Output
    public:
        SAT_Solver() {}
        void init();                    // Formula initiation
        void solve();                   // Solver call
};

void SAT_Solver::init(){
  string str;

  // Read from the text file
  ifstream cnf_file("test2.cnf");

  int idx=0;

  // Use a while loop together with the getline() function to read the file line by line
  while(getline(cnf_file, str)){
      
    if(str[0]=='c') continue;
    else if(str[0]=='p'){
      int i=6;
      string s="";
      while(str[i]!=' '){
          s+=str[i];
          i++;
      }
      literal_count=stoi(s);  // Store the value of literal_count

      s="";
      while(i!=str.length()){
          s+=str[i];
          i++;
      }
      clause_count=stoi(s);   // Store the value of clause_count


      // Resize the vectors on basis of the literal count
      formula.literals.resize(literal_count, -1);
      formula.clauses.resize(clause_count);
      formula.freq.resize(literal_count, 0);
      formula.signs.resize(literal_count, 0);
    }
    else{
      int i=0;
      string s="";
      while(i!=str.length()-1){
        if(str[i]==' '){
          int val=stoi(s);
          s="";
          if(val>0){
            string s1=to_string(val-1);
            s1=s1+"0";
            formula.clauses[idx].push_back(stoi(s1)); // store it in the form n+"0" or 10n
            
            // increase frequency and sign of the literal
            formula.freq[val-1]++;
            formula.signs[val-1]++;
          }
          else{
            string s1=to_string(-val-1);
            s1=s1+"1";
            formula.clauses[idx].push_back(stoi(s1)); // store it in the form n+"1" or 10n+1
            
            // increase frequency and sign of the literal
            formula.freq[-val-1]++;
            formula.signs[-val-1]--;

          }
        }
        else{
            s+=str[i];
        }
        i++;
      }
      idx++;
    }
  }

  // Close the file
  cnf_file.close();
}


/* Applying the unit propagation on the clauses and applying the respective
transformations on the other clauses*/

int SAT_Solver::uni_prop(Formula &f) {
  bool found=false; // bool to get if a unit clause is found or not
  
  if(f.clauses.size()==0){
    return 1;
  }
  do{
    found=false;

    for(int i=0;i<f.clauses.size();i++){
      if(f.clauses[i].size()==1){ // If clause contains one literal, it is a unit clause
        found=true;
        f.literals[f.clauses[i][0]/10]=f.clauses[i][0]%10;  // Assigning the truth value to the clause
        f.freq[f.clauses[i][0]/10]=-1;   // Marking the clause as closed  

        int res=transform(f, f.clauses[i][0]/10);  // Applying the transformations over all clauses

        if( res==0 || res==1) return res; 

        break;  // Check for another open clause
      }
      else if(f.clauses[i].size()==0){
        return 0; // Given clause is empty so unsatisfiable
      }
    }
  }while(found);

  return 2; // Ended normally
}



int SAT_Solver::transform(Formula &f, int target) {
  int val_target = f.literals[target];  // 0 is True, 1 if False

  
  for (int i = 0; i < f.clauses.size(); i++){
    // iterate over the clauses in the formula
    for (int j = 0; j < f.clauses[i].size(); j++){
      // Iterate over the variables in clause

      if ((10 * target + val_target) == f.clauses[i][j]){   // If literal applies with samae polarity as it is being tagetted
                                                            // remove the clause from the list
        f.clauses.erase(f.clauses.begin()+i);
        i--;  // reset iterator

        if (f.clauses.size() ==0){ 
          // If no clauses remain, formula is satisfied
          return 1;
        }
        break; // move to the next clause

      }else if (f.clauses[i][j]/10==target){ 

        // the literal appears with opposite polarity
        f.clauses[i].erase(f.clauses[i].begin()+j); 
        // remove the literal from the clause, as it is false in it
        j--;    // reset the iterator
        
        if (f.clauses[i].size()==0){  
          //If the clause is empty the clause is unsatisfiable right now
          return 0;
        }
        break; // move to the next clause
      }
    }
  }

  // if reached here, the function is exiting normally
  return 2;
}

int SAT_Solver::Algorithm(Formula f) {
  int res = uni_prop(f); // perform unit propagation on the formula
  if (res == 1) // If formula gets satisfies  show the result
  {
    result(f, res);
    return -1;
  } else if (res == 0) // If formula isn't satisfied  then return normally
  {
    return 2;
  }
  // find the variable with maximum frequency in f, which will be the next to be
  // assigned a value already assigned variables have this field reset to -1 in
  // order to ignore them
  int i = distance(f.freq.begin(), max_element(f.freq.begin(), f.freq.end()));
  // once for true and once for false

  for (int j = 0; j < 2; j++) {
    Formula new_f = f; // Copying the formula into other formula

    if (new_f.signs[i]>0){
      // If the positive sign literals are more then assign true first
      new_f.literals[i] = j; 
    } 
    else{
      // Else assign negative first
      new_f.literals[i] = (j + 1) % 2;
    }
    new_f.freq[i] =-1; // Reset the frquency to close the branch

    int transform_result =transform(new_f, i); // Apply the transformations to all the groups

    if (transform_result==1){ // If formula is satisfied then show result and return
      result(new_f, transform_result);
      return -1;
    } 
    else if (transform_result == 0){ // If formula not satisfied in this branch, return normally
      continue;
    }

    int dpll_result = Algorithm(new_f); // Recursive call for the DPLL Algorithm 
    if (dpll_result == -1) // If done then forward the result
    {
      return dpll_result;
    }
  }

  // Reaching here means that the function returned normally
  return 2;
}

void SAT_Solver::result(Formula &f, int res) {
  if(res==1){   // If SATisfiable
    cout<<"SAT"<<endl;

    for(int i=0;i<f.literals.size();i++){
      if(f.literals[i]!=-1){
        cout<<pow(-1, f.literals[i])*(i+1)<<" ";
      }
      else{   // For literals unassigned and can take any value, assign true
        cout<<(i+1)<<" ";
      }
    }
    cout<<"0"<<endl;
  }
  else{         
  // If UnSATisfiable
    cout<<"UnSAT"<<endl;
  }
}

void SAT_Solver::solve() {
  int res = Algorithm(formula); // result of DPLL algorithm on the formula
  if (res == 2) {
    result(formula, 0);
  }
};

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    
    SAT_Solver solver;
    solver.init();
    solver.solve();

    return 0;
}