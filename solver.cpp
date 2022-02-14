#include <iostream>
#include<bits/stdc++.h>
#include <fstream>
#define all(v) v.begin(), v.end()
#define allr(v) v.rbegin(), v.rend()

using namespace std;

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
        int transform(Formula &,int);   // Gives a value to literal in every clause
        void result(Formula &, int);    // Output
    public:
        SAT_Solver() {}
        void init();                    // Formula initiation
        void solve();                   // Solver call
};

void SAT_Solver::init(){
    string str;

    // Read from the text file
    ifstream cnf_file("test.cnf");
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
                        // increment frequency and polarity of the literal
                        formula.literal_freq[val-1]++;
                        formula.literal_sign[val-1]++;
                    }
                    else{
                        formula.clauses[idx].push_back(2*((-1)*val-1)+1); // store it in the form 2n
                        // increment frequency and polarity of the literal
                        formula.literal_freq[-val-1]++;
                        formula.literal_sign[-val-1]++;

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


int main(){
    ios::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    
    SAT_Solver solver;
    solver.init();

    return 0;
}