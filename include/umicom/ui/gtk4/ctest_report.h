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
#include "umicom/platform/path.h"
#include "umicom/testing/ctest_report_file.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Floating native panel. File reads and parsing run on a worker after explicit
 * Open report. The path and filter are captured together at admission. Invalid
 * filters, failed reads and cancelled replacements retain the accepted report,
 * page and source label. Controls edited during a load apply only after another
 * explicit action. Only mapped controls accept actions. Closing/unmapping the
 * panel prevents late publication. The panel owns report and view lifetimes;
 * its host must not destroy individual controls. No tests or tools execute. */
/* Cancellation boundary: closing/unmapping before admission prevents late
 * publication. Once text is accepted, its source/status labels finish updating
 * even if an observer hides the host from that text notification. Cancel load
 * cannot undo an already accepted page. The panel retains owned widgets through
 * completion; callers must not replace or destroy its individual controls. */
GtkWidget *UmiGtk4CtestReportCreate(void);
/* GTK-thread observation; cancellation retires publication but an in-progress
 * local file read may still be releasing resources while this returns true. */
gboolean UmiGtk4CtestReportBusy(GtkWidget *panel);

/* An owned copy of the accepted report's identity and complete outcome counts.
 * Filters and paging never change these whole-report observations. The path is
 * the captured input name, not proof that the file still has the same content. */
typedef struct UmiCtestReportEvidence {
    char path[UMI_PATH_CAPACITY];
    UmiCtestReportFileIdentity identity;
    UmiCtestReportSummary summary;
} UmiCtestReportEvidence;
/* GTK owner thread only. No I/O, report mutation or callbacks. Read the accepted
 * immutable state after Busy becomes false. BUSY during observable publication,
 * NOT_FOUND before successful admission or after Close report. Failure leaves
 * out unchanged. Copies remain valid after the panel or file is closed.
 * This identifies parsed saved bytes, not a source revision, authenticated
 * author, current run, process exit or required test coverage. */
UmiStatus UmiGtk4CtestReportReadEvidence(GtkWidget *panel, UmiCtestReportEvidence *out);

#ifdef __cplusplus
}
#endif
#endif
