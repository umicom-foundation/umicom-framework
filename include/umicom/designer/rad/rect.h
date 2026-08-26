/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/rect.h
 *
 * PURPOSE:
 *   Provide rectangle geometry used by selection, hit testing and layout editing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_RECT_H
#define UMICOM_DESIGNER_RAD_RECT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDesignerRect {
    UmiRadRect value;
} UmiRadDesignerRect;
UmiRadDesignerRect umi_rad_rect_make(int32_t x,int32_t y,int32_t width,int32_t height);
int umi_rad_rect_contains(const UmiRadDesignerRect *rect,UmiRadPoint point);
int umi_rad_rect_intersects(const UmiRadDesignerRect *left,const UmiRadDesignerRect *right);
#ifdef __cplusplus
}
#endif
#endif
