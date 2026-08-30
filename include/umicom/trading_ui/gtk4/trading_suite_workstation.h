/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/gtk4/trading_suite_workstation.h
 *
 * PURPOSE:
 *   Compose a complete GTK4 professional trading workstation from canonical
 *   Application Suite layouts, guarded trading actions and simulation market
 *   data without moving reusable behaviour into a product repository.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_GTK4_TRADING_SUITE_WORKSTATION_H
#define UMICOM_TRADING_UI_GTK4_TRADING_SUITE_WORKSTATION_H

#include <gtk/gtk.h>

#include "umicom/application/suite_layout/gtk4_workstation.h"
#include "umicom/trading_ui/action_controller.h"
#include "umicom/trading_ui/simulation_market.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiGtk4TradingSuiteWorkstationConfig {
    UmiTradingWorkspace *workspace;
    const char *application_id;
    const char *title;
    int seed_simulation_market;
    int animate_simulation_market;
    int allow_live_environment;
    unsigned int simulation_step_interval_ms;
} UmiGtk4TradingSuiteWorkstationConfig;

typedef struct UmiGtk4TradingSuiteWorkstationSnapshot {
    UmiApplicationSuiteGtk4WorkstationSnapshot layout;
    UmiTradingWorkspaceSnapshot trading;
    UmiTradingUiControllerSnapshot controller;
    size_t simulation_instrument_count;
    uint64_t simulation_sequence;
    int simulation_running;
} UmiGtk4TradingSuiteWorkstationSnapshot;

typedef struct UmiGtk4TradingSuiteWorkstation UmiGtk4TradingSuiteWorkstation;

UmiGtk4TradingSuiteWorkstationConfig
umi_gtk4_trading_suite_workstation_config_default(
    UmiTradingWorkspace *workspace);
UmiStatus umi_gtk4_trading_suite_workstation_create(
    const UmiGtk4TradingSuiteWorkstationConfig *config,
    UmiGtk4TradingSuiteWorkstation **out_workstation);
void umi_gtk4_trading_suite_workstation_destroy(
    UmiGtk4TradingSuiteWorkstation *workstation);
GtkWidget *umi_gtk4_trading_suite_workstation_widget(
    UmiGtk4TradingSuiteWorkstation *workstation);
UmiStatus umi_gtk4_trading_suite_workstation_select_layout(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *layout_id);
UmiStatus umi_gtk4_trading_suite_workstation_refresh(
    UmiGtk4TradingSuiteWorkstation *workstation);
UmiGtk4TradingSuiteWorkstationSnapshot
umi_gtk4_trading_suite_workstation_snapshot(
    UmiGtk4TradingSuiteWorkstation *workstation);
UmiTradingUiController *umi_gtk4_trading_suite_workstation_controller(
    UmiGtk4TradingSuiteWorkstation *workstation);

#ifdef __cplusplus
}
#endif
#endif
