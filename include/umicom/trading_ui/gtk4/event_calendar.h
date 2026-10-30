/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/gtk4/event_calendar.h
 *
 * PURPOSE:
 *   Expose a reusable native economic and earnings review with no broker mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_UI_GTK4_EVENT_CALENDAR_H
#define UMICOM_TRADING_UI_GTK4_EVENT_CALENDAR_H
#include <gtk/gtk.h>
#include "umicom/trading/event_calendar.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Floating widget. Its immutable snapshot and filters belong to the widget;
 * it borrows no workspace or broker. GTK owning thread only. */
GtkWidget *UmiGtk4EventCalendarCreate(void);
/* Atomic programmatic/provider import, using the same parser as the chooser.
 * Failure retains the previous snapshot and rows. Busy while a chooser/read
 * is pending. This API neither fetches a feed nor attests provider accuracy. */
UmiStatus UmiGtk4EventCalendarLoadCsv(GtkWidget *panel, const void *bytes, size_t length);
/* Start a bounded local-file read without opening a chooser. OK means queued,
 * not imported. The completed import or failure is reported in the panel.
 * The file is retained until completion. Nonlocal URIs are refused; no network
 * feed is fetched. Caller and completion both use the GTK owning context. */
UmiStatus UmiGtk4EventCalendarReadFile(GtkWidget *panel, GFile *file);
/* Cancel publication of the current read immediately. Late worker completion
 * cannot replace a newer snapshot. The worker may still be releasing its I/O. */
void UmiGtk4EventCalendarCancelRead(GtkWidget *panel);
bool UmiGtk4EventCalendarIsReading(GtkWidget *panel);
/* Cancel pending reads, retire controls and preserve readable event evidence.
 * A retained widget cannot import after its product workspace has closed. */
void UmiGtk4EventCalendarDetach(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
