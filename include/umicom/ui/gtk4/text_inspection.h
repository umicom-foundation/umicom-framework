/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/text_inspection.h
 *
 * PURPOSE:
 *   Expose a selectable read-only inspection component without retaining input or editing
 * authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_TEXT_INSPECTION_H
#define UMICOM_UI_GTK4_TEXT_INSPECTION_H
#include "umicom/editor/text_inspection.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Build an immutable summary and invisible-character location list on the GTK
 * thread. Copies presentation values and never retains inspection. Counts use
 * the exact semantics in editor/text_inspection.h; notices are review cues,
 * not a security certification. No source text, path, file or network is read.
 * The output must address NULL. Success returns a floating widget for parenting
 * or ref-sinking by the caller. No callbacks, navigation or mutations are made. */
UmiStatus UmiGtk4TextInspectionCreate(const UmiEditorTextInspection *inspection, GtkWidget **out);
#ifdef __cplusplus
}
#endif
#endif
