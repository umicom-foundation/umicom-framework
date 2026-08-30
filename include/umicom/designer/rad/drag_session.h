/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/drag_session.h
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
#ifndef UMICOM_DESIGNER_RAD_DRAG_SESSION_H
#define UMICOM_DESIGNER_RAD_DRAG_SESSION_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDragSession {
    char component_id[UMI_RAD_ID_CAPACITY];
    UmiRadPoint start;
    UmiRadPoint current;
    bool active;
} UmiRadDragSession;
UmiStatus umi_rad_drag_session_init(UmiRadDragSession *item);
int umi_rad_drag_session_is_valid(const UmiRadDragSession *item);
#ifdef __cplusplus
}
#endif
#endif
