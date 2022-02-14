#include <iostream>
#include<bits/stdc++.h>
#include <fstream>
#define all(v) v.begin(), v.end()
#define allr(v) v.rbegin(), v.rend()

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
    vector<int> literal_freq; // vector to store the number of occurrences of each literal

    vector<int> literal_sign; // vector to stor different occurences of literals with different signs

    
    vector<vector<int>> clauses; //vector to store clauses
    // If literal is positive store it in form of 2n else 2n+1 if literal is negative 
    
    Formula() {}
    Formula(const Formula &f) {
        literals = f.literals;
        clauses = f.clauses;
        literal_freq = f.literal_freq;
        literal_sign = f.literal_sign;
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
        // cout<<str<<endl;
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
            formula.literal_freq.resize(literal_count, 0);
            formula.literal_sign.resize(literal_count, 0);
        }
        else{
            int i=0;
            string s="";
            while(i!=str.length()-1){
                if(str[i]==' '){
                    int val=stoi(s);
                    s="";
                    if(val>0){
                        formula.clauses[idx].push_back(2*(val-1)); // store it in the form 2n
                        
                        // increase frequency and sign of the literal
                        formula.literal_freq[val-1]++;
                        formula.literal_sign[val-1]++;
                    }
                    else{
                        formula.clauses[idx].push_back(2*((-1)*val-1)+1); // store it in the form 2n+1
                        
                        // increase frequency and sign of the literal
                        formula.literal_freq[-val-1]++;
                        formula.literal_sign[-val-1]--;

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
        f.literals[f.clauses[i][0]/2]=f.clauses[i][0]%2;  // Assigning the truth value to the clause
        f.literal_freq[f.clauses[i][0]/2]=-1;   // Marking as closed by resetting the frequency 

        int res=transform(f, f.clauses[i][0]/2);  // Applying the transformations over all clauses

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



int SAT_Solver::transform(Formula &f, int literal_to_apply) {
  int value_to_apply = f.literals[literal_to_apply]; // the value to apply, 0 -
                                                     // if true, 1 - if false
  // iterate over the clauses in f
  for (int i = 0; i < f.clauses.size(); i++) {
    // iterate over the variables in the clause
    for (int j = 0; j < f.clauses[i].size(); j++) {
      // if this is true, then the literal appears with the same polarity as it
      // is being applied that is, if assigned true, it appears positive if
      // assigned false, it appears negative, in this clause hence, the clause
      // has now become true
      if ((2 * literal_to_apply + value_to_apply) == f.clauses[i][j]) {
        f.clauses.erase(f.clauses.begin() +
                        i); // remove the clause from the list
        i--;                // reset iterator
        if (f.clauses.size() ==
            0) // if all clauses have been removed, the formula is satisfied
        {
          return 1;
        }
        break; // move to the next clause
      } else if (f.clauses[i][j] / 2 ==
                 literal_to_apply) // the literal appears with opposite polarity
      {
        f.clauses[i].erase(
            f.clauses[i].begin() +
            j); // remove the literal from the clause, as it is false in it
        j--;    // reset the iterator
        if (f.clauses[i].size() ==
            0) // if the clause is empty, the formula is unsatisfiable currently
        {
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
  if (res == 1) // if formula satisfied, show result and return
  {
    result(f, res);
    return -1;
  } else if (res == 0) // if formula not satisfied in this
                                         // branch, return normally
  {
    return 2;
  }
  // find the variable with maximum frequency in f, which will be the next to be
  // assigned a value already assigned variables have this field reset to -1 in
  // order to ignore them
  int i = distance(
      f.literal_freq.begin(),
      max_element(f.literal_freq.begin(), f.literal_freq.end()));
  // need to apply twice, once true, the other false
  for (int j = 0; j < 2; j++) {
    Formula new_f = f; // copy the formula before recursing
    if (new_f.literal_sign[i] >
        0) // if the number of literals with positive polarity are greater
    {
      new_f.literals[i] = j; // assign positive first
    } else                   // if not
    {
      new_f.literals[i] = (j + 1) % 2; // assign negative first
    }
    new_f.literal_freq[i] =
        -1; // reset the frequency to -1 to ignore in the future
    int transform_result =
        transform(new_f, i); // apply the change to all the clauses
    if (transform_result ==
        1) // if formula satisfied, show result and return
    {
      result(new_f, transform_result);
      return -1;
    } else if (transform_result == 0) // if formula not satisfied
                                                     // in this branch, return
                                                     // normally
    {
      continue;
    }
    int dpll_result = Algorithm(new_f); // recursively call DPLL on the new formula
    if (dpll_result == -1) // propagate the result, if completed
    {
      return dpll_result;
    }
  }
  // if the control reaches here, the function has returned normally
  return 2;
}

void SAT_Solver::result(Formula &f, int res) {
  if(res==1){   // If SATisfiable
    cout<<"SAT"<<endl;

    for(int i=0;i<f.literals.size();i++){
      if(i!=0){
        cout<<" ";
      }

      if(f.literals[i]!=-1){
        cout<<pow(-1, f.literals[i])*(i+1);
      }
      else{   // For literals unassigned and can take any value, assign true
        cout<<(i+1)<<endl;
      }
    }
    cout<<" 0"<<endl;
  }
  else{         // If UnSATisfiable
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