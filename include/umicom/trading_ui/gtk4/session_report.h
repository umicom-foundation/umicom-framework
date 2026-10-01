/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/gtk4/session_report.h
 * PURPOSE: Provide reusable native review and export controls for a captured trading session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TRADING_UI_GTK4_SESSION_REPORT_H
#define UMICOM_TRADING_UI_GTK4_SESSION_REPORT_H
#include <gtk/gtk.h>
#include "umicom/trading/session_report.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Workspace is borrowed on its owning thread. Call Detach before it dies,
 * including when another owner retains this widget. No trading controller or
 * broker capability is needed by these read-only actions. */
GtkWidget *UmiGtk4TradingSessionReportCreate(UmiTradingWorkspace *workspace);
void UmiGtk4TradingSessionReportMarkStale(GtkWidget *panel);
void UmiGtk4TradingSessionReportDetach(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
