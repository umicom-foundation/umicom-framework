/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/distribution_guide.h
 *
 * PURPOSE:
 *   Represent equal-spacing evidence for multiple selected components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DISTRIBUTION_GUIDE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DISTRIBUTION_GUIDE_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer distribution guide data shared with callers of this public contract.
 */
typedef struct UmiRadDistributionGuide {
    char group_id[UMI_RAD_ID_CAPACITY];
    UmiRadOrientation orientation;
    int32_t spacing;
    size_t item_count;
} UmiRadDistributionGuide;
/**
 * Initialise visual designer distribution guide from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_distribution_guide_init(UmiRadDistributionGuide *item);
/**
 * Check that visual designer distribution guide satisfies its contract before another service relies
 * on it.
 */
int umi_rad_distribution_guide_is_valid(const UmiRadDistributionGuide *item);
#ifdef __cplusplus
}
#endif
#endif
