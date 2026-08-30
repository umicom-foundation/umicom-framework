/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/trading_suite_workstation_gtk4.c
 *
 * PURPOSE:
 *   Own GTK4 trading-workstation composition, guarded UI mutations, canonical
 *   suite layout refresh and optional deterministic simulation animation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_ui/gtk4/trading_suite_workstation.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/trading_ui/gtk4/trading_panels.h"

struct UmiGtk4TradingSuiteWorkstation {
    UmiGtk4TradingSuiteWorkstationConfig config;
    UmiTradingUiController controller;
    UmiTradingSimulationMarket simulation;
    UmiGtk4TradingPanelContext panel_context;
    UmiApplicationSuiteGtk4Workstation *suite;
    guint pending_refresh;
    guint simulation_timer;
    int simulation_seeded;
};

static gboolean rebuild_idle(gpointer data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = data;
    UmiApplicationSuiteGtk4WorkstationSnapshot snapshot;
    if (workstation == NULL || workstation->suite == NULL)
        return G_SOURCE_REMOVE;
    workstation->pending_refresh = 0U;
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation->suite);
    if (snapshot.active_layout_id[0] != '\0')
        (void)umi_application_suite_gtk4_workstation_select_layout(
            workstation->suite, snapshot.active_layout_id);
    return G_SOURCE_REMOVE;
}

static void schedule_rebuild(UmiGtk4TradingSuiteWorkstation *workstation)
{
    if (workstation == NULL || workstation->pending_refresh != 0U) return;
    workstation->pending_refresh = g_idle_add(rebuild_idle, workstation);
}

static void on_controller_changed(uint64_t revision, void *user_data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = user_data;
    (void)revision;
    schedule_rebuild(workstation);
}

static gboolean simulation_tick(gpointer data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = data;
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status;
    if (workstation == NULL || !workstation->simulation_seeded)
        return G_SOURCE_CONTINUE;
    status = umi_trading_workspace_snapshot(
        workstation->config.workspace, &snapshot);
    if (status == UMI_STATUS_OK &&
        snapshot.environment == UMI_TRADING_SIMULATION) {
        status = umi_trading_simulation_market_step(
            &workstation->simulation,
            (int64_t)workstation->config.simulation_step_interval_ms);
        if (status == UMI_STATUS_OK) schedule_rebuild(workstation);
    }
    return G_SOURCE_CONTINUE;
}

static GtkWidget *trading_panel_factory(const UmiUiWorkspaceWindow *window,
                                        void *user_data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = user_data;
    if (workstation == NULL) return NULL;
    return umi_gtk4_trading_panel_create(window, &workstation->panel_context);
}

UmiGtk4TradingSuiteWorkstationConfig
umi_gtk4_trading_suite_workstation_config_default(
    UmiTradingWorkspace *workspace)
{
    UmiGtk4TradingSuiteWorkstationConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.workspace = workspace;
    config.application_id = "org.umicom.trader";
    config.title = "Umicom Trader";
    config.seed_simulation_market = 1;
    config.animate_simulation_market = 1;
    config.allow_live_environment = 0;
    config.simulation_step_interval_ms = 1000U;
    return config;
}

UmiStatus umi_gtk4_trading_suite_workstation_create(
    const UmiGtk4TradingSuiteWorkstationConfig *config,
    UmiGtk4TradingSuiteWorkstation **out_workstation)
{
    UmiGtk4TradingSuiteWorkstation *workstation;
    UmiTradingUiControllerConfig controller_config;
    UmiApplicationSuiteGtk4WorkstationConfig suite_config;
    UmiTradingWorkspaceSnapshot trading_snapshot;
    UmiStatus status;
    if (config == NULL || out_workstation == NULL ||
        config->workspace == NULL || config->application_id == NULL ||
        config->application_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_workstation = NULL;
    workstation = calloc(1U, sizeof(*workstation));
    if (workstation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workstation->config = *config;
    if (workstation->config.simulation_step_interval_ms < 100U)
        workstation->config.simulation_step_interval_ms = 1000U;

    controller_config = umi_trading_ui_controller_config_default();
    controller_config.allow_live_environment =
        workstation->config.allow_live_environment != 0;
    status = umi_trading_ui_controller_init(
        &workstation->controller,
        workstation->config.workspace,
        &controller_config);
    if (status != UMI_STATUS_OK) goto fail;
    umi_trading_ui_controller_set_changed_handler(
        &workstation->controller, on_controller_changed, workstation);

    status = umi_trading_simulation_market_init(
        &workstation->simulation, workstation->config.workspace);
    if (status != UMI_STATUS_OK) goto fail;
    if (workstation->config.seed_simulation_market) {
        status = umi_trading_workspace_snapshot(
            workstation->config.workspace, &trading_snapshot);
        if (status != UMI_STATUS_OK) goto fail;
        if (trading_snapshot.watchlist_count == 0U) {
            status = umi_trading_simulation_market_seed_default(
                &workstation->simulation,
                (int64_t)(g_get_real_time() / 1000));
            if (status != UMI_STATUS_OK) goto fail;
            workstation->simulation_seeded = 1;
        }
    }

    workstation->panel_context.workspace = workstation->config.workspace;
    workstation->panel_context.controller = &workstation->controller;
    workstation->panel_context.allow_live_environment =
        workstation->config.allow_live_environment != 0;

    (void)memset(&suite_config, 0, sizeof(suite_config));
    suite_config.application_id = workstation->config.application_id;
    suite_config.title = workstation->config.title;
    suite_config.panel_factory = trading_panel_factory;
    suite_config.user_data = workstation;
    status = umi_application_suite_gtk4_workstation_create(
        &suite_config, &workstation->suite);
    if (status != UMI_STATUS_OK) goto fail;

    if (workstation->config.animate_simulation_market &&
        workstation->simulation_seeded) {
        workstation->simulation_timer = g_timeout_add(
            workstation->config.simulation_step_interval_ms,
            simulation_tick,
            workstation);
    }
    *out_workstation = workstation;
    return UMI_STATUS_OK;

fail:
    umi_gtk4_trading_suite_workstation_destroy(workstation);
    return status;
}

void umi_gtk4_trading_suite_workstation_destroy(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    if (workstation == NULL) return;
    if (workstation->simulation_timer != 0U) {
        g_source_remove(workstation->simulation_timer);
        workstation->simulation_timer = 0U;
    }
    if (workstation->pending_refresh != 0U) {
        g_source_remove(workstation->pending_refresh);
        workstation->pending_refresh = 0U;
    }
    umi_trading_ui_controller_set_changed_handler(
        &workstation->controller, NULL, NULL);
    umi_application_suite_gtk4_workstation_destroy(workstation->suite);
    workstation->suite = NULL;
    free(workstation);
}

GtkWidget *umi_gtk4_trading_suite_workstation_widget(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    return workstation != NULL
        ? umi_application_suite_gtk4_workstation_widget(workstation->suite)
        : NULL;
}

UmiStatus umi_gtk4_trading_suite_workstation_select_layout(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *layout_id)
{
    if (workstation == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_select_layout(
        workstation->suite, layout_id);
}

UmiStatus umi_gtk4_trading_suite_workstation_refresh(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_trading_ui_controller_refresh(&workstation->controller);
}

UmiGtk4TradingSuiteWorkstationSnapshot
umi_gtk4_trading_suite_workstation_snapshot(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    UmiGtk4TradingSuiteWorkstationSnapshot snapshot;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    if (workstation == NULL) return snapshot;
    snapshot.layout = umi_application_suite_gtk4_workstation_snapshot(
        workstation->suite);
    (void)umi_trading_workspace_snapshot(
        workstation->config.workspace, &snapshot.trading);
    snapshot.controller = umi_trading_ui_controller_snapshot(
        &workstation->controller);
    snapshot.simulation_instrument_count =
        umi_trading_simulation_market_instrument_count(&workstation->simulation);
    snapshot.simulation_sequence =
        umi_trading_simulation_market_sequence(&workstation->simulation);
    snapshot.simulation_running = workstation->simulation_timer != 0U;
    return snapshot;
}

UmiTradingUiController *umi_gtk4_trading_suite_workstation_controller(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    return workstation != NULL ? &workstation->controller : NULL;
}
