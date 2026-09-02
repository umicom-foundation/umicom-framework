/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/preview_target.h
 *
 * PURPOSE:
 *   Describe GTK4, Qt6, Native Web or abstract-device preview targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PREVIEW_TARGET_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PREVIEW_TARGET_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer preview target data shared with callers of this public contract.
 */
typedef struct UmiRadPreviewTarget {
    char target_id[UMI_RAD_ID_CAPACITY];
    UmiRadTargetKind kind;
    UmiRadSize viewport;
    uint32_t dpi;
} UmiRadPreviewTarget;
/**
 * Initialise visual designer preview target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_preview_target_init(UmiRadPreviewTarget *item);
/**
 * Check that visual designer preview target satisfies its contract before another service relies on
 * it.
 */
int umi_rad_preview_target_is_valid(const UmiRadPreviewTarget *item);
#ifdef __cplusplus
}
#endif
#endif
