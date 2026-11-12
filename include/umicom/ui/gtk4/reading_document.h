/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/reading_document.h
 *
 * PURPOSE:
 *   Project immutable reading ranges into GTK using shared appearance typography.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_READING_DOCUMENT_H
#define UMICOM_UI_GTK4_READING_DOCUMENT_H
#include <gtk/gtk.h>
#include "umicom/browser/reading_document.h"
#include "umicom/ui/appearance.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Called on the GTK thread before and after observable mutations. Return zero
 * when the host has retired. Context must remain valid until the call returns;
 * the callback must not release that context or the caller's buffer reference.
 * It may close its window. All model ranges and typography have been captured
 * before the first callback, so their original owners may then be released. */
typedef int (*UmiReadingDocumentGtk4Continue)(void *context);

/* Style an existing buffer whose complete text exactly matches the immutable
 * model. This never edits text, selection, cursor or user font preferences.
 * Fonts and sizes come from the validated profile; zoom accepts 50..200 percent.
 * Reserved tag names start with "umicom-reading-". Existing unrelated tags are
 * retained. Links are only underlined; this API never activates or opens them.
 *
 * Admission rejects invalid UTF-8, non-character range boundaries and invalid
 * metadata before any GTK mutation. Calls on the same buffer cannot nest.
 * If a notification edits its text or removes a tag from its table, INVALID_STATE is
 * returned; an owner-continuation refusal returns CANCELLED. Such interruption
 * may leave partial styling. The host should discard a private candidate buffer
 * or keep its existing text and retry. This is not a cross-thread lock.
 * Signal observers must still obey GTK iterator rules: an apply/remove handler
 * before the default action must not invalidate its range iterators.
 * Borrowed inputs must remain valid until captured; output font availability
 * depends on the installed Pango font environment and requires native review. */
UmiStatus UmiReadingDocumentGtk4Style(const UmiBrowserReadingDocument *document,
    GtkTextBuffer *buffer, const UmiUiAppearanceProfile *appearance,
    unsigned int zoom_percent, UmiReadingDocumentGtk4Continue keep_going,
    void *context);

/* Create and completely style a private, independently owned buffer. On failure
 * *out_buffer remains unchanged. On success the caller owns one GObject ref.
 * A replacement is published by the host only after checking its own lifetime. */
UmiStatus UmiReadingDocumentGtk4CreateBuffer(const UmiBrowserReadingDocument *document,
    const UmiUiAppearanceProfile *appearance, unsigned int zoom_percent,
    GtkTextBuffer **out_buffer);
#ifdef __cplusplus
}
#endif
#endif
