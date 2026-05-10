#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>

using namespace std;
namespace opts = boost::program_options;

int main(int argc, char** argv) {
    opts::options_description desc("Options");
    opts::positional_options_description popts;

    desc.add_options()
            ("program", opts::value<string>(), "program file")
            ("dim", opts::value<int>(), "specify the dimension of the program")
            ("allow-reads,r", "allow file reading via (i)")
            ("allow-writes,w", "allow file writing via (o)")
            ("allow-system,as", "allow system calls via (=)");

    opts::variables_map map;
    opts::store(opts::parse_command_line(argc, argv, desc), map);
    opts::notify(map);
}