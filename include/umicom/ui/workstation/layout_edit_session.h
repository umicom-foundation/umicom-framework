/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_edit_session.h
 *
 * PURPOSE:
 *   Define transactional layout edit state for unlock, preview, commit and cancel workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_EDIT_SESSION_H
#define UMICOM_UI_WORKSTATION_LAYOUT_EDIT_SESSION_H

#include <stdbool.h>
#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsLayoutEditSession {
    UmiWsLayoutTemplate original;
    UmiWsLayoutTemplate working;
    bool active;
    bool dirty;
} UmiWsLayoutEditSession;

UmiStatus umi_ws_layout_edit_session_begin(UmiWsLayoutEditSession *session, const UmiWsLayoutTemplate *layout);
UmiWsLayoutTemplate *umi_ws_layout_edit_session_working(UmiWsLayoutEditSession *session);
void umi_ws_layout_edit_session_mark_dirty(UmiWsLayoutEditSession *session);
UmiStatus umi_ws_layout_edit_session_commit(UmiWsLayoutEditSession *session, UmiWsLayoutTemplate *out_layout);
void umi_ws_layout_edit_session_cancel(UmiWsLayoutEditSession *session);

#ifdef __cplusplus
}
#endif

#endif
