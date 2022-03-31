/** @file sudoku.cpp
 * A heuristic Sudoku solver.
 *
 *Copyright 2015, 2021 by Francisco Cancillo & Luis Cancillo
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIN 9       //lines in a Sdoku
#define COL 9       //columns in a Sudoku
#define NONETLIN 3  //lines in a nonet (3 x 3 subtable in a Sudoku)
#define NONETCOL 3  //columns in a nonet
#define MAXDIGITS 9
#define NOTSET 10

int recLevel;				//recurrence level
int ierr, jerr;             //in case of error
int numSolRequested = 1;    //number of solutions requested
int numCurrentSol = 1;      //number of the current solution (if found)
int numAttempts = 1;        //number of attempts before this solution
char aStr[100] = { 0};  //a string to store data

/** readSudoku
 * reads data for the Sudoku to be solved from the standard input, and store them in a Sudoku matrix.
 * A Sudoku matrix contains LIN by COL cells. Each cell is a MAXDIGITS bit pattern of the possible digits: bit in
 * position 0 corresponds to digit 1, position 1 to 2, and so on.
 * During the process of solving the Sudoku, when a cell has 2 bits or more set, it means that there are options in the
 * digits to be placed in its position.
 * If no bits are set in the cell, it means that no options have been established.
 *
 * @param sdkMatrix the Sudoku matrix 
 */
void readSdk(int sdkMatrix[LIN][COL]) {
    int i, j;

    i = 0;
    while ((fgets(aStr, sizeof aStr - 2, stdin) != NULL) && i < LIN) {
        for (j = 0; j < COL; j++)
            switch (aStr[j]) {
                case '1' :
                    sdkMatrix[i][j] = 0001;
                    break;
                case '2' :
                    sdkMatrix[i][j] = 0002;
                    break;
                case '3' :
                    sdkMatrix[i][j] = 0004;
                    break;
                case '4' :
                    sdkMatrix[i][j] = 0010;
                    break;
                case '5' :
                    sdkMatrix[i][j] = 0020;
                    break;
                case '6' :
                    sdkMatrix[i][j] = 0040;
                    break;
                case '7' :
                    sdkMatrix[i][j] = 0100;
                    break;
                case '8' :
                    sdkMatrix[i][j] = 0200;
                    break;
                case '9' :
                    sdkMatrix[i][j] = 0400;
                    break;
            }
        i++;
    }
}
/** outputRecuLevel
  * writes the recurrence level.
  * @param beVerbose if true, print recurrence level
  */
void outputRecuLevel (int beVerbose) {
    if (beVerbose) printf("{%d}", recLevel);
}
/** outputCell
  * prints in the stdout the digit or digits in a cell
  * @param beVerbose if true, print cell mask
  * @param cell the cell mask to print
  * @param suffix the suffix to separate
  */
void outputCell(int beVerbose, unsigned int cell, const char *suffix, const char *endline) {
    if (!beVerbose) return;
    char digit = '1';
    for (int i=0; i < MAXDIGITS; i++)
    {
        if ((cell & 01) == 1) putc(digit, stdout);
        else putc('.', stdout);
        cell >>= 1;
        digit++;
    }
    printf("%s%s", suffix, endline);
}

/** outputPass
 * writes the current values in the Sudoku matrix being solved.
 * It can be an option or a solution.
 * @param beVerbose if true, print Sudoku matrix
 * @param currentPass an identification of the pass to be printed
 * @param sdkMatrix the Sudoku matrix to print
 */
void outputPass(int beVerbose, const char *currentPass, int sdkMatrix[LIN][COL]) {
    int i, j;

    if (!beVerbose) return;
    outputRecuLevel(beVerbose);
    puts(currentPass);
    for (i = 0; i < LIN; i++) {
        outputRecuLevel(beVerbose);
        for (j = 0; j < COL-1; j++) {
            if ((j % NONETCOL) == 2) outputCell(beVerbose, sdkMatrix[i][j], "|", "");
            else outputCell(beVerbose, sdkMatrix[i][j], ":", "");
        }
        outputCell(beVerbose, sdkMatrix[i][j], "|", "\n");
    }
}
/** digitsInCell
 * computes the number of digits in the given cell mask (of a Sudoku)
 * @param cell the Sudoku cell to be analyzed
 * @return the number of digits in the given cell
 */
int digitsInCell(int cell) {
    int digits = 0;
    for (digits = 0; cell != 0; cell >>= 1)
        if (cell & 01) digits++;
    return (digits);
}
/** isCompliant
 * checks if the given Sudoku satisfies the the Sudoku rules.
 * Each cell having a digit (not an option) shall be unique in the line, column and nonet.
 * @param sdk Sudoku matrix to be checked
 * @return true if satisfy all rules or false otherwise
 */
int isCompliant(int sdk[LIN][COL]) {
    int i, j, k, l, cellij;

    for (i = 0; i < LIN; i++)
        for (j = 0; j < COL; j++) {
            cellij = sdk[i][j];
            if (digitsInCell(cellij) == 1) {
                ierr = i;
                jerr = j;
                for (k = 0; k < LIN; k++) if ((k != i) && (cellij == sdk[k][j])) return (0);
                for (k = 0; k < COL; k++) if ((k != j) && (cellij == sdk[i][k])) return (0);
                for (k = (i / NONETLIN) * NONETLIN; k < (i / NONETLIN) * NONETLIN + NONETLIN; k++)
                    for (l = (j / NONETCOL) * NONETCOL; l < (j / NONETCOL) * NONETCOL + NONETCOL; l++)
                        if ((cellij == sdk[k][l]) && ((k != i) || (l != j))) return (0);
            }
        }
    return (1);
}
/** isSolution
 * checks if the given Sudoku matrix is a solution: each cell has one and only one digit and is compliant with rules.
 * @param sdk Sudoku matrix to be checked
 * @return true if is a solution, false otherwise
 */
int isSolution(int sdk[LIN][COL]) {
    int i, j;

    for (i = 0; i < LIN; i++)
        for (j = 0; j < COL; j++)
            if (digitsInCell(sdk[i][j]) != 1) return (0);
    return (isCompliant(sdk));
}
/** setOptions
 *  fills the output Sudoku with the following values for each cell:
 * - if the corresponding one in the input Sudoku has a digit, copy it.
 * - if the corresponding one in the input Sudoku is empty, set the values that could be placed in it, taking into
 *  account the sudoku rules.
 * @param inSdk the input Sudoku matrix
 * @param outSdk the output Sodoku matrix to fill
 * @return minimum number of options in any cell of the output Sudoku
 */
int setOptions(int inSdk[LIN][COL], int outSdk[LIN][COL]) {
    int sumLin[LIN] = {0};  //a bitwise OR of all cells in the line
    int sumCol[COL] = {0};  //a bitwise OR of all cells in the column
    int sumNonet[LIN / NONETLIN][COL / NONETCOL] = {0}; //idem
    int i, j, minOptions;

    minOptions = NOTSET;
    for (i = 0; i < LIN; i++)
        for (j = 0; j < COL; j++) sumLin[i] |= inSdk[i][j];
    for (j = 0; j < COL; j++)
        for (i = 0; i < LIN; i++) sumCol[j] |= inSdk[i][j];
    for (i = 0; i < LIN; i++)
        for (j = 0; j < COL; j++) sumNonet[i / NONETLIN][j / NONETCOL] |= inSdk[i][j];
    for (i = 0; i < LIN; i++)
        for (j = 0; j < COL; j++) {
            if (inSdk[i][j] != 0) outSdk[i][j] = inSdk[i][j];
            else {
                outSdk[i][j] = 0777 & ~(sumLin[i] | sumCol[j] | sumNonet[i / NONETLIN][j / NONETCOL]);
                if (minOptions > digitsInCell(outSdk[i][j])) minOptions = digitsInCell(outSdk[i][j]);
            }
        }
    return (minOptions);
}
/** consolidateOpt
 * Copy cells without options (having only one digit) to the solution.
 * @param sdkOptions    //the input Sudoku matrix with options
 * @param sdkSolutions   //the output Sudoku matrix with some options replaced
 * @return number of cell consolidated
 */
int consolidateOpt (int sdkOptions[LIN][COL], int sdkSolution[LIN][COL]) {
    int i, j;

    int numConsolidated = 0;
    if (isCompliant(sdkOptions))
        for (i = 0; i < LIN; i++)
            for (j = 0; j < COL; j++)
                if ((sdkSolution[i][j] == 0) && (digitsInCell(sdkOptions[i][j]) == 1)) {
                    sdkSolution[i][j] = sdkOptions[i][j];
                    numConsolidated++;
                }
    return (numConsolidated);
}
/** solveSdk
 * solves the given Sudoku matrix recursively
 *
 * @param sdk the Sudoku matrix to be solved
 * @return true if the Sudoku has been solved, false otherwise
 */
int solveSdk(int beVerbose, int sdk[LIN][COL]) {
    int options[LIN][COL];
    int sudoku[LIN][COL];
    int i, j, k, opc, result, numOptions;

    recLevel++;
    result = 0;
    numAttempts++;
    // save matrix to solve, needed due to recurrence
    for (i = 0; i < LIN; i++)
        for (j = 0; j < COL; j++) sudoku[i][j] = sdk[i][j];
    //set options placing in each empty cell its options (feasible digits for a solution)
    do {
        numOptions = setOptions(sudoku, options);
        if (numOptions != 1) break;
    } while (consolidateOpt(options, sudoku));
    outputPass(beVerbose, "Options:", options);
    switch (numOptions) {
        case 0:
        case 1:
        case NOTSET:
            if (isSolution(sudoku)) {
                snprintf(aStr, sizeof aStr, "Solution %d in %d attemps:", numCurrentSol, numAttempts);
                outputPass(1, aStr, sudoku);
                numCurrentSol++;
                result = 1;
            } else {
                if (beVerbose) {
                    outputRecuLevel(beVerbose);
                    printf("Option w/o solution (lin=%d, col=%d)\n", ierr, jerr);
                }
            }
            break;
        default:
            //search the 1st cell having options
            i = j = 0;
            while (digitsInCell(options[i][j]) < 2) {
                j++;
                if (j == COL) {
                    i++;
                    j = 0;
                }
                if (i == LIN) break;
            }
            //solve for all options in this cell
            opc = 01;
            for (k = 0; k < MAXDIGITS && numSolRequested >= numCurrentSol; k++) {
                if ((options[i][j] & opc) != 0) {
                    sudoku[i][j] = opc;
                    if (beVerbose) {
                        outputRecuLevel(beVerbose);
                        printf("Checking alternative in cell [%d, %d]:", i, j);
                        outputCell(beVerbose, opc, ":", "\n");
                    }
                    result = solveSdk(beVerbose, sudoku);
                }
                opc <<= 1;
            }
    }
    recLevel--;
    return (result);
}
/**main
 *
 * @param argc number of arguments
 * @param argv arguments
 * @return 0 if finished OK
 */
int main(int argc, char* argv[]) {
    int sdkToSolve[LIN][COL] = {0};
    int beVerbose = 0;

    printf("Usage: sudoku <input.txt >output.txt [-sn][-v]\n");
    printf("-sn find n solutions. Default is 1 solution.\n");
    printf("-v be verbose printing all information. Default is no verbose.\n");
    printf("input.txt shall include the initial Sudoku having 9 lines with 9 columns each: 9 x 9 matrix of characters\n");
    printf("Values for each character shall be the initial digit (1 to 9) in this cell, or a blank or '.', if is empty\n");
    printf("For example:\n");
    printf("...2...3.\n");
    printf("..5.7.2..\n");
    printf("..9....41\n");
    printf("..3..9..7\n");
    printf(".5......4\n");
    printf("8......1.\n");
    printf("9..3.....\n");
    printf("..86.....\n");
    printf(".145..37.\n");

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-s", 2) == 0) numSolRequested = atoi((argv[i] + 2));
        if (strncmp(argv[i], "-v", 2) == 0) beVerbose = 1;
    }
    printf("Solutions to find: %d\n", numSolRequested);
    readSdk(sdkToSolve);
    outputPass(1, "Initial Sudoku:", sdkToSolve);
    if (isCompliant(sdkToSolve)) solveSdk(beVerbose, sdkToSolve);
    else printf("The initial values are not compliant: check cell [%d, %d] = %d\n", ierr, jerr, sdkToSolve[ierr][jerr]);
    return 0;
}

