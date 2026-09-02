/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/session_view.h
 *
 * PURPOSE:
 *   Project current layout, lock and active-panel state without coupling to a UI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_SESSION_VIEW_H
#define UMICOM_APPLICATION_UI_SESSION_VIEW_H

#include "umicom/application/runtime/session.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application ui session row data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationUiSessionRow {
    const char *application_id;
    const char *layout_id;
    size_t active_panel_count;
    int layout_locked;
    uint64_t revision;
} UmiApplicationUiSessionRow;

/**
 * Provide the application ui session row operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_ui_session_row(
    const UmiApplicationSession *session,
    UmiApplicationUiSessionRow *out_row);

#ifdef __cplusplus
}
#endif

#endif
