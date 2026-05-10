#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>

using namespace std;
namespace opts = boost::program_options;

int main(int argc, char** argv) {
    opts::options_description desc("Options");
    desc.add_options()
            ("program", opts::value<string>(), "program file")
            ("dim", opts::value<int>(), "specify the dimension of the program")
            ("allow-system,as", "allow system calls via (=)");

    opts::variables_map map;
    opts::store(opts::parse_command_line(argc, argv, desc), map);
}