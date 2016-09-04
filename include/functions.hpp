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

extern bool DEBUG_MODE;

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
                width = precision + 10;
                break;
            case O_FXD:
                stream << fixed << setprecision(precision);
                width = precision + 6;
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
        for (uint i = 0; i < m.size1(); ++i) {
            for (uint j = 0; j < m.size2(); ++j)
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
        uint i, j;
        for (i = 0, j = 0; i < m.size1() && j < m.size2() ; ++i, ++j) {
            //find non-zero element
            if (m (i, j) == 0) {
                matrix_row<matrix<T>> zero_row(m, i);
                bool found = false;
                for (uint k = i + 1; k < m.size1(); ++k)
                    if (m(k, j) != 0) {
                        matrix_row<matrix<T>> non_zero_row(m, k);
                        non_zero_row.swap(zero_row);
                        found = true;
                        break;
                    }
                if (!found){
                    --i;
                    continue;
                }
            }
            //find row with min j element and swap it with current row for stability
            uint max_row_ind = i;
            for (uint k = i + 1; k < m.size1(); ++k)
                if ( m(k, j) != 0 && abs(m(k, j)) > abs(m(max_row_ind, j)))
                    max_row_ind = k;
            if(max_row_ind != i) {
                matrix_row<matrix<T>> cur_row(m, i);
                matrix_row<matrix<T>> max_row(m, max_row_ind);
                max_row.swap(cur_row);
            }
            if (DEBUG_MODE){
                print_matrix(m);
                std::cout << endl;
            }
            matrix_row<matrix<T>> step_row(m, i);
            step_row /= (T) step_row(j);
            for (uint k = i + 1; k < m.size1(); ++k) {
                matrix_row<matrix<T>> div_row(m, k);
                div_row -= step_row * ((T) div_row(j));
            }
            if (DEBUG_MODE){
                print_matrix(m);
                std::cout << endl;
            }
        }
        if (DEBUG_MODE){
            cout << "Matrix after first step:" << endl;
            print_matrix(m);
            std::cout << endl;
        }

        //check for rows like: 0 0 0 .. 0 1 => no solutions
        for (i = (uint) m.size1(); i-- > 0;){
            if (m(i, m.size2() - 1) != 0) {
                bool found = false;
                for (j = 0; j < m.size2() - 1; ++j)
                    if (m(i, j) != 0){
                        found = true;
                        break;
                    }
                if (!found) {
                    cout << "No solutions exist" << endl;
                    return boost_vector<T>(0);
                }
            }
        }

        //Check for zero rows => infinite amount of solutions
        for (i = (uint) m.size1(); i-- > 0;){
            bool found = false;
            for (j = 0; j < m.size2(); ++j)
                if (m(i, j) != 0){
                    found = true;
                    break;
                }
            if (!found) {   //zero row?
                cout << "Infinite amount of solutions" << endl;
                return boost_vector<T>(0);
            }
        }

        boost_vector<T> sol(m.size2() - 1);
        for (i = (uint) m.size2() - 1; i-- > 0;) {
            sol(i) = m(i, m.size2() - 1);
            for (j = (uint) m.size2() - 1 ; j-- > i+1;){
                sol(i) -= m(i, j) * sol(j);
            }
        }
        boost_vector<T> vec(sol.size());
        std::copy(sol.begin(), sol.end(), vec.begin());
        std::cout << "Solution:" << endl << vec << endl;
        return vec;
    }
}
#endif //LINEARSYSTEMSOLVE_FUNCTIONS_HPP