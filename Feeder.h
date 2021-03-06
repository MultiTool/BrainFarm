#ifndef FEEDER_H_INCLUDED
#define FEEDER_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <curl/curl.h>
#include <string>

#include <fstream>
#include <sstream>

#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <string>
#include <vector>
#include "Base.h"

using namespace std;

// will be merged with Feed.h

/* ********************************************************************** */
class Feeder {
public:
  /* ********************************************************************** */
  virtual void Open() {}
  virtual void Close() {}
  virtual void Fetch(std::string &glob) { }
};
/* ********************************************************************** */
class FileFeeder : public Feeder {
public:
  std::string infilepath ="sample.txt";
  ifstream infile;
  /* ********************************************************************** */
  virtual void Open() {
    infile.open(infilepath.c_str());
    if (infile.is_open()) {
    } else cout << "Unable to open file";
  }
  /* ********************************************************************** */
  void Close() {
    infile.close();
  }
  /* ********************************************************************** */
  void Fetch(std::string &glob) override {
    string line;
    // string glob;
    glob.clear();
    if (!infile.is_open()) {
      return;
    }
    bool InChunk = false;
    while (getline(infile, line)) {
      if (InChunk) {
        if(line.find("<endcut/>") != string::npos) {
          InChunk=false; break;
        }
        glob.append(line); glob.append("\n");
      } else {
        if(line.find("<cut>") != string::npos) {
          // process header cut here
          InChunk=true; glob.clear();
        }
      }
    }
  }
};
/* ********************************************************************** */
class WebFeeder : public Feeder {
public:
  CURL *curl;
  // http://finance.yahoo.com/d/quotes.csv?s=XOM+BBD-B.TO+JNJ+MSFT+WAG+HOG+AAPL+BAC+FLPSX+VTI+RHT+NVAX+WDAY+HALO+TSLA+LNKD &f=d1t1l1yrsn
  std::string readBuffer;
  std::string baseurl = "http://finance.yahoo.com/d/quotes.csv";
  std::string bigurl = baseurl + "?s=XOM+BBD-B.TO+JNJ+MSFT+WAG+HOG+AAPL+BAC+FLPSX+VTI+RHT+NVAX+WDAY+HALO+TSLA+LNKD &f=d1t1l1yrsn";
  std::vector<std::string> symbols {
    "XOM", "BBD-B.TO", "JNJ", "MSFT", "WAG", "HOG", "AAPL", "BAC", "FLPSX", "VTI", "RHT", "NVAX", "WDAY", "HALO", "TSLA", "LNKD"
  };
  const char *url = bigurl.c_str();
  int FetchCnt;
  /* ********************************************************************** */
  WebFeeder() {
    BuildUrl();
    baseurl = "http://finance.yahoo.com/d/quotes.csv";
    bigurl = baseurl + "?s=XOM+BBD-B.TO+JNJ+MSFT+WAG+HOG+AAPL+BAC+FLPSX+VTI+RHT+NVAX+WDAY+HALO+TSLA+LNKD &f=d1t1l1yrsn";
    url = bigurl.c_str();
    FetchCnt = 0;
  }
  /* ********************************************************************** */
  void Open() override {
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url);
      //curl_easy_setopt(curl, CURLOPT_PROXY, "http://proxy");//test
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    }
  }
  /* ********************************************************************** */
  void Close() override {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }
  /* ********************************************************************** */
  void Fetch(std::string &glob) override {
    CURLcode res;
    struct timeval tim0, tim1;
    struct tm *tmlocal, *tmutc;
    double timnum0, timnum1;

    gettimeofday(&tim0, NULL);
    timnum0 = FullTime(tim0);

    string line;
    glob.clear();

    readBuffer.clear();
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    gettimeofday(&tim1, NULL);
    timnum1 = FullTime(tim1);
    tmlocal = localtime(&tim1.tv_sec);
    tmutc = gmtime(&tim1.tv_sec);

    const char *txt = readBuffer.c_str();
    //printf("<cut>%06i; %02d:%02d:%02d; %20.3f</cut>\n", FetchCnt, tmlocal->tm_hour, tmlocal->tm_min, tmlocal->tm_sec, timnum1);
    //printf("%s", txt);
    glob = readBuffer;
    FetchCnt++;
  }
  /* ********************************************************************** */
  void SortSymbols() {
    std::sort(symbols.begin(), symbols.end());
  }
  /* ********************************************************************** */
  void BuildUrl() {
    SortSymbols();
    std::string symtxt;
    std::string UrlTxt = baseurl + "?s=";
    size_t siz = symbols.size();
    for (int cnt=0; cnt<siz; cnt++) {
      symtxt = symbols.at(cnt);
      UrlTxt.append(symtxt + "+");
    }
    UrlTxt.append(" &f=d1t1l1yrsn");
  }
  /* ********************************************************************** */
  static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
  {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
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

    gettimeofday(&tim0, NULL);
    timnum0 = FullTime(tim0);

    CURLcode res;
    std::string readBuffer;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
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
        timnum1 = FullTime(tim1);
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
};
/* ********************************************************************** */
class JunkYard { // this holds experimental scraps to be cleaned and added to other parts
public:
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
  inline static uint64_t BaseN(char chr) {
    uint64_t num = 0;
    if ('0' <= chr && chr <= '9') {
      num = chr - 48;
    } else if ('A' <= chr && chr <= 'Z') {
      num = 10 + (chr - 65);
    } else  if ('a' <= chr && chr <= 'z') {
      num = 10 + (chr - 97);
    } else  if (chr == '-') {
      num = 36;
    } else  if (chr == '.') {
      num = 37; // TextRadix-1
    } else {
      printf("Character out of range!");
    }
    return num;
  }
#define TextRadix 38;
#define ShiftDist 6
#define MaxShifts ((uint64_t)((sizeof(uint64_t)*8)/ShiftDist))
  /* ********************************************************************** */
  inline static uint64_t TxtDex(const std::string &line) {
    const char *txt = line.c_str();// final number will NOT sort alphabetically
    uint64_t retval = 0;// http://eoddata.com/stocklist/NYSE/B.htm
    uint64_t lsiz = line.size();
    if (lsiz>5) {
      printf("Symbol is too long!");
    }
    int limit = std::min(MaxShifts, lsiz);// not right in this context, redo
    for (int cnt=0; cnt<limit; cnt++) {
      retval *= TextRadix; // more compact than shifting
      retval += BaseN(txt[cnt]);
    }
    //retval *= pow(TextRadix, (???-limit));// uncomment if you want order to be alphabetical
    return retval;
  }
  /* ********************************************************************** */
  inline static uint64_t TxtDex2(const std::string &line) {
    const char *txt = line.c_str();// final number will NOT sort alphabetically
    uint64_t retval = 0;// http://eoddata.com/stocklist/NYSE/B.htm
    uint64_t lsiz = line.size();
    if (lsiz>5) {
      printf("Symbol is too long!");
    }
    int limit = std::min(MaxShifts, lsiz);
    for (int cnt=0; cnt<limit; cnt++) {
      retval <<= ShiftDist;
      retval |= BaseN(txt[cnt]);
    }
    //retval <<= (ShiftDist*(MaxShifts-limit));// uncomment if you want order to be alphabetical
    return retval;
  }
  /* ********************************************************************** */
  static void ParseYahoo(const std::string txt, IoJackBase *IoJack) {
    string glob;
    std::string field;
    //std::string txt = "what,goes,here:folks";
    //std::string txt = "";
    char delim = ',';
    std::vector<std::string> elems;//http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
    //"4/8/2014",  "4:00pm",  66.76,  1.36,  20.35,  "HOG",  "Harley-Davidson, "
    split2(txt, delim, elems, 6);

    int siz = elems.size();
    for (int cnt=0; cnt<siz; cnt++) {
      Trim(elems.at(cnt));// remove spaces
      Trim(elems.at(cnt),"\"");// remove quotes
      field = elems.at(cnt);
      //std::replace( field.begin(), field.end(), 'x', 'y'); // replace all 'x' to 'y'
      cout << "[" << field << "]\n";
    }
    std::string ValueTxt = elems.at(2);
    std::string SymbolTxt = elems.at(5);
    // http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring trim would be better
    cout << "SymbolTxt[" << SymbolTxt << "]\n";
    cout << "ValueTxt[" << ValueTxt << "]\n";

    uint64_t hashdex = TxtDex(SymbolTxt);
    double value = atof(ValueTxt.c_str());

    IoJack->PortId = hashdex;
    IoJack->Value = value;
    // double value = strtod (ValueTxt.c_str(), NULL);
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
  std::vector<std::string> ParseLine2(const std::string &linetxt) {
    std::vector<std::string> ray0 = split(linetxt, '"');
    std::vector<std::string> ray1;
    std::vector<std::string> rayout;
    std::string txt0, txt1;
    bool inquote = false;
    size_t siz0 = ray0.size();
    {
      txt0 = ray0.at(0);
      ray1 = split(txt0, ',');
      size_t siz1 = ray1.size(); siz1--;
      for (int cnt1=0; cnt1<siz1; cnt1++) {
        txt1 = ray1.at(cnt1);
        rayout.push_back(txt1);
      }
      inquote=true;
    }
    for (int cnt0=1; cnt0<siz0; cnt0++){
      txt0 = ray0.at(cnt0);
      if (!inquote) { // even, not quoted, split more
        ray1 = split(txt0, ',');
        size_t siz1 = ray1.size(); siz1--; // clip off ends
        for (int cnt1=1; cnt1<siz1; cnt1++) {
          txt1 = ray1.at(cnt1);
          rayout.push_back(txt1);
        }
        inquote=true;
      } else { // odd, quoted, copy whole
        rayout.push_back(txt0);
        inquote=false;
      }
    }
    if (!inquote){
      txt0 = ray0.at(siz0-1);
      ray1 = split(txt0, ',');
      size_t siz1 = ray1.size();
      for (int cnt1=1; cnt1<siz1; cnt1++) {
        txt1 = ray1.at(cnt1);
        rayout.push_back(txt1);
      }
      inquote=true;
    }
    printf("return rayout;\n");
    return rayout;
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
  int WebMain()
  {
    cout << "Hello world!" << endl;
    if (true) {
      std::string glob = "abcdefghijklmnop";
      FileFeeder feed;
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
  }
};

#endif // FEEDER_H_INCLUDED
