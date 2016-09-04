#include "../include/functions.hpp"
#include <fstream>

using namespace Func;

int main(int argc, char* argv[]) {

    uint m, n;
    ifstream f(argv[1], ifstream::in);
    try {
        if (argc != 2)
            throw Exception("No input file");

        if (!f.is_open())
            throw Exception("Could not open file");
        f >> n;
        f >> m;
        if (m != n + 1)
            throw Exception("Wrong matrix in file");
    }
    catch (Exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    matrix<double> matr(n, m);

    fill_matrix(matr, f);
    f.close();
    print_matrix(matr);
    cout << endl;
    system_solve(matr);

    return 0;
}