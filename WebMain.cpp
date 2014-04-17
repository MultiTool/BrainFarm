#include <iostream>
#include <stdio.h>
#include <curl/curl.h>
#include <string>

#include <fstream>
#include <sstream>

//#include <chrono>
//#include <thread>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <string>
//#include <sstring>
#include <vector>

#include "Feeder.h"

using namespace std;

string data; //will hold the url's contents

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
/* ********************************************************************** */
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}
/* ********************************************************************** */
const double uprecision = 1000000.0;
double FullTime2(struct timeval tm0) {// returns time in seconds and fractions of seconds
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
int mainstr()
{
  DelayUntil(12);
  struct timeval tim0, tim1;
/*
  struct timespec sleepsecs, sleepleft;
  sleepsecs.tv_sec  = 0;
  sleepsecs.tv_nsec = 250000000L;// quarter second
  //sleepsecs.tv_nsec = 500000000L;
*/
  struct tm *tmlocal, *tmutc;
  double timnum0, timnum1;

  std::string baseurl = "http://finance.yahoo.com/d/quotes.csv";
  //const char *url = "http://finance.yahoo.com/d/quotes.csv?s=XOM+BBDb.TO+JNJ+MSFT+WAG+HOG+AAPL+BAC+FLPSX+VTI+RHT+NVAX+WDAY+HALO+TSLA&f=d1t1l1yrsn";
  std::string bigurl = baseurl + "?s=XOM+BBDb.TO+JNJ+MSFT+WAG+HOG+AAPL+BAC+FLPSX+VTI+RHT+NVAX+WDAY+HALO+TSLA+LNKD &f=d1t1l1yrsn";
  const char *url = bigurl.c_str();

  gettimeofday(&tim0, NULL);
  timnum0 = FullTime2(tim0);

  CURL *curl;
  CURLcode res;
  std::string readBuffer;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if(curl) {
    //curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com");
    //curl_easy_setopt(curl, CURLOPT_URL, "http://finance.yahoo.com/d/quotes.csv?s=XOM+BBDb.TO+JNJ+MSFT+HNZ+WAG+HOG+AAPL+BAC+FLPSX+VTI&f=d1t1l1yrsn");
    // curl_easy_setopt(curl, CURLOPT_URL, "http://finance.yahoo.com/d/quotes.csv?s=XOM+BBDb.TO+JNJ+MSFT+WAG+HOG+AAPL+BAC+FLPSX+VTI+RHT+NVAX+WDAY+HALO+TSLA&f=d1t1l1yrsn");
    curl_easy_setopt(curl, CURLOPT_URL, url);

    //curl_easy_setopt(curl, CURLOPT_PROXY, "http://proxy");//test

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    int cnt=0;
    while (true) { //while (cnt<10000) {
      readBuffer.clear();
      res = curl_easy_perform(curl);
      if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      }

      gettimeofday(&tim1, NULL);
      timnum1 = FullTime2(tim1);
      tmlocal = localtime(&tim1.tv_sec);
      tmutc = gmtime(&tim1.tv_sec);
      //readBuffer.replace(readBuffer.begin(), readBuffer.end(), 'a', 'X');
      //readBuffer.replace(readBuffer.begin(), readBuffer.end(), "\n", "<time/>");
      if (false) {
        printf("<cut>%06i</cut>\n", cnt);
        std::vector<std::string> chunks = split(readBuffer, '\n');
        for (int cnt1=0; cnt1<chunks.size(); cnt1++) {
          const char *txt0 = chunks.at(cnt1).c_str();
          printf("****** %s ****** \n", txt0);
        }
      } else {
        //std::cout << readBuffer << std::endl;
        const char *txt = readBuffer.c_str();
        //printf("\n %i: HAHAHAHA %s\n HAHAHAHA\n", cnt, txt);
        printf("<cut>%06i; %02d:%02d:%02d; %20.3f</cut>\n", cnt, tmlocal->tm_hour, tmlocal->tm_min, tmlocal->tm_sec, timnum1);
        //printf("<cut>%06i; %02d:%02d:%02d; %20.3f</cut>\n", cnt, tmutc->tm_hour, tmutc->tm_min, tmutc->tm_sec, t1);
        printf("%s", txt);
        printf("<endcut/>\n");

        //printf("<cut/>\n%s", txt);
        //printf(" \n %d:%02d:%02d %ld \n", tm->tm_hour, tm->tm_min, tm->tm_sec, tim0.tv_usec);
      }
      if (tmutc->tm_hour>=21) {
        //if (tmlocal->tm_hour>=17){
        break;
      }
      //usleep(250000L);// one quarter second
      usleep(500000L);// one half second
      //nanosleep(&sleepsecs, &sleepleft);
      //std::this_thread::sleep_for(std::chrono::milliseconds(x));
      //std::this_thread::sleep_for(std::chrono::milliseconds(100));
      //std::this_thread::sleep_for(100);
      cnt++;
    }// end loop
  }
  printf("\n\n\n");
  printf("Seconds elapsed:%20.3f\n", timnum1-timnum0);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  return 0;
}
/* ********************************************************************** */
void Parse() {
  string line;
  string glob;
  std::string infilepath ="s2clipped.txt";
  ifstream myfile (infilepath.c_str());
  if (myfile.is_open())
  {
    while ( getline(myfile, line) ) {
      if(line.find("<cut>") != string::npos) { // if found
        cout << "<endcut/>\n";
        cout << line << '\n';
        glob.clear();
      } else {
        cout << line << '\n';
      }
      glob.append(line);
    }
    myfile.close();
  } else cout << "Unable to open file";
  return;

  {
    std::string txt = "what:goes:here:folks";
    //std::string txt = "what,goes,here:folks";
    //std::string txt = "";
    char delim = ':';
    std::vector<std::string> elems;//http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
    split2(txt, delim, elems, 2);

    int siz = elems.size();
    for (int cnt=0; cnt<siz; cnt++) {
      cout << elems.at(cnt) << "\n";
    }
  }
}
/* ********************************************************************** */
int MainFile() {
  string line;
  ifstream myfile ("example.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      cout << line << '\n';
    }
    myfile.close();
  }

  else cout << "Unable to open file";

  return 0;
}
/* ********************************************************************** */
std::vector<std::string> ParseLine(const std::string &linetxt) {
  std::vector<std::string> ray0 = split(linetxt, '"');
  std::vector<std::string> ray1;
  std::vector<std::string> rayout;
  std::string txt0, txt1;
  //printf("ParseLine\n");
  size_t siz0 = ray0.size();
  for (int cnt0=0; cnt0<siz0; cnt0++) {
    txt0 = ray0.at(cnt0);
    if (cnt0%2==0) { // even, not quoted, split more
      //printf("even\n");
      ray1 = split(txt0, ',');
      size_t siz1 = ray1.size();
      for (int cnt1=0; cnt1<siz1; cnt1++) {
        txt1 = ray1.at(cnt1);
        rayout.push_back(txt1);
      }
    } else { // odd, quoted, copy whole
      //printf("odd\n");
      rayout.push_back(txt0);
    }
  }
  printf("return rayout;\n");
  return rayout;
}
/* ********************************************************************** */
void ParseGlob(const std::string &glob) {
  // this does not do anything real yet. we must parse to a structure
  std::vector<std::string> ray0 = split(glob, '\n');// split up all the lines
  std::vector<std::string> rayout;
  std::string txt0, txt1;
  size_t siz0 = ray0.size();
  for (int cnt0=0; cnt0<siz0; cnt0++) {
    txt0 = ray0.at(cnt0);
    rayout = ParseLine(txt0);
    size_t siz1 = rayout.size();
    for (int cnt1=0; cnt1<siz1; cnt1++) {
      txt1 = rayout.at(cnt1);
      printf("[%s]", txt1.c_str());
    }
    printf("\n");
  }
}
/* ********************************************************************** */
int Webmain()
{
  cout << "Hello world!" << endl;
  if (true) {
    std::string glob = "abcdefghijklmnop";
    Feeder feed;
    feed.Open();
    for (int cnt=0; cnt<6; cnt++) {
      feed.Fetch(glob);
      cout << "glob is:[" << glob << "]";
      printf("\n");
      ParseGlob(glob);
    }
    feed.Close();
    return 0;
  }

  //Parse(); return 0;

  if(false) {
    std::string txt = "what:goes:here:folks";
    //std::string txt = "what,goes,here:folks";
    //std::string txt = "";
    char delim = ':';
    std::vector<std::string> elems;//http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
    split2(txt, delim, elems, 2);

    int siz = elems.size();
    for (int cnt=0; cnt<siz; cnt++) {
      cout << elems.at(cnt) << "\n";
    }
    return 0;
  }
  mainstr();
  //mainX();
  //mainY();
  return 0;
}

