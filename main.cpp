#include <boost/numeric/ublas/matrix.hpp>
#include <iostream>
#include <iomanip>
#include "Exception.hpp"

using namespace std;
using namespace boost::numeric::ublas;

#define O_SCI 0
#define O_FXD 1

//Call this with precision == 0 to print in scientific mode
template <typename T>
void print_matrix(const matrix<T>& m, int o_type = O_FXD, int precision = 8){
    int width;

    switch (o_type){
        case O_SCI:
            cout << scientific << setprecision(precision);
            width = precision + 8;
            break;
        case O_FXD:
            cout << fixed << setprecision(precision);
            width = precision + 4;
            break;
        default: throw Exception("Wrong output mode");
    }


    for(uint i = 0; i < m.size1(); ++i){
        for(uint j = 0; j < m.size2(); ++j){
            cout << setw(width) << m(i, j);
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]) {
    assert(argc == 4);
    int n = atoi(argv[2]);
    int m = atoi(argv[3]);

    matrix<double> matr(5, 6);
    print_matrix(matr, O_FXD, 2);


    return 0;
}