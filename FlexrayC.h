#ifndef FLEXRAYC_H_INCLUDED
#define FLEXRAYC_H_INCLUDED

/* **************************************************************************

  flexray.h, a library supportting trees-of-lookup-tables.

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

************************************************************************** */

#include <stdint.h>
#include "Base.h"

#ifndef bool
#define bool unsigned char
#define true 0x1
#define false 0x0
#endif

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
#define mask4 (~(~(uint32_t)0<<bitwdt)) // #define mask4 0xf

/* number of 4-bit chunks in an int32_t = (number of bytes)*2. */
#define max_strata (sizeof(uint32_t) * (8>>bitwdtpow))
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

namespace FR {
  /* generic routine for disposing of user-defined leaves. */

  /* **************************************************************************** */
  template <class LeafType>
  class Flexray {
  public:
    typedef LeafType *LeafTypePtr;
    uint32_t ramcnt, leaframcnt;
    typedef void flexray_leafkillT(LeafType *leaf);

    typedef struct flexelT
    /* *************************************************************
    *  Flex array element.
    ************************************************************* */
    {
      uint8_t num_children;
      union down {
        struct flexelT *next;/* the bar (array) I point to. */
        //leafT *leaf;/* leaf node. */
        LeafType *Leaf;/* leaf node. */
      } down;
    };

    flexelT topnode;

    /* **************************************************************************** */
    Flexray() {
      this->Init();
    }
    /* **************************************************************************** */
    ~Flexray() {
      this->Destroy(NULL);
    }
    void Init()
    /* *************************************************************
    ************************************************************* */
    {
      ramcnt=0; leaframcnt=0;
      topnode.num_children=0; topnode.down.next=NULL;
    }
    bool Insert(uint32_t key,LeafType *newleaf)
    /* *************************************************************
    *   Insert a new element in the flexarray.  Returns true on
    * success, false if the space is already occupied.
    ************************************************************* */
    {
      flexelT temptop= {0,NULL};
      flexelT *upcell=&temptop;
      flexelT *headcell=&topnode;
      uint32_t cnt,tkey;/* temp key */
      bool retval=false;

      temptop.down.next=&topnode; temptop.num_children=0;

      for (cnt=0; cnt<max_strata; cnt++) { /* up from 0 to 3. */

        /* if strata not found, create a new one. */
        if (headcell->down.next==NULL) {
          headcell->down.next=allocsafe(flexelT, bitrange);
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
      if (headcell->down.Leaf==NULL) {
        upcell->num_children++;
        headcell->down.Leaf=newleaf;
        leaframcnt+=sizeof(LeafType);
        retval=true;
      } else {
        retval=false;
      }
      return(retval);
    }/* Insert */

    static void Recurse_Destroy(flexelT *node, int depth, flexray_leafkillT *leafkill)
    /* *************************************************************
    *  Recursive part of Destroy.
    ************************************************************* */
    {
      uint32_t cnt;
      depth++;
      if (depth<max_strata) {
        for (cnt=0; cnt<bitrange; cnt++) { /* up from 0 to 15. */
          /* if strata found, recurse. */
          if (node[cnt].down.next!=NULL) {
            Recurse_Destroy(node[cnt].down.next, depth, leafkill);
            node[cnt].down.next=NULL;
          }
        }
      } else if (leafkill!=NULL){ /* depth has reached layer 8. */
        /* only do this if the leaves are dynamically allocated. */
        for (cnt=0; cnt<bitrange; cnt++) { /* up from 0 to 15. */
          /* if leaf found, free it. */
          if (node[cnt].down.Leaf!=NULL) {
            leafkill(node[cnt].down.Leaf);
            //node[cnt].down.Leaf=NULL;
          }
        }
      }
      freesafe(node); //node=NULL;
    }/* Recurse_Destroy */

    void Destroy(flexray_leafkillT *leafkill)
    /* *************************************************************
    *  Recursively free all memory of a flexray.
    *  This part is the non-recursive wrapper.
    ************************************************************* */
    {
      if (topnode.num_children>0) {
        Recurse_Destroy(topnode.down.next, 0, leafkill);
        topnode.down.next=NULL; topnode.num_children=0;
      }
    }/* Destroy */

    LeafType *Lookup(uint32_t key)
    /* *************************************************************
    *  Look up one element in the flexray.  Returns NULL if the
    * leaf is not found.
    ************************************************************* */
    {
      flexelT temptop= {0,NULL},*headcell=&temptop;
      uint32_t cnt,tkey;/* temp key */

      headcell=&topnode;
      cnt=0; /* Abort if strata not found. */
      while (cnt<max_strata && headcell->down.next!=NULL) {
        tkey=keyof(key,cnt);
        /* look up next level in this array. */
        headcell=&(headcell->down.next[tkey]);
        cnt++;
      }
      return(headcell->down.Leaf);
    }/* Lookup */

    LeafType *Remove(uint32_t key)
    /* *************************************************************
    *  Look up one element in the flexray and remove it.
    * Returns NULL if leaf is not found.
    ************************************************************* */
    {
      flexelT temptop= {0,NULL},*headcell=&temptop;
      flexelT *stack[max_strata];
      uint32_t cnt,tkey;/* temp key */
      LeafType *leaf;

      headcell=&topnode;

      cnt=0; /* Abort if strata not found. */
      while (cnt<max_strata && headcell->down.next!=NULL) {
        tkey=keyof(key,cnt);

        stack[cnt]=headcell;
        /* look up next level in this array. */
        headcell=&(headcell->down.next[tkey]);
        cnt++;
      }
      if (headcell->down.next==NULL) {
        leaf=NULL;
      } else {
        leaf=headcell->down.Leaf;
      }
      if (leaf!=NULL) { /* Abort if strata not found. */
        headcell->down.Leaf=NULL;
        cnt=max_strata;
        while (cnt>0) {
          cnt--;
          /* on first iteration,
           last bar loses a leaf, dec counter in second-to-last. */
          stack[cnt]->num_children--;
          if (stack[cnt]->num_children>0) break;
          freesafe(stack[cnt]->down.next); stack[cnt]->down.next=NULL;
        }/* end while. */
      }
      return(leaf);
    }/* Remove */

    LeafType *flexray_toggle(uint32_t key,LeafType *newleaf)
    /* *************************************************************
    *  If nothing is there, add a new leaf.
    *  If an old leaf is there, remove it and add nothing.
    *  Returns a leaf if a leaf was removed, NULL if one was added.
    ************************************************************* */
    {
      LeafType *oldleaf=NULL;

      oldleaf=Remove(key);
      if (oldleaf==NULL) {
        flexray_insert(key,newleaf);
      }
      return(oldleaf);
    }/* flexray_toggle */
  };

}

#endif // FLEXRAYC_H_INCLUDED
