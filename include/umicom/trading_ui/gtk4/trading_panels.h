/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/gtk4/trading_panels.h
 *
 * PURPOSE:
 *   Render interactive GTK4 trading panels over the toolkit-neutral trading
 *   workspace and guarded UI controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_GTK4_TRADING_PANELS_H
#define UMICOM_TRADING_UI_GTK4_TRADING_PANELS_H

#include <gtk/gtk.h>

#include "umicom/trading_ui/action_controller.h"
#include "umicom/ui/workspace_layout.h"
#include "umicom/trading/chart_persistence.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the gtk4 trading panel context data shared with callers of this public
 * contract.
 */
typedef struct UmiGtk4TradingPanelContext {
    UmiTradingWorkspace *workspace;
    UmiTradingUiController *controller;
    int allow_live_environment;
} UmiGtk4TradingPanelContext;

/**
 * Initialise gtk4 trading panel from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_trading_panel_create(
    const UmiUiWorkspaceWindow *window,
    UmiGtk4TradingPanelContext *context);

/* Refresh one provider body without rebuilding its layout, titlebar or
 * sibling chart. Market ticks retain focused edits; explicit actions refresh
 * the authoritative ticket/selection even when focus has not moved. Chart
 * gestures stay alive while the shared renderer receives new market data. */
UmiStatus UmiGtk4TradingPanelRefresh(GtkWidget *panel, int explicit_action);
/* Bind only chart bodies; other trading panel types are unchanged. */
void UmiGtk4TradingPanelBindChartPersistence(GtkWidget *panel, UmiTradingChartPersistence *service);
/* Disconnect a chart body before the suite's context or storage owner dies. */
void UmiGtk4TradingPanelDetachChart(GtkWidget *panel);


#ifdef __cplusplus
}
#endif
#endif
