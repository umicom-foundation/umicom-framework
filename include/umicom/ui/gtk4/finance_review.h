/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/finance_review.h
 * PURPOSE:
 *   Create a reusable manual funds-review component for banking and treasury workstations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_FINANCE_REVIEW_H
#define UMICOM_UI_GTK4_FINANCE_REVIEW_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiGtk4FinanceReviewKind {
    UMI_GTK4_FINANCE_REVIEW_ACCOUNT = 1,
    UMI_GTK4_FINANCE_REVIEW_CASH = 2
} UmiGtk4FinanceReviewKind;

/** Create a collapsed, keyboard-accessible manual review panel.
 * The returned widget has GTK's initial floating reference. Adopt it in one
 * container or ref_sink/unref it if used without a parent. It owns its field
 * values and callbacks; no application or financial service pointer is retained.
 * Account review: ledger, reserved, proposed debit. Cash review: opening,
 * inflows, outflows, buffer. Currency and decimal places are explicit.
 * Calculate calls funds_review.h; editing inputs invalidates prior results.
 * No account, payment, trade, filesystem, network or clipboard access occurs.
 * See Bank/TMS src/gtk/workstation.c for complete host composition. */
UmiStatus UmiGtk4FinanceReviewCreate(UmiGtk4FinanceReviewKind kind,
    GtkWidget **outWidget);
#ifdef __cplusplus
}
#endif
#endif
