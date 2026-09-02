/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/drag_session.h
 *
 * PURPOSE:
 *   Define explicit drag-session state shared by GTK4 pointer DnD and keyboard panel movement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DRAG_SESSION_H
#define UMICOM_UI_WORKSTATION_DRAG_SESSION_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws drag session data shared with callers of this public contract.
 */
typedef struct UmiWsDragSession {
    char surface_id[UMI_UI_ID_CAPACITY];
    UmiUiPoint start;
    UmiUiPoint current;
    UmiWsDockRegion proposed_region;
    bool active;
    bool committed;
} UmiWsDragSession;

/**
 * Provide the ws drag session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_drag_session_begin(UmiWsDragSession *session, const char *surface_id, UmiUiPoint start);
/**
 * Provide the ws drag session move operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_drag_session_move(UmiWsDragSession *session, UmiUiPoint current, UmiWsDockRegion proposed_region);
/**
 * Provide the ws drag session commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_drag_session_commit(UmiWsDragSession *session);
/**
 * Provide the ws drag session cancel operation used by this module and its client
 * applications.
 */
void umi_ws_drag_session_cancel(UmiWsDragSession *session);
/**
 * Provide the ws drag session distance operation used by this module and its client
 * applications.
 */
int32_t umi_ws_drag_session_distance(const UmiWsDragSession *session);

#ifdef __cplusplus
}
#endif

#endif
