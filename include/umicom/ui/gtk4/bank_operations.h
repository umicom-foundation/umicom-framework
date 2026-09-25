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
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating Framework-owned launch bar. The caller parents it into
 * an existing box without replacing that box's children. No database is opened
 * until the user presses the button. The operational window is transient for
 * the widget's actual root window and closes with that parent.
 *
 * The workspace is labelled local simulation, stores no card credentials,
 * and uses an absolute per-user Data Server path. All financial commands and
 * queries delegate to Umicom::bank_operations. GTK never computes balances. */
GtkWidget *UmiGtk4BankOperationsLauncherCreate(void);
#ifdef __cplusplus
}
#endif
#endif
