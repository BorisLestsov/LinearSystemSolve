#include "../include/functions.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace Func;

typedef double T;
bool SHOW_FLAG      = false;
bool RAND_FLAG      = false;
bool CHECK_FLAG     = true;

int main(int argc, const char* argv[]) {
    ifstream f;
    matrix<T> matr;

    po::options_description desc{"Allowed options"};
    desc.add_options()
            ("help", "Print help information")
            ("rand,r", po::value< std::vector<int> >()->multitoken(),
                 "Randomize system. Arguments are: n, m, range")
            ("show,s", "Show process of solution")
            ("noprint", "Don't print system")
            ("input,i", po::value<string>(), "Input file")
            ("nocheck", "Don't show error");
    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    try {
        if (vm.count("help")) {
            cout << desc << endl;
            return 1;
        }
        if (vm.count("show")) {
            SHOW_FLAG = true;
        }
        if (vm.count("nocheck")) {
            CHECK_FLAG = true;
        }
        if (vm.count("rand")) {
            std::vector<int> v = vm["rand"].as< std::vector<int> >();
            if (v.size() != 3)
                throw Exception("Wrong Arguments: Expected 3 arguments after --rand");
            RAND_FLAG = true;
            fill_matrix_rand(matr, v[0], v[1], v[2]);
        }
        if (vm.count("input")) {
            if(vm.count("rand"))
                throw Exception("Wrong arguments: --input and --rand at the same time");
            f.open(vm["input"].as<string>(), ifstream::in);
            if (!f.is_open())
                throw Exception("Could not open file");
            fill_matrix_from_stream(matr, f);
        } else if (!vm.count("rand")) {
            cout << "Input n, m and n*m numbers: " << endl;
            fill_matrix_from_stream(matr, cin);
        }
        if (!vm.count("noprint")) {
            print_matrix(matr);
            cout << endl;
        }
    } catch (Exception &e) {
        cerr << e.what() << endl;
        return -1;
    }

    matrix<T> matr_copy = matr;
    Solution<T> sol = system_solve(matr_copy);
    cout << sol << endl;

    if(CHECK_FLAG && sol.get_sol_type() != NO_SOL)
        check_solution(matr, sol);

    return 0;
}