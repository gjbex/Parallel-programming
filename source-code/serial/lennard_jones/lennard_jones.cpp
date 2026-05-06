#include <boost/program_options.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <tuple>
#include <vector>

struct LennardJones {
    private:
        float epsilon_;
        float sigma_;
    public:
        LennardJones(float epsilon, float sigma) : epsilon_(epsilon), sigma_(sigma) {}
        float force(float r) const {
            float r6 = pow(sigma_ / r, 6);
            float r12 = r6 * r6;
            return 24 * epsilon_ * (2 * r12 - r6) / r;
        }
        float energy(float r) const {
            float r6 = pow(sigma_ / r, 6);
            float r12 = r6 * r6;
            return 4 * epsilon_ * (r12 - r6);
        }
        float epsilon() const { return epsilon_; }
        float sigma() const { return sigma_; }
};

struct Particle {
    private:
        float x_;
        float y_;
        float z_;
        float vx_;
        float vy_;
        float vz_;
        float mass_;
    public:
        Particle(float x, float y, float z, float vx,
                float vy, float vz, float mass) :
            x_(x), y_(y), z_(z), vx_(vx), vy_(vy), vz_(vz), mass_(mass) {}
        float x() const { return x_; }
        float y() const { return y_; }
        float z() const { return z_; }
        float vx() const { return vx_; }
        float vy() const { return vy_; }
        float vz() const { return vz_; }
        float mass() const { return mass_; }
        void move(const float dt) {
            x_ += vx_ * dt;
            y_ += vy_ * dt;
            z_ += vz_ * dt;
        }
        void accelerate(const float ax, const float ay, const float az, const float dt) {
            vx_ += ax * dt;
            vy_ += ay * dt;
            vz_ += az * dt;
        }
        float kinetic_energy() const {
            return 0.5*mass_*(vx_*vx_ + vy_*vy_ + vz_*vz_);
        }
};

float distance(const Particle& p1, const Particle& p2) {
    const float dx {p1.x() - p2.x()};
    const float dy {p1.y() - p2.y()};
    const float dz {p1.z() - p2.z()};
    return sqrt(dx*dx + dy*dy + dz*dz);
}

// struct that allows to sample from a Maxwell-Boltzmann distribution
// with a given temperature and mass of the particles
struct MaxwellBoltzmannDistribution {
    private:
        float temperature_;
        float mass_;
        std::mt19937 generator_;
        std::normal_distribution<float> velocity_distr_;
        std::uniform_real_distribution<float> component_distr_;
    public:
        MaxwellBoltzmannDistribution(const float temperature, const float mass, std::mt19937& generator) :
            temperature_ {temperature}, mass_ {mass}, generator_{std::ref(generator)},
                         velocity_distr_(0.0f, sqrt(temperature/mass)),
                         component_distr_(-1.0f, 1.0f) {}
        float temperature() const { return temperature_; }
        float mass() const { return mass_; }
        float sample() {
            return velocity_distr_(generator_);
        }
        auto sample_vector() {
            float velocity {sample()};
            float x {component_distr_(generator_)};
            float y {component_distr_(generator_)};
            float z {component_distr_(generator_)};
            float norm {sqrtf(x*x + y*y + z*z)};
            return std::make_tuple(velocity*x/norm, velocity*y/norm, velocity*z/norm);
        }
};

// struct that allows to sample from a uniform distribution to determine
// the initial position of the particles
/*
 * @brief Struct that allows to sample from a uniform distribution
 *
 * This struct allows to sample from a uniform distribution to determine
 * the initial position of the particles.
 */
struct PositionDistribution {
    private:
        float min_value_;
        float max_value_;
        std::mt19937 generator_;
        std::uniform_real_distribution<float> position_distr_;
    public:
        /*
         * @brief Constructor
         *
         * @param generator The random number generator
         * @param min_value The minimum value of the distribution
         * @param max_value The maximum value of the distribution
         */
        explicit PositionDistribution(std::mt19937& generator, const float min_value, const float max_value) :
            min_value_ {min_value}, max_value_ {max_value},
            generator_ {std::ref(generator)}, position_distr_(min_value, max_value) {}
        /*
         * @brief Returns the minimum value of the distribution
         *
         * @return The minimum value of the distribution
         */
        float min_value() const { return min_value_; }
        /*
         * @brief Returns the maximum value of the distribution
         *
         * @return The maximum value of the distribution
         */
        float max_value() const { return max_value_; }
        /*
         * @brief Samples from the distribution
         *
         * @return A tuple containing the sampled x, y and z coordinates
         */
        auto sample_vector() {
            float x {position_distr_(generator_)};
            float y {position_distr_(generator_)};
            float z {position_distr_(generator_)};
            return std::make_tuple(x, y, z);
        }
};

/*
 * @brief Initializes the particles
 *
 * This function initializes the particles by sampling their positions
 * from a uniform distribution and their velocities from a Maxwell-Boltzmann
 * distribution.
 *
 * @param nr_particles The number of particles to be initialized
 * @param temperature The temperature of the system
 * @param mass The mass of the particles
 * @param seed The seed for the random number generator
 * @return A vector of particles
 */
std::vector<Particle> initialize_particles(const int nr_particles, const float temperature, const float mass,
        const std::mt19937::result_type seed) {
    std::mt19937 generator(seed);
    MaxwellBoltzmannDistribution mb_distr(temperature, mass, generator);
    PositionDistribution pos_distr(generator, -1.0e-6f, 1.0e-6f);
    std::vector<Particle> particles;
    for (int i = 0; i < nr_particles; ++i) {
        auto [x, y, z] = pos_distr.sample_vector();
        auto [vx, vy, vz] = mb_distr.sample_vector();
        particles.emplace_back(x, y, z, vx, vy, vz, mass);
    }
    return particles;
}

// function that returns a unit vector that points from p1 to p2 as a tuple
/*
 * @brief Calculates the unit vector pointing from p1 to p2
 *
 * This function calculates the unit vector pointing from p1 to p2
 * as a tuple.
 *
 * @param p1 The first particle
 * @param p2 The second particle
 * @return A tuple containing the x, y and z component of the unit vector
 */
auto unit_vector(const Particle& p1, const Particle& p2) {
    float dx {p2.x() - p1.x()};
    float dy {p2.y() - p1.y()};
    float dz {p2.z() - p1.z()};
    float norm {sqrtf(dx*dx + dy*dy + dz*dz)};
    return std::make_tuple(dx/norm, dy/norm, dz/norm);
}

// q: in the function total_energy, could we use an STL algorithm instead of a for loop?
// a: yes, we could use std::accumulate
// q: could you show me how?
// a: sure, here you go
// q: thanks!
// a: you're welcome!
/*
 * @brief Calculates the total force acting on a particle
 *
 * This function calculates the total force acting on a particle
 * by summing over the forces acting on it from all other particles
 * in the system.
 *
 * @param particle The particle for which the total force is calculated
 * @param particles The vector of all particles in the system
 * @param lennard_jones The LennardJones object that describes the interaction
 *        between two particles
 * @return A tuple containing the x, y and z component of the total force
 */
auto total_force(const Particle& particle, const std::vector<Particle>& particles,
        const LennardJones& lennard_jones) {
    float force_x {0.0f};
    float force_y {0.0f};
    float force_z {0.0f};
    for (const auto& other_particle : particles) {
        if (&particle != &other_particle) {
            float r {distance(particle, other_particle)};
            float force {lennard_jones.force(r)};
            auto [dx, dy, dz] = unit_vector(particle, other_particle);
            force_x += force * dx;
            force_y += force * dy;
            force_z += force * dz;
        }
    }
    return std::make_tuple(force_x, force_y, force_z);
}

/*
 * @brief Calculates the total energy of the system
 *
 * This function calculates the total energy of the system by summing
 * the kinetic energy of each particle and the potential energy between
 * each pair of particles.
 *
 * @param particles The vector of particles
 * @param lennard_jones The Lennard-Jones potential
 * @return The total energy of the system
 */
float total_energy(const std::vector<Particle>& particles, const LennardJones& lennard_jones) {
    float energy {0.0f};
    for (const auto& particle : particles) {
        energy += particle.kinetic_energy();
        for (const auto& other_particle : particles) {
            if (&particle != &other_particle) {
                float r {distance(particle, other_particle)};
                energy += lennard_jones.energy(r);
            }
        }
    }
    return energy;
}

/*
 * @brief Updates the positions and velocities of the particles
 *
 * This function updates the positions and velocities of the particles
 * by calculating the total force acting on each particle and using
 * Newton's second law to calculate the acceleration and velocity.
 * The positions are updated using the new velocity.
 *
 * @param particles The vector of particles
 * @param dt The time step
 * @param lennard_jones The Lennard-Jones potential
 */
void update_particles(std::vector<Particle>& particles, const float dt, const LennardJones& lennard_jones) {
    for (auto& particle : particles) {
        auto [force_x, force_y, force_z] = total_force(particle, particles, lennard_jones);
        float ax {force_x / particle.mass()};
        float ay {force_y / particle.mass()};
        float az {force_z / particle.mass()};
        particle.accelerate(ax, ay, az, dt);
        particle.move(dt);
    }
}

/*
 * @brief Calculates the center of mass of a system of particles
 *
 * This function calculates the center of mass of a system of particles
 * by summing the mass of each particle and the product of its position
 * and mass and dividing by the total mass.
 *
 * @param particles The vector of particles
 * @return The center of mass as a tuple of the x, y and z coordinates
 */
auto center_of_mass(const std::vector<Particle>& particles) {
    float x {0.0f};
    float y {0.0f};
    float z {0.0f};
    float total_mass {0.0f};
    for (const auto& particle : particles) {
        x += particle.x() * particle.mass();
        y += particle.y() * particle.mass();
        z += particle.z() * particle.mass();
        total_mass += particle.mass();
    }
    return std::make_tuple(x/total_mass, y/total_mass, z/total_mass);
}

/*
 * @brief Calculates the maximum distance of a particle from the center of mass
 *
 * This function calculates the maximum distance of a particle from the center of mass
 *
 * @param particles The vector of particles
 * @return The maximum distance of a particle from the center of mass
 */
float maximum_distance(const std::vector<Particle>& particles) {
    float max_distance {0.0f};
    auto [x_com, y_com, z_com] = center_of_mass(particles);
    for (const auto& particle : particles) {
        float dist {sqrtf(powf(particle.x() - x_com, 2) + powf(particle.y() - y_com, 2) + powf(particle.z() - z_com, 2))};
        if (dist > max_distance) {
            max_distance = dist;
        }
    }
    return max_distance;
}

// function that returns the average temperature of the particles
// by averaging over the kinetic energy of the particles
// (which is proportional to the temperature)
// the function also returns the standard deviation of the temperature
// as a tuple
/*
 * @brief Calculates the average temperature of the particles as well as its standard deviation
 *
 * This function calculates the average temperature of the particles
 *
 * @param particles The vector of particles
 * @return A tuple containing the average temperature and the standard deviation
 */
auto average_temperature(const std::vector<Particle>& particles) {
    float temperature {0.0f};
    float temperature_squared {0.0f};
    for (const auto& particle : particles) {
        temperature += particle.kinetic_energy()/particle.mass();
        temperature_squared += powf(particle.kinetic_energy()/particle.mass(), 2);
    }
    temperature /= particles.size();
    temperature_squared /= particles.size();
    float standard_deviation {sqrtf(temperature_squared - temperature*temperature)};
    return std::make_tuple(temperature, standard_deviation);
}

// function that handle command line arguments using the Boost library
// and returns the values as a tuple.
// default values:
// nr_particles = 10
// nr_steps = 20
// dt = 1.0e-9
// temperature = 300.0
// mass = 3.0e-26
// seed = 0
auto read_input(int argc, char* argv[]) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("nr_particles", po::value<int>()->default_value(10), "number of particles")
        ("nr_steps", po::value<int>()->default_value(20), "number of steps")
        ("dt", po::value<float>()->default_value(1.0e-9f), "time step")
        ("temperature", po::value<float>()->default_value(300.0f), "temperature")
        ("mass", po::value<float>()->default_value(3.0e-26f), "mass")
        ("seed", po::value<std::mt19937::result_type>()->default_value(static_cast<std::mt19937::result_type>(0)), "seed for random number generator")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << "\n";
        exit(0);
    }
    po::notify(vm);
    return std::make_tuple(vm["nr_particles"].as<int>(), vm["nr_steps"].as<int>(),
            vm["dt"].as<float>(), vm["temperature"].as<float>(), vm["mass"].as<float>(),
            vm["seed"].as<std::mt19937::result_type>());
}

// function to validate the input parameters
// if the input parameters are invalid, the program exits
// with an error message
void validate_input(const int nr_particles, const int nr_steps, const float dt, const float temperature, const float mass, const std::mt19937::result_type seed) {
    if (nr_particles <= 0) {
        std::cerr << "Error: number of particles must be positive\n";
        exit(1);
    }
    if (nr_steps <= 0) {
        std::cerr << "Error: number of steps must be positive\n";
        exit(1);
    }
    if (dt <= 0.0f) {
        std::cerr << "Error: time step must be positive\n";
        exit(1);
    }
    if (temperature <= 0.0f) {
        std::cerr << "Error: temperature must be positive\n";
        exit(1);
    }
    if (mass <= 0.0f) {
        std::cerr << "Error: mass must be positive\n";
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    const float k_b {1.38064852e-23f};
    const float equilibrium_distance {3.0e-10f};
    const float well_depth {40.0f * 1.38064852e-23f};
    auto [nr_particles, nr_steps, dt, temperature, mass, seed] = read_input(argc, argv);
    std::vector<Particle> particles {initialize_particles(nr_particles, temperature*k_b, mass, seed)};
    LennardJones lennard_jones(equilibrium_distance, well_depth);
    std::cout << "# time temperature standard_deviation energy max_distance\n";
    float time {0.0f};
    auto [initial_temperature, initial_standard_deviation] = average_temperature(particles);
    std::cout << time << " " << initial_temperature << " " << initial_standard_deviation << " "
        << total_energy(particles, lennard_jones) << " " << maximum_distance(particles) << "\n";
    for (int step = 1; step <= nr_steps; ++step) {
        time += dt;
        update_particles(particles, dt, lennard_jones);
        auto [temperature, standard_deviation] = average_temperature(particles);
        std::cout << time << " " << temperature << " " << standard_deviation << " "
            << total_energy(particles, lennard_jones) << " " << maximum_distance(particles) << "\n";
    }
    return 0;
}
