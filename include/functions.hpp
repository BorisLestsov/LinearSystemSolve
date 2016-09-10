#ifndef LINEARSYSTEMSOLVE_FUNCTIONS_HPP
#define LINEARSYSTEMSOLVE_FUNCTIONS_HPP

#include "../include/Exception.hpp"
#include "../include/Solution.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/tuple/tuple.hpp>

// This is for random matrix initialization
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stack>

using namespace boost::numeric::ublas;
using namespace boost::random;
using namespace boost::tuples;
using namespace std;

template <typename T>
using boost_vector = boost::numeric::ublas::vector<T>;

typedef     boost::mt19937 RNG_type;
typedef     boost::random::uniform_real_distribution< > Real_dist;
typedef     std::stack<boost::tuples::tuple<uint, uint>> tuple_stack;

extern bool SHOW_FLAG;

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
                width = precision + 12;
                break;
            case O_FXD:
                stream << fixed << setprecision(precision);
                width = precision + 8;
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
    bool eq_safe_compare(T p1, T p2){
        T eps = std::numeric_limits<T>::epsilon() * 10;
        return abs(p1 - p2) <= eps;
    }


    template <typename T>
    void fill_matrix_from_stream(matrix<T> & matr, istream &f) throw(Exception)  {
        uint n, m;
        f >> n;
        f >> m;
        matr = matrix<T>(n, m);

        for (uint i = 0; i < matr.size1(); ++i) {
            for (uint j = 0; j < matr.size2(); ++j){
                if (f.eof())
                    throw Exception("Input error: Unexpected EOF");
                f >> matr(i, j);
            }
        }
    }


    template <typename T>
    void fill_matrix_rand(matrix<T> & matr, int n, int m, int range){
        RNG_type randomNumbergenerator( time(0) );
        Real_dist uniformDistribution( -range, +range );
        matr = matrix<T>(n, m);

        boost::variate_generator< RNG_type & , Real_dist >
                rand( randomNumbergenerator, uniformDistribution );

        for (uint i = 0; i < matr.size1(); ++i) {
            for (uint j = 0; j < matr.size2(); ++j)
                matr(i, j) = rand();
        }
    }

    template <typename T>
    void swap_matrix_rows(matrix<T>& m, uint i, uint j) {
        matrix_row< matrix<T>> rowa (m, i);
        matrix_row< matrix<T>> rowb (m, j);
        rowa.swap(rowb);
        if (SHOW_FLAG){
            cout << "row swap:" << endl;
            cout << i << "  <->  " << j << endl;
        }
    }

    template <typename T>
    void swap_matrix_columns(matrix<T>& m, uint i, uint j) {
        matrix_column< matrix<T>> cola (m, i);
        matrix_column< matrix<T>> colb (m, j);
        cola.swap(colb);
        if (SHOW_FLAG){
            cout << "column swap:" << endl;
            cout << i << "  <->  " << j << endl;
        }
    }

    template <typename T>
    Solution<T> system_solve(matrix<T>& m){
        //1st step - make matrix upper triangular
        uint i;
        tuple_stack swapped;

        for (i = 0 ; i < m.size1() && i < m.size2() - 1 ; ++i) {
            //find max element
            uint k, j;
            T max = abs(m(i, i));
            uint maxcol = i, maxrow = i;
            bool found = false;
            for (k = i; k < m.size1(); ++k) {
                for (j = i + 1; j < m.size2() - 1; ++j) {
                    if (!eq_safe_compare(m(k, j), 0.0) && abs(m(k, j)) > max) {
                        max = m(k, j);
                        maxcol = j;
                        maxrow = k;
                        found = true;
                    }
                }
            }
            if (found || !eq_safe_compare(m(i, i), 0.0)) {
                if (i != maxrow)
                    swap_matrix_rows(m, i, maxrow);
                if (i != maxcol) {
                    swap_matrix_columns(m, i, maxcol);
                    swapped.push(make_tuple<uint, uint>(i, maxcol));
                }
                if (SHOW_FLAG) {
                    print_matrix(m);
                    cout << endl;
                }
                matrix_row<matrix<T>> step_row(m, i);
                step_row /= (T) step_row(i);
                if (SHOW_FLAG) {
                    print_matrix(m);
                    cout << endl;
                }
                for (k = i + 1; k < m.size1(); ++k) {
                    matrix_row<matrix<T>> div_row(m, k);
                    div_row -= step_row * ((T) div_row(i));
                }
                if (SHOW_FLAG) {
                    print_matrix(m);
                    cout << endl;
                }
            } else break;
        }

        // Gauss reverse step
        Solution<T> sol(m.size2() - 1, ONE_SOL);
        for (i = (uint) m.size1() ; i-- > 0 ;) {
            int j;
            bool found = false;
            for (j = (int) m.size2() - 1; j-- > 0;)
                if (!eq_safe_compare(m(i, j), 0.0)) {
                    found = true;
                    break;
                }
            if (!found) {   //zero row of variables
                if (!eq_safe_compare(m(i, m.size2() - 1), 0.0))
                    return Solution<T>(NO_SOL);
            } else        // normal variable
                break;

        }
        uint last_non_zero_row = i;
        if (last_non_zero_row < m.size2() - 2 )
            sol.set_sol_type(INF_SOL);
        for (i = last_non_zero_row + 1; i-- > 0; ){
            sol[i] = m(i, m.size2() - 1);
            for (uint j = last_non_zero_row + 1 ; j-- > i+1;){
                sol[i] -= m(i, j) * sol[j];
            }
        }

        while (!swapped.empty()){
            boost::tuples::tuple<uint, uint> tup;
            tup = swapped.top();
            uint i = get<0>(tup);
            uint j = get<1>(tup);
            T tmp = sol[i];
            sol[i] = sol[j];
            sol[j] = tmp;
            swapped.pop();
        }

        return sol;
    }

    template <typename T>
    void check_solution(matrix<T> & m, boost_vector<T> & sol){
        matrix_column<matrix<T>> b(m, m.size2() - 1);
    	boost_vector<T> th_b = prod(project(m, range(0, m.size1()), range(0, m.size2() - 1)), sol);
        boost_vector<T> abs_err = th_b - b;
        cout << "Absolute error: " << endl;
        cout << abs_err << endl;
        cout << "Norm of error: " << norm_2(th_b - b) << endl;
    }
}

#endif //LINEARSYSTEMSOLVE_FUNCTIONS_HPP