/* **************************************************************************

  flexray.c, a library supportting trees-of-lookup-tables.

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

#ifndef FLEXRAY_H_INCLUDED
#define FLEXRAY_H_INCLUDED

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef bool
  #define bool unsigned char
  #define true 0x1
  #define false 0x0
#endif
#ifndef int8
  #define int8 char
#endif
#ifndef uint8
  #define uint8 unsigned int8
#endif
#ifndef int32
  #define int32 int
#endif
#ifndef uint32
  #define uint32 unsigned int32
  #define maxuint32 (~(uint32)0)
#endif

#if 1
typedef struct leafT *leafTPtr;
typedef struct leafT
/* *************************************************************
************************************************************* */
{
  int value;
  double xyz[3];
}leafT;
#else
//typedef void leafT;
typedef uint32 leafT;
#endif

typedef struct flexelT
/* *************************************************************
*  Flex array element.
************************************************************* */
{
  uint8 num_children;
  union down{
    struct flexelT *next;/* the bar (array) I point to. */
    leafT *leaf;/* leaf node. */
  }down;
}flexelT;

/* generic routine for disposing of user-defined leaves. */
typedef void flexray_leafkillT(void *leaf);

void flexray_init(flexelT *topnode);
bool flexray_insert(flexelT *topnode,uint32 key,leafT *newleaf);
void flexray_destroy(flexelT *topnode,flexray_leafkillT *leafkill);
leafT *flexray_lookup(flexelT *topnode,uint32 key);
leafT *flexray_remove(flexelT *topnode,uint32 key);
leafT *flexray_toggle(flexelT *topnode,uint32 key,leafT *newleaf);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif // FLEXRAY_H_INCLUDED
