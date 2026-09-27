/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/native_gtk4.h
 *
 * PURPOSE:
 *   Render the checked native controls profile without application-local GTK logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_NATIVE_GTK4_H
#define UMICOM_DESIGNER_NATIVE_GTK4_H
#include "umicom/designer/native_project.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Call on the GTK owner thread after successful GTK initialisation. Output is
 * an owned reference: call gtk_window_destroy() and then g_object_unref().
 * The input document is neither retained nor mutated. All edits are temporary.
 * text controls accept 4,096 characters; editors accept 262,144 Unicode scalar
 * values. The explicit text.count action counts scalars, not grapheme clusters. */
UmiStatus UmiDesignerNativeGtkCreate(const UmiDeclDocument *document, GtkWindow **outWindow);
/* An owned reference for inspection/testing; NULL if removed or destroyed.
 * Caller must unref the returned widget. No callback is executed by lookup. */
GtkWidget *UmiDesignerNativeGtkRefControl(GtkWindow *window, const char *nodeId);
/* Standalone, blocking owner-thread loop. No file, shell or external command
 * is opened. Returns UNAVAILABLE without a display. Closing discards draft text. */
UmiStatus UmiDesignerNativeGtkRun(const UmiDeclDocument *document);

#ifdef __cplusplus
}
#endif
#endif
