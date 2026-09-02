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
/*
 * Perform ui ent column resize through the module contract so client applications do not
 * duplicate its policy.
 */
int32_t umi_ui_ent_column_resize_apply(const UmiUiEntColumnDescriptor *c,int32_t w,int32_t d){int64_t x;/* Apply this branch only when its contract condition is satisfied. */ if(!c||!c->resizable)return w;x=(int64_t)w+(int64_t)d;/* Apply this branch only when its contract condition is satisfied. */ if(x<c->minimum_width)x=c->minimum_width;/* Apply this branch only when its contract condition is satisfied. */ if(x>c->maximum_width)x=c->maximum_width;return (int32_t)x;}
