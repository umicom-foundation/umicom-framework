/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/desk_federation.h
 *
 * PURPOSE:
 *   Add workspace review controls without replacing Desk or its context strip.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_DESK_FEDERATION_H
#define UMICOM_UI_GTK4_DESK_FEDERATION_H
#include <gtk/gtk.h>
#include "umicom/desktop/federation/runtime_bridge.h"
#ifdef __cplusplus
extern "C" {
#endif
/* GTK owning thread only. root is an existing vertical GtkBox. The runtime
 * is borrowed. Attach is inert: no storage, timers, child processes or extra
 * toplevel windows. A collapsed expander preserves space for the existing
 * Desk launcher, taskbar, layouts and context controls.
 * Call Invalidate before destroying the runtime, including partial startup.
 * root owns the UI state; an explicitly retained root becomes inert after
 * invalidation and may be released later without touching the old runtime. */
UmiStatus UmiDeskFederationGtkAttach(GtkWidget *root, UmiDeskRuntime *runtime);
void UmiDeskFederationGtkInvalidate(GtkWidget *root);
/* Explicit storage action. NULL uses the user-data directory, never CWD.
 * A non-NULL test/deployment path must be absolute. No silent memory fallback.
 * An already opened service is reloaded rather than switching databases. */
UmiStatus UmiDeskFederationGtkOpenStorage(GtkWidget *root, const char *absolutePath);
/* Called by the host's existing poll, not a separately owned recurring task. */
UmiStatus UmiDeskFederationGtkPoll(GtkWidget *root);
#ifdef __cplusplus
}
#endif
#endif
