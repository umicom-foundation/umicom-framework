/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/gtk4/interactive_chart.h
 * PURPOSE: Expose the shared native trading chart with drawing and ticket gestures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_UI_GTK4_INTERACTIVE_CHART_H
#define UMICOM_TRADING_UI_GTK4_INTERACTIVE_CHART_H
#include "umicom/trading_ui/gtk4/trading_panels.h"
#include "umicom/trading/chart_persistence.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Context and its workspace outlive the returned root widget. GTK main thread
 * only. This chart creates no connections, providers or live order transport. */
GtkWidget *UmiGtk4TradingInteractiveChartCreate(UmiGtk4TradingPanelContext *context);
/* Update copied scene evidence while retaining the active native gesture. */
void UmiGtk4TradingInteractiveChartRefresh(GtkWidget *chart);
/* Borrow the service until detached or the chart root dies. Binding never
 * reads storage or changes chart state. A NULL service disables persistence. */
void UmiGtk4TradingInteractiveChartBindPersistence(GtkWidget *chart, UmiTradingChartPersistence *service);
/* Stop timer and disconnect model/storage access before either owner dies.
 * Retained widgets continue to own a read-only copied scene. Idempotent. */
void UmiGtk4TradingInteractiveChartDetach(GtkWidget *chart);

#ifdef __cplusplus
}
#endif
#endif
