/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/cash_plan_private.h
 * PURPOSE: Keep native plan fields and worker ownership private to the GTK adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK4_CASH_PLAN_PRIVATE_H
#define UMICOM_GTK4_CASH_PLAN_PRIVATE_H
#include "umicom/ui/gtk4/cash_plan.h"
#include "umicom/cash_planning/document.h"
#include "umicom/finance/decimal.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>
enum
{
    CASH_TITLE,
    CASH_CURRENCY,
    CASH_OPENING,
    CASH_BUFFER,
    CASH_START,
    CASH_END,
    CASH_ID,
    CASH_LABEL,
    CASH_DATE,
    CASH_AMOUNT,
    CASH_PATH,
    CASH_FIELDS
};
typedef struct CashPanel
{
    UmiCashPlan *plan, *previous, *pending;
    GtkWidget *fields[CASH_FIELDS], *scale, *receive, *enabled, *selector, *approval, *output, *note;
    GtkWidget *textExport; /* Retained with the other controls. */
    GtkWidget *pathPickers[3]; /* Hide on retirement to cancel pending native choosers. */
    GtkStringList *names;
    GPtrArray *retained;
    uint64_t generation;
    int updating, busy, configDirty, entryDirty;
    int retired; /* Logical host lifetime ends before retained widget finalisation. */
} CashPanel;
/* All helpers run on the GTK owner thread except the private file worker.
 * Signals hold a watched root, never a raw application runtime pointer. */
char *UmiCashPanelComparisonText(const UmiCashPlan *baseline, const UmiCashPlan *scenario);
void UmiCashPanelRender(CashPanel *panel);
void UmiCashPanelSync(CashPanel *panel);
void UmiCashPanelInvalidate(CashPanel *panel);
void UmiCashPanelFile(GtkButton *button, gpointer root);
void UmiCashPanelApplyLoaded(GtkButton *button, gpointer root);
#endif
