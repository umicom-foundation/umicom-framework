/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/preview_target.h
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
#ifndef UMICOM_DESIGNER_RAD_PREVIEW_TARGET_H
#define UMICOM_DESIGNER_RAD_PREVIEW_TARGET_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad preview target data shared with callers of this public contract.
 */
typedef struct UmiRadPreviewTarget {
    char target_id[UMI_RAD_ID_CAPACITY];
    UmiRadTargetKind kind;
    UmiRadSize viewport;
    uint32_t dpi;
} UmiRadPreviewTarget;
/**
 * Initialise rad preview target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_preview_target_init(UmiRadPreviewTarget *item);
/**
 * Check that rad preview target satisfies its contract before another service relies on
 * it.
 */
int umi_rad_preview_target_is_valid(const UmiRadPreviewTarget *item);
#ifdef __cplusplus
}
#endif
#endif
