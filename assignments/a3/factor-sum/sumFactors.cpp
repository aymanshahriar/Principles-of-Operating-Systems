// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================

#include "sumFactors.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

// calculates smallest divisor of n that is less than n
//
// returns 0 if n is prime or n < 2
//
// examples:
//    n =  1         --> result = 0
//    n =  2         --> result = 0
//    n =  9         --> result = 3
//    n = 35         --> result = 5
//    n = 8589934591 --> result = 7
//
// you may modify this function
//
int64_t get_smallest_divisor(int64_t n)
{
  if( n <= 3) return 0; // 2 and 3 are primes
  if( n % 2 == 0) return 2; // handle multiples of 2
  if( n % 3 == 0) return 3; // handle multiples of 3
  int64_t i = 5;
  int64_t max = sqrt(n);
  while( i <= max) {
    if (n % i == 0) return i;
    if (n % (i+2) == 0) return i + 2;
    i += 6;
  }
  return 0;
}

// reads in a positive integers in range (2 .. 2^63-1) from standard input
// for each number on input:
//   if the number is prime, it is ignored
//   determines the smallest factor of the composite number
// prints out the sum of all smallest factors
//
// if no composite numbers are found, it returns 0
//
// please note: the current implementation is single-threades, so
// n_threads argument is ignored
//
// you may modify this function
//
int64_t sum_factors(int n_threads)
{
  int64_t sum = 0;
  while( 1) {
    int64_t num;
    if( ! (std::cin >> num) ) break;
    int64_t div = get_smallest_divisor(num);
    sum += div;
  }
  return sum;
}
