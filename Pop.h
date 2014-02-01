#ifndef POP_H_INCLUDED
#define POP_H_INCLUDED

#include "Org.h"
#include "Lugar.h"

//#define popmax 1000
#define popmax 100

/*
forest array:
pro
fast for sure
size will not change
con
sorting is more complicated

vector
pro
higher level perks, sorting
not used enough to make major speed impact (may be fast anyway)
tighter types?
con

***

the big debate now is how to include lugares AND sort populations.

lugares:
do not use to sort by score.
next gen?  each lugar should have 2 pointers.


*/

/* ********************************************************************** */
class Pop;
typedef Pop *PopPtr;
class Pop {
public:
  uint32_t popsz;
  LugarVec forestv;
  OrgVec ScoreDexv; // ? for sorting?

  /* ********************************************************************** */
  Pop() {
    Init(popmax);
  }
  /* ********************************************************************** */
  ~Pop() {
    int sz, pcnt;
    sz = forestv.size();
    for (pcnt=0; pcnt<sz; pcnt++) {
      delete forestv.at(pcnt);
    }
  }
  /* ********************************************************************** */
  void Fire_Cycle() {
    LugarPtr lugar;
    OrgPtr org;
    int siz = this->forestv.size();
    for (int cnt=0; cnt<siz; cnt++) {
      lugar=this->forestv.at(cnt);
      org=lugar->tenant;
      org->Fire_Cycle();
    }
  }
  /* ********************************************************************** */
  void Init(int popsize) {
    LugarPtr lugar;
    Org *org;
    int pcnt;
    this->popsz = popsize;
    for (pcnt=0; pcnt<popsize; pcnt++) {
      lugar = new Lugar();
      org = Org::Abiogenate(); lugar->Attach_Tenant(org);
      forestv.push_back(lugar);
      ScoreDexv.push_back(org);
    }
  }
  /* ********************************************************************** */
  uint32_t GetMaxSize() {
    uint32_t MaxSize=0, SumSize=0, AvgSize=0;
    uint32_t popsize = this->forestv.size();
    uint32_t numnodes;
    LugarPtr lugar;
    OrgPtr org;
    uint32_t pcnt;
    LugarPtr place;
    for (pcnt=0; pcnt<popsize; pcnt++) {
      lugar = forestv[pcnt];
      org = lugar->tenant;
      numnodes=org->NGene.size();
      if (MaxSize<numnodes) {
        MaxSize= numnodes;
      }
      SumSize += numnodes;
    }
    return MaxSize;
  }
  /* ********************************************************************** */
  void Gen() { // new generation
    uint32_t popsize = this->forestv.size();
    LugarPtr lugar;
    Org *parent, *child;
    uint32_t pcnt;
    LugarPtr place;

    if (true) {
      printf("     Fire_Cycle\n");
      for (int fcnt=0; fcnt<10; fcnt++) {
        this->Fire_Cycle();
      }
    }
    /*
    place holder
    first we need to score and sort the parents, then we create children
    */
    // LugarVec forestv_unref = this->forestv;
    for (pcnt=0; pcnt<popsize; pcnt++) {
      lugar = forestv[pcnt];
      parent = lugar->tenant;
      child = parent->Spawn();
      lugar->Attach_Next_Tenant(child);
    }
    for (pcnt=0; pcnt<popsize; pcnt++) {// delete the parents and replace them.
      lugar = forestv[pcnt];
      lugar->Rollover_Tenant();
    }
  }
  /* ********************************************************************** */
  static bool AscendingScore(Org* b0, Org* b1) {
    return (b0->Score < b1->Score);
  }
  void Sort() {
    LugarVec forestv_unref = this->forestv;
    size_t siz = forestv_unref.size();
    int cnt;
    for (cnt=0; cnt<siz; cnt++) {
      ScoreDexv[cnt] = forestv_unref[cnt]->tenant;
    }
    std::sort (ScoreDexv.begin(), ScoreDexv.end(), AscendingScore);
  }
  /* ********************************************************************** */
  void Mutate() {
    size_t siz = this->forestv.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LugarPtr lugar = this->forestv.at(cnt);
      OrgPtr org = lugar->tenant;
      org->Mutate_Me();
    }
  }
  /* ********************************************************************** */
  void Connect_Jacks(FeedPtr food) {
    size_t siz = this->forestv.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LugarPtr lugar = this->forestv.at(cnt);
      OrgPtr org = lugar->tenant;
      org->Connect_Jacks(food);
    }
  }
  /* ********************************************************************** */
  void Gather_IoNodes_And_Connect(FeedPtr food) {
    size_t siz = this->forestv.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LugarPtr lugar = this->forestv.at(cnt);
      OrgPtr org = lugar->tenant;
      org->Gather_IoNodes_And_Connect(food);
    }
  }
  /* ********************************************************************** */
  void Compile_Me() {
    size_t siz = this->forestv.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LugarPtr lugar = this->forestv.at(cnt);
      OrgPtr org = lugar->tenant;
      org->Compile_Me();
    }
  }
  /* ********************************************************************** */
  void Clean_Me() {
    size_t siz = this->forestv.size();// remove links based on lack of use
    for (int cnt=0; cnt<siz; cnt++) {
      LugarPtr lugar = this->forestv.at(cnt);
      OrgPtr org = lugar->tenant;
      org->Clean_Me();
    }
  }
  /* ********************************************************************** */
  void Clean_Inventory() {
    UidVec uvec;// remove node links that do not connect to any existing type of node
    this->Inventory(&uvec);
    size_t siz = this->forestv.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LugarPtr lugar = this->forestv.at(cnt);
      OrgPtr org = lugar->tenant;
      org->Clean_Inventory(&uvec);
    }
  }
  /* ********************************************************************** */
  static bool AscendingUid(UidType b0, UidType b1) {
    return (b0 < b1);
  }
  /* ********************************************************************** */
  void Inventory(UidVec *uvec) {
    UidType ndp, ID_prev;// fill a list of all unique node ids
    uvec->clear();
    size_t siz = this->forestv.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LugarPtr lugar = this->forestv.at(cnt);
      OrgPtr org = lugar->tenant;
      org->Inventory(uvec);
    }
    std::sort (uvec->begin(), uvec->end(), AscendingUid);
    int UniqueCnt=0;// eliminate duplicates
    ID_prev = UidNull;
    for (int cnt=0; cnt<uvec->size(); cnt++) {
      ndp = uvec->at(cnt);
      if (ndp != ID_prev) {
        uvec->at(UniqueCnt) = ndp; // pack them down
        ID_prev = ndp;
        UniqueCnt++;
      }
    }
    uvec->resize(UniqueCnt);
    siz = UniqueCnt;
  }
};

#endif // POP_H_INCLUDED
