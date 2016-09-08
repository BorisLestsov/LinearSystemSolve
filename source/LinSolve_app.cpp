#include "../include/functions.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace Func;

typedef double T;
bool DEBUG_MODE     = false;
bool PRINT_MODE     = true;
bool RAND_MATRIX    = false;
bool CHECK_MODE     = true;
double RAND_RANGE   = 1e1;

int main(int argc, const char* argv[]) {
    uint m, n;
    ifstream f;

    po::options_description desc{"Allowed options"};
    desc.add_options()
            ("help", "Print help information")
            ("rand", po::value<uint>(), "Randomize matrix")
            ("range", po::value<double>(), "Set range of random numbers in the matrix")
            ("debug", "Debug mode")
            ("noprint", "Don't print matrix")
            ("input", po::value<string>(), "Input file")
            ("nocheck", "Don't check solution");
    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }
    if (vm.count("debug")) {
        DEBUG_MODE = true;
    }
    if (vm.count("nocheck")) {
        CHECK_MODE = false;
    }
    if (vm.count("noprint")) {
        PRINT_MODE = false;
    }
    if (vm.count("rand")) {
        RAND_MATRIX = true;
        n = vm["rand"].as<uint>();
    }
    if (vm.count("range")) {
        RAND_RANGE = abs(vm["range"].as<double>());
    }
    if (vm.count("input")){
        try {
            f.open(vm["input"].as<string>(), ifstream::in);
            if (!f.is_open())
                throw Exception("Could not open file");
            f >> n;
        }
        catch (Exception &e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    m = n + 1;
    matrix<T> matr;


    if (RAND_MATRIX) {
        matr = matrix<T>(n, m);
        fill_matrix_rand(matr);
    } else {
        try {
            matr = matrix<T>(n, m);
            fill_matrix_from_stream(matr, f);
            f.close();
        }
        catch (Exception &e) {
            cerr << e.what() << endl;
            return -1;
        }
    }

    if(PRINT_MODE){
     	print_matrix(matr);
    	cout << endl;
    }

    matrix<T> matr_copy = matr;
    boost_vector<T> sol = system_solve(matr_copy);

    if(CHECK_MODE && sol.size() != 0)
        check_solution(matr, sol);

    return 0;
}