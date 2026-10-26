/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/forward_practice.h
 *
 * PURPOSE:
 *   Expose an independent local treasury capture and review window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_FORWARD_PRACTICE_H
#define UMICOM_UI_GTK4_FORWARD_PRACTICE_H

#include <gtk/gtk.h>
#include <stdbool.h>
#include "umicom/open_tms/fx_forward_book.h"
#ifdef __cplusplus
extern "C" {
#endif
/* The original no-file window contract is retained for API history. Explicit
 * file actions now own captured snapshots; construction itself still performs no I/O.
 * Former contract: GTK owner thread only. Create an unpresented independent local practice
 * window with an empty FX-forward book. No accounts, credentials, providers,
 * cash movements or files are accessed. Data lives only until the window closes.
 * Its parent is transient; removal of either host retires retained controls.
 * NULL parent is allowed. Present or destroy the returned GTK-owned window.  */
/* GTK owner thread only. Create an unpresented independent empty practice book.
 * Save explicitly to a new absolute path; Open prepares a review, never silently
 * replaces records. Closing discards unsaved changes. Accepted worker writes
 * finish from an owned snapshot; late loads cannot affect a retired host.
 * No credentials, providers, cash movements or external settlements are used.
 * Its parent is transient; removal of either host retires retained controls.
 * NULL parent is allowed. Present or destroy the returned GTK-owned window. */
GtkWindow *UmiGtk4ForwardPracticeCreate(GtkWindow *parent);
/* Copy current local records while the window is live. Caller supplies a full
 * book, normally allocated on the heap. Failure leaves output unchanged.
 * A local Confirmed/Settled state is not external approval/settlement evidence. */
UmiStatus UmiGtk4ForwardPracticeRead(GtkWindow *window, UmiOpenTmsFxForwardBook *outBook);
/* Read worker/replacement availability on the GTK owner thread. Both distinct
 * outputs remain unchanged on failure. Ready is not approval to apply. */
UmiStatus UmiGtk4ForwardPracticeFileState(GtkWindow *window, bool *outRunning, bool *outReady);
/* Floating launcher button opens a new independent practice window.
 * A retained launcher without a live parent window is inert. */
GtkWidget *UmiGtk4ForwardPracticeLauncherCreate(void);
#ifdef __cplusplus
}
#endif
#endif
