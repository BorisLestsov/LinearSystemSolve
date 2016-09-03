#include "../include/functions.hpp"
#include <fstream>

using namespace Func;

int main(int argc, char* argv[]) {
    assert(argc == 2);

    ifstream f (argv[1], ifstream::in);
    assert(f.is_open());

    int m, n;
    f >> n;
    f >> m;
    matrix<double> matr(n, m);
    fill_matrix(matr, f);
    print_matrix(matr, O_FXD, 2, cout);




    f.close();

    return 0;
}