/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/alignment_guide.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_ALIGNMENT_GUIDE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_ALIGNMENT_GUIDE_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer alignment guide data shared with callers of this public contract.
 */
typedef struct UmiRadAlignmentGuide {
    char source_id[UMI_RAD_ID_CAPACITY];
    char peer_id[UMI_RAD_ID_CAPACITY];
    UmiRadOrientation orientation;
    int32_t position;
} UmiRadAlignmentGuide;
/**
 * Initialise visual designer alignment guide from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_alignment_guide_init(UmiRadAlignmentGuide *item);
/**
 * Check that visual designer alignment guide satisfies its contract before another service relies on
 * it.
 */
int umi_rad_alignment_guide_is_valid(const UmiRadAlignmentGuide *item);
#ifdef __cplusplus
}
#endif
#endif
