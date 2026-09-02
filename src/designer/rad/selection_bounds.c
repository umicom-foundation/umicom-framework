/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/selection_bounds.c
 *
 * PURPOSE:
 *   Calculate the bounding rectangle of a multi-component selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/selection_bounds.h"
/*
 * Provide the rad selection bounds compute operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_selection_bounds_compute(const UmiRadRect *r,size_t n,UmiRadSelectionBounds *o){size_t i;int32_t minx,miny,maxx,maxy;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||o==NULL||n==0U)return UMI_STATUS_INVALID_ARGUMENT;minx=r[0].x;miny=r[0].y;maxx=r[0].x+r[0].width;maxy=r[0].y+r[0].height;/* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<n;i++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r[i].x<minx)minx=r[i].x;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r[i].y<miny)miny=r[i].y;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r[i].x+r[i].width>maxx)maxx=r[i].x+r[i].width;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r[i].y+r[i].height>maxy)maxy=r[i].y+r[i].height;}o->bounds=(UmiRadRect){minx,miny,maxx-minx,maxy-miny};o->item_count=n;return UMI_STATUS_OK;}
