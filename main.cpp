
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

#include <hash_map>

#include "Org.h"
#include "Pop.h"
#include "flexray.h"
#include "FlexrayC.h"

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

  printf("sum:%lf \n", sum);

  printf("tm0:%ld:%ld\n", tm0.tv_sec, tm0.tv_usec);
  printf("tm1:%ld:%ld\n", tm1.tv_sec, tm1.tv_usec);
  printf("\n");
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
    printf("SpeciesId:%ld\n", node1->SpeciesId);
  } else {
    printf("SpeciesId not found.\n");
  }
}

const double uprecision = 1000000.0;

double FullTime(struct timeval tm0) {
  return tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
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
    printf("NOTHING but loop %li, %li\n", bigness, dummy);
    printf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    printf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    printf("delta:%lf\n", tf1-tf0);
    printf("\n");
  }
  if (true) {
    leafT **ray = allocsafe(leafTPtr, bigness);
    gettimeofday(&tm0, NULL);
    for (int cnt=0; cnt<bigness; cnt++) {
      ray[cnt] = newleaf;
    }
    gettimeofday(&tm1, NULL);
    freesafe(ray);
    printf("ray %li\n", bigness);
    printf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    printf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    printf("delta:%lf\n", tf1-tf0);
    printf("\n");
  }
  if (true) {
    gettimeofday(&tm0, NULL);
    for (int cnt=0; cnt<bigness; cnt++) {
      fred0.push_back(newleaf);
    }
    gettimeofday(&tm1, NULL);
    printf("vector push_back %li\n", bigness);
    printf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    printf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    printf("delta:%lf\n", tf1-tf0);
    printf("\n");
  }
  if (false) {
    gettimeofday(&tm0, NULL);
    fred1.resize(bigness);
    // printf("fred1 size %li\n", fred1.size());
    for (int cnt=0; cnt<bigness; cnt++) {
      fred1.at(cnt) = rnum;
    }
    gettimeofday(&tm1, NULL);
    printf("vector at %li\n", bigness);
    printf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    printf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    printf("delta:%lf\n", tf1-tf0);
    printf("\n");
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
      printf("hash access\n");
      gettimeofday(&tm0, NULL);
      for (int cnt=0; cnt<bigness; cnt++) {
        //dex=rand()%maxuint32;
        dex=cnt;
        newleaf = yeah[dex];
      }
      gettimeofday(&tm1, NULL);
    }
    printf("hash %li\n", bigness);
    printf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    printf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    printf("delta:%lf\n", tf1-tf0);
    printf("\n");
  }
  //printf("flexray skip 13.\n");
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
      printf("flexray access\n");
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
      printf("randoms removed.\n");
      fray.Destroy(&flexray_leaf_delete);
    }

    printf("Flexray Class %li\n", bigness);
    printf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    printf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    printf("delta:%lf\n", tf1-tf0);
    printf("\n");
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
      printf("flexray access\n");
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
      printf("randoms removed.\n");
      flexray_destroy(&topnode,&flexray_leaf_free);
    }

    printf("flexray %li\n", bigness);
    printf("tm0:%ld.%ld\n", tm0.tv_sec, tm0.tv_usec);
    printf("tm1:%ld.%ld\n", tm1.tv_sec, tm1.tv_usec);
    tf0 = tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
    tf1 = tm1.tv_sec + ((double)tm1.tv_usec)/uprecision;
    printf("delta:%lf\n", tf1-tf0);
    printf("\n");
  }
  if (newleaf!=NULL) freesafe(newleaf);
}
// next;
/*
remove redundant population vectors - done
figure out inputs, outputs, and testing
link lugares
add inter-org communication

*/

/* ********************************************************************** */
void PopSession() {
  // 3.129000 seconds for a pop of 100, for 100 generations
  int NumGenerations = 100;
  int CleanPause = 1;//16
  //int NumGenerations = 1000000;// for about 10 hours
  int MaxSize=0, SumSize = 0, AvgSize=0;
  uint32_t PopMaxSize;
  PopPtr pop;
  OrgPtr org0;
  printf("Pop_Create!\n");
  pop = new Pop();
  printf("Pop Init! %lu\n", pop->forestv.size());
  LugarPtr lug;
  lug = pop->forestv.at(0);
  printf("lug %lu\n", lug);

  Feed food;
  food.GenerateTestPorts();

  Org *org;
  org = lug->tenant;
  printf("org %lu\n", org);

  printf("Pop Init! %lu\n", pop->forestv.at(0));
  gettimeofday(&tm0, NULL);
  uint32_t gencnt;
  for (gencnt=0; gencnt<NumGenerations; gencnt++) {
    int numnodes = pop->forestv[0]->tenant->NGene.size();
    printf("Pop_Gen! %lu, %lf, numnodes:%li\n", gencnt, pop->forestv[0]->tenant->Score, numnodes);
    pop->Gen();
    pop->Mutate();
    if (gencnt % CleanPause == 0) {
      pop->Clean_Inventory();
    }
    pop->Compile_Me();
    pop->Gather_IoNodes_And_Connect(&food);
    //pop->Clean_Me();
    PopMaxSize=pop->GetMaxSize();
    org0 = pop->forestv[0]->tenant;
    if (MaxSize<PopMaxSize) {
      MaxSize= PopMaxSize;
    }
    SumSize += org0->NGene.size();
  }
  gettimeofday(&tm1, NULL);

  {
    org0 = pop->forestv[0]->tenant;
    printf("Org 0, gen:%li, ", NumGenerations-1);
    org0->Print_Me();
    printf("Org 0, gen:%li, ", NumGenerations-1);
    AvgSize = SumSize/(double)NumGenerations;
    printf("size:%li, MaxSize:%li, AvgSize:%lf\n", org0->NGene.size(), MaxSize, AvgSize);
  }

  double t0 = FullTime(tm0);
  double t1 = FullTime(tm1);
  double delta = t1-t0;
  printf("delta T:%lf\n", delta);
  printf("Pop_Delete! %lf\n", pop->forestv[0]->tenant->Score);
  delete pop;
}

#if false
void randtest()
/* *************************************************************
************************************************************* */
{
  uint32 cnt,dex;
  flexelT topnode;
  leafT *newleaf;

  flexray_init(&topnode);
  for (cnt=0; cnt<testsize; cnt++) {
    dex=rand()%maxuint32;
    //newleaf=malloc(sizeof(leafT));
    newleaf=allocsafe(leafT,1);
    newleaf->value=cnt;
    flexray_insert(&topnode,dex,newleaf);
  }
  //printf("ramcnt:%u, leaframcnt:%u.\n",ramcnt,leaframcnt);

  for (cnt=0; cnt<testsize; cnt+=1) {
    dex=rand()%maxuint32;
    newleaf=flexray_remove(&topnode,dex);
    if (newleaf!=NULL) free(newleaf);
  }
  printf("randoms removed.\n");
  flexray_destroy(&topnode,&flexray_leaf_free);
}/* randtest */

void consec_test()
/* *************************************************************
************************************************************* */
{
  uint32 cnt;
  flexelT topnode;
  leafT *newleaf;

  flexray_init(&topnode);

  for (cnt=0; cnt<testsize; cnt++) {
    // newleaf=malloc(sizeof(leafT));
    newleaf=allocsafe(leafT,1);
    newleaf->value=cnt;
    flexray_insert(&topnode,cnt,newleaf);
  }
  //printf("ramcnt:%u, leaframcnt:%u.\n",ramcnt,leaframcnt);

  for (cnt=0; cnt<testsize; cnt+=2) {
    newleaf=flexray_remove(&topnode,cnt);
    if (newleaf!=NULL) free(newleaf);
  }
  printf("consecutives removed.\n");
  flexray_destroy(&topnode,&flexray_leaf_free);
}/* consec_test */
#endif


/* ********************************************************************** */
int main() {
  srand(time(NULL));

  PopSession(); return 0;

  maptest(); return 0;

  // randtest(); return 0;

  struct timeval tm0, tm1;
  gettimeofday(&tm0, NULL);

  {
    OrgPtr org0, org1;
    org0 = Org::Abiogenate();
    org0->Mutate_Me();
    org0->Compile_Me();
    printf("\n\n\n");
    org0->Print_Me();
    if (false) {
      org1 = org0->Spawn();
      org1->Mutate_Me();
      org1->Compile_Me();
      printf("\n\n\n");
      org1->Print_Me();
      delete org1;
    }
    if (true) {
      int MaxSize = 0;
      double SumSize = 0;
      int genlimit = 1000;
      gettimeofday(&tm0, NULL);
      for (int cnt=0; cnt<genlimit; cnt++) {
        org1 = org0->Spawn();
        delete org0; org0=org1;
        org1->Mutate_Me();
        org1->Compile_Me();
        //if (cnt%10==0)
        {
          printf("gen:%li, size:%li\n", cnt, org1->NGene.size());
        }
        if (MaxSize<org1->NGene.size()) {
          MaxSize= org1->NGene.size();
        }
        SumSize += org1->NGene.size();
      }
      gettimeofday(&tm1, NULL);
      double AvgSize = SumSize/(double)genlimit;
      printf("\n\n\n");
      org1->Print_Me();
      printf("Org 1, gen:%li, ", genlimit-1);
      printf("size:%li, MaxSize:%li, AvgSize:%lf\n", org1->NGene.size(), MaxSize, AvgSize);
      delete org1;

      double t0 = FullTime(tm0);
      double t1 = FullTime(tm1);
      double delta = t1-t0;
      printf("delta T:%lf\n", delta);
    }
    // delete org0;
    return 0;
  }

  sorttest();
  return 0;
  // TimeTest(); return 0;
  Pop *pop;
  if (false) {
    pop = new Pop();
    cout << "Hello world!" << endl;
    delete pop;
  }

  printf("Pop_Create!\n");
  pop = new Pop();
  printf("Pop Init! %lu\n", pop->forestv.size());
  LugarPtr lug;
  lug = pop->forestv.at(0);
  printf("lug %lu\n", lug);

  Org *org;
  org = lug->tenant;
  printf("org %lu\n", org);

  printf("Pop Init! %lu\n", pop->forestv.at(0));

  uint32_t gencnt;
  for (gencnt=0; gencnt<100; gencnt++) {
    printf("Pop_Gen! %lu, %lf\n", gencnt, pop->forestv[0]->tenant->Score);
    pop->Gen();
  }

  printf("Pop_Delete! %lf\n", pop->forestv[0]->tenant->Score);
  delete pop;

  return 0;
}
