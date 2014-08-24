#ifndef ORG_H_INCLUDED
#define ORG_H_INCLUDED

#if false
/*
to do:
make 2 orgs, one for networks and one of simple vectors.
*/
#endif

#include "Base.h"
#include "FlexrayC.h"
#include "Node.h"
#include "Feed.h"

#define genomelen 1000
#define genomelen2 4
#define baseamp INT_MAX

/* ********************************************************************** */
class Genome {
public:
  class Org *mine;
};
const int NodeNumLimit = 100;
/* ********************************************************************** */
class Org;
typedef Org *OrgPtr;
typedef std::vector<Org*> OrgVec;
class Org {
public:
  const static int NumScores = 2;
  double Score[NumScores];
  struct Lugar *home;// my location
  NodeVec NGene;
  NodeVec *NGenePtr;
  IoJackVec GlobalJackVec;
  /* ********************************************************************** */
  Org() {
    NGenePtr = &(NGene);
    for (int cnt=0; cnt<NumScores; cnt++) {
      this->Score[cnt] = 0.0;
    }
    this->home = NULL;
  }
  /* ********************************************************************** */
  ~Org() {
    NodePtr ndp;
    for (int ncnt=0; ncnt<this->NGene.size(); ncnt++) {
      ndp = this->NGene.at(ncnt);
      delete ndp;
    }
    Clear_IoNodes();
  }
  /* ********************************************************************** */
  static OrgPtr Abiogenate() {
    OrgPtr org = new Org();
    org->Mutate_Me(1.0);// 100% mutated, no inherited genetic info
    return org;
  }
  /* ********************************************************************** */
  void Fire_Cycle() {
    NodePtr node;
    size_t siz;

    siz = this->GlobalJackVec.size();
    for (int ncnt=0; ncnt<siz; ncnt++) {
      this->GlobalJackVec.at(ncnt)->UpwardValue = 0.0;
    }

    siz = this->NGene.size();
    for (int cnt=0; cnt<siz; cnt++) {
      node=this->NGene.at(cnt);
      node->Push_Fire();
    }
    for (int cnt=0; cnt<siz; cnt++) {
      node=this->NGene.at(cnt);
      node->Collect_And_Fire();
    }
  }
  /* ********************************************************************** */
  void Rand_Init() {
    this->Mutate_Me(1.0);
  }
  /* ********************************************************************** */
  void Random_Increase(double dupequota) {
    size_t siz = this->NGene.size();
    int rnum;
    NodePtr ndp, dupe;
    if (siz==0) {
      this->NGene.resize(genomelen2);
      for (int ncnt=0; ncnt<genomelen2; ncnt++) {
        dupe = Node::Abiogenate();
        this->NGene.at(ncnt) = dupe;
      }
      for (int ncnt=0; ncnt<genomelen2; ncnt++) {
        dupe = this->NGene.at(ncnt);
        dupe->Random_Increase(this->NGenePtr, 1.0);
        if (dupe->LGenome.size()>4) {
          bool nop = true;
        }
      }
      return;
    }
    size_t siz1=siz;
    for (int ncnt=0; ncnt<siz; ncnt++) {// duplicate nodes
      rnum = frand();
      if (rnum < dupequota) {
        //if (siz1>=NodeNumLimit){break;}
        ndp = this->NGene.at(ncnt);
        dupe = ndp->Spawn();
        dupe->SpeciesId = IdMaker::MakeId();
        this->NGene.push_back(dupe);
        if (this->NGene.size()>=NodeNumLimit) {break;}
        //siz1++;
      }
    }// Node list is not sorted at this point!
  }
  /* ********************************************************************** */
  void Mutate_Me(double MRate) {
    // Mutate my node list in situ.
    int ncnt, siz;
    double rnum, killquota, dupequota, monsterquota;
    killquota = dupequota = monsterquota = 0.5;// for testing
    killquota = dupequota = monsterquota = MRate;// for testing
    killquota = 0.5;// for testing
    dupequota = 0.5;// for testing
    monsterquota = 0.5;// for testing
    NodePtr ndp, dupe;
    int KeepCnt=0;
    siz = this->NGene.size();

    {
      // first remove some
      for (ncnt=0; ncnt<siz; ncnt++) {
        rnum = frand();
        ndp = this->NGene.at(ncnt);
        if (rnum < killquota) {
          delete ndp;
        } else {
          this->NGene.at(KeepCnt) = ndp; // pack them down
          KeepCnt++;
        }
      }
      this->NGene.resize(KeepCnt);
      siz = KeepCnt;
    }
    Random_Increase(dupequota);
    siz = this->NGene.size();
    // Node list is not guaranteed to be sorted at this point!
    Sort_Nodes();
    {
      // finally inner-mutate some
      for (ncnt=0; ncnt<siz; ncnt++) {
        rnum = frand();
        if (rnum < monsterquota) {
          ndp = this->NGene.at(ncnt);
          ndp->Mutate_Me(NGenePtr);
        }
      }
    }
  }
  /* ********************************************************************** */
  void Sort_Nodes() {
    std::sort (NGene.begin(), NGene.end(), AscendingNodeUid);
  }
  /* ********************************************************************** */
  OrgPtr Spawn() {
    OrgPtr child;
    child = new Org();
    size_t siz = this->NGene.size();
    NodePtr ndp;
    child->NGene.resize(siz);
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);
      ndp = ndp->Spawn();
      child->NGene.at(cnt) = ndp;
    }
    return child;
  }
  /* ********************************************************************** */
  static bool AscendingNodeUid(NodePtr b0, NodePtr b1) {
    return (b0->SpeciesId < b1->SpeciesId);
  }
  /* ********************************************************************** */
  void Uncompile_Me() {
    int ncnt, siz;// disconnect and clear all connections
    NodePtr ndp;
    siz = this->NGene.size();
    for (ncnt=0; ncnt<siz; ncnt++) {
      ndp = this->NGene.at(ncnt);
      ndp->Uncompile_Me();
    }
  }
  /* ********************************************************************** */
  void Compile_Me() {
    UidType ID_prev;
    NodePtr ndp;
    if(false) {
      std::random_shuffle ( NGene.begin(), NGene.end() );// shuffle to randomize which duplicates are deleted
      Sort_Nodes();
    }
    if(!Is_Sorted()) {
      bugprintf("NODES NOT SORTED!!!");
      throw 123;
    }
    if (false) {
      Remove_Duplicate_Nodes();
    }
    size_t siz = this->NGene.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);
      ndp->Compile_Me(NGenePtr);
    }
    //this->Gather_IoNodes();
  }
  /* ********************************************************************** */
  void Clear_Score() {
    for (int cnt=0; cnt<NumScores; cnt++) {
      this->Score[cnt]=0.0;
    }
  }
  /* ********************************************************************** */
  void Oneify_Score() { // this is for accumulating scores by multiplication: Score *= subscore
    for (int cnt=0; cnt<NumScores; cnt++) {
      this->Score[cnt]=1.0;
    }
  }
  /* ********************************************************************** */
  void Rescale_Score(double Factor) {
    for (int cnt=0; cnt<NumScores; cnt++) {
      this->Score[cnt]*=Factor;
    }
  }
  /* ********************************************************************** */
  bool Succeeded(double Margin) {
    bool Success = true;
    double Real, Guessed, Error;
    IoJackPtr Jack;
    size_t siz;
    siz = this->GlobalJackVec.size();
    for (int ncnt=0; ncnt<siz; ncnt++) {
      Jack = this->GlobalJackVec.at(ncnt);
      Guessed = Jack->UpwardValue;
      Real = Jack->GetValue();
      Error = (fabs(Real-Guessed))/2.0;
      if (Error>Margin){
        Success = false; break;
      }
    }
    return Success;
  }
  /* ********************************************************************** */
  bool Calculate_Score_And_Success(double Margin) {
    bool Success = true;
    NodePtr node;
    double Real, Guessed, Error, Temp0, Temp1, SumScore0, SumScore1;
    IoJackPtr Jack;
    size_t siz;
    siz = this->GlobalJackVec.size();
    SumScore0 = 1.0; SumScore1 = 0.0;
    for (int ncnt=0; ncnt<siz; ncnt++) {
      Jack = this->GlobalJackVec.at(ncnt);
      Guessed = Jack->UpwardValue;
      Real = Jack->GetValue();
      Error = (fabs(Real-Guessed))/2.0;// range 0.0 to 1.0
      Temp0 = (1.0-Error)+1.0;// range 1.0 to 2.0
      Temp1 = math_sgn(Guessed)*math_sgn(Real);
      if (Error>Margin){ Success = false; }
      SumScore0 *= Temp0;
      SumScore1 += Temp1;
    }
    this->Score[0] += SumScore0;
    this->Score[1] += SumScore1;
    return Success;
  }
#if 1
  /* ********************************************************************** */
  void Calculate_Score() {
    NodePtr node;
    double Real, Guessed, Temp0, Temp1, SumScore0, SumScore1;
    IoJackPtr Jack;
    size_t siz;
    siz = this->GlobalJackVec.size();
    SumScore0 = 1.0; SumScore1 = 0.0;
    for (int ncnt=0; ncnt<siz; ncnt++) {
      Jack = this->GlobalJackVec.at(ncnt);
      Guessed = Jack->UpwardValue;
      Real = Jack->GetValue();
      Temp0 = (2.0-fabs(Real-Guessed))/2.0;
      Temp0+=1.0;
      Temp1 = math_sgn(Guessed)*math_sgn(Real);
      if (fabs(Temp1)<Fudge) {
        bool nop = true;
      }
      SumScore0 *= Temp0;
      SumScore1 += Temp1;
    }
    this->Score[0] += SumScore0;
    this->Score[1] += SumScore1;
    bool nop = true;
  }
#else
  /* ********************************************************************** */
  void Calculate_Score() {
    NodePtr node;
    double Real, Guessed, Temp0, Temp1, SumScore0, SumScore1;
    IoJackPtr Jack;
    size_t siz;
    siz = this->GlobalJackVec.size();
    SumScore0 = 0.0; SumScore1 = 0.0;
    for (int ncnt=0; ncnt<siz; ncnt++) {
      Jack = this->GlobalJackVec.at(ncnt);
      Guessed = Jack->UpwardValue;
      Real = Jack->GetValue();
      Temp0 = math_sgn(Guessed)*math_sgn(Real);
      Temp1 = Guessed*Real;
      if (fabs(Temp1)<Fudge) {
        bool nop = true;
      }
      SumScore0 += Temp0;
      SumScore1 += Temp1;
    }
    this->Score[0] += SumScore0;
    this->Score[1] += SumScore1;
    bool nop = true;
  }
#endif
  /* ********************************************************************** */
  void Update_From_Feed() {
    IoJackPtr LocalJack;
    size_t siz = this->GlobalJackVec.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LocalJack = this->GlobalJackVec.at(cnt);// for every node
    }
    /*
    steps are:
    all infires have been collected and actfunned, AND have been pushed to local jacks where nodes were IO type.

    then for each local jack,
    (finish off your sum variable any way you need, then)
    import value from its global feed ref, and do comparison against its own fire sum or whatever.
    then localjack replaces its own fire sum value with global feed value.

    establish total fitness of GlobalJackVec here.

    then when nodes push fires, the IO nodes first suck in their jack values and push *them* instead.

    */
  }
  /* ********************************************************************** */
  void Gather_IoNodes() {
    FR::Flexray<IoJack> fray;// Flexray Class
    IoDexType IoSpeciesId;
    IoJackPtr jack;
    NodePtr ndp;// TODO (Phosphenes#1#): Add IO!
    size_t siz = this->NGene.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);// for every node
      switch (ndp->MyType) {
      case IoType::Intra:
        ndp->Jack = NULL;
        break;
      case IoType::GlobalIO:
        IoSpeciesId = ndp->IoSpeciesId;
        if ((jack = fray.Lookup(IoSpeciesId))==NULL) {
          jack = new IoJack(); jack->PortId = ndp->IoSpeciesId;
          fray.Insert(IoSpeciesId, jack); GlobalJackVec.push_back(jack);
        }
        ndp->Jack = jack->AddRef();
        break;
      case IoType::NbrIO:
        IoSpeciesId = ndp->IoSpeciesId;
        break;
      }
    }
    /* at this point, GlobalJackVec holds a list of all unique node output types. */
  }
  /* ********************************************************************** */
  static void Sweep_Jacks_Callback(IoJack *jack) {
    if(jack->Deprecated) { delete jack; }
  }
  /* ********************************************************************** */
  void Clear_IoNodes() {
    size_t siz = this->GlobalJackVec.size();
    for (int cnt=0; cnt<siz; cnt++) {
      delete this->GlobalJackVec.at(cnt);
    }
    this->GlobalJackVec.clear();
  }
  /* ********************************************************************** */
  void Gather_IoNodes_And_Connect(FeedPtr food) {
    FR::Flexray<IoJack> fray;// merge with Connect_Jacks
    size_t portsiz = food->size();
    IoDexType IoSpeciesId;
    IoJackPtr LocalJack;
    GlobalIoJackPtr GlobalJack;
    uint32_t finddex;
    NodePtr ndp;
    Clear_IoNodes();
    size_t siz = this->NGene.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);// for every node
      switch (ndp->MyType) {
      case IoType::Intra:
        ndp->Jack = NULL;
        break;
      case IoType::GlobalIO:
        IoSpeciesId = ndp->IoSpeciesId;
        if ((LocalJack = fray.Lookup(IoSpeciesId))==NULL) {
          LocalJack = new IoJack();
          fray.Insert(IoSpeciesId, LocalJack);
          {
            LocalJack->PortId = IoSpeciesId;
            finddex = food->TreeSearchPorts(0, IoSpeciesId);
            if (finddex<portsiz && (GlobalJack=food->Ports.at(finddex))->PortId == IoSpeciesId) {
              LocalJack->UpStream = GlobalJack->AddRef();
              GlobalJackVec.push_back(LocalJack);// WILL CAUSE MEMORY LEAK!!
            } else {
              LocalJack->Deprecated = true;
            }
          }
        }
        if (LocalJack->Deprecated) {
          ndp->MyType = IoType::Intra;// If not in master feed, we will never see it again as such. Mutate the node for all eternity.
        } else {
          ndp->Jack = LocalJack->AddRef();
        }
        break;
      case IoType::NbrIO:
        IoSpeciesId = ndp->IoSpeciesId;
        break;
      }
    }
    /* at this point, GlobalJackVec holds a list of all unique node output types. */

    // fray.Destroy(&Sweep_Jacks_Callback);// meh, best just save all jacks to the GlobalJackVec and pack them down after. Sweep_Jacks_Callback adds more overhead.

    // finally pack down the local jack list and delete jacks that don't go anywhere.
    uint32_t packcnt = 0;
    siz = this->GlobalJackVec.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LocalJack = this->GlobalJackVec.at(cnt);
      if(LocalJack->Deprecated) { delete LocalJack; }
      else {
        this->GlobalJackVec.at(packcnt) = LocalJack;
        packcnt++;
      }
    }
    this->GlobalJackVec.resize(packcnt);
  }
  /* ********************************************************************** */
  void Connect_Jacks(FeedPtr food) {
    FR::Flexray<IoJack> fray;// Flexray for collision detection
    size_t portsiz = food->size();
    IoDexType PortId;
    IoJackPtr LocalJack;
    GlobalIoJackPtr GlobalJack;
    uint32_t finddex;
    uint32_t packcnt = 0;
    size_t siz = this->GlobalJackVec.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LocalJack = this->GlobalJackVec.at(cnt);// for every node
      PortId = LocalJack->PortId;
      finddex = food->TreeSearchPorts(0, PortId);
      if (false) {
        if (finddex<portsiz) {
          GlobalJack=food->Ports.at(finddex);
          if (GlobalJack->PortId == PortId) {
            LocalJack->UpStream = GlobalJack->AddRef();
            this->GlobalJackVec.at(packcnt) = LocalJack;// pack them down
            packcnt++;
          }
        }
      } else {
        if (finddex<portsiz && (GlobalJack=food->Ports.at(finddex))->PortId == PortId) {
          LocalJack->UpStream = GlobalJack->AddRef();
          this->GlobalJackVec.at(packcnt) = LocalJack;// pack them down
          packcnt++;
        } else {
          LocalJack->Deprecated = true;// if not in master feed, we will never see it again as such.
        }
      }
    }
    this->GlobalJackVec.resize(packcnt);
  }
  /* ********************************************************************** */
  void Remove_Duplicate_Nodes() {
    UidType ID_prev;
    NodePtr ndp;
    std::random_shuffle ( NGene.begin(), NGene.end() );// shuffle to randomize which duplicates are deleted
    Sort_Nodes();
    size_t siz = this->NGene.size();
    int UniqueCnt=0;// eliminate duplicates
    ID_prev = UidNull;
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);
      if (ndp->SpeciesId != ID_prev) {
        this->NGene.at(UniqueCnt) = ndp; // pack them down
        ID_prev = ndp->SpeciesId;
        UniqueCnt++;
      } else {
        delete ndp;
      }
    }
    this->NGene.resize(UniqueCnt);
  }
  /* ********************************************************************** */
  bool Is_Sorted() {
    UidType ID_prev = 0;
    NodePtr ndp;
    size_t siz = this->NGene.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);
      if(ID_prev>ndp->SpeciesId) {
        return false;
      }
      ID_prev=ndp->SpeciesId;
    }
    return true;
  }
  /* ********************************************************************** */
  void Clean_Me() {
    NodePtr ndp;// remove links based on lack of use
    size_t siz = this->NGene.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);
      ndp->Clean_Me();
    }
  }
  /* ********************************************************************** */
  void Clean_Inventory(UidVec *uvec) {
    NodePtr ndp;
    size_t siz = this->NGene.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);
      ndp->Clean_Inventory(uvec);
    }
  }
  /* ********************************************************************** */
  void Inventory(UidVec *uvec) {
    size_t siz = this->NGene.size();
    for (int cnt=0; cnt<siz; cnt++) {
      NodePtr ndp = this->NGene.at(cnt);
      uvec->push_back(ndp->SpeciesId);
    }
  }
  /* ********************************************************************** */
  void Print_Me() {
    NodePtr ndp;
    bugprintf("Org\n");
    Print_Score();
    size_t siz = this->NGene.size();
    bugprintf("num nodes:%li\n", siz);
    for (int cnt=0; cnt<siz; cnt++) {
      ndp = this->NGene.at(cnt);
      bugprintf(" Node addr:%p\n", ndp);
      ndp->Print_Me();
    }
  }
  /* ********************************************************************** */
  void Print_Jacks() {
    IoJackPtr jckp;
    char ch;
    size_t siz = this->GlobalJackVec.size();
    for (int cnt=0; cnt<siz; cnt++) {
      jckp = this->GlobalJackVec.at(cnt);
      ch = jckp->PortId;
      bugprintf("%c, ", ch);
    }
  }
  /* ********************************************************************** */
  void Print_Score() {
    bugprintf(" Score:%f, %f\n", this->Score[0], this->Score[1]);
  }
  /* ********************************************************************** */
  int Compare_Score(OrgPtr other) {
    int cnt = 0;
    double *ScoreMe, *ScoreYou;
    ScoreMe=this->Score; ScoreYou=other->Score;
    while (cnt<NumScores) {
      if (ScoreMe[cnt]<ScoreYou[cnt]) {return 1;}
      if (ScoreMe[cnt]>ScoreYou[cnt]) {return -1;}
      cnt++;
    }
    return 0;
  }
#if false
  /* ********************************************************************** */
  void Inventory(UidMap *umap) {
    UidMap umap2;
    std::pair<std::map<UidType,UidType>::iterator,bool> ret;
    size_t siz = this->NGene.size();
    //UidMapIter
    for (int cnt=0; cnt<siz; cnt++) {
      NodePtr ndp = this->NGene.at(cnt);
      //ret = umap->insert ( std::pair<UidType,UidType>(ndp->SpeciesId, ndp->SpeciesId) );
      if (ret.second==false) {
        std::cout << "element 'z' already existed";
        //std::cout << " with a value of " << ret.first->second << '\n';
      }
      umap2[ndp->SpeciesId] = ndp->SpeciesId;
    }
  }
#endif
};

#endif // ORG_H_INCLUDED
