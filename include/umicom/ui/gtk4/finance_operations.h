/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/finance_operations.h
 *
 * PURPOSE:
 *   Compose a reusable GTK4 financial operations panel and the existing branded product application host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_FINANCE_OPERATIONS_H
#define UMICOM_UI_GTK4_FINANCE_OPERATIONS_H
#include <gtk/gtk.h>
#include "umicom/finance_operations/operations.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinanceOperationsGtkPanel UmiFinanceOperationsGtkPanel;
/** The panel borrows operations, which must outlive it. Construction does not
 * post sample records or create storage; the owner chooses the Data Server.
 * description is copied and should clearly identify the practice storage. */
UmiStatus UmiFinanceOperationsGtkPanelCreate(UmiFinanceOperations *operations,
    const char *description, UmiFinanceOperationsGtkPanel **outPanel);
/** Borrow the panel's root widget for an existing window or container. */
GtkWidget *UmiFinanceOperationsGtkPanelWidget(UmiFinanceOperationsGtkPanel *panel);
/** Invalidate callbacks before releasing the panel's own widget reference.
 * Retained widgets become inert. The caller still owns operations. */
void UmiFinanceOperationsGtkPanelDestroy(UmiFinanceOperationsGtkPanel *panel);
/** Run with the canonical Framework splash, titlebar, logo/icon, window fit,
 * checkpointed layouts and closing recovery. Accountant and Exchange share
 * a user-local SQLite practice book, never the Bank application's database.
 * UMICOM_FINANCE_OPERATIONS_DB may select an explicit absolute SQLite path.
 * Database failures stay visible and never switch silently to memory. */
int UmiFinanceOperationsGtkRun(const char *applicationId, const char *title,
    int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
