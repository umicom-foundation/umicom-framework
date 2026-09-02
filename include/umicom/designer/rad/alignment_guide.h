/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/alignment_guide.h
 *
 * PURPOSE:
 *   Represent alignment evidence between visual components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_ALIGNMENT_GUIDE_H
#define UMICOM_DESIGNER_RAD_ALIGNMENT_GUIDE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad alignment guide data shared with callers of this public contract.
 */
typedef struct UmiRadAlignmentGuide {
    char source_id[UMI_RAD_ID_CAPACITY];
    char peer_id[UMI_RAD_ID_CAPACITY];
    UmiRadOrientation orientation;
    int32_t position;
} UmiRadAlignmentGuide;
/**
 * Initialise rad alignment guide from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_alignment_guide_init(UmiRadAlignmentGuide *item);
/**
 * Check that rad alignment guide satisfies its contract before another service relies on
 * it.
 */
int umi_rad_alignment_guide_is_valid(const UmiRadAlignmentGuide *item);
#ifdef __cplusplus
}
#endif
#endif
