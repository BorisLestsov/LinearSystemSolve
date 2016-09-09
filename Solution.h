#ifndef LINEARSYSTEMSOLVE_SOLUTION_H
#define LINEARSYSTEMSOLVE_SOLUTION_H

#include <iostream>
#include <vector>

using namespace std;

typedef enum {NO_SOL, ONE_SOL, INF_SOL} Sol_t;

template <typename T>
class Solution {
    vector<T> sol;
    Sol_t s_type;

public:
    Solution(vector<T> &, Sol_t);

    T& operator[](uint);
};



#endif //LINEARSYSTEMSOLVE_SOLUTION_H
