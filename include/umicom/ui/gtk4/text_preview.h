/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/text_preview.h
 *
 * PURPOSE:
 *   Publish text into an existing owned read-only preview without retaining application callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_TEXT_PREVIEW_H
#define UMICOM_UI_GTK4_TEXT_PREVIEW_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_GTK4_TEXT_PREVIEW_MAX_BYTES (4U * 1024U * 1024U)
/** Replace a read-only preview buffer and its summary. Both widgets must be
 * rooted in the live owner window. Text is valid UTF-8, at most 4 MiB; summary
 * at most 4096 bytes. Empty strings are allowed. Inputs are copied before
 * observable changes, so a caller may pass current widget text.
 * Nested publication on this view returns BUSY. Retained references survive
 * synchronous observers; owner closure, reparenting, making the view editable,
 * replacing its buffer or changing published contents returns CANCELLED.
 * Refusal before publication leaves controls unchanged. Observer cancellation
 * can leave partially published controls; it does not overwrite observer changes
 * with an automatic rollback. Existing buffers remain valid for their holders.
 * No source document or browser history changes. Call on the GTK thread.
 * GTK constructors retain GLib allocation policy; input-copy allocation failure
 * returns OUT_OF_MEMORY before publication. */
UmiStatus UmiGtk4TextPreviewPublish(GtkWindow *owner, GtkLabel *summary,
    GtkTextView *view, const char *summary_text, const char *text);

/** Select the first case-insensitive match in a read-only preview, or clear
 * selection and move to the start for an empty query. Query is valid UTF-8 and
 * at most 1024 bytes. NOT_FOUND preserves the previous selection.
 * The owner must be live and the view rooted in it. Search and publication on
 * the same view share a guard; nested operations return BUSY. Selection and
 * scrolling can notify observers. Changes to owner, buffer, content or chosen
 * selection return CANCELLED without restoring stale iterators or overriding
 * observer changes. Copy query before callbacks; call on the GTK owner thread. */
UmiStatus UmiGtk4TextPreviewFind(GtkWindow *owner, GtkTextView *view, const char *query);

#ifdef __cplusplus
}
#endif
#endif
