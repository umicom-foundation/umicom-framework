/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/indentation_preferences.h
 *
 * PURPOSE:
 *   Compose reusable native indentation controls without owning application settings or
 * persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_INDENTATION_PREFERENCES_H
#define UMICOM_UI_GTK4_INDENTATION_PREFERENCES_H
#include "umicom/editor/indentation.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Return a floating GtkBox. Parent or ref-sink it normally. The widget owns
 * its controls; the caller must not replace, remove or reparent them.
 * No document, settings store, application callbacks or window is retained.
 * All operations run on the GTK owner thread. Failure clears out_widget. */
UmiStatus UmiGtk4IndentationPreferencesCreate(const UmiEditorIndentation *initial,
                                              GtkWidget **out_widget);
/** Read the actual width entry, not a stale spin-button numeric value.
 * Reject invalid or incomplete input without clamping or changing the draft.
 * Failure leaves out_options untouched. No signal-emitting updates occur. */
UmiStatus UmiGtk4IndentationPreferencesRead(GtkWidget *widget, UmiEditorIndentation *out_options);
/** Borrow controls while the owning widget remains alive. */
GtkWidget *UmiGtk4IndentationPreferencesWidth(GtkWidget *widget);
GtkWidget *UmiGtk4IndentationPreferencesSpaces(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
