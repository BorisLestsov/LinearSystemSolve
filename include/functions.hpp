#ifndef LINEARSYSTEMSOLVE_FUNCTIONS_HPP
#define LINEARSYSTEMSOLVE_FUNCTIONS_HPP

#include "../include/Exception.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace boost::numeric::ublas;

template <typename T>
using boost_vector = boost::numeric::ublas::vector<T>;

namespace Func {

    #define O_SCI 0
    #define O_FXD 1
    template<typename T>
    void print_matrix(const matrix<T> & m,
                      int o_type = O_FXD,
                      int precision = 8,
                      ostream& stream = cout) throw(Exception) {
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

    template <typename T>
    boost_vector<T> system_solve(matrix<T>& m){

        //1st step - make matrix upper triangular
        for (uint i = 0; i < m.size1(); ++i) {
            matrix_row<matrix<T>> step_row(m, i);
            step_row /= (T) step_row(i);
            for (uint j = i + 1; j < m.size1(); ++j) {
                matrix_row<matrix<T>> div_row(m, j);
                div_row -= step_row * ((T) div_row(i));
            }
            /*print_matrix(m);
            std::cout << endl;*/
        }
        print_matrix(m);
        std::cout << endl;

        //2nd step
        boost_vector<T> sol(m.size2() - 1);
        for (uint i = (uint) m.size2() - 1; i-- > 0;) {
            sol(i) = m(i, m.size2() - 1);
            for (uint j = (uint) m.size2() - 1 ; j-- > i+1;){
                sol(i) -= m(i, j) * sol(j);
            }
        }
        std::cout << sol;
        /*matrix_row<matrix<T>> step_row(m, 0);
        std::cout << step_row << std::endl;
        boost_vector<T> vec(step_row.size());
        std::copy(step_row.begin(), step_row.end(), vec.begin());
        std::cout << vec;*/
        return boost_vector<T>(0);
    }
}
#endif //LINEARSYSTEMSOLVE_FUNCTIONS_HPP