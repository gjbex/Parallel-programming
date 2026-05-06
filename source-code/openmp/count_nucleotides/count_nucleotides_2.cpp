#include <iostream>
#include <fstream>
#include <omp.h>
#include <tuple>
#include <unordered_map>

using CountMap = std::unordered_map<char, long>;

/*
 * @brief Get the size of a file
 *
 * @param file The file to get the size of
 *
 * @return The size of the file
 */
auto file_size(std::ifstream& file) {
    file.seekg(0, std::ios::end);
    auto size {file.tellg()};
    file.seekg(0, std::ios::beg);
    return size;
}

/*
 * @brief Count the number of each nucleotide in a file
 *
 * @param dna The file to count the nucleotides in
 * @param nr_nucleotides The number of nucleotides to count
 * @param offset The offset to start counting from
 *
 * @return A map of nucleotide to count
 */
CountMap count_nucleotides(std::ifstream& dna, long nr_nucleotides, long offset, int buffer_size) {
    CountMap counts;
    dna.seekg(offset);
    char buffer[buffer_size];
    for (long i = 0; i < nr_nucleotides/buffer_size; ++i) {
        dna.read(buffer, buffer_size);
        for (const auto& c : buffer) {
            ++counts[c];
        }
    }
    dna.read(buffer, nr_nucleotides % buffer_size);
    for (long i = 0; i < nr_nucleotides % buffer_size; ++i) {
        ++counts[buffer[i]];
    }
    return counts;
}

auto parse_args(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <dna_file> <buffer_size>" << std::endl;
        std::exit(1);
    }
    std::string dna_file = argv[1];
    int buffer_size = std::stoi(argv[2]);
    return std::make_tuple(dna_file, buffer_size);
}

int main(int argc, const char* argv[]) {
    const auto [file_name, buffer_size] = parse_args(argc, argv);
    std::ifstream dna(file_name);
    if (!dna) {
        std::cerr << "Could not open file: '" << file_name << "'" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <dna_file> <buffer_size>" << std::endl;
        std::exit(1);
    }
    auto size = file_size(dna);
    dna.close();

    CountMap total_counts;
    // initiliaze the map
    total_counts['A'] = 0;
    total_counts['C'] = 0;
    total_counts['G'] = 0;
    total_counts['T'] = 0;

#pragma omp parallel default(none) shared(file_name, size, total_counts, buffer_size) if(size > 1000)
    {
        std::ifstream dna(file_name);
        int num_threads {1};
        num_threads = omp_get_num_threads();
        auto nr_nucleotides = size/num_threads;
        auto offset = omp_get_thread_num()*nr_nucleotides;
        if (omp_get_thread_num() == num_threads - 1) {
            nr_nucleotides += size % num_threads;
        }
        auto counts = count_nucleotides(dna, nr_nucleotides, offset, buffer_size);
        #pragma omp critical
        {
            for (const auto& p : counts) {
                total_counts[p.first] += p.second;
            }
        }
    }
    std::cout << "file contains " << size << " bytes" << std::endl;
    int total {0};
    for (auto& p : total_counts) {
        if (p.first == 'A' || p.first == 'C' || p.first == 'G' || p.first == 'T') {
            total += p.second;
            std::cout << p.first << ": " << p.second << std::endl;
        }
    }
    std::cout << "total: " << total << std::endl;
    return 0;
}
