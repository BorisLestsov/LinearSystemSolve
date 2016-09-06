#include "../include/functions.hpp"
#include <fstream>

using namespace Func;

typedef float T;
bool DEBUG_MODE     = false;
bool PRINT_MODE     = true;
bool RAND_MATRIX    = true;
double RAND_RANGE   = 1e10;

int main(int argc, char* argv[]) {

    uint m, n;
    if(RAND_MATRIX) {
        n = 50;
        m = 51;
    }
        matrix<T> matr;


    if (RAND_MATRIX) {
        matr = matrix<T>(n, m);
        fill_matrix_rand(matr);
    } else {
        try {
            ifstream f(argv[1], ifstream::in);
            if (argc != 2)
                throw Exception("Wrong arguments");
            if (!f.is_open())
                throw Exception("Could not open file");
            f >> n;
            f >> m;
            if (m != n + 1)
                throw Exception("Wrong matrix in file");
            matr = matrix<T>(n, m);
            fill_matrix_from_stream(matr, f);
            f.close();
        }
        catch (Exception &e) {
            cerr << e.what() << endl;
            return -1;
        }
    }

    if(PRINT_MODE){
     	print_matrix(matr);
    	cout << endl;
    }
    boost_vector<T> sol = system_solve(matr);
    if(sol.size() != 0) 
        check_solution(matr, sol);

    return 0;
}