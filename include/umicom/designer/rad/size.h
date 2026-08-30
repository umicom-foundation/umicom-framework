/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/size.h
 *
 * PURPOSE:
 *   Provide bounded designer size values and clamp operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_SIZE_H
#define UMICOM_DESIGNER_RAD_SIZE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDesignerSize {
    int32_t width;
    int32_t height;
} UmiRadDesignerSize;
UmiRadDesignerSize umi_rad_size_make(int32_t width,int32_t height);
UmiRadDesignerSize umi_rad_size_clamp(UmiRadDesignerSize value,UmiRadDesignerSize minimum,UmiRadDesignerSize maximum);
#ifdef __cplusplus
}
#endif
#endif
