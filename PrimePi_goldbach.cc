/*
##########################################################
###   Written by: Michel Eduardo Beleza Yamagishi       ##
###   Goldbach's conjecture and HOW TO calculate Pi(n)  ##
###   Pi_2(n) and Pi_3(n)                               ##
###                                                     ##
###   Campinas, February 19 2025                        ##
##########################################################
*/

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<map>
#include<iomanip>
#include<math.h>
#include <sstream>


using namespace std;

bool is_prime(unsigned long int num){
  if (num <=1) return false;
  if (num <=3) return true;
  if ((num % 2 == 0) || (num % 3 == 0)) return false;
  unsigned long int i =5;
  while (i*i <= num){
    if ((num % i == 0) || (num % (i+2)) == 0) return false;
     i+=6;
  }//while
  return true;
} // is_prime


int main(int argc, char const ** argv)
{
  if (argc<2){
   cerr<<"PrimePi_Goldbach number_max[INT]\n";
   return 1;
  }
    int max = atoi(argv[1]);
    map<unsigned long int,unsigned long int> primes,dist;
    map<unsigned long int, unsigned long int>::iterator it;

    // inicialization of primes
    primes[3]=1;
    cout<<"4 = 2 + 2"<<endl;
    for( unsigned long int i=6; i<=max; i+=2){
      if (is_prime(i-3)){
         dist[3]+=1;
         primes[i-3]+=1;
         cout<<i<<" = "<<"3 + "<<i-3<<endl;
      }
      else{
        for( it = primes.begin(); it!=primes.end(); it++){
          unsigned long int F = i - it->first;
          if ((F % 6 == 1) || (F % 6 == 5)){
            if (is_prime(F)) {
              primes[F]+=1;
              dist[it->first]+=1;
              cout<<i<<" = "<<it->first<<" + "<<F<<endl;
              break;
            }
          } // if
        }// for it
      } // else
    } //for i

    cout<<"END"<<endl;
    cout<<"Number of primes (<="<<max<<") = "<<primes.size()+1<<endl;
    cout<<"Prime"<<"\t"<< "Frequency"<<endl;
    for( it = dist.begin(); it!=dist.end(); it++)
            cout<<it->first<<"\t"<<it->second<<endl;



    return 0;
}
