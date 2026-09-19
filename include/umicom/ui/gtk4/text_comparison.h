/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/text_comparison.h
 *
 * PURPOSE:
 *   Present an immutable, selectable pair of texts with shared diff navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_TEXT_COMPARISON_H
#define UMICOM_UI_GTK4_TEXT_COMPARISON_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Create a read-only GTK component on the GTK owner thread. Copies the two
 * terminated UTF-8 spans and labels before returning. It retains no caller
 * pointer, document or coordinator, and never saves or applies changes.
 * outWidget receives a floating GtkWidget reference, owned by the parent after
 * parenting (or by a caller that sinks/unrefs it). Out-of-alignment-limit texts
 * remain visible in full with navigation disabled and a visible explanation. */
UmiStatus UmiGtk4TextComparisonCreate(const char *left, size_t leftLength,
    const char *right, size_t rightLength, const char *leftLabel,
    const char *rightLabel, GtkWidget **outWidget);

#ifdef __cplusplus
}
#endif
#endif
