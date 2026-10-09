/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/filtered_choices.h
 * PURPOSE: Share explicit text filtering with stable source selection across native applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_FILTERED_CHOICES_H
#define UMICOM_UI_GTK4_FILTERED_CHOICES_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Return a floating controls box for an existing dropdown. The box retains
 * the dropdown; the caller owns its placement. Use one controls box per picker
 * and do not mutate its model directly. All calls run on the GTK thread.
 * Controls offer literal query, ASCII case matching and original/A-Z/Z-A order.
 * Apply filter is explicit. No provider, filesystem or application action runs.
 * Automation IDs under the returned root: choices.query/order/case/apply/count.
 * Listen to the picker's notify::selected; a final notification follows complete
 * publication. SelectedSource returns BUSY during intermediate notifications.
 * Application actions must still check their own lifetime and data freshness. */
    GtkWidget *UmiGtk4FilteredChoicesCreate(GtkDropDown *picker);
    /** Copy a UTF-8 snapshot, replace the visible list and clear selection. Empty
 * input clears the snapshot. Invalid data retains the previous one. The active
 * filter/order also applies to new rows. No selection crosses snapshot boundaries.
 * A nonempty native model has an additional prompt after its data rows.
 * Use SelectedSource to distinguish that prompt from a business choice; never
 * treat the raw GtkDropDown index or its model count as a source-row identity.
 * This synchronous operation is bounded by UmiUiTextProjection limits. */
    UmiStatus UmiGtk4FilteredChoicesSetRows(GtkWidget *controls, const char *const *rows, size_t count);
    /** Return the original snapshot index, never the displayed row number.
 * A hidden/unselected row returns NOT_FOUND; outputs remain unchanged on error.
 * External replacement of the dropdown model returns INVALID_STATE. */
    UmiStatus UmiGtk4FilteredChoicesSelectedSource(GtkWidget *controls, size_t *out_source);
#ifdef __cplusplus
}
#endif
#endif
