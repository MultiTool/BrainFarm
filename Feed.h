#ifndef FEED_H_INCLUDED
#define FEED_H_INCLUDED

#include "Base.h"
#include "IoJack.h"

typedef class Feed *FeedPtr;
class Feed {
public:
  uint64_t Generation;
  std::vector<GlobalIoJackPtr> Ports;
  /* ********************************************************************** */
  Feed() {
    Generation = 0;
  }
  /* ********************************************************************** */
  ~Feed() {
    for(int cnt=0; cnt<Ports.size(); cnt++) {
      delete Ports.at(cnt);
    }
  }
  /* ********************************************************************** */
  inline GlobalIoJackPtr at(size_t dex) { return Ports.at(dex); }
  /* ********************************************************************** */
  inline size_t size() { return Ports.size(); }
  /* ********************************************************************** */
  void Sort_Ports() {
    std::sort (Ports.begin(), Ports.end(), AscendingPortId);
  }
  /* ********************************************************************** */
  static bool AscendingPortId(GlobalIoJackPtr b0, GlobalIoJackPtr b1) {
    return (b0->PortId < b1->PortId);
  }
  /* ********************************************************************** */
  uint32_t TreeSearchPorts(uint64_t startplace, IoDexType target) {// assumes Node list has been sorted by ID, ascending
    int LoDex, MedDex, HiDex;
    GlobalIoJackPtr MedNode;
    LoDex=startplace; HiDex=Ports.size();
    while (LoDex<HiDex) {
      MedDex = (LoDex+HiDex)/2;
      MedNode = Ports.at(MedDex);
      if (target <= MedNode->PortId) { HiDex = MedDex; }
      else { LoDex = MedDex+1; }
    }
    return LoDex;
  }
  /* ********************************************************************** */
  void GenerateTestPorts() {
    GlobalIoJackPtr jack;
    char ch;
    ch = 'a';
    while (ch<='z') {
      jack = new GlobalIoJack(); this->Ports.push_back(jack);
      jack->PortId = ch;
      ch++;
    }
  }
  /* ********************************************************************** */
  void NextGen() {
    // in wiki, reads 1 new char from wiki, blanks all ports, and sets port[char] value to 1.0.
    // in massively parallel, goes through whole list and updates values for every jack.
    GlobalIoJackPtr Jack;
    size_t siz = Ports.size();
    for(int cnt=0; cnt<siz; cnt++) {
      Jack = Ports.at(cnt);
      Jack->Value = -1.0;
    }
    size_t LivePortDex = Generation % siz;
    Jack = Ports.at(LivePortDex);
    Jack->Value = 1.0;
    Generation++;
  }
};

#endif // FEED_H_INCLUDED
