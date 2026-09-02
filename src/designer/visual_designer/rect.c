/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/rect.c
 *
 * PURPOSE:
 *   Provide rectangle geometry used by selection, hit testing and layout editing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/rect.h"
/* Provide the visual designer rect make operation used by this module and its client applications. */
UmiRadDesignerRect umi_rad_rect_make(int32_t x,int32_t y,int32_t w,int32_t h){UmiRadDesignerRect r={{x,y,w,h}};return r;}
/* Provide the visual designer rect contains operation used by this module and its client applications. */
int umi_rad_rect_contains(const UmiRadDesignerRect *r,UmiRadPoint p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return 0;return p.x>=r->value.x&&p.y>=r->value.y&&p.x<r->value.x+r->value.width&&p.y<r->value.y+r->value.height;}
/*
 * Provide the visual designer rect intersects operation used by this module and its client
 * applications.
 */
int umi_rad_rect_intersects(const UmiRadDesignerRect *a,const UmiRadDesignerRect *b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||b==NULL)return 0;return a->value.x<b->value.x+b->value.width&&a->value.x+a->value.width>b->value.x&&a->value.y<b->value.y+b->value.height&&a->value.y+a->value.height>b->value.y;}
