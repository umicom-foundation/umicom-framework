/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/accounting_report.h
 *
 * PURPOSE:
 *   Display and export one immutable accounting CSV capture through the shared native host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_GTK4_ACCOUNTING_REPORT_H
#define UMICOM_UI_GTK4_ACCOUNTING_REPORT_H
#include <gtk/gtk.h>
#include "umicom/open_tms/accounting_book.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Call on the GTK owner thread. The book is borrowed only for construction.
 * On success the window owns a complete report capture; later book edits do not
 * change it. No authority or exact-money ledger state is created by a report.
 * Parent is NULL or a registered, live GtkWindow. GTK owns a successful window;
 * callers present/destroy it and take a reference if retaining it after destroy.
 * outWindow must not alias the inputs and is set to NULL on failure.
 * Parent closure retires further export; an accepted write may finish. */
UmiStatus UmiGtk4AccountingReportCreate(GtkWindow *parent,
    const UmiOpenTmsAccountingBook *book, GtkWindow **outWindow);
#ifdef __cplusplus
}
#endif
#endif
