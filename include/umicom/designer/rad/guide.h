/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/guide.h
 *
 * PURPOSE:
 *   Represent user-created horizontal and vertical design guides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_GUIDE_H
#define UMICOM_DESIGNER_RAD_GUIDE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadGuide {
    char guide_id[UMI_RAD_ID_CAPACITY];
    UmiRadOrientation orientation;
    int32_t position;
    bool locked;
} UmiRadGuide;
UmiStatus umi_rad_guide_init(UmiRadGuide *item);
int umi_rad_guide_is_valid(const UmiRadGuide *item);
#ifdef __cplusplus
}
#endif
#endif
