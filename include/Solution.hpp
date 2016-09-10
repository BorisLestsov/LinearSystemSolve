#ifndef LINEARSYSTEMSOLVE_SOLUTION_H
#define LINEARSYSTEMSOLVE_SOLUTION_H

#include <iostream>
#include <boost/numeric/ublas/vector.hpp>

using namespace std;

template <typename T>
using boost_vector = boost::numeric::ublas::vector<T>;

typedef enum {NO_SOL, ONE_SOL, INF_SOL} Sol_t;


template <typename T>
class Solution: public boost_vector<T> {
    Sol_t sol_type;

public:
    Solution(boost_vector<T> &) = delete;
    Solution(ulong dim, Sol_t);
    Solution(const Solution &);
    Solution(const boost_vector<T> &, Sol_t);
    explicit Solution(Sol_t);

    inline Sol_t get_sol_type() const;

    template <typename Y>
    friend ostream& operator<<(ostream&, const Solution<Y> &);
};

template <typename T>
ostream& operator<<(ostream& o, const Solution<T> & s){
    if (s.get_sol_type() == NO_SOL)
        cout << "No solution" << endl;
    else {
        if (s.get_sol_type() == INF_SOL)
            cout << "Infinite amount of solutions" << endl;
        boost_vector<T> v = s;
        o << "Solution" << endl << v << endl;
    }
    return o;
}

template <typename T>
Solution<T>::Solution(const boost_vector<T> & v, Sol_t t):
        boost_vector<T>(v),
        sol_type(t)
{}

template <typename T>
Solution<T>::Solution(const Solution & s):
        boost_vector<T>(s),
        sol_type(s.get_sol_type())
{}

template <typename T>
Solution<T>::Solution(ulong dim, Sol_t t):
        boost_vector<T>(dim),
        sol_type(t)
{}

template <typename T>
Solution<T>::Solution(Sol_t s):
        boost_vector<T>(0),
        sol_type(s)
{}

template <typename T>
Sol_t Solution<T>::get_sol_type() const{
    return sol_type;
}

#endif //LINEARSYSTEMSOLVE_SOLUTION_H
