/*
##########################################################
###   Written by: Michel Eduardo Beleza Yamagishi       ##
###   Goldbach's conjecture and HOW TO calculate Pi(n)  ##
###   Pi_2(n) and Pi_3(n)                               ##
###                                                     ##
###   This version is optimized for RAM memory. It only ##
###   stores the smallest values (p) in the memory,     ##
###   writing to the file "pprimes.txt" all the primes  ##
###   computed.                                         ##
###   Using this version, you can compute all primes    ##
###   for a REAL large number "N" with a simple desktop ##
###                                                     ##
###                                                     ##
###   Campinas, February 25 2025                        ##
##########################################################
*/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <zlib.h>

using namespace std;

bool is_prime(uint64_t num) {
    if (num <= 1) return false;
    if (num <= 3) return true;
    if ((num % 2 == 0) || (num % 3 == 0)) return false;
    uint64_t i = 5;
    while (i * i <= num) {
        if ((num % i == 0) || (num % (i + 2) == 0)) return false;
        i += 6;
    }
    return true;
}

int main(int argc, char const **argv) {
    if (argc < 3) {
        cerr << "PrimePi_Final number_max[INT] output_file.gz\n";
        return 1;
    }
    uint64_t max = atoi(argv[1]);

    // Open output files (compressed)
    gzFile out_file = gzopen(argv[2], "wb");
    if (out_file == NULL) {
        cerr << "Error opening output file\n";
        return 1;
    }
    gzFile p_write = gzopen("pprimes.txt.gz", "wb");
    if (p_write == NULL) {
        cerr << "Error opening pprimes.txt.gz\n";
        gzclose(out_file);
        return 1;
    }

    // Buffers for output
    vector<string> output_buffer;
    vector<string> primes_buffer;
    const size_t BUFFER_LIMIT = 500000;  // Flush every 500,000 entries

    // Smallest primes stored in memory
    map<uint64_t, uint64_t> p;
    p[3] = 2;  // Initial prime and count
    uint64_t count = 2;
    uint64_t maximum = 3;
    uint64_t TOL = 1299709;
    auto it2 = p.end();

    // Initial case
    output_buffer.push_back("4 = 2 + 2\n");
    primes_buffer.push_back("3\n");

    for (uint64_t i = 6; i <= max; i += 2) {
        if (is_prime(i - 3)) {
            count++;
            if (count == 100010) it2 = p.find(1299709);
            p[i - 3] = count;
            primes_buffer.push_back(to_string(i - 3) + "\n");
            if (count >= 200000) {
                it2 = p.erase(it2);
            }
            output_buffer.push_back(to_string(i) + " = 3 + " + to_string(i - 3) + "\n");
        } else {
            bool temp = false;
            for (auto it = p.begin(); it != p.end(); ++it) {
                uint64_t F = i - it->first;
                if (((F % 6 == 1) || (F % 6 == 5)) && (p.find(F) != p.end())) {
                    temp = true;
                    if (it->first > maximum) maximum = it->first;
                    output_buffer.push_back(to_string(i) + " = " + to_string(it->first) + " + " + to_string(F) + "\n");
                    break;
                }
            }
            if (!temp) {
                output_buffer.push_back("You've just disproved Goldbach for " + to_string(i) + "\n");
            }
        }

        // Flush buffers if they reach the limit
        if (output_buffer.size() >= BUFFER_LIMIT) {
            for (const auto& line : output_buffer) {
                gzputs(out_file, line.c_str());
            }
            output_buffer.clear();
        }
        if (primes_buffer.size() >= BUFFER_LIMIT) {
            for (const auto& line : primes_buffer) {
                gzputs(p_write, line.c_str());
            }
            primes_buffer.clear();
        }
    }

    // Flush remaining buffer contents
    for (const auto& line : output_buffer) {
        gzputs(out_file, line.c_str());
    }
    for (const auto& line : primes_buffer) {
        gzputs(p_write, line.c_str());
    }

    gzclose(p_write);
    gzclose(out_file);
    return 0;
}
