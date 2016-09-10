#ifndef LINEARSYSTEMSOLVE_FUNCTIONS_HPP
#define LINEARSYSTEMSOLVE_FUNCTIONS_HPP

#include "../include/Exception.hpp"
#include "../include/Solution.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/numeric/ublas/storage.hpp>
#include <boost/lockfree/stack.hpp>
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

using namespace std;
using namespace boost::numeric::ublas;
using namespace boost::random;
using namespace boost::lockfree;
using namespace boost::tuples;

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
    bool real_compare_eq(T p1, T p2){
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


    //It appeared I didn't need these
    template <typename T>
    void swap_matrix_rows(matrix<T>& m, uint i, uint j) {
        matrix_row< matrix<T>> rowa (m, i);
        matrix_row< matrix<T>> rowb (m, j);
        rowa.swap(rowb);
    }

    template <typename T>
    void swap_matrix_columns(matrix<T>& m, uint i, uint j) {
        matrix_column< matrix<T>> cola (m, i);
        matrix_column< matrix<T>> colb (m, j);
        cola.swap(colb);
    }


    template <typename T>
    Solution<T> system_solve(matrix<T>& m){
        //1st step - make matrix upper triangular
        uint i, j;
        tuple_stack swapped;
        for (i = 0, j = 0; i < m.size1() && j < m.size2() ; ++i, ++j) {
            //find non-zero element
            if (real_compare_eq<T>(m(i, j), 0)) {
                matrix_row<matrix<T>> zero_row(m, i);
                bool found = false;
                for (uint k = i + 1; k < m.size1(); ++k)
                    if (!real_compare_eq<T>(m(k, j), 0)) {
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
                if ( !real_compare_eq<T>(m(k, j), 0) && abs(m(k, j)) > abs(m(max_row_ind, j)))
                    max_row_ind = k;
            if(max_row_ind != i) {
                matrix_row<matrix<T>> cur_row(m, i);
                matrix_row<matrix<T>> max_row(m, max_row_ind);
                max_row.swap(cur_row);
            }
            if (SHOW_FLAG){
                print_matrix(m);
                cout << endl;
            }

            uint max_col_ind = j;
            for (uint k = j + 1; k < m.size2() - 1; ++k)
                if ( !real_compare_eq<T>(m(i, k), 0) && abs(m(i, k)) > abs(m(i, max_col_ind)))
                    max_col_ind = k;
            if(max_col_ind != j) {
                matrix_column<matrix<T>> cur_col(m, i);
                matrix_column<matrix<T>> max_col(m, max_col_ind);
                max_col.swap(cur_col);
                swapped.push(make_tuple<uint, uint>(j, max_col_ind));
                if (SHOW_FLAG)
                    cout << i << "  <->  " << max_col_ind << endl;
            }
            if (SHOW_FLAG){
                print_matrix(m);
                cout << endl;
            }

            matrix_row<matrix<T>> step_row(m, i);
            step_row /= (T) step_row(j);
            for (uint k = i + 1; k < m.size1(); ++k) {
                matrix_row<matrix<T>> div_row(m, k);
                div_row -= step_row * ((T) div_row(j));
            }
            if (SHOW_FLAG){
                print_matrix(m);
                cout << endl;
            }
        }
        if (SHOW_FLAG){
            cout << "Matrix after first step:" << endl;
            print_matrix(m);
            cout << endl;
        }

        //check for rows like: 0 0 0 .. 0 1 => no solutions
        for (i = (uint) m.size1(); i-- > 0;){
            if (!real_compare_eq<T>(m(i, m.size2() - 1), 0)) {
                bool found = false;
                for (j = 0; j < m.size2() - 1; ++j)
                    if (!real_compare_eq<T>(m(i, j), 0)){
                        found = true;
                        break;
                    }
                if (!found)
                    return Solution<T>(NO_SOL);
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
            if (!found)   //zero row?
                return Solution<T>(INF_SOL);
        }

        Solution<T> sol((ulong) m.size2() - 1, ONE_SOL);
        for (i = (uint) m.size2() - 1; i-- > 0;) {
            sol(i) = m(i, m.size2() - 1);
            for (j = (uint) m.size2() - 1 ; j-- > i+1;){
                sol(i) -= m(i, j) * sol(j);
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
        cout << "Norm of difference: " << norm_2(th_b - b) << endl;
    }
}

#endif //LINEARSYSTEMSOLVE_FUNCTIONS_HPP