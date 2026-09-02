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
/**
 * Represent the rad guide data shared with callers of this public contract.
 */
typedef struct UmiRadGuide {
    char guide_id[UMI_RAD_ID_CAPACITY];
    UmiRadOrientation orientation;
    int32_t position;
    bool locked;
} UmiRadGuide;
/**
 * Initialise rad guide from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_guide_init(UmiRadGuide *item);
/**
 * Check that rad guide satisfies its contract before another service relies on it.
 */
int umi_rad_guide_is_valid(const UmiRadGuide *item);
#ifdef __cplusplus
}
#endif
#endif
