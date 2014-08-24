//next();
/*
******************************************************

create IoJack class, fill big vector with these.
create big IO status/feed vector.
create alternative node class, with
  IOspecies type (done)
  IOdex (chosen from master feed vector)

we can dispose of the IoJack self-buffer members through refcounting.
HOWEVER, we cannot dispose of the master feed through refcounting. could have deathless Ref() and UnRef()?  Maybe even a separate counter?
*/

#include <iostream>
#include <vector> // example std::vector *vex;
#include <algorithm>    // std::sort
#include <math.h>
#include <time.h>       /* time */
#include <sys/time.h>
#include <stdio.h> // printf
#include <stdint.h>
#include <thread> // std::thread
#include <atomic> // std::atomic
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <hash_map>
//#include <conio.h>
//#include <ctype.h>
//#include <windows.h>

#include "Org.h"
#include "Pop.h"
#include "flexray.h"
#include "FlexrayC.h"
#include "Feeder.h"

using namespace std;
using namespace __gnu_cxx;

#define genomelen 1000
#define baseamp INT_MAX

class Genome;
class Org;

/*
population
creature
genome

*/
struct timeval tm0, tm1;
double tf0, tf1;
double delta;

void TimeTest() {
  struct timeval tm0, tm1;
  gettimeofday(&tm0, NULL);

  double sum = 0;
  double answer, question;
  for (int cnt=0; cnt<1000000; cnt++) {
    answer = sqrt((double)cnt);
    question = sin((double)cnt);
    sum += answer + question;
    {
      Org *beast = new Org();
      delete beast;
    }
  }

  gettimeofday(&tm1, NULL);

  bugprintf("sum:%f \n", sum);

  bugprintf("tm0:%ld:%ld\n", tm0.tv_sec, tm0.tv_usec);
  bugprintf("tm1:%ld:%ld\n", tm1.tv_sec, tm1.tv_usec);
  bugprintf("\n");
}

void sorttest() {
  uint64_t NDex;
  NodeVec *NList;
  NodePtr node0, node1;
  UidType target = 5;// IdMaker::MakeId();
  NList = new NodeVec();
  for (int cnt=0; cnt<11; cnt++) {
    node0 = new Node();
    node0->SpeciesId = cnt;
    NList->push_back(node0);
    NList->push_back(node0);
  }
  NDex = Node::TreeSearchNodeList(NList, 0, target);
  if (NDex < NList->size()) {
    node1 = NList->at(NDex);
    bugprintf("SpeciesId:%ld\n", node1->SpeciesId);
  } else {
    bugprintf("SpeciesId not found.\n");
  }
}
#define testsize 1000
//uint32 ramcnt=0,leaframcnt=0;
void flexray_leaf_free(void *leaf)
/* *************************************************************
************************************************************* */
{
  freesafe(leaf);
}
void flexray_leaf_delete(leafT *leaf)
/* *************************************************************
************************************************************* */
{
  delete leaf;
}
/* ********************************************************************** */
void maptest() {
  // speed performance test comparing types of arrays/collections
  hash_map<int, leafT *> yeah;
  std::vector<leafT *> fred0;
  std::vector<int> fred1;
  int bigness = 10000000;

  //bigness = 100000000;
  bigness = 10000000;
  struct timeval tm0, tm1;
  double tf0, tf1;
  double delta;
  uint32 cnt,dex;
  int rnum = rand();
  leafT *newleaf;//FR::leafT *newleaf;
  newleaf=allocsafe(leafT,1);
  //newleaf = new FR::leafT();
  if (true) {
    int dummy=0;
    gettimeofday(&tm0, NULL);
    for (int cnt=0; cnt<bigness; cnt++) {
      dummy+=3;
    }
    gettimeofday(&tm1, NULL);
    bugprintf("NOTHING but loop %li, %li\n", bigness, dummy);
    bugprintf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    bugprintf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    bugprintf("delta:%f\n", tf1-tf0);
    bugprintf("\n");
  }
  if (true) {
    leafT **ray = allocsafe(leafTPtr, bigness);
    gettimeofday(&tm0, NULL);
    for (int cnt=0; cnt<bigness; cnt++) {
      ray[cnt] = newleaf;
    }
    gettimeofday(&tm1, NULL);
    freesafe(ray);
    bugprintf("ray %li\n", bigness);
    bugprintf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    bugprintf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    bugprintf("delta:%f\n", tf1-tf0);
    bugprintf("\n");
  }
  if (true) {
    gettimeofday(&tm0, NULL);
    for (int cnt=0; cnt<bigness; cnt++) {
      fred0.push_back(newleaf);
    }
    gettimeofday(&tm1, NULL);
    bugprintf("vector push_back %li\n", bigness);
    bugprintf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    bugprintf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    bugprintf("delta:%f\n", tf1-tf0);
    bugprintf("\n");
  }
  if (false) {
    gettimeofday(&tm0, NULL);
    fred1.resize(bigness);
    // bugprintf("fred1 size %li\n", fred1.size());
    for (int cnt=0; cnt<bigness; cnt++) {
      fred1.at(cnt) = rnum;
    }
    gettimeofday(&tm1, NULL);
    bugprintf("vector at %li\n", bigness);
    bugprintf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    bugprintf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    bugprintf("delta:%f\n", tf1-tf0);
    bugprintf("\n");
  }
  if (true) {
    gettimeofday(&tm0, NULL);// hash
    for (int cnt=0; cnt<bigness; cnt++) {
      // dex=rand()%maxuint32;
      dex=cnt;
      yeah[dex] = newleaf;
    }
    gettimeofday(&tm1, NULL);

    if(false) { // hash access
      bugprintf("hash access\n");
      gettimeofday(&tm0, NULL);
      for (int cnt=0; cnt<bigness; cnt++) {
        //dex=rand()%maxuint32;
        dex=cnt;
        newleaf = yeah[dex];
      }
      gettimeofday(&tm1, NULL);
    }
    bugprintf("hash %li\n", bigness);
    bugprintf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    bugprintf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    bugprintf("delta:%f\n", tf1-tf0);
    bugprintf("\n");
  }
  //bugprintf("flexray skip 13.\n");
  if (true) {
    FR::Flexray<leafT> fray;// Flexray Class
    gettimeofday(&tm0, NULL);
    for (cnt=0; cnt<bigness; cnt++) {
      //dex=rand()%INT32_MAX;
      dex=cnt*1;
      fray.Insert(dex,newleaf);
    }
    gettimeofday(&tm1, NULL);
    if (false) { // flexray access
      bugprintf("flexray access\n");
      gettimeofday(&tm0, NULL);
      for (cnt=0; cnt<bigness; cnt++) {
        //dex=rand()%INT32_MAX;
        dex=cnt;
        newleaf = fray.Lookup(dex);
      }
      gettimeofday(&tm1, NULL);
    }
    if (false) {
      for (cnt=0; cnt<bigness; cnt+=1) {
        dex=rand()%INT32_MAX;
        newleaf=fray.Remove(dex);
        if (newleaf!=NULL) freesafe(newleaf);
      }
      bugprintf("randoms removed.\n");
      fray.Destroy(&flexray_leaf_delete);
    }

    bugprintf("Flexray Class %li\n", bigness);
    bugprintf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    bugprintf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    bugprintf("delta:%f\n", tf1-tf0);
    bugprintf("\n");
  }
  if (true) {
    flexelT topnode;// flexray C
    leafT *newleaf;

    flexray_init(&topnode);
    //newleaf=allocsafe(leafT,1);
    gettimeofday(&tm0, NULL);
    for (cnt=0; cnt<bigness; cnt++) {
      //dex=rand()%maxuint32;
      dex=cnt*1;
      //newleaf=allocsafe(leafT,1); newleaf->value=cnt;
      //newleaf=&cnt;
      flexray_insert(&topnode,dex,newleaf);
    }
    gettimeofday(&tm1, NULL);
    if (false) { // flexray access
      bugprintf("flexray access\n");
      gettimeofday(&tm0, NULL);
      for (cnt=0; cnt<bigness; cnt++) {
        //dex=rand()%maxuint32;
        dex=cnt;
        newleaf = flexray_lookup(&topnode, dex);
      }
      gettimeofday(&tm1, NULL);
    }
    if (false) {
      for (cnt=0; cnt<bigness; cnt+=1) {
        dex=rand()%maxuint32;
        newleaf=flexray_remove(&topnode,dex);
        if (newleaf!=NULL) free(newleaf);
      }
      bugprintf("randoms removed.\n");
      flexray_destroy(&topnode,&flexray_leaf_free);
    }

    bugprintf("flexray %li\n", bigness);
    bugprintf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    bugprintf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    bugprintf("delta:%f\n", tf1-tf0);
    bugprintf("\n");
  }
  if (newleaf!=NULL) freesafe(newleaf);
}
// next;
/*
highest priority now is to create feeds and feed a formula

figure out inputs, outputs, and testing
link lugares
add inter-org communication

*/

void foo() {
  bugprintf("foo\n");
}
void bar(int x) {
  bugprintf("bar\n");
}
void baz(int x) {
  bugprintf("baz\n");
}
/* ********************************************************************** */
void ThreadTest() {
#if false
  // http://www.cplusplus.com/reference/thread/thread/thread/
  std::thread first (foo);     // spawn new thread that calls foo()
  std::thread second (bar,0);  // spawn new thread that calls bar(0)
  std::thread *third;
  std::thread **thray;
  std::vector<std::thread> threads;

  third = new std::thread(baz, 0);// spawn new thread that calls baz(0)

  std::cout << "main, foo and bar now execute concurrently...\n";

  // synchronize threads:
  first.join();                // pauses until first finishes
  second.join();               // pauses until second finishes
  third->join();
  delete third;
  std::cout << "foo and bar completed.\n";
#endif
}
/* ********************************************************************** */
class Hilos {
public:
  std::atomic<int> global_counter;// = std::atomic<int>(0);
  void increase_global (int n) { for (int i=0; i<n; ++i) ++global_counter; }
  void increase_reference (std::atomic<int>& variable, int n) { for (int i=0; i<n; ++i) ++variable; }
  void handler () { }

  Hilos() {
    global_counter.store(0, std::memory_order_relaxed);// = new std::atomic<int>(0);
    //Hilos() : global_counter(0) {
    //global_counter(0);
  }

  struct Catom : std::atomic<int> {
    Catom() : std::atomic<int>(0) {}
    void increase_member (int n) { for (int i=0; i<n; ++i) fetch_add(1); }
  };
  int ThreadTest2()
  {
#if false
    std::vector<std::thread> threads;

    std::cout << "increase global counter with 10 threads...\n";
    for (int cnt=1; cnt<=10; ++cnt)
      threads.push_back(std::thread(&Hilos::increase_global, this, 1000));

    // http://stackoverflow.com/questions/14453329/how-do-i-pass-an-instance-member-function-as-callback-to-stdthread
    //std::thread myThread(&Hilos::handler,this);
    if (false) {
      std::cout << "increase counter (foo) with 10 threads using reference...\n";
      std::atomic<int> foo;
      for (int cnt=1; cnt<=10; ++cnt) {
        //threads.push_back(std::thread(increase_reference,std::ref(foo),1000));
        //threads.push_back(std::thread(&Hilos::increase_reference, std::ref(foo), 1000));
      }
    }
#if false
    std::cout << "increase counter (bar2) with 10 threads using member...\n";
    Catom bar2;
    for (int cnt=1; cnt<=10; ++cnt)
      threads.push_back(std::thread(&Catom::increase_member,std::ref(bar2),1000));
#endif

    std::cout << "synchronizing all threads...\n";
    for (auto& th : threads) th.join();

    std::cout << "global_counter: " << global_counter << '\n';
    std::cout << "foo: " << foo << '\n';
    //std::cout << "bar2: " << bar2 << '\n';

    return 0;
#endif
  }
};
/* ********************************************************************** */
void PopSession() {
  size_t PopMaxSize;
  PopPtr pop;
  OrgPtr org0;
  uint32_t gencnt;
  // 3.129000 seconds for a pop of 100, for 100 generations
  printf("PopSession()\n");
  //int NumGenerations = 50;
  //int NumGenerations = 100;
  //int NumGenerations = 500;
  //int NumGenerations = 1000;
  //int NumGenerations = 3000;
  //int NumGenerations = 4000;
  //int NumGenerations = 1000000;// for about 10 hours
  //int NumGenerations = 10000000;// ten million
  int NumGenerations = 100000000;// hundred million
  bool KeepGoing = true;
  int CleanPause = 1;//16
  int MaxSize=0, SumSize = 0, AvgSize=0;
  double SumScore0 = 0.0, SumScore1 = 0.0;
  double SumAvgAvgScore = 0.0;
  double FlywheelScore = 0.0;

  printf("Pop_Create!\n");
  pop = new Pop();
  printf("Pop Init! %li\n", pop->forestv.size());

  Feed food;
  printf("GenerateTestPorts()\n");
  food.GenerateTestPorts();
  pop->Attach_Global_Feed(&food);

  gettimeofday(&tm0, NULL);
  pop->Compile_Me();
  pop->Gather_IoNodes_And_Connect(&food);
  for (gencnt=0; gencnt<NumGenerations; gencnt++) {
    pop->Gen();// running and testing happens here

    org0 = pop->ScoreDexv[0];//org0 = pop->forestv[0]->tenant;
    int numnodes = org0->NGene.size();
    double AvgBeastScore = pop->AvgBeastScore(0.75);
    SumAvgAvgScore += AvgBeastScore;
    double score0 = org0->Score[0];
    double score1 = org0->Score[1];
    SumScore0 += score0;
    SumScore1 += score1;

    double AvgAvgScore = SumAvgAvgScore/(double)(gencnt+1.0);
    double avgscore0 = SumScore0/(double)(gencnt+1.0);
    double avgscore1 = SumScore1/(double)(gencnt+1.0);
    FlywheelScore = (FlywheelScore*0.999) + (score1*0.001);

    int NumJacks = org0->GlobalJackVec.size();
    printf("Pop_Gen:%04li, s:%6.2f, %7.2f, %7.2f, numnodes:%3li, NumJacks:%1li: ", gencnt, score0, score1, AvgBeastScore, numnodes, NumJacks);
    printf("%7.2f, %7.2f ", avgscore0, avgscore1);
    printf("%7.2f, ", AvgAvgScore);
    printf("fw:%5.2f,  ", FlywheelScore);
    org0->Print_Jacks();
    printf("\n");

#if false
    if(GetAsyncKeyState( 'X' ) & 0x8000)
    { // the 'X' key is currently being held down
      if(KeepGoing) {
        NumGenerations = gencnt + 30;// hell hack
        KeepGoing = false;
      }
    }
#endif
    if (NumGenerations-gencnt > 20) { // stop mutating for 20 generations in the final stretch
      //pop->Mutate(0.8, 0.8);
      //pop->Mutate(0.8, 0.1);
      //pop->Mutate(0.2, 0.2);
      //pop->Mutate(0.1, 0.1);
      //pop->Mutate(0.05, 0.05);// 5% of population is 5% mutated
      //pop->Mutate(0.1, 0.05);// 10% of population is 5% mutated
      //pop->Mutate(0.3, 0.05);// 30% of population is 5% mutated
      pop->Mutate(0.5, 0.05);// 50% of population is 5% mutated  ******
      //pop->Mutate(0.2, 0.05);// 20% of population is 5% mutated
      //pop->Mutate(0.2, 0.01);// 20% of population is 1% mutated
    }
    if (gencnt % CleanPause == 0) {
      pop->Clean_Inventory();
    }
    pop->Compile_Me();
    pop->Gather_IoNodes_And_Connect(&food);
    //pop->Clean_Me();
    PopMaxSize=pop->GetMaxSize();
    org0 = pop->forestv[0]->tenant;
    if (MaxSize<PopMaxSize) {
      MaxSize = PopMaxSize;
    }
    SumSize += org0->NGene.size();
  }
  gettimeofday(&tm1, NULL);

  { // final survey
    pop->Print_Sorted_Scores();
    org0 = pop->ScoreDexv.at(pop->ScoreDexv.size()-1);
    org0 = pop->forestv[0]->tenant;
    if (false) {
      bugprintf("Org 0, gen:%li, ", NumGenerations-1);
      org0->Print_Me();
    }
    bugprintf("Org 0, gen:%li, ", NumGenerations-1);
    AvgSize = SumSize/(double)NumGenerations;
    bugprintf("size:%i, MaxSize:%i, AvgSize:%i\n", org0->NGene.size(), MaxSize, AvgSize);
  }

  double t0 = FullTime(tm0);
  double t1 = FullTime(tm1);
  double delta = t1-t0;
  bugprintf("delta T:%f\n", delta);
  bugprintf("Pop_Delete! %f\n", pop->forestv[0]->tenant->Score[0]);
  delete pop;
}
/* ********************************************************************** */
void FeedTest() {
  std::string glob = "abcdefghijklmnop";
  //FileFeeder feed;
  WebFeeder feed;
  feed.Open();
  for (int cnt=0; cnt<6; cnt++) {
    feed.Fetch(glob);
    cout << "glob is:[" << glob << "]";
    printf("\n");
    //ParseGlob(glob);
  }
  feed.Close();
}
/* ********************************************************************** */
int main() {
  if (true) {
    uint64_t zult;
    //zult = JunkYard::TxtDex("ABCDEFG");
    //zult = JunkYard::TxtDex("ABCDE");
    //zult = JunkYard::TxtDex("ZZZZZ");
    zult = JunkYard::TxtDex("zzzzz");
    printf("zult[%016llX]\n", zult);
    zult = JunkYard::TxtDex(".....");
    printf("zult[%016llX]\n", zult);
    zult = JunkYard::TxtDex("........");// maximum
    printf("zult[%016llX]\n", zult);
    //printf("zult[%X]\n", zult);
    //printf("zult[%016llX]\n", zult);// hex         zult[4142434445464700]
    //printf("zult[%" PRIu64 "]\n", zult); // dec zult[4702394921427289856]
    //cout << "zult[" << zult << "]\n";
  }
  if (false) {
    IoJackBase IoJack;
    //JunkYard::ParseYahoo("\"4/8/2014\",  \"4:00pm\",  66.76,  1.36,  20.35,  \"HOG\",  \"Harley-Davidson, \"", &IoJack);
    JunkYard::ParseYahoo("\"4/8/2014\",\"4:00pm\",4.11,2.43,13.46,\"BBD-B.TO\",\"BOMBARDIER INC., \"", &IoJack);

    printf("IoJack.PortId[%016llX]\n", IoJack.PortId);// hex         zult[4142434445464700]
    printf("IoJack.Value[%f]\n", IoJack.Value); // dec zult[4702394921427289856]
    return 0;
  }
  if (false) {
    // std::find_if()
    std::string field = "hello, folks, there,";
    //std::replace(field.begin(), field.end(), ',', 'X');
    find_and_replace(field, ",", "");
    //std::replace(field.begin(), field.end(), ',', "");
    //field.replace(field.begin(), field.end(), ',', 'X');
    //std::replace(field.begin(), field.end(), ",", "X");
    //field.replace(field.begin(), field.end(), ",", "X");
    cout << "[" << field << "]";
    return 0;
  }
  // FeedTest(); return 0;
  //usleep(30*1000000L);// thirty seconds
  printf("main()\n");
  srand(time(NULL));
  PopSession(); return 0;
  Hilos hil;
  hil.ThreadTest2(); return 0;;
  ThreadTest(); return 0;;
  maptest(); return 0;
  return 0;
}
