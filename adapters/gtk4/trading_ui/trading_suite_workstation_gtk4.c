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

/* Translate safety-critical environment state into a short readable badge.
 * The text is informative only; order permission still comes from policy. */
static const char *environment_badge(UmiTradingEnvironment environment)
{
    switch (environment) {
    case UMI_TRADING_SIMULATION:
        return "Simulation";
    case UMI_TRADING_PAPER:
        return "Paper";
    case UMI_TRADING_LIVE:
        return "Live";
    default:
        return "Unknown";
    }
}

/* Read the authoritative workspace environment and refresh only the shared
 * identity badge. No trading state is changed by this presentation update. */
static void refresh_environment_badge(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    UmiTradingWorkspaceSnapshot snapshot;

    if (workstation == NULL || workstation->suite == NULL ||
        workstation->config.workspace == NULL) {
        return;
    }
    if (umi_trading_workspace_snapshot(
            workstation->config.workspace, &snapshot) == UMI_STATUS_OK) {
        (void)umi_application_suite_gtk4_workstation_set_mode_badge(
            workstation->suite,
            environment_badge(snapshot.environment));
    }
}

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
    refresh_environment_badge(workstation);
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

    /* Read the environment again after optional simulation seeding so the
     * first rendered frame already carries the correct safety mode badge. */
    status = umi_trading_workspace_snapshot(
        workstation->config.workspace, &trading_snapshot);
    if (status != UMI_STATUS_OK) goto fail;
    (void)memset(&suite_config, 0, sizeof(suite_config));
    suite_config.application_id = workstation->config.application_id;
    suite_config.title = workstation->config.title;
    suite_config.mode_badge = environment_badge(
        trading_snapshot.environment);
    suite_config.panel_factory = trading_panel_factory;
    suite_config.user_data = workstation;
    status = umi_application_suite_gtk4_workstation_create(
        &suite_config, &workstation->suite);
    if (status != UMI_STATUS_OK) goto fail;
    refresh_environment_badge(workstation);

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

/* Keep presentation selection in the suite workstation and leave trading
 * data, orders and broker state unchanged. */
UmiStatus umi_gtk4_trading_suite_workstation_select_appearance(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *profile_id)
{
    if (workstation == NULL || profile_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_select_appearance(
        workstation->suite, profile_id);
}

/* Forward complete custom presentation values to the reusable editor. */
UmiStatus umi_gtk4_trading_suite_workstation_apply_custom_appearance(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const UmiUiAppearanceProfile *profile)
{
    if (workstation == NULL || profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_apply_custom_appearance(
        workstation->suite, profile);
}

/* Return appearance by value so trading clients never depend on GTK widgets. */
UmiStatus umi_gtk4_trading_suite_workstation_active_appearance(
    const UmiGtk4TradingSuiteWorkstation *workstation,
    UmiUiAppearanceProfile *out_profile)
{
    if (workstation == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_active_appearance(
        workstation->suite, out_profile);
}

UmiStatus umi_gtk4_trading_suite_workstation_begin_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_begin_layout_edit(
        workstation->suite);
}

UmiStatus umi_gtk4_trading_suite_workstation_commit_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_commit_layout_edit(
        workstation->suite);
}

UmiStatus umi_gtk4_trading_suite_workstation_cancel_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_cancel_layout_edit(
        workstation->suite);
}

/* Trading composition forwards export without learning the layout format. */
UmiStatus umi_gtk4_trading_suite_workstation_export_layout(
    const UmiGtk4TradingSuiteWorkstation *workstation,
    uint64_t saved_at_ns,
    char *out_text,
    size_t capacity)
{
    if (workstation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_export_layout(
        workstation->suite, saved_at_ns, out_text, capacity);
}

/* Trading composition forwards import and keeps validation in Framework UI. */
UmiStatus umi_gtk4_trading_suite_workstation_import_layout(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *text,
    int activate,
    UmiUiWorkspaceImportReport *out_report)
{
    if (workstation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_import_layout(
        workstation->suite, text, activate, out_report);
}

/* Store a recovery point for the trading workstation's current arrangement. */
UmiStatus umi_gtk4_trading_suite_workstation_save_checkpoint(
    UmiGtk4TradingSuiteWorkstation *workstation,
    uint64_t saved_at_ns)
{
    if (workstation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_save_checkpoint(
        workstation->suite, saved_at_ns);
}

/* Restore the trading workstation through the shared validated importer. */
UmiStatus umi_gtk4_trading_suite_workstation_restore_checkpoint(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    if (workstation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_restore_checkpoint(
        workstation->suite);
}

UmiStatus umi_gtk4_trading_suite_workstation_open_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *tool_id,
    const char *region_id,
    int floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_open_window(
        workstation->suite,
        tool_id,
        region_id,
        floating,
        opened_at_ms,
        out_window_id,
        out_window_id_capacity);
}

UmiStatus umi_gtk4_trading_suite_workstation_move_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *window_id,
    const char *region_id,
    double x,
    double y,
    double width,
    double height)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_move_window(
        workstation->suite,
        window_id,
        region_id,
        x,
        y,
        width,
        height);
}

UmiStatus umi_gtk4_trading_suite_workstation_close_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *window_id)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_close_window(
        workstation->suite, window_id);
}

/* Forward panel settings without introducing trading-specific layout rules. */
UmiStatus umi_gtk4_trading_suite_workstation_apply_panel_settings(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const UmiUiWorkspacePanelSettings *settings)
{
    if (workstation == NULL || settings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Trading UI code forwards the semantic request unchanged; all layout
     * policy, validation and rollback stay in the application suite layer. */
    return umi_application_suite_gtk4_workstation_apply_panel_settings(
        workstation->suite, settings);
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
