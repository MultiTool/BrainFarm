#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include <stdlib.h>     /* srand, rand */
#include <string.h>
#include <stdint.h>
#include <float.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <iostream>
#include <vector> // example std::vector *vex;
//#include <array>
#include <hash_map>

#include <algorithm>    // std::sort
#include <math.h>
#include <cmath>  /* for std::abs(double) */
#include <time.h>       /* time */
#include <sys/time.h>
#include <stdio.h> // printf
// #define __need___va_list
#include <stdarg.h>
#include <cstdarg>

using namespace __gnu_cxx;

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

#ifndef int8_t
#define int8_t char
#endif
#ifndef uint8_t
#define uint8_t unsigned int8_t
#endif
#ifndef int32_t
#define int32_t int
//#define INT32_MAX ((~(int32_t)0)>>1)
//#define INT32_MIN (~(int32_t)0)
#define INT32_MAX 2147483647
#define INT32_MIN (-2147483647 - 1)
#endif
#ifndef uint32_t
#define uint32_t unsigned int32_t
#define INT32_MAX (~(uint32_t)0)
#endif

// #define Fudge (0.0000000000000000000001) // DBL_EPSILON  ?
//#define Fudge (DBL_EPSILON*2)
//#define Fudge (2.01e-015) // like epsilon, but epsilon tends to disappear during some math operations
#define Fudge (2.01e-010) // like epsilon, but epsilon tends to disappear during some math operations

//#ifndef override
//#define override
//#endif

#define allocsafe(T, num) (T*)malloc(sizeof(T)*num)
#define freesafe(obj) free(obj)
#define math_sgn(num) ((num>0)?1:((num<0)?-1:0))

//#define zee 'b'
//#define zee 'c'
#define zee 'm'
//#define zee 'z'

typedef uint64_t UidType;
typedef std::vector<UidType> UidVec;
typedef hash_map<UidType, UidType> UidMap;
typedef hash_map<UidType, UidType>::iterator UidMapIter;

static UidType UidSource2=0;
const UidType UidNull=0;
class IdMaker {
public:
  static UidType UidSource;
  static UidType MakeId() {
    //UidSource++;// starts at 1
    UidSource2++;// starts at 1
    return UidSource2;
  }
};

double frand() {
  return  ((double)rand()) / ((double)RAND_MAX);
}

#if true
#define bugprintf printf
#else
int bugprintf(const char *format, ...) {
  int done;// = 1;
  //return 1;

  va_list arg;

  va_start (arg, format);
  done = vfprintf (stdout, format, arg);
  va_end (arg);

  return done;
}
#endif

namespace BitInt {
  /* ********************************************************************** */
  inline static double TransBit(int val, int bitnum) {
    return ((double)((val >> bitnum)&0x1)) * 2.0 - 1.0;
  }
  inline static double TransInt(int val) {
    return ((double)val) * 2.0 - 1.0;
  }
  inline static uint32_t Bit2Int(int val, int bitnum) {
    return ((val >> bitnum)&0x1);
  }
}
/* **************************************************************************** */
static void Distribution() {
  int raysz = 10;// array size
  int numitems = 1;// number of items going into the array, randomly distributed and not overlapping

  for (int cellnum = 0; cellnum < raysz; cellnum++) {
    int raysublen = raysz - cellnum;
    double inprob = 0;
    double factout = 0;// the chance that a previous item is in the cell
    for (int cnt = 0; cnt < numitems; cnt++) {
      inprob += (1.0 - factout) / (raysublen - cnt);
      factout = inprob;
    }
    // inprob here is probability that cellnum cell is filled.
    // System.Console.WriteLine("cellnum:{0}, raysublen:{1}, inprob:{2}", cellnum, raysublen, inprob);
  }
}

#if false
/* **************************************************************************** */
static void Distribution()
{
  int raysz = 10;// array size
  int numitems = 5;// number of items going into the array, randomly distributed and not overlapping

  double TestNumer, TestDenom, ShaveNum, ShaveDenom;

  ShaveNum = 9; ShaveDenom = 10;

  TestNumer = 9 * 8 * 7 * 6 * 5; TestDenom = 10 * 9 * 8 * 7 * 6;

  // TestNumer = (factorial of (raysz-1))/(factorial of (numitems-1))
  // TestDenom = (factorial of (raysz))/(factorial of (numitems))

  /*

   (factorial of (numitems)) * (factorial of (raysz-1))
   /
   (factorial of (raysz)) * (factorial of (numitems-1))

  */

  for (int cellnum = 0; cellnum < raysz; cellnum++)
  {
    int raysublen = raysz - cellnum;
    if (raysublen < numitems)
    {
      break;// break from loop
    }
    double inprob = 0;
    double factout = 0;// the chance that a previous item is in the cell
    for (int cnt = 0; cnt < numitems; cnt++)
    {
      inprob += (1.0 - factout) / (raysublen - cnt);
      factout = inprob;
    }

    TestNumer = SubFactorial(numitems, raysublen - 1);
    TestDenom = SubFactorial(numitems + 1, raysublen);
    double fract = TestNumer / TestDenom;

    // inprob here is probability that cellnum cell is filled.
    //System.Console.WriteLine("cellnum:{0}, raysublen:{1}, inprob:{2}", cellnum, raysublen, inprob);
    System.Console.WriteLine("cellnum:{0}, raysublen:{1}, outprob:{2}, fract:{3}", cellnum, raysublen, 1.0 - inprob, fract - inprob);

    ShaveNum--; ShaveDenom--;
  }
  bool nop = true;
  /* factorial LUT
  [1,1,2,6,24,120,720,5040,40320,362880,3628800,
  39916800,479001600,6227020800,87178291200,
  1307674368000,20922789888000,355687428096000,
  6402373705728000,121645100408832000,
  2432902008176640000]
   */
}
static double SubFactorial(int start, int finish)
{
  double gather = 1.0;
  for (double cnt = start; cnt <= finish; cnt++)
  {
    gather *= cnt;
  }
  return gather;
}
#endif

#endif // BASE_H_INCLUDED
