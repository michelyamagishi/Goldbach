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


#include<algorithm>
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstdint>
#include<string>
#include<map>
#include<iomanip>
#include<math.h>
#include<cmath>
#include<sstream>
#include<vector>
#include<set>
#include<zlib.h>

using namespace std;

bool is_prime(uint64_t num){
    if (num <=1) return false;
    if (num <=3) return true;
    if ((num % 2 == 0) || (num % 3 == 0)) return false;
    uint64_t i =5;
    while (i*i <= num){
        if ((num % i == 0) || (num % (i+2)) == 0) return false;
        i+=6;
    }
    return true;
}
 
int main(int argc, char const ** argv)
{
    if (argc<3){
        cerr<<"Goldbach_128bits number_max[128bits] output_file.gz\n";
        return 1;
    }
    uint64_t max = atoi(argv[1]);
    
    // Open output file for writing (compressed)
    gzFile out_file = gzopen(argv[2], "wb");
    if (out_file == NULL) {
        cerr << "Error opening output file for writing\n";
        return 1;
    }

    // the smallest primes
    map<uint64_t,uint64_t> p;
    map<uint64_t,uint64_t>::iterator it;

    p[3]=2;
    
    // initialization of primes
    gzFile p_write = gzopen("pprimes.txt.gz", "wb");
    if (p_write == NULL) {
        cerr << "Error opening pprimes.txt.gz for writing\n";
        gzclose(out_file);
        return 1;
    }
    
    uint64_t count =2;
    uint64_t maximum = 3;
    uint64_t TOL = 1299709;
    auto it2 = p.end();
    
    char buffer[64];  // Buffer for writing to files
    
    // Write initial case to output file
    snprintf(buffer, sizeof(buffer), "4 = 2 + 2\n");
    gzputs(out_file, buffer);
    
    // Write initial prime to pprimes file
    snprintf(buffer, sizeof(buffer), "%lu\n", (unsigned long)3);
    gzputs(p_write, buffer);
    
    for(uint64_t i=6; i<=max; i+=2){
        if (is_prime(i-3)){
            count++;
            if (count == 100010) it2=p.find(1299709);     
            // Write prime to pprimes file
            snprintf(buffer, sizeof(buffer), "%lu\n", (unsigned long)(i-3));
            gzputs(p_write, buffer);
            p[i-3]=count;
            if (count>=200000){
                it2 = p.erase(it2);
            }        
            // Write to output file instead of cout
            snprintf(buffer, sizeof(buffer), "%lu = 3 + %lu\n", (unsigned long)i, (unsigned long)(i-3));
            gzputs(out_file, buffer);
        }
        else{
            bool temp=false;      
            for(it = p.begin(); it!=p.end(); ++it){
                uint64_t F = i - it->first;
                if (((F % 6 == 1) || (F % 6 == 5)) && (p.find(F)!=p.end())){
                    temp = true;
                    if (it->first > maximum) maximum=it->first;
                    // Write to output file instead of cout
                    snprintf(buffer, sizeof(buffer), "%lu = %lu + %lu\n", 
                            (unsigned long)i, (unsigned long)it->first, (unsigned long)F);
                    gzputs(out_file, buffer);
                    break;
                }    
            }
            if (not(temp)){
                // Write Goldbach failure to output file
                snprintf(buffer, sizeof(buffer), "You've just disproved Goldbach for %lu\n", (unsigned long)i);
                gzputs(out_file, buffer);
            }
        }
    }
    
    gzclose(p_write);    // Close primes file
    gzclose(out_file);   // Close output file
    return 0;
}
