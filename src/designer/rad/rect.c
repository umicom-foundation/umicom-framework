/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/rect.c
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
#include "umicom/designer/rad/rect.h"
UmiRadDesignerRect umi_rad_rect_make(int32_t x,int32_t y,int32_t w,int32_t h){UmiRadDesignerRect r={{x,y,w,h}};return r;}
int umi_rad_rect_contains(const UmiRadDesignerRect *r,UmiRadPoint p){if(r==NULL)return 0;return p.x>=r->value.x&&p.y>=r->value.y&&p.x<r->value.x+r->value.width&&p.y<r->value.y+r->value.height;}
int umi_rad_rect_intersects(const UmiRadDesignerRect *a,const UmiRadDesignerRect *b){if(a==NULL||b==NULL)return 0;return a->value.x<b->value.x+b->value.width&&a->value.x+a->value.width>b->value.x&&a->value.y<b->value.y+b->value.height&&a->value.y+a->value.height>b->value.y;}
