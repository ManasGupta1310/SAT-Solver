# SAT-Solver
A SAT-solver implemented in C++  using the DPLL algorithm.


### Input
The input is taken in a CNF file (.cnf)
A sample input file would be:

> c sentences starting with c are comments and not compiled<br>
> c next line would be compiled and the first number gives the literal count<br>
> c and second number gives the number of clauses <br>
> p cnf 3 6 <br>
> 1 2 0 <br>
> 1 -2 0 <br>
> 3 2 0 <br>
> -3 1 0 <br>
> 1 2 3 0 <br>
> -1 -2 0 <br>
