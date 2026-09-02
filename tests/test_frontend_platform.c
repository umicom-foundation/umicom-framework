/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_platform.c
 *
 * PURPOSE:
 *   Exercise server-driven widget, signal and session contracts used by future
 *   web and remote frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>

#include "umicom/frontend/signal.h"
#include "umicom/frontend/web_session.h"
#include "umicom/frontend/widget_tree.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiFrontendWidgetRegistry *widgets = NULL;
    UmiFrontendWidgetSnapshot widget = {0};
    UmiFrontendSignalRegistry *signals = NULL;
    UmiFrontendSignalSnapshot signal = {0};
    UmiFrontendSessionRegistry *sessions = NULL;
    UmiFrontendSessionSnapshot session = {0};

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_frontend_widget_tree_registry_create(&widgets) != UMI_STATUS_OK) return 1;
    (void)strcpy(widget.id, "root");
    (void)strcpy(widget.type, "container");
    widget.visible = 1;
    widget.enabled = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_frontend_widget_tree_registry_upsert(widgets, &widget) != UMI_STATUS_OK) return 2;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_frontend_signal_registry_create(&signals) != UMI_STATUS_OK) return 3;
    (void)strcpy(signal.id, "click");
    (void)strcpy(signal.widget_id, "root");
    (void)strcpy(signal.signal_name, "clicked");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_frontend_signal_registry_upsert(signals, &signal) != UMI_STATUS_OK) return 4;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_frontend_web_session_registry_create(&sessions) != UMI_STATUS_OK) return 5;
    (void)strcpy(session.id, "session-1");
    (void)strcpy(session.transport, "websocket");
    session.connected = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_frontend_web_session_registry_upsert(sessions, &session) != UMI_STATUS_OK) return 6;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_frontend_web_session_registry_touch(sessions, "session-1", 99U, 0, 1) != UMI_STATUS_OK) return 7;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_frontend_web_session_registry_find(sessions, "session-1", &session) != UMI_STATUS_OK ||
        session.last_activity != 99U || session.connected || !session.suspended) return 8;

    umi_frontend_web_session_registry_destroy(sessions);
    umi_frontend_signal_registry_destroy(signals);
    umi_frontend_widget_tree_registry_destroy(widgets);
    return 0;
}
