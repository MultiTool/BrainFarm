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

#include <thread> // std::thread
#include <atomic> // std::atomic
#include <unistd.h>

#include <fstream>
#include <sstream>

using namespace std;

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
/* ********************************************************************** */
const double uprecision = 1000000.0;
inline double FullTime(struct timeval tm0) {// returns time in seconds and fractions of seconds
  return tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
}
/* ********************************************************************** */
void DelayUntil(int hour) {
  struct timeval tim1;
  struct tm *tmutc;
  do { //libcurl.dll.a
    gettimeofday(&tim1, NULL);
    tmutc = gmtime(&tim1.tv_sec);
    usleep(30*1000000L);// thirty seconds
    printf("time:%02d:%02d:%02d\n", tmutc->tm_hour, tmutc->tm_min, tmutc->tm_sec);
  } while (tmutc->tm_hour<hour);
}

/* ********************************************************************** */
std::vector<std::string> &split2(const std::string &s, char delim, std::vector<std::string> &elems, int MaxDelimeters) {
  std::stringstream sstream(s);// http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
  std::string item;
  int cnt = 0;
  while (std::getline(sstream, item, delim)) {
    elems.push_back(item);
    if (++cnt>=MaxDelimeters) {break;}
  }
  if(std::getline(sstream, item)) { elems.push_back(item); }// append the leftovers
  return elems;
}
std::vector<std::string> split2(const std::string &s, char delim, int MaxDelimeters) {
  std::vector<std::string> elems;//http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
  split2(s, delim, elems, MaxDelimeters);
  return elems;
}
/* ********************************************************************** */
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);// http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;//http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
  split(s, delim, elems);
  return elems;
}

/* **************************************************************************** */
void find_and_replace(string& source, string const& find, string const& replace){
  for(std::string::size_type siz = 0; (siz = source.find(find, siz)) != std::string::npos;){
    source.replace(siz, find.length(), replace);// http://minhazulhaque.blogspot.com/2012/09/find-and-replace-all-occurrences-in-cpp-string.html.html
    siz += replace.length() - find.length() + 1;
  }
}
/* **************************************************************************** */
const std::string WhiteSpaces( " \f\n\r\t\v" );
void TrimRight( std::string& str, const std::string& TrimChars = WhiteSpaces )
{//http://stackoverflow.com/questions/479080/trim-is-not-part-of-the-standard-c-c-library
   std::string::size_type pos = str.find_last_not_of( TrimChars );
   str.erase( pos + 1 );
}
void TrimLeft( std::string& str, const std::string& TrimChars = WhiteSpaces )
{//http://stackoverflow.com/questions/479080/trim-is-not-part-of-the-standard-c-c-library
   std::string::size_type pos = str.find_first_not_of( TrimChars );
   str.erase( 0, pos );
}
void Trim( std::string& str, const std::string& TrimChars = WhiteSpaces )
{//http://stackoverflow.com/questions/479080/trim-is-not-part-of-the-standard-c-c-library
   TrimRight( str, TrimChars );
   TrimLeft( str, TrimChars );
}
/* **************************************************************************** */
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
