/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/drag_session.h
 *
 * PURPOSE:
 *   Track a visual component drag operation from press through commit/cancel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DRAG_SESSION_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DRAG_SESSION_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer drag session data shared with callers of this public contract.
 */
typedef struct UmiRadDragSession {
    char component_id[UMI_RAD_ID_CAPACITY];
    UmiRadPoint start;
    UmiRadPoint current;
    bool active;
} UmiRadDragSession;
/**
 * Initialise visual designer drag session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_drag_session_init(UmiRadDragSession *item);
/**
 * Check that visual designer drag session satisfies its contract before another service relies on it.
 */
int umi_rad_drag_session_is_valid(const UmiRadDragSession *item);
#ifdef __cplusplus
}
#endif
#endif
