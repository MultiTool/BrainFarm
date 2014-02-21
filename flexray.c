/* ******************************************************************
  Flexray.c - a program to simulate a huge array with flexible
dynamic ram usage.  Main purpose is to use as a lookup
table when large discontinuous integers are the input.

  Data is organized in a tree structure where every node is a
4-element array.  4-child nodes test to be generally optimum over
both 2-child nodes and 16-child nodes.

Copyright (C) 2000 John Conner Atkeson

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

For further information, contact:
John Atkeson jcatkeson@juno.com
210 Drummond Place, Apt 19, Norfolk Va. 23507 USA

****************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "flexray.h"

#define testflex 0

#define highroot 1

#define testsize 1000

/* Power of 2 that makes bitwdt.
 0=2-child nodes, 1=4-child nodes, 2=16-child nodes, 3=256-child nodes. */
#define bitwdtpow 0x2

/* 4 bit index per tier. */
#define bitwdt (1<<bitwdtpow)

/* Maximum index is 16. */
#define bitrange (1<<bitwdt)

/* 4-bit mask = 0xf */
#define mask4 (~(~(uint32)0<<bitwdt)) // #define mask4 0xf

/* number of 4-bit chunks in an int32 = (number of bytes)*2. */
#define max_strata (sizeof(uint32) * (8>>bitwdtpow))
#define last_strata (max_strata-1)

/* highroot defines whether the trunk of the flexray tree is most
 significant digit or least significant digit.  If highroot=1 then
 the trunk is MSD. */
#if highroot
   /* first get hbyte, shift key right and mask. */
   /* <<bitwdtpow = times bitwdt. */
  #define keyof(key,cnt) ( (key>>((last_strata-cnt)<<bitwdtpow)) & mask4 )
#else
   /* first get lbyte, shift key right and mask. */
  #define keyof(key,cnt) ( (key>>(cnt<<bitwdtpow)) & mask4 )
#endif

uint32 ramcnt=0,leaframcnt=0;

void flexray_leaf_free(void *leaf)
/* *************************************************************
************************************************************* */
{
  free(leaf);
}

void flex_test(){
}

bool flexray_insert(flexelT *topnode,uint32 key,leafT *newleaf)
/* *************************************************************
*   Insert a new element in the flexarray.  Returns true on
* success, false if the space is already occupied.
************************************************************* */
{
  flexelT temptop={0,NULL};
  flexelT *upcell=&temptop;
  flexelT *headcell=topnode;
  uint32 cnt,tkey;/* temp key */
  bool retval=false;

  temptop.down.next=topnode; temptop.num_children=0;

  for (cnt=0;cnt<max_strata;cnt++){/* up from 0 to 3. */

    /* if strata not found, create a new one. */
    if (headcell->down.next==NULL){
      headcell->down.next=calloc(bitrange,sizeof(flexelT));
      memset(headcell->down.next,0,bitrange*sizeof(flexelT));
      upcell->num_children++;
#if bitwdtpow==0
      ramcnt+=bitrange*sizeof(flexelT)-(sizeof(uint8)*bitrange);
#else
      ramcnt+=bitrange*sizeof(flexelT);
#endif
    }
    tkey=keyof(key,cnt);

    /* upcell is 1 tier up from headcell, and is used only to increment
      child-counter. */
    upcell=headcell;

    /* set headcell to next layer down, indexed by tkey. */
    headcell=&(headcell->down.next[tkey]);
  }
  /* only insert a leaf if one not already there. */
  if (headcell->down.leaf==NULL){
    upcell->num_children++;
    headcell->down.leaf=newleaf;
    leaframcnt+=sizeof(leafT);
    retval=true;
  }else{
    retval=false;
  }
  return(retval);
}/* flexray_insert */

static void flexray_recurse_destroy(flexelT *node,int depth,flexray_leafkillT *leafkill)
/* *************************************************************
*  Recursive part of flexray_destroy.
************************************************************* */
{
  uint32 cnt;

  depth++;
  if (depth<max_strata){

    for (cnt=0;cnt<bitrange;cnt++){/* up from 0 to 15. */
      /* if strata found, recurse. */
      if (node[cnt].down.next!=NULL){
        flexray_recurse_destroy(node[cnt].down.next,depth,leafkill);
        node[cnt].down.next=NULL;
      }
    }
  }else{/* depth has reached layer 8. */
    /* only do this if the leaves are dynamically allocated. */
    for (cnt=0;cnt<bitrange;cnt++){/* up from 0 to 15. */
      /* if leaf found, free it. */
      if (node[cnt].down.leaf!=NULL){
        if (leafkill!=NULL) leafkill(node[cnt].down.leaf);
        node[cnt].down.leaf=NULL;
      }
    }
  }
  free(node); node=NULL;
}/* flexray_recurse_destroy */
void flexray_destroy(flexelT *topnode,flexray_leafkillT *leafkill)
/* *************************************************************
*  Recursively free all memory of a flexray.
*  This part is the non-recursive wrapper.
************************************************************* */
{
  if (topnode->num_children>0){
    flexray_recurse_destroy(topnode->down.next,0,leafkill);
    topnode->down.next=NULL; topnode->num_children=0;
  }
}/* flexray_destroy */


leafT *flexray_lookup(flexelT *topnode,uint32 key)
/* *************************************************************
*  Look up one element in the flexray.  Returns NULL if the
* leaf is not found.
************************************************************* */
{
  flexelT temptop={0,NULL},*headcell=&temptop;
  uint32 cnt,tkey;/* temp key */

  headcell=topnode;
  cnt=0; /* Abort if strata not found. */
  while (cnt<max_strata && headcell->down.next!=NULL){
    tkey=keyof(key,cnt);
    /* look up next level in this array. */
    headcell=&(headcell->down.next[tkey]);
    cnt++;
  }
  return(headcell->down.leaf);
}/* flexray_lookup */

leafT *flexray_remove(flexelT *topnode,uint32 key)
/* *************************************************************
*  Look up one element in the flexray and remove it.
* Returns NULL if leaf is not found.
************************************************************* */
{
  flexelT temptop={0,NULL},*headcell=&temptop;
  flexelT *stack[max_strata];
  uint32 cnt,tkey;/* temp key */
  leafT *leaf;

  headcell=topnode;

  cnt=0; /* Abort if strata not found. */
  while (cnt<max_strata && headcell->down.next!=NULL){
    tkey=keyof(key,cnt);

    stack[cnt]=headcell;
    /* look up next level in this array. */
    headcell=&(headcell->down.next[tkey]);
    cnt++;
  }
  if (headcell->down.next==NULL){
    leaf=NULL;
  }else{
    leaf=headcell->down.leaf;
  }
  if (leaf!=NULL){/* Abort if strata not found. */
    headcell->down.leaf=NULL;
    cnt=max_strata;
    while (cnt>0){
      cnt--;
      /* on first iteration,
       last bar loses a leaf, dec counter in second-to-last. */
      stack[cnt]->num_children--;
      if (stack[cnt]->num_children>0) break;
      free(stack[cnt]->down.next); stack[cnt]->down.next=NULL;
    }/* end while. */
  }
  return(leaf);
}/* flexray_remove */

leafT *flexray_toggle(flexelT *topnode,uint32 key,leafT *newleaf)
/* *************************************************************
*  If nothing is there, add a new leaf.
*  If an old leaf is there, remove it and add nothing.
*  Returns a leaf if a leaf was removed, NULL if one was added.
************************************************************* */
{
  leafT *oldleaf=NULL;

  oldleaf=flexray_remove(topnode,key);
  if (oldleaf==NULL){
    flexray_insert(topnode,key,newleaf);
  }
  return(oldleaf);
}/* flexray_toggle */

void flexray_init(flexelT *topnode)
/* *************************************************************
************************************************************* */
{
  topnode->num_children=0; topnode->down.next=NULL;
}

#if testflex
void randtest()
/* *************************************************************
************************************************************* */
{
  uint32 cnt,dex;
  flexelT topnode;
  leafT *newleaf;

  flexray_init(&topnode);
  for (cnt=0;cnt<testsize;cnt++){
    dex=random()%maxuint32;
    newleaf=malloc(sizeof(leafT)); newleaf->value=cnt;
    flexray_insert(&topnode,dex,newleaf);
  }
  bugprintf("ramcnt:%u, leaframcnt:%u.\n",ramcnt,leaframcnt);

  for (cnt=0;cnt<testsize;cnt+=1){
    dex=random()%maxuint32;
    newleaf=flexray_remove(&topnode,dex);
    if (newleaf!=NULL) free(newleaf);
  }
  bugprintf("randoms removed.\n");
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

  for (cnt=0;cnt<testsize;cnt++){
    newleaf=malloc(sizeof(leafT)); newleaf->value=cnt;
    flexray_insert(&topnode,cnt,newleaf);
  }
  bugprintf("ramcnt:%u, leaframcnt:%u.\n",ramcnt,leaframcnt);

  for (cnt=0;cnt<testsize;cnt+=2){
    newleaf=flexray_remove(&topnode,cnt);
    if (newleaf!=NULL) free(newleaf);
  }
  bugprintf("consecutives removed.\n");
  flexray_destroy(&topnode,&flexray_leaf_free);
}/* consec_test */


int main()
/* *************************************************************
************************************************************* */
{
  uint32 cnt;
  flexelT *helen=NULL,melvin;
  leafT *newleaf,*frieda,*wendy;

  bugprintf("Starting.\n\n\n");


  consec_test(); getchar(); return(0);
  randtest(); getchar(); return(0);

  flexray_init(&melvin);

  newleaf=malloc(sizeof(leafT)); newleaf->value=30;
  flexray_insert(&melvin,0x01234567,newleaf);
//flexray_insert(&melvin,0x76543210,newleaf);

  newleaf=malloc(sizeof(leafT)); newleaf->value=29;
  flexray_insert(&melvin,0x76543210,newleaf);
//  flexray_insert(&melvin,0x01233210,newleaf);

  wendy=flexray_remove(&melvin,0x76543210);
//  wendy=flexray_remove(&melvin,0x01233210);
  if (wendy==NULL){
    bugprintf("NULL wendy:%x\n",wendy);
  }else{
    bugprintf("wendy:%li\n",wendy->value);
    free(wendy);
  }

  for (cnt=0;cnt<10000;cnt++){
    newleaf=malloc(sizeof(leafT)); newleaf->value=cnt;
    flexray_insert(&melvin,cnt,newleaf);
  }
  bugprintf("ramcnt:%u, leaframcnt:%u.\n",ramcnt,leaframcnt);

  for (cnt=0;cnt<10000;cnt+=2){
    newleaf=flexray_remove(&melvin,cnt);
    free(newleaf);
  }
  getchar();

  bugprintf("\n\n");
  frieda=flexray_lookup(&melvin,0x76543210);
  if (frieda==NULL){
    bugprintf("NULL frieda:%x\n",frieda);
  }else{
    bugprintf("frieda:%li\n",frieda->value);
  }
  frieda=flexray_lookup(&melvin,0x7654f210);
  if (frieda==NULL){
    bugprintf("NULL frieda:%x\n",frieda);
  }else{
    bugprintf("frieda:%li\n",frieda->value);
  }

  bugprintf("\n\n");
  flexray_destroy(&melvin,&flexray_leaf_free);
//  205.156.14.66   ahbo2k

  getchar();

//  free(newleaf);
  return(0);
}

#endif

