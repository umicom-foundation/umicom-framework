/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/qualification_review.h
 * Purpose: Present copied qualification evidence without borrowing worker-owned run state.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_QUALIFICATION_REVIEW_H
#define UMICOM_UI_GTK4_QUALIFICATION_REVIEW_H
#include <gtk/gtk.h>
#include "umicom/distribution/runtime/qualification_view.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating GTK box with a summary, persistent blockers and paged rows.
 * GTK owning thread only. No file/process/network work or publication actions.
 * Empty views say no record, never passed. The widget owns adopted snapshots
 * until replaced or finalized. Parent it or explicitly sink/unref its reference. */
GtkWidget *UmiGtk4QualificationReviewCreate(void);
/** Transfer *view to the widget and clear the caller's pointer on success.
 * Capture on the ledger owner, then marshal this call to the GTK thread.
 * A null *view clears the panel. Refusal preserves both panel and caller state.
 * New snapshots reset paging, retaining the selected filter. Names are plain
 * text, never markup or commands; long labels are visibly shortened for display.
 * Full names remain in the immutable view and exported qualification evidence. */
UmiStatus UmiGtk4QualificationReviewSetView(GtkWidget *widget, UmiQualificationView **view);
/** Copied render counts help hosts test and announce the visible scope.
 * This does not expose borrowed worker/model pointers or certify execution. */
UmiStatus UmiGtk4QualificationReviewReadPage(GtkWidget *widget,
    size_t *outOffset, size_t *outVisible, size_t *outTotal);
#ifdef __cplusplus
}
#endif
#endif
