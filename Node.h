#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

/* ********************************************************************** */
/*  THIS IS GENWAVE NODE */
/* ********************************************************************** */

#include <iostream>
#include <stdio.h> // printf
#include <map>
#include <list>
#include "Base.h"
#include "IoJack.h"

#define WeightAmp 2.0;
#define WeightShiftAmp 0.2;

const uint16_t Num_IoTypes = 3;
namespace IoType {
  enum IoType {Intra=0, GlobalIO=1, NbrIO=2};
}
const int DisuseThresh = 1;
//typedef uint64_t UidType;
//static UidType UidSource;

//enum class IoType : uint8_t {Intra, Output};// will be valid in future C++11
enum class IoType2 : uint8_t {Intra=0, GlobalIO=1, NbrIO=2};// will be valid in future C++11
/* ********************************************************************** */
class Node;
typedef Node *NodePtr;
typedef std::vector<NodePtr> NodeVec;
typedef std::map<UidType, NodePtr> NodeMap;
typedef std::map<UidType, NodePtr>::const_iterator NodeMapIterator;

static uint32_t bugcnt = 0;
typedef double WeightType;
/* ********************************************************************** */
class Link;
typedef Link *LinkPtr;
class Link {
public:
  double Weight;
  double FireVal;
  UidType USID;
  NodePtr USNode,DSNode;
  int Disuse;
  Link() {
    this->Clear();
  }
  LinkPtr Spawn() {
    LinkPtr child = new Link();
    child->USID = this->USID;
    child->Weight = this->Weight;
    child->Disuse = this->Disuse;
    return child;
  }
  inline void Clear() {
    this->FireVal=0.0;
    this->USNode=NULL; this->DSNode=NULL;
  }
  inline double GetFire() {
    return this->FireVal*this->Weight;
  }
  static LinkPtr Abiogenate() {
    LinkPtr lnp = new Link();
    lnp->Disuse=0;
    lnp->Randomize_Weight();
    return lnp;
  }
  void Randomize_Weight() {
    this->Weight = (frand()-0.5) * WeightAmp;// to do: do this with a distribution change
  }
  void Mutate_Weight() {
    this->Weight += ((frand()*2.0)-1.0) * WeightShiftAmp;// to do: do this with a distribution change
  }
  void Print_Me() {
    bugprintf("  Link USID:%li, ", this->USID);
    bugprintf("USNode:%p, DSNode:%p ", this->USNode, this->DSNode);
    bugprintf("Weight:%f \n", this->Weight);
  }
};
typedef std::vector<LinkPtr> LinkVec;
/* ********************************************************************** */
typedef void (Node::*FireFunc)(int);
typedef void (*FireFunc2)(NodePtr);
// typedef void (Person::*PPMF)();

class Node {
public:
  UidType SpeciesId;
  IoType::IoType MyType;//IOspecies type
  IoDexType IoSpeciesId;// (chosen from master feed vector)
  IoJackBasePtr Jack;

  LinkVec LGenome;
  LinkVec Working_Ins, Working_Outs;
  double FireVal, NextFireVal;
  FireFunc SpecialFire;
  FireFunc2 SpecialFire2;
  std::vector<double> FireHistory;
  /* ********************************************************************** */
  void test(int ha) {
    //next;
  }
  static void test2(NodePtr ha) {
  }
  /* ********************************************************************** */
  Node() {
    this->Jack = NULL;
    this->FireVal = ((frand()*2.0)-1.0)*0.001;
    this->NextFireVal = ((frand()*2.0)-1.0)*0.001;
    this->SpecialFire = &Node::test;
    this->SpecialFire2 = test2;
    this->Working_Ins.clear();// probably not necessary
    this->Working_Outs.clear();
    this->FireHistory.clear();
  }
  /* ********************************************************************** */
  inline void testmore() {
    (*this.*SpecialFire)(12);
    (this->*SpecialFire)(12);

    this->SpecialFire2(this);
  }
  /* ********************************************************************** */
  ~Node() {
    size_t cnt;
    this->Working_Ins.clear();// probably not necessary
    this->Working_Outs.clear();
    this->FireHistory.clear();
    for (cnt=0; cnt<this->LGenome.size(); cnt++) {
      delete this->LGenome.at(cnt);
    }
    this->LGenome.clear();// probably not necessary
  }
  /* ********************************************************************** */
  static NodePtr Abiogenate() {
    NodePtr ndp = new Node();
    ndp->Jack = NULL;
    switch (rand()%2) {
    case 0:
      ndp->MyType = IoType::Intra;
      break;
    case 1:
      ndp->MyType = IoType::GlobalIO;
      ndp->IoSpeciesId = 'a' + rand()%(1+zee-'a');
      break;
    case 2:
      ndp->MyType = IoType::NbrIO;
      break;
    }
    ndp->SpeciesId = IdMaker::MakeId();
    ndp->FireVal = ((frand()*2.0)-1.0)*0.001;
    ndp->NextFireVal = ((frand()*2.0)-1.0)*0.001;
    return ndp;
  }
  /* ********************************************************************** */
  NodePtr Spawn() {
    // this just clones everything. we need to think about mating and mutation.
    NodePtr parent, child;
    LinkPtr lparent, lchild;
    size_t siz = this->LGenome.size();
    child = new Node();
    child->MyType = this->MyType;
    child->SpeciesId = this->SpeciesId;
    child->IoSpeciesId = this->IoSpeciesId;
    child->LGenome.resize(siz);
    for (size_t cnt=0; cnt<siz; cnt++) {
      lparent = this->LGenome.at(cnt);
      //bugprintf("lchild = lparent->Spawn(); cnt:%li, lparent:%p, bugcnt:%li\n", cnt, lparent, bugcnt);
      lchild = lparent->Spawn();
      bugcnt++;
      //bugprintf("child->LGenome.at(cnt) = lchild;%li\n", cnt);
      child->LGenome.at(cnt) = lchild;
    }
    return child;
  }
  /* ********************************************************************** */
  void Update_From_Feed() {
    if (this->MyType == IoType::GlobalIO) {
      //this->Jack->Value += this->FireVal;
      this->FireVal = this->Jack->Value;
    }
  }
  /* ********************************************************************** */
  double ActFun(double xin) {
    double OutVal;
    OutVal = xin / sqrt(1.0 + xin * xin);/* symmetrical sigmoid function in range -1.0 to 1.0. */
    return OutVal;
    /* General formula: double power = 2.0; OutVal = xin / Math.pow(1 + Math.abs(Math.pow(xin, power)), 1.0 / power); */
  }
  /* ********************************************************************** */
  void Push_Fire() {
    LinkPtr downs;
    double MyFire=this->FireVal;
    size_t siz = this->Working_Outs.size();
    for (int cnt=0; cnt<siz; cnt++) {
      downs = this->Working_Outs.at(cnt);
      downs->FireVal = MyFire;
    }
  }
  /* ********************************************************************** */
  void Collect_And_Fire() {
    LinkPtr ups;
    double Sum=0;
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      Sum+=ups->GetFire();
    }
    this->NextFireVal = ActFun(Sum);
    Exchange_With_Feed();
    //testmore();
  }
  /* ********************************************************************** */
  void Exchange_With_Feed() {
    if (this->MyType == IoType::GlobalIO) {
      this->Jack->UpwardValue += this->NextFireVal;
      this->FireVal = this->Jack->GetValue();
      //printf("this->FireVal[%f]\n", this->FireVal);
    } else {
      this->FireVal = this->NextFireVal;
    }
  }
  /* ********************************************************************** */
  void Random_Increase(NodeVec *others, double dupequota) {
    double rnum;
    UidType lid, nid;// assumes links and nodes are already sorted by ID
    int TempDex = 0;
    int LDex = 0, NDex = 0;
    int LSize = LGenome.size();
    int OSize = others->size();
    NodePtr ndp;
    LinkPtr lnk0, lnk1;
    LinkPtr temp[LSize + OSize];// NEEDS OPTIMIZING.  only create this array once for each Org
    // to do: optimize this as 3 while loops inside one while loop
    while (LDex < LSize && NDex < OSize) {
      lnk0 = LGenome.at(LDex); lid = lnk0->USID;
      ndp = others->at(NDex); nid = ndp->SpeciesId;
      if (lid <= nid) {
        temp[TempDex] = lnk0; TempDex++;// nodes are ahead of links, run until link equals or exceeds node
        LDex++;
        if (lid == nid) {NDex++;}// nodes are equal to links, run until tie is broken
      } else {
        rnum = frand();// links are ahead of nodes, race through nodes until node equals or exceeds link
        if (rnum < dupequota) {
          lnk1 = Link::Abiogenate(); lnk1->USID = nid;
          temp[TempDex] = lnk1; TempDex++;
        }
        NDex++;
      }
    }
    while (LDex < LSize) {
      temp[TempDex] = LGenome.at(LDex);
      LDex++; TempDex++;
    }
    while (NDex < OSize) {
      rnum = frand();// links are ahead of nodes, race through nodes until node equals or exceeds link
      if (rnum < dupequota) {
        ndp = others->at(NDex);
        lnk1 = Link::Abiogenate(); lnk1->USID = ndp->SpeciesId;
        temp[TempDex] = lnk1; TempDex++;
      }
      NDex++;
    }
    LGenome.resize(TempDex);
    for (int cnt=0; cnt<TempDex; cnt++) {
      LGenome.at(cnt) = temp[cnt];
    }
  }
  /* ********************************************************************** */
  bool Is_Sorted() {
    UidType ID_prev = 0;
    LinkPtr lnp;
    size_t siz = this->LGenome.size();
    for (int cnt=0; cnt<siz; cnt++) {
      lnp = this->LGenome.at(cnt);
      if(ID_prev>lnp->USID) {
        return false;
      }
      ID_prev=lnp->USID;
    }
    return true;
  }
  /* ********************************************************************** */
  void Clean_Me() {
    int ncnt, siz;// remove links that have not connected to anything in a long time.
    LinkPtr lnp;
    siz = this->LGenome.size();
    int KeepCnt=0;
    for (ncnt=0; ncnt<siz; ncnt++) {
      lnp = this->LGenome.at(ncnt);
      if (lnp->Disuse > DisuseThresh) {
        bugprintf("Disuse:%lu ",lnp->Disuse);
      }
      if (lnp->Disuse >= DisuseThresh) {
        delete lnp;
      } else {
        this->LGenome.at(KeepCnt) = lnp; // pack them down
        KeepCnt++;
      }
    }
    this->LGenome.resize(KeepCnt);
  }
  /* ********************************************************************** */
  void Clean_Inventory(UidVec *uvec) {
    int ncnt, siz, nsiz;// remove links who don't connect to any known node ID in uvec.
    int NDex;
    LinkPtr lnp;
    nsiz = uvec->size();
    siz = this->LGenome.size();
    int KeepCnt=0;
    for (ncnt=0; ncnt<siz; ncnt++) {
      lnp = this->LGenome.at(ncnt);
      NDex = TreeSearchUidList(uvec, NDex, lnp->USID);
      if (NDex < uvec->size() && (uvec->at(NDex) == lnp->USID) ) {
        this->LGenome.at(KeepCnt) = lnp; // pack them down
        KeepCnt++;
      } else {
        delete lnp;
      }
    }
    this->LGenome.resize(KeepCnt);
  }
  /* ********************************************************************** */
  void Mutate_Me(NodeVec *others) {
    // Mutate my link list in situ.
    int ncnt, siz;
    double rnum, killquota, dupequota, monsterquota, rejackquota;
    killquota = 0.0;// for testing
    dupequota = 0.5;// for testing
    monsterquota = 0.5;// for testing
    killquota = dupequota = monsterquota = 0.5;// for testing
    rejackquota = 0.01;
    //dupequota = 1.0;
    LinkPtr lnp, dupe;
    siz = this->LGenome.size();
    if (true) {
      int KeepCnt=0;// first remove some
      for (ncnt=0; ncnt<siz; ncnt++) {
        rnum = frand();
        lnp = this->LGenome.at(ncnt);
        if (rnum < killquota) {
          delete lnp;
        } else {
          this->LGenome.at(KeepCnt) = lnp; // pack them down
          KeepCnt++;
        }
      }
      this->LGenome.resize(KeepCnt);
      siz = KeepCnt;
    }
    Random_Increase(others, dupequota);// then add some
    // list of Links is not guaranteed to be sorted at this point!
    this->Sort_Links();
    {
      for (ncnt=0; ncnt<siz; ncnt++) {// finally inner-mutate some
        rnum = frand();
        if (rnum < monsterquota) {
          lnp = this->LGenome.at(ncnt);
          lnp->Mutate_Weight();
        }
      }
    }
    rnum = frand();
    if (rnum < rejackquota) {
      int randint = rand()%Num_IoTypes;
      // no waitaminute, this creates a net flow from majority types to minority types, favoring an equal proportion of all IO types even when selection didn't like that.
      this->MyType=static_cast<IoType::IoType>(randint);
      this->IoSpeciesId = 'a' + rand()%(1+zee-'a');
      // but, if most nodes come from duplication, and type-mutation is very rare, then proportion will tend to be perserved.
    }
  }
  /* ********************************************************************** */
  static bool AscendingLinkUid(LinkPtr b0, LinkPtr b1) {
    return (b0->USID < b1->USID);
  }
  /* ********************************************************************** */
  void Sort_Links() {
    if (false) {
      if(!Is_Sorted()) {
        bugprintf("LINKS NOT SORTED!!!");
        throw 123;
      }
    }
    std::sort (this->LGenome.begin(), this->LGenome.end(), AscendingLinkUid);
  }
  /* ********************************************************************** */
  void Uncompile_Me() {
    LinkPtr lnp;
    size_t cnt;
    size_t siz = this->LGenome.size();
    this->Working_Ins.clear();
    this->Working_Outs.clear();
    for (cnt=0; cnt<siz; cnt++) {
      lnp = this->LGenome.at(cnt);
      lnp->Clear();
    }
  }
  /* ********************************************************************** */
  void Compile_Me(NodeVec *others) {
    this->Uncompile_Me();
    size_t siz = this->LGenome.size();
    int NDex;
    UidType NbrId, NbrId_prev;
    NodePtr nbr;
    LinkPtr lnp;
    NbrId_prev = 0;
    int cnt;
    NDex = 0;
    for (cnt=0; cnt<siz; cnt++) {
      lnp = this->LGenome.at(cnt);
      lnp->FireVal = 0.0;// snox, to catch a segfault
      NbrId = lnp->USID;
      NbrId_prev=NbrId;
      NDex = TreeSearchNodeList(others, NDex, NbrId);// error, links may not be sorted here and they should be
      if (NDex < others->size() && ((nbr = others->at(NDex))->SpeciesId == NbrId) ) {
        this->Attach_US(nbr, lnp);
      } else {
        lnp->USNode = NULL; lnp->DSNode = NULL; lnp->Disuse++;
      }
    }
    switch(this->MyType) {
    case IoType::Intra: // do nothing, not an IO node
      break;
    case IoType::GlobalIO: // attach to global IO jack
      break;
    case IoType::NbrIO: // attach to neighbor IO jack
      break;
    }
  }
  /* ********************************************************************** */
  void Attach_US(NodePtr other, LinkPtr lnp) {// attach upstream node to me
    this->Working_Ins.push_back(lnp);// this approach uses less memory, fewer allocations/frees and is probably faster.
    other->Working_Outs.push_back(lnp);
    lnp->USNode = other; lnp->DSNode = this;
  }
  /* ********************************************************************** */
  int mainX () {
    std::map<char,int> mymap;
    std::map<char,int>::iterator iter;

    mymap['a']=50;
    mymap['b']=100;
    mymap['c']=150;
    mymap['d']=200;

    iter=mymap.find('b');
    mymap.erase (iter);
    mymap.erase (mymap.find('d'));

    // print content:
    std::cout << "elements in mymap:" << '\n';
    std::cout << "a => " << mymap.find('a')->second << '\n';
    std::cout << "c => " << mymap.find('c')->second << '\n';

    return 0;
  }
  /* ********************************************************************** */
  static uint32_t TreeSearchUidList(UidVec *NList, uint64_t startplace, UidType target) {// assumes Node list has been sorted by ID, ascending
    int LoDex, MedDex, HiDex;
    UidType MedNode;
    LoDex=startplace; HiDex=NList->size();
    while (LoDex<HiDex) {
      MedDex = (LoDex+HiDex)/2;
      MedNode = NList->at(MedDex);
      if (target <= MedNode) { HiDex = MedDex; }
      else { LoDex = MedDex+1; }
    }
    return LoDex;
  }
  /* ********************************************************************** */
  static uint32_t TreeSearchNodeList(NodeVec *NList, uint64_t startplace, UidType target) {// assumes Node list has been sorted by ID, ascending
    int LoDex, MedDex, HiDex;
    NodePtr MedNode;
    LoDex=startplace; HiDex=NList->size();
    while (LoDex<HiDex) {
      MedDex = (LoDex+HiDex)/2;
      MedNode = NList->at(MedDex);
      if (target <= MedNode->SpeciesId) { HiDex = MedDex; }
      else { LoDex = MedDex+1; }
    }
    return LoDex;
  }
  /* ********************************************************************** */
  static uint32_t TreeSearchLinkList(LinkVec *LList, uint64_t startplace, UidType target) {// assumes Node list has been sorted by ID, ascending
    int LoDex, MedDex, HiDex;
    LinkPtr MedLink;
    LoDex=startplace; HiDex=LList->size();
    while (LoDex<HiDex) {
      MedDex = (LoDex+HiDex)/2;
      MedLink = LList->at(MedDex);
      if (target <= MedLink->USID) { HiDex = MedDex; }
      else { LoDex = MedDex+1; }
    }
    return LoDex;
  }
  /* ********************************************************************** */
  void Print_Me() {
    //const void *address = static_cast<const void*>(this);
    //void *address = (this);
    void *address = &SpeciesId;
    bugprintf(" Node SpeciesId:%li, MyType:%li, this:%p\n", this->SpeciesId, this->MyType, address);
    size_t siz = this->LGenome.size();
    bugprintf(" numlinks:%li\n", siz);
    for (int cnt=0; cnt<siz; cnt++) {
      LinkPtr lnk = this->LGenome.at(cnt);
      lnk->Print_Me();
    }
  }
};

#endif // NODE_H_INCLUDED

#if false
/*
nodes can only add connections to nodes in their current org
when a node mutates it changes id? not necessarily.
lugar topology only matters for: picking mates, communication, where to put children.
listener nodes are hard-coded and always the same IDs. mutations to listen to them are frequent.
outputter nodes - did that already, how?
maybe all nodes can have an input and output genome? result is union of both?
foreach node {
 foreach input {
  lookup innode, if found {
    add me to innode's out stack, for later real link
  }
 }
 foreach output {
  lookup outnode, if found {
    add outnode to my stack, for later real link
  }
 }
}

each org has say 8 listeners and 8 talkers.

or, every node can possibly be an output?  separate genome for each node.

also, for comm to go big, aysnc reproduction.  only elim lowest in excessing pattern. but what of fat scores?
scores are averages over time. initial score is global average?

probably better in java

*/
#endif // false

