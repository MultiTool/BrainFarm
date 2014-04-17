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

using namespace std;

class Feeder {
public:
  //std::string infilepath ="s2clip2.txt";
  std::string infilepath ="sample.txt";
  ifstream infile;
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
  void Open() {
    infile.open(infilepath.c_str());
    if (infile.is_open()) {
    } else cout << "Unable to open file";
  }
  /* ********************************************************************** */
  void Close() {
    infile.close();
  }
  /* ********************************************************************** */
  void Fetch(std::string &glob) {
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

#endif // FEEDER_H_INCLUDED
