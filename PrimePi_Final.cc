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
###   Campinas, February 21 2025                        ##
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
#include<deque>


using namespace std;

bool is_prime(uint64_t num){
  if (num <=1) return false;
  if (num <=3) return true;
  if ((num % 2 == 0) || (num % 3 == 0)) return false;
  uint64_t i =5;
  while (i*i <= num){
    if ((num % i == 0) || (num % (i+2)) == 0) return false;
     i+=6;
  }//while
  return true;
} // is_prime

int main(int argc, char const ** argv)
{

  if (argc<2){
   cerr<<"PrimePi_optimal number_max[INT]\n";
   return 1;
  }
    uint64_t max = atoi(argv[1]);
    // the smallest primes
    map<uint64_t, uint64_t> p;
    deque<uint64_t> q;

    q.push_back(3);
    q.push_back(5);
    p[3]=0;

    map<uint64_t, uint64_t>::iterator it;

    // inicialization of primes

    ofstream p_write("pprimes.txt");
    // ifstream p_read("pprimes.txt");
    uint64_t count =0;


    cout<<"4 = 2 + 2"<<endl;
    for( uint64_t i=6; i<=max+2; i+=2){
      if (is_prime(i-3)){
//       cout<<q.size()<<"\t"<<q.front()<<"\t"<<q.back()<<endl;
         auto lastp = prev(p.end());
         if ( (q.size()>3) && (q.back()-q.front() > lastp->first) ){
            q.pop_front();
         }
         q.push_back(i-3);
         p_write<<i-3<<endl;
         count++;
         p[3]+=1;
         cout<<i<<" = "<<"3 + "<<i-3<<endl;
      }
      else{
        bool temp=false;
        for( it = p.begin(); it!=p.end(); ++it){
          uint64_t F = i - it->first;
          if (((F % 6 == 1) || (F % 6 == 5)) && ( find(q.begin(),q.end(),F)!=q.end() ) ){
              p[it->first]+=1;
              temp = true;
              cout<<i<<" = "<<it->first<<" + "<<F<<endl;
              break;
          } // if
        }// for it
        if (temp==false){
         string s;
         ifstream p_read("pprimes.txt");
         while (temp == false && getline(p_read,s) ){
           unsigned long int t1 = atoi(s.c_str());
           unsigned long int t2 = i - t1;

           if(((t2 % 6 == 1) || (t2 % 6 == 5)) && ( find(q.begin(),q.end(),t2)!=q.end() )   ){
           // p_write<<t2<<endl;
            temp = true;
            p[t1]+=1;
            cout<<i<<" = "<<t1<<" + "<<t2<<endl;
          }
         } //while
         p_read.close();
        }// if temp=0
      } // else
    } //for i
    p_write.close();
    cout<<"END"<<endl;
    cout<<"Number of primes (<="<<max<<") = "<<count<<endl;
    cout<<"Prime"<<"\t"<< "Frequency"<<endl;
    for( it = p.begin(); it!=p.end(); it++)
            cout<<it->first<<"\t"<<it->second<<endl;



    return 0;
}
