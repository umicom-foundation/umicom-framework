/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/bank_operations.h
 *
 * PURPOSE:
 *   Expose an additive banking operations launcher for existing GTK4 workstations.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_BANK_OPERATIONS_H
#define UMICOM_UI_GTK4_BANK_OPERATIONS_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating Framework-owned launch bar. The caller parents it into
 * an existing box without replacing that box's children. No database is opened
 * until the user selects an absolute local filename and presses Banking operations.
 * Browse existing and Choose filename only update the entry. No directory is
 * created automatically, and the selected location is not remembered on disk. The operational window is transient for
 * the widget's actual root window and closes with that parent.
 *
 * The workspace is labelled local simulation, stores no card credentials,
 * and delegates the selected absolute filename to Data Server. All financial commands and
 * queries delegate to Umicom::bank_operations. GTK never computes balances. */
GtkWidget *UmiGtk4BankOperationsLauncherCreate(void);

/** Seed the path field from an explicit project choice, on the GTK thread.
 * A valid UTF-8 absolute local filename or an empty string (clear) is accepted.
 * Invalid input leaves the entry unchanged. No database is opened or created.
 * Changing the field while a workspace is open affects the next open only. */
UmiStatus UmiGtk4BankOperationsLauncherSetPath(GtkWidget *launcher, const char *absolutePath);
#ifdef __cplusplus
}
#endif
#endif
