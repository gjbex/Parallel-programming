#include <iostream>
#include <vector>
#include <random>
#include <tuple>
#include <boost/program_options.hpp>

struct Ising {
    private:
        int N_;
        std::vector<int> spins_;
        float J_;
        float H_;
        float T_;
        float beta_;
        std::mt19937::result_type seed_;
        std::mt19937 mt_;
        std::uniform_real_distribution<double> distr_;
        float energy_;
        float magnetization_;

    public:
        Ising(const int N, const float J, const float H, const float T, const std::mt19937::result_type seed);
        void initialize();
        void update();
        float energy() const { return energy_; }
        float magnetization() const { return magnetization_; }
        float beta() const { return beta_; }
        float T() const { return T_; }
        int N() const { return N_; }
        int& sigma(const int i, const int j) { return spins_[i*N_ + j]; }
        void print_parameters(const std::string& prefix = "") const;
        friend std::ostream& operator<<(std::ostream& os, const Ising& ising);
};

Ising::Ising(const int N, const float J, const float H, const float T, const std::mt19937::result_type seed) :
    N_{N}, spins_(N*N), J_ {J}, H_ {H}, T_ {T}, beta_ {1.0f/T}, seed_ {seed}, mt_(seed_), distr_(0.0, 1.0) {
    initialize();
}

void Ising::initialize() {
    std::uniform_int_distribution<int> spin_distr(0, 1);
    for (int i = 0; i < N_; ++i) {
        for (int j = 0; j < N_; ++j) {
            sigma(i, j) = 2*spin_distr(mt_) - 1;
        }
    }
    energy_ = 0.0f;
    magnetization_ = 0.0f;
    for (int i_idx = 0; i_idx < N_; ++i_idx) {
        for (int j_idx = 0; j_idx < N_; ++j_idx) {
            int i {(i_idx + N_) % N_};
            int j {(j_idx + N_) % N_};
            energy_ -= J_*sigma(i, j)*(sigma(i, j + 1) + sigma(i + 1, j) + sigma(i - 1, j) + sigma(i, j - 1)) - H_*sigma(i, j);
            magnetization_ += spins_[i*N_ + j];
        }
    }
}

void Ising::update() {
    for (int i_idx = 0; i_idx < N_; ++i_idx) {
        for (int j_idx = 0; j_idx < N_; ++j_idx) {
            int i {(i_idx + N_) % N_};
            int j {(j_idx + N_) % N_};
            float delta_E {2*J_*sigma(i, j)*(sigma(i, j + 1) + sigma(i + 1, j) + sigma(i - 1, j) + sigma(i, j - 1)) + 2*H_*sigma(i, j)};
            if (distr_(mt_) < std::exp(-beta_*delta_E)) {
                sigma(i, j) *= -1;
                energy_ += delta_E;
                magnetization_ += 2*sigma(i, j);
            }
        }
    }
}

void Ising::print_parameters(const std::string& prefix) const {
    std::cout << prefix << "N = " << N_ << std::endl;
    std::cout << prefix << "J = " << J_ << std::endl;
    std::cout << prefix << "H = " << H_ << std::endl;
    std::cout << prefix << "T = " << T_ << std::endl;
    std::cout << prefix << "seed = " << seed_ << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Ising& ising) {
    for (int i = 0; i < ising.N(); ++i) {
        for (int j = 0; j < ising.N(); ++j) {
            os << const_cast<Ising&>(ising).sigma(i, j) << " ";
        }
        os << std::endl;
    }
    return os;
}

// function to read parameters for the Ising constructor from command line using the Boost library
// and return them as a tuple
auto read_parameters(int argc, char* argv[]) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("N", po::value<int>()->default_value(10), "set the lattice size")
        ("J", po::value<float>()->default_value(1.0f), "set the coupling constant")
        ("H", po::value<float>()->default_value(0.0f), "set the external magnetic field")
        ("T", po::value<float>()->default_value(1.0f), "set the temperature")
        ("t_max", po::value<int>()->default_value(10), "set the number of iterations")
        ("seed", po::value<std::mt19937::result_type>()->default_value(0), "set the seed for the random number generator")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        std::exit(0);
    }
    po::notify(vm);
    return std::make_tuple(vm["N"].as<int>(), vm["J"].as<float>(), vm["H"].as<float>(), vm["T"].as<float>(),
            vm["t_max"].as<int>(), vm["seed"].as<std::mt19937::result_type>());
}

int main(int argc, char* argv[]) {
    auto [N, J, H, T, t_max, seed] = read_parameters(argc, argv);
    Ising ising(N, J, H, T, seed);
    ising.print_parameters("# ");
    int t {0};
    std::cout << t << " " << ising.energy() << " " << ising.magnetization() << std::endl;
    for (t = 1; t <= t_max; ++t) {
        ising.update();
        std::cout << t << " " << ising.energy() << " " << ising.magnetization() << std::endl;
    }
    return 0;
}
