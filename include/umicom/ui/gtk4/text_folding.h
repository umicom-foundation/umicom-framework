/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/text_folding.h
 * PURPOSE: Hide selected complete source lines without changing text, ownership or document history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_TEXT_FOLDING_H
#define UMICOM_UI_GTK4_TEXT_FOLDING_H
#include <gtk/gtk.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Collapse complete selected lines after the first selected line, which stays
 * visible as a header. A selection ending at the next line's start excludes
 * that line. At least two lines must remain in the selected range. The caret
 * moves to the header; source bytes, dirty state and text Undo are unchanged.
 *
 * Up to 64 explicit ranges use the existing Framework folding projection.
 * Repeating an identical range succeeds without adding another record.
 * Separate selections can produce several folds. Selecting into a hidden
 * range reveals existing folds first. All folds are revealed on a content
 * change or selection/caret navigation into hidden text. Read-only
 * views are supported. State belongs to the current GtkTextBuffer and is not
 * persisted. Use only on the GTK owner thread with a live GtkTextView.
 *
 * This service owns one private invisible-text tag. Hosts must include hidden
 * text when copying source for saves, edits, searches or provider requests.
 * It does not parse source or obtain folding ranges from a language server. */
    UmiStatus UmiGtk4TextFoldSelection(GtkTextView *view);
    /* Reveal this service's ranges only. Other syntax and annotation tags remain.
 * An untouched valid view is already clear and succeeds without allocating. */
    UmiStatus UmiGtk4TextFoldClear(GtkTextView *view);
    size_t UmiGtk4TextFoldCount(GtkTextView *view);
    /* Query a one-based source line against the bounded projection. Invalid view
 * or line zero returns false. This is source-line visibility, not visual wraps. */
    int UmiGtk4TextFoldLineHidden(GtkTextView *view, uint32_t line);
#ifdef __cplusplus
}
#endif
#endif
