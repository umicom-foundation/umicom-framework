/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/column_resize.c
 *
 * PURPOSE:
 *   Implement bounded column resizing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_resize.h"
#include <stdint.h>
int32_t umi_ui_ent_column_resize_apply(const UmiUiEntColumnDescriptor *c,int32_t w,int32_t d){int64_t x;if(!c||!c->resizable)return w;x=(int64_t)w+(int64_t)d;if(x<c->minimum_width)x=c->minimum_width;if(x>c->maximum_width)x=c->maximum_width;return (int32_t)x;}
