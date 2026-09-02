/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/rect.h
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
#ifndef UMICOM_DESIGNER_RAD_RECT_H
#define UMICOM_DESIGNER_RAD_RECT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad designer rect data shared with callers of this public contract.
 */
typedef struct UmiRadDesignerRect {
    UmiRadRect value;
} UmiRadDesignerRect;
/**
 * Provide the rad rect make operation used by this module and its client applications.
 */
UmiRadDesignerRect umi_rad_rect_make(int32_t x,int32_t y,int32_t width,int32_t height);
/**
 * Provide the rad rect contains operation used by this module and its client applications.
 */
int umi_rad_rect_contains(const UmiRadDesignerRect *rect,UmiRadPoint point);
/**
 * Provide the rad rect intersects operation used by this module and its client
 * applications.
 */
int umi_rad_rect_intersects(const UmiRadDesignerRect *left,const UmiRadDesignerRect *right);
#ifdef __cplusplus
}
#endif
#endif
