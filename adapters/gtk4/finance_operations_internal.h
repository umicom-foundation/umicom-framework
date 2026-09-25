/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/finance_operations_internal.h
 *
 * PURPOSE:
 *   Keep GTK widget ownership separate from the shared financial state and its command validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_OPERATIONS_GTK4_INTERNAL_H
#define UMICOM_FINANCE_OPERATIONS_GTK4_INTERNAL_H
#include "umicom/ui/gtk4/finance_operations.h"
#include "umicom/finance_operations/input.h"
#define FINANCE_PANEL_KEY "umicom-finance-operations-panel"
typedef enum FinanceField {
    FINANCE_FIELD_REQUEST, FINANCE_FIELD_ACTOR, FINANCE_FIELD_ID,
    FINANCE_FIELD_NAME, FINANCE_FIELD_CURRENCY, FINANCE_FIELD_SCALE,
    FINANCE_FIELD_CLASS, FINANCE_FIELD_REFERENCE, FINANCE_FIELD_PERIOD,
    FINANCE_FIELD_DATE, FINANCE_FIELD_END_DATE, FINANCE_FIELD_ACCOUNT,
    FINANCE_FIELD_PARTICIPANT, FINANCE_FIELD_INSTRUMENT, FINANCE_FIELD_SIDE,
    FINANCE_FIELD_PRICE, FINANCE_FIELD_LOTS, FINANCE_FIELD_TICK,
    FINANCE_FIELD_UNITS, FINANCE_FIELD_MAX_LOTS, FINANCE_FIELD_AMOUNT,
    FINANCE_FIELD_OPEN, FINANCE_FIELD_LINES, FINANCE_FIELD_COUNT
} FinanceField;
struct UmiFinanceOperationsGtkPanel {
    UmiFinanceOperations *operations; /* Borrowed, single GTK-thread owner. */
    GtkWidget *root;
    GtkWidget *kind, *hint, *status;
    GtkWidget *fields[FINANCE_FIELD_COUNT], *rows[FINANCE_FIELD_COUNT];
    GtkWidget *view, *report, *reportPeriod, *reportCurrency, *reportScale, *exportPath;
    uint64_t mask;
};
void FinanceGtkRefresh(UmiFinanceOperationsGtkPanel *panel);
UmiFinanceOperationsGtkPanel *FinanceGtkPanelFromRoot(gpointer root);
#endif
