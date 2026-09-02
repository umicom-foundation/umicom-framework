/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_edit_session.h
 *
 * PURPOSE:
 *   Define transactional layout edit state for unlock, preview, commit and cancel workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_EDIT_SESSION_H
#define UMICOM_UI_WORKSTATION_LAYOUT_EDIT_SESSION_H

#include <stdbool.h>
#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws layout edit session data shared with callers of this public contract.
 */
typedef struct UmiWsLayoutEditSession {
    UmiWsLayoutTemplate original;
    UmiWsLayoutTemplate working;
    bool active;
    bool dirty;
} UmiWsLayoutEditSession;

/**
 * Provide the ws layout edit session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_edit_session_begin(UmiWsLayoutEditSession *session, const UmiWsLayoutTemplate *layout);
/**
 * Provide the ws layout edit session working operation used by this module and its client
 * applications.
 */
UmiWsLayoutTemplate *umi_ws_layout_edit_session_working(UmiWsLayoutEditSession *session);
/**
 * Provide the ws layout edit session mark dirty operation used by this module and its
 * client applications.
 */
void umi_ws_layout_edit_session_mark_dirty(UmiWsLayoutEditSession *session);
/**
 * Provide the ws layout edit session commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_edit_session_commit(UmiWsLayoutEditSession *session, UmiWsLayoutTemplate *out_layout);
/**
 * Provide the ws layout edit session cancel operation used by this module and its client
 * applications.
 */
void umi_ws_layout_edit_session_cancel(UmiWsLayoutEditSession *session);

#ifdef __cplusplus
}
#endif

#endif
