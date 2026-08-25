/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/drag_session.h
 *
 * PURPOSE:
 *   Define explicit drag-session state shared by GTK4 pointer DnD and keyboard panel movement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DRAG_SESSION_H
#define UMICOM_UI_WORKSTATION_DRAG_SESSION_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsDragSession {
    char surface_id[UMI_UI_ID_CAPACITY];
    UmiUiPoint start;
    UmiUiPoint current;
    UmiWsDockRegion proposed_region;
    bool active;
    bool committed;
} UmiWsDragSession;

UmiStatus umi_ws_drag_session_begin(UmiWsDragSession *session, const char *surface_id, UmiUiPoint start);
UmiStatus umi_ws_drag_session_move(UmiWsDragSession *session, UmiUiPoint current, UmiWsDockRegion proposed_region);
UmiStatus umi_ws_drag_session_commit(UmiWsDragSession *session);
void umi_ws_drag_session_cancel(UmiWsDragSession *session);
int32_t umi_ws_drag_session_distance(const UmiWsDragSession *session);

#ifdef __cplusplus
}
#endif

#endif
