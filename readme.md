##Sudoku solver 

Sodoku solver is a CLI tool that solves a given sudoku using a heuristic algorithm.

The Sudoku to solve is read from the standard input (or from a file redirected to it). Such input shall include the initial Sudoku having 9 lines with 9 columns each: matrix of 9 x 9 cells, each one with a character.

Values for each character shall be the initial digit (1 to 9) in this Sudoku cell, or a blank or '.', if it is empty.

An example for the input file contents can be:

...2...3.

..5.7.2..

..9....41

..3..9..7

.5......4

8......1.

9..3.....

..86.....

.145..37.

Results are sent to the standard output. If the 'verbose' option is selected, they are printed all options being analized plus the solutions found. When no 'verbose' only solutions are printed.

If the option 'number of solutions' is selected, the program stops when this limit is reached (or when no additional solutions are found).

##Usage

Usage: sudoku <input.txt >output.txt [-sn][-v]

being:

-sn :find n solutions. Default is 1 solution.

-v :be verbose printing all information. Default is no verbose.

