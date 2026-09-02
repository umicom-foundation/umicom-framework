/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/drop_target.h
 *
 * PURPOSE:
 *   Represent validated parent/slot destinations during component drag and drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DROP_TARGET_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DROP_TARGET_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer drop target data shared with callers of this public contract.
 */
typedef struct UmiRadDropTarget {
    char parent_id[UMI_RAD_ID_CAPACITY];
    char slot_id[UMI_RAD_ID_CAPACITY];
    UmiRadRect bounds;
    bool accepted;
} UmiRadDropTarget;
/**
 * Initialise visual designer drop target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_drop_target_init(UmiRadDropTarget *item);
/**
 * Check that visual designer drop target satisfies its contract before another service relies on it.
 */
int umi_rad_drop_target_is_valid(const UmiRadDropTarget *item);
#ifdef __cplusplus
}
#endif
#endif
