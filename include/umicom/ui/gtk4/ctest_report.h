/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/ctest_report.h
 *
 * PURPOSE:
 *   Present saved CTest observations without replacing live test state or executing tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_GTK4_CTEST_REPORT_H
#define UMICOM_UI_GTK4_CTEST_REPORT_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Floating native panel. File reads and parsing run on a worker after explicit
 * Open report. Only mapped controls accept actions. Closing/unmapping the
 * panel prevents late publication. The panel owns report and view lifetimes;
 * its host must not destroy individual controls. No tests or tools execute. */
GtkWidget *UmiGtk4CtestReportCreate(void);
/* GTK-thread observation; cancellation retires publication but an in-progress
 * local file read may still be releasing resources while this returns true. */
gboolean UmiGtk4CtestReportBusy(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
