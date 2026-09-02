/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/breakpoint_preview.h
 *
 * PURPOSE:
 *   Resolve a named responsive preview breakpoint for the visual canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_BREAKPOINT_PREVIEW_H
#define UMICOM_DESIGNER_RAD_BREAKPOINT_PREVIEW_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad breakpoint preview data shared with callers of this public contract.
 */
typedef struct UmiRadBreakpointPreview {
    char breakpoint_id[UMI_RAD_ID_CAPACITY];
    UmiRadSize viewport;
    uint32_t dpi;
    bool touch;
} UmiRadBreakpointPreview;
/**
 * Initialise rad breakpoint preview from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_breakpoint_preview_init(UmiRadBreakpointPreview *item);
/**
 * Check that rad breakpoint preview satisfies its contract before another service relies
 * on it.
 */
int umi_rad_breakpoint_preview_is_valid(const UmiRadBreakpointPreview *item);
#ifdef __cplusplus
}
#endif
#endif
