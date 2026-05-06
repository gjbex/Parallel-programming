/*
 * This application simulates the diffusion of a particle in 2D space.
 * The particle is initially placed at the origin and moves in a random direction
 * with constant speed.
 *
 * You can impose boundary conditions by setting the boundary conditions to true.
 * In that case, the particle will bounce off the walls.
 * If you set the boundary conditions to false, the particle will move indefinitely.
 *
 * The application will run a simulation as many times as specified, and will
 * output the average distance from the origin.
 *
 */

#include <boost/program_options.hpp>
#include <cmath>
#include <iostream>
#include <random>

/*
 * @brief Handle the input from the command line
 *
 * @param argc Number of arguments
 * @param argv Arguments
 * @return A tuple containing the number of particles, the number of steps, whether
 *        to use a bounding box, and the seed for the RNG
 */
auto handle_input(int argc, char* argv[]) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")(
        "particles", po::value<int>()->default_value(5), "number of particles")(
        "steps", po::value<int>()->default_value(10), "number of steps")(
        "box", po::value<bool>()->default_value(false), "bounding box")(
        "seed", po::value<std::mt19937::result_type>()->default_value(0), "seed for the RNG");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        std::exit(1);
    }

    auto nr_particles = vm["particles"].as<int>();
    auto max_steps = vm["steps"].as<int>();
    auto box = vm["box"].as<bool>();
    auto seed = vm["seed"].as<std::mt19937::result_type>();

    return std::make_tuple(nr_particles, max_steps, box, seed);
}

int main(int argc, char* argv[]) {
    auto [nr_particles, max_steps, bounding_box, seed] = handle_input(argc, argv);
    const int box_size {max_steps/4 };
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, 3);
    float average_distance {0.0};
    for (int particle = 0; particle < nr_particles; ++particle) {
        int x {0};
        int y {0};
        for (int step = 0; step < max_steps; ++step) {
            switch (dis(gen)) {
                case 0:
                    if (x < box_size || !bounding_box) {
                        ++x;
                    }
                    break;
                case 1:
                    if (x > -box_size || !bounding_box) {
                        --x;
                    }
                    break;
                case 2:
                    if (y < box_size || !bounding_box) {
                        ++y;
                    }
                    break;
                case 3:
                    if (y > -box_size || !bounding_box) {
                        --y;
                    }
                    break;
            }
        }
        average_distance += std::sqrt(x*x + y*y);
    }
    average_distance /= nr_particles;
    std::cout << "Average distance: " << average_distance << "\n";
    return 0;
}
