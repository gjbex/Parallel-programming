#include <boost/program_options.hpp>
#include <iostream>
#include <random>

// Returns the number of nucleotides to generate and the seed for the random
// number generator.
// The default values are:
// - number of nucleotides: 100
// - seed: 0
// Return the result as a tuple.
// Use the Boost library to parse the command line arguments.
 auto get_command_line_arguments(int argc, char* argv[]) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("nr-nucleotides", po::value<int>()->default_value(100), "number of nucleotides")
        ("seed", po::value<int>()->default_value(0), "seed for random number generator");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) { std::cout << desc << '\n'; exit(0); }
    po::notify(vm);

    return std::make_tuple(vm["nr-nucleotides"].as<int>(), vm["seed"].as<int>());
 }

int main(int argc, char* argv[]) {
    auto [nr_nucleotides, seed] = get_command_line_arguments(argc, argv);
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, 3);
    auto nucleotide = [] (const int n) {
        switch (n) {
            case 0: return 'A';
            case 1: return 'C';
            case 2: return 'G';
            case 3: return 'T';
            default: throw std::runtime_error("Invalid nucleotide");
        }
    };

    for (int n = 0; n < nr_nucleotides; ++n) {
        std::cout << nucleotide(dis(gen));
    }
    std::cout << std::endl;
    return 0;
}
