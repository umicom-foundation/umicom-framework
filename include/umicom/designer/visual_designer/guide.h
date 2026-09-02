/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/guide.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_GUIDE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_GUIDE_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer guide data shared with callers of this public contract.
 */
typedef struct UmiRadGuide {
    char guide_id[UMI_RAD_ID_CAPACITY];
    UmiRadOrientation orientation;
    int32_t position;
    bool locked;
} UmiRadGuide;
/**
 * Initialise visual designer guide from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_guide_init(UmiRadGuide *item);
/**
 * Check that visual designer guide satisfies its contract before another service relies on it.
 */
int umi_rad_guide_is_valid(const UmiRadGuide *item);
#ifdef __cplusplus
}
#endif
#endif
