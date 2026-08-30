/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/ruler.h
 *
 * PURPOSE:
 *   Describe design-time rulers and origin offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_RULER_H
#define UMICOM_DESIGNER_RAD_RULER_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadRuler {
    UmiRadOrientation orientation;
    int32_t origin;
    int32_t major_step;
    bool visible;
} UmiRadRuler;
UmiStatus umi_rad_ruler_init(UmiRadRuler *item);
int umi_rad_ruler_is_valid(const UmiRadRuler *item);
#ifdef __cplusplus
}
#endif
#endif
