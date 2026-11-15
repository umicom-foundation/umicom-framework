/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/output_gtk4.h
 *
 * PURPOSE:
 *   Adapt retained CodeGuard review evidence to the shared native output presenter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CODEGUARD_OUTPUT_GTK4_H
#define UMICOM_CODEGUARD_OUTPUT_GTK4_H
#include <gtk/gtk.h>
#include "umicom/codeguard/review.h"
#ifdef __cplusplus
extern "C" {
#endif
/* GTK owner-thread API. No service, task, timer or application owner is retained.
 * The floating widget reuses the shared Follow/Refresh/Copy output controls. */
GtkWidget *UmiCodeGuardOutputGtk4Create(void);
/* current_context is the host workspace identity, or zero when closed.
 * Supply the matching controller snapshot and its borrowed result only when
 * has_report is true. Keep both stable for this call. Report bytes are copied
 * before GTK notifications, never borrowed by the widget.
 *
 * A changed context or loss of its retained report clears accepted/displayed
 * text even when Follow is paused. Existing output selection is preserved for
 * unchanged evidence. The host must reject old asynchronous work before this
 * call; this presenter does not poll or cancel scans.
 *
 * OK means presentation was updated or already current, not a successful scan.
 * A report too large for the shared bounded view, or a report rendering error,
 * is shown as an explicit unavailable-preview status with empty text. The
 * service's full result is unchanged. Nested publication returns BUSY.
 * Passing different findings with an unchanged snapshot is outside the contract;
 * publish them through a new review request. */
UmiStatus UmiCodeGuardOutputGtk4Update(GtkWidget *panel, uint64_t current_context,
    const UmiCodeGuardReviewSnapshot *snapshot, const UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
