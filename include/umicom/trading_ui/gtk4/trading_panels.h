/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/gtk4/trading_panels.h
 *
 * PURPOSE:
 *   Render interactive GTK4 trading panels over the toolkit-neutral trading
 *   workspace and guarded UI controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_GTK4_TRADING_PANELS_H
#define UMICOM_TRADING_UI_GTK4_TRADING_PANELS_H

#include <gtk/gtk.h>

#include "umicom/trading_ui/action_controller.h"
#include "umicom/ui/workspace_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiGtk4TradingPanelContext {
    UmiTradingWorkspace *workspace;
    UmiTradingUiController *controller;
    int allow_live_environment;
} UmiGtk4TradingPanelContext;

GtkWidget *umi_gtk4_trading_panel_create(
    const UmiUiWorkspaceWindow *window,
    UmiGtk4TradingPanelContext *context);

#ifdef __cplusplus
}
#endif
#endif
