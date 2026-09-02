/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/resize_handle.h
 *
 * PURPOSE:
 *   Describe resize-handle semantics without depending on a toolkit cursor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_RESIZE_HANDLE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_RESIZE_HANDLE_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer resize handle data shared with callers of this public contract.
 */
typedef struct UmiRadResizeHandle {
    uint32_t edges;
    UmiRadPoint location;
    bool enabled;
} UmiRadResizeHandle;
/**
 * Initialise visual designer resize handle from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_resize_handle_init(UmiRadResizeHandle *item);
/**
 * Check that visual designer resize handle satisfies its contract before another service relies on it.
 */
int umi_rad_resize_handle_is_valid(const UmiRadResizeHandle *item);
#ifdef __cplusplus
}
#endif
#endif
