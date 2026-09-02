/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/distribution_guide.h
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
#ifndef UMICOM_DESIGNER_RAD_DISTRIBUTION_GUIDE_H
#define UMICOM_DESIGNER_RAD_DISTRIBUTION_GUIDE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad distribution guide data shared with callers of this public contract.
 */
typedef struct UmiRadDistributionGuide {
    char group_id[UMI_RAD_ID_CAPACITY];
    UmiRadOrientation orientation;
    int32_t spacing;
    size_t item_count;
} UmiRadDistributionGuide;
/**
 * Initialise rad distribution guide from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_distribution_guide_init(UmiRadDistributionGuide *item);
/**
 * Check that rad distribution guide satisfies its contract before another service relies
 * on it.
 */
int umi_rad_distribution_guide_is_valid(const UmiRadDistributionGuide *item);
#ifdef __cplusplus
}
#endif
#endif
