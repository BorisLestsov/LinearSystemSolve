#ifndef LINEARSYSTEMSOLVE_FUNCTIONS_HPP
#define LINEARSYSTEMSOLVE_FUNCTIONS_HPP

#include "../include/Exception.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace boost::numeric::ublas;

namespace Func {

    #define O_SCI 0
    #define O_FXD 1
    template<typename T>
    void print_matrix(const matrix<T> & m, int o_type = O_FXD, int precision = 8, ostream& stream = cout) {
        int width;

        switch (o_type) {
            case O_SCI:
                stream << scientific << setprecision(precision);
                width = precision + 8;
                break;
            case O_FXD:
                stream << fixed << setprecision(precision);
                width = precision + 4;
                break;
            default:
                throw Exception("Wrong output mode");
        }
        for (uint i = 0; i < m.size1(); ++i) {
            for (uint j = 0; j < m.size2(); ++j) {
                stream << setw(width) << m(i, j);
            }
            stream << endl;
        }
    }

    template <typename T>
    void fill_matrix(matrix<T>& m, ifstream& f) {
        for (int i = 0; i < m.size1(); ++i) {
            for (int j = 0; j < m.size2(); ++j)
                f >> m(i, j);
        }
    }

    template <typename T>
    void swap_matrix_rows(matrix<T>& m, uint i, uint j) {
        matrix_row< matrix<double>> rowa (m, i);
        matrix_row< matrix<double>> rowb (m, j);
        rowa.swap(rowb);
    }

    template <typename T>
    void swap_matrix_columns(matrix<T>& m, uint i, uint j) {
        matrix_column< matrix<double>> cola (m, i);
        matrix_column< matrix<double>> colb (m, j);
        cola.swap(colb);
    }
}
#endif //LINEARSYSTEMSOLVE_FUNCTIONS_HPP