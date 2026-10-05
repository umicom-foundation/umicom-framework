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
#include "umicom/ui/gtk4/workstation/workspace_storage.h"

struct UmiGtk4TradingSuiteWorkstation {
    UmiGtk4TradingSuiteWorkstationConfig config;
    UmiTradingUiController controller;
    UmiTradingSimulationMarket simulation;
    UmiGtk4TradingPanelContext panel_context;
    UmiTradingChartPersistence *chart_persistence;
    UmiDataServer *owned_chart_server;
    UmiApplicationSuiteGtk4Workstation *suite;
    GPtrArray *panel_observers;
    guint pending_refresh;
    guint simulation_timer;
    int simulation_seeded;
    int refresh_from_command;
};

static UmiStatus TradingSuiteEnableStorage(UmiGtk4TradingSuiteWorkstation *workstation,
    const char *profile, int restore_saved);

/* Translate safety-critical environment state into a short readable badge.
 * The text is informative only; order permission still comes from policy. */
static const char *environment_badge(UmiTradingEnvironment environment)
{
    /* Select the behaviour associated with the requested command or state value. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->suite == NULL ||
        workstation->config.workspace == NULL) {
        return;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_trading_workspace_snapshot(
            workstation->config.workspace, &snapshot) == UMI_STATUS_OK) {
        (void)umi_application_suite_gtk4_workstation_set_mode_badge(
            workstation->suite,
            environment_badge(snapshot.environment));
    }
}

/* Provide the rebuild idle operation used by this module and its client applications. */
/* Data notifications no longer reselect a layout. Stable provider mounts update chart scenes and surrounding panels independently, avoiding interrupted gestures and delayed order monitoring. The superseded implementation is retained for engineering review. */
#if 0
static gboolean rebuild_idle(gpointer data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = data;
    UmiApplicationSuiteGtk4WorkstationSnapshot snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->suite == NULL)
        return G_SOURCE_REMOVE;
    workstation->pending_refresh = 0U;
    int command_refresh = workstation->refresh_from_command;
    workstation->refresh_from_command = 0;
    /* Keep a typed order search intact while the market continues ingesting
     * quotes. Enter/Apply releases focus and schedules the canonical redraw;
     * otherwise the next normal refresh after focus leaves updates the panel. */
    GtkWidget *host = umi_application_suite_gtk4_workstation_widget(workstation->suite);
    if (!command_refresh && TradingChartHasInteraction(host)) return G_SOURCE_REMOVE;
    GtkRoot *root = host != NULL ? gtk_widget_get_root(host) : NULL;
    GtkWidget *focus = root != NULL ? gtk_root_get_focus(root) : NULL;
    GtkWidget *entry = focus != NULL ? gtk_widget_get_ancestor(focus, GTK_TYPE_SEARCH_ENTRY) : NULL;
    if (!command_refresh && entry != NULL && g_object_get_data(G_OBJECT(entry), "umicom-trading-hold-refresh") != NULL)
        return G_SOURCE_REMOVE;

    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation->suite);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (snapshot.active_layout_id[0] != '\0')
        (void)umi_application_suite_gtk4_workstation_select_layout(
            workstation->suite, snapshot.active_layout_id);
    return G_SOURCE_REMOVE;
}


#endif
static void TradingPanelObserverDestroy(gpointer data)
{
    GWeakRef *observer = data;
    g_weak_ref_clear(observer); g_free(observer);
}
static gboolean rebuild_idle(gpointer data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = data;
    if (workstation == NULL || workstation->suite == NULL) return G_SOURCE_REMOVE;
    workstation->pending_refresh = 0U;
    int explicit_action = workstation->refresh_from_command;
    workstation->refresh_from_command = 0;
    if (workstation->panel_observers == NULL) return G_SOURCE_REMOVE;
    guint index = 0;
    while (index < workstation->panel_observers->len) {
        GWeakRef *observer = g_ptr_array_index(workstation->panel_observers, index);
        GtkWidget *panel = g_weak_ref_get(observer);
        if (panel == NULL) { g_ptr_array_remove_index_fast(workstation->panel_observers, index); continue; }
        /* Weak observations cover detached windows too. They do not keep a
         * closed panel or its product context alive. Focused drafts may defer
         * their own refresh; other panes continue showing current evidence. */
        (void)UmiGtk4TradingPanelRefresh(panel, explicit_action);
        g_object_unref(panel); ++index;
    }
    return G_SOURCE_REMOVE;
}


/* Provide the schedule rebuild operation used by this module and its client applications. */
static void schedule_rebuild(UmiGtk4TradingSuiteWorkstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || workstation->pending_refresh != 0U) return;
    workstation->pending_refresh = g_idle_add(rebuild_idle, workstation);
}

/*
 * Provide the on controller changed operation used by this module and its client
 * applications.
 */
static void on_controller_changed(uint64_t revision, void *user_data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = user_data;
    (void)revision;
    /* Explicit actions must update tickets, orders and rejections immediately,
     * even if a market-only redraw was postponed during chart interaction. */
    workstation->refresh_from_command = 1;
    refresh_environment_badge(workstation);
    schedule_rebuild(workstation);
}

/* Provide the simulation tick operation used by this module and its client applications. */
static gboolean simulation_tick(gpointer data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = data;
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || !workstation->simulation_seeded)
        return G_SOURCE_CONTINUE;
    status = umi_trading_workspace_snapshot(
        workstation->config.workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        snapshot.environment == UMI_TRADING_SIMULATION) {
        /* A delayed timer must generate events at the current host time, not
         * fall further behind on each nominal one-second increment. */
        status = UmiTradingSimulationMarketAdvanceTo(
            &workstation->simulation, (int64_t)(g_get_real_time() / 1000));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) schedule_rebuild(workstation);
    }
    return G_SOURCE_CONTINUE;
}

/*
 * Provide the trading panel factory operation used by this module and its client
 * applications.
 */
/* The suite observes provider mounts weakly so market refresh reaches docked and detached panels without owning an extra widget lifetime. The superseded implementation is retained for engineering review. */
#if 0
static GtkWidget *trading_panel_factory(const UmiUiWorkspaceWindow *window,
                                        void *user_data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = user_data;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return NULL;
    return umi_gtk4_trading_panel_create(window, &workstation->panel_context);
}

#endif
static GtkWidget *trading_panel_factory(const UmiUiWorkspaceWindow *window, void *user_data)
{
    UmiGtk4TradingSuiteWorkstation *workstation = user_data;
    if (workstation == NULL) return NULL;
    GtkWidget *panel = umi_gtk4_trading_panel_create(window, &workstation->panel_context);
    if (panel != NULL) {
        UmiGtk4TradingPanelBindChartPersistence(panel, workstation->chart_persistence);
        if (workstation->panel_observers == NULL)
            workstation->panel_observers = g_ptr_array_new_with_free_func(TradingPanelObserverDestroy);
        /* Prune expired observers even when layouts change faster than ticks. */
        for (guint i = 0; i < workstation->panel_observers->len;) {
            GWeakRef *previous = g_ptr_array_index(workstation->panel_observers, i);
            gpointer existing = g_weak_ref_get(previous);
            if (existing == NULL) g_ptr_array_remove_index_fast(workstation->panel_observers, i);
            else { g_object_unref(existing); ++i; }
        }
        GWeakRef *observer = g_new0(GWeakRef, 1);
        g_weak_ref_init(observer, G_OBJECT(panel));
        g_ptr_array_add(workstation->panel_observers, observer);
    }
    return panel;
}


/*
 * Provide the gtk4 trading suite workstation config default operation used by this module
 * and its client applications.
 */
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

/*
 * Initialise gtk4 trading suite workstation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_gtk4_trading_suite_workstation_create(
    const UmiGtk4TradingSuiteWorkstationConfig *config,
    UmiGtk4TradingSuiteWorkstation **out_workstation)
{
    UmiGtk4TradingSuiteWorkstation *workstation;
    UmiTradingUiControllerConfig controller_config;
    UmiApplicationSuiteGtk4WorkstationConfig suite_config;
    UmiTradingWorkspaceSnapshot trading_snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || out_workstation == NULL ||
        config->workspace == NULL || config->application_id == NULL ||
        config->application_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_workstation = NULL;
    workstation = calloc(1U, sizeof(*workstation));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workstation->config = *config;
    /* Apply this branch only when its contract condition is satisfied. */
    if (workstation->config.simulation_step_interval_ms < 100U)
        workstation->config.simulation_step_interval_ms = 1000U;

    controller_config = umi_trading_ui_controller_config_default();
    controller_config.allow_live_environment =
        workstation->config.allow_live_environment != 0;
    status = umi_trading_ui_controller_init(
        &workstation->controller,
        workstation->config.workspace,
        &controller_config);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    umi_trading_ui_controller_set_changed_handler(
        &workstation->controller, on_controller_changed, workstation);

    status = umi_trading_simulation_market_init(
        &workstation->simulation, workstation->config.workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    /* Apply this branch only when its contract condition is satisfied. */
    if (workstation->config.seed_simulation_market) {
        status = umi_trading_workspace_snapshot(
            workstation->config.workspace, &trading_snapshot);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto fail;
        /* Apply this branch only when its contract condition is satisfied. */
        if (trading_snapshot.watchlist_count == 0U) {
            status = umi_trading_simulation_market_seed_default(
                &workstation->simulation,
                (int64_t)(g_get_real_time() / 1000));
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) goto fail;
            workstation->simulation_seeded = 1;
        }
    }

    /* Reusable chart persistence is owned by the suite, not transient panels.
     * Creation is memory-only; the native launcher explicitly opens storage. */
    status = UmiTradingChartPersistenceCreate(workstation->config.workspace, &workstation->chart_persistence);
    if (status != UMI_STATUS_OK) goto fail;
    workstation->panel_context.workspace = workstation->config.workspace;
    workstation->panel_context.controller = &workstation->controller;
    workstation->panel_context.allow_live_environment =
        workstation->config.allow_live_environment != 0;

    /* Read the environment again after optional simulation seeding so the
     * first rendered frame already carries the correct safety mode badge. */
    status = umi_trading_workspace_snapshot(
        workstation->config.workspace, &trading_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    refresh_environment_badge(workstation);

    /* Apply this branch only when its contract condition is satisfied. */
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

/* Forward explicit native titlebar adoption to the existing Framework owner. */
UmiStatus umi_gtk4_trading_suite_workstation_bind_window(
    UmiGtk4TradingSuiteWorkstation *workstation, GtkWindow *window)
{
    return workstation != NULL
        ? umi_application_suite_gtk4_workstation_bind_window(workstation->suite, window)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Delegate explicit persistence to the existing shared layout owner. */
UmiStatus umi_gtk4_trading_suite_workstation_enable_checkpoint_storage(
    UmiGtk4TradingSuiteWorkstation *workstation, int restore_saved)
{
/* The suite now enables explicit chart checkpoints alongside layout persistence in the same user-local profile. The shared helper keeps both bindings aligned. The previous implementation remains for engineering review. */
#if 0
    return workstation != NULL
        ? umi_application_suite_gtk4_workstation_enable_checkpoint_storage(
            workstation->suite, restore_saved)
        : UMI_STATUS_INVALID_ARGUMENT;
#endif
    return TradingSuiteEnableStorage(workstation, NULL, restore_saved);
}


/*
 * Release or reset state held by gtk4 trading suite workstation so the same storage can be
 * reused safely.
 */
/* Release this composition after its layout-owned storage and widgets. */
void umi_gtk4_trading_suite_workstation_destroy(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (workstation->simulation_timer != 0U) {
        g_source_remove(workstation->simulation_timer);
        workstation->simulation_timer = 0U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (workstation->pending_refresh != 0U) {
        g_source_remove(workstation->pending_refresh);
        workstation->pending_refresh = 0U;
    }
    umi_trading_ui_controller_set_changed_handler(
        &workstation->controller, NULL, NULL);
    /* Stop chart callbacks while their borrowed context and storage are alive,
     * including charts whose widgets were externally retained. */
    if (workstation->panel_observers != NULL) {
        for (guint i = 0U; i < workstation->panel_observers->len; ++i) {
            GWeakRef *observer = g_ptr_array_index(workstation->panel_observers, i);
            GtkWidget *panel = g_weak_ref_get(observer);
            if (panel != NULL) { UmiGtk4TradingPanelDetachChart(panel); g_object_unref(panel); }
        }
    }
    umi_application_suite_gtk4_workstation_destroy(workstation->suite);
    workstation->suite = NULL;
    g_clear_pointer(&workstation->panel_observers, g_ptr_array_unref);
    UmiTradingChartPersistenceDestroy(workstation->chart_persistence);
    umi_data_server_destroy(workstation->owned_chart_server);
    free(workstation);
}

/*
 * Provide the gtk4 trading suite workstation widget operation used by this module and its
 * client applications.
 */
GtkWidget *umi_gtk4_trading_suite_workstation_widget(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    return workstation != NULL
        ? umi_application_suite_gtk4_workstation_widget(workstation->suite)
        : NULL;
}

/*
 * Provide the gtk4 trading suite workstation select layout operation used by this module
 * and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_select_layout(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *layout_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_active_appearance(
        workstation->suite, out_profile);
}

/*
 * Provide the gtk4 trading suite workstation begin layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_begin_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_begin_layout_edit(
        workstation->suite);
}

/*
 * Provide the gtk4 trading suite workstation commit layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_commit_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_commit_layout_edit(
        workstation->suite);
}

/*
 * Provide the gtk4 trading suite workstation cancel layout edit operation used by this
 * module and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_cancel_layout_edit(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_application_suite_gtk4_workstation_restore_checkpoint(
        workstation->suite);
}

/*
 * Provide the gtk4 trading suite workstation open window operation used by this module and
 * its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_open_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *tool_id,
    const char *region_id,
    int floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the gtk4 trading suite workstation move window operation used by this module and
 * its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_move_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *window_id,
    const char *region_id,
    double x,
    double y,
    double width,
    double height)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the gtk4 trading suite workstation close window operation used by this module
 * and its client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_close_window(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const char *window_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_close_window(
        workstation->suite, window_id);
}

/* Forward panel settings without introducing trading-specific layout rules. */
UmiStatus umi_gtk4_trading_suite_workstation_apply_panel_settings(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const UmiUiWorkspacePanelSettings *settings)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || settings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Trading UI code forwards the semantic request unchanged; all layout
     * policy, validation and rollback stay in the application suite layer. */
    return umi_application_suite_gtk4_workstation_apply_panel_settings(
        workstation->suite, settings);
}

/* Forward a multi-panel edit to the shared suite so Trader and other products
 * use exactly the same validation and rollback rules. */
UmiStatus umi_gtk4_trading_suite_workstation_apply_panel_batch(
    UmiGtk4TradingSuiteWorkstation *workstation,
    const UmiUiWorkspacePanelSettings *settings,
    size_t setting_count)
{
    /* Reject incomplete requests before the native workstation is touched. */
    if (workstation == NULL || settings == NULL || setting_count == 0U ||
        setting_count > UMI_UI_WORKSPACE_MAX_PANEL_BATCH) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* The suite owns policy, staging and rendering for this shared surface. */
    return umi_application_suite_gtk4_workstation_apply_panel_batch(
        workstation->suite, settings, setting_count);
}

/*
 * Provide the gtk4 trading suite workstation refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_trading_suite_workstation_refresh(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_trading_ui_controller_refresh(&workstation->controller);
}

/*
 * Provide the gtk4 trading suite workstation snapshot operation used by this module and
 * its client applications.
 */
UmiGtk4TradingSuiteWorkstationSnapshot
umi_gtk4_trading_suite_workstation_snapshot(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    UmiGtk4TradingSuiteWorkstationSnapshot snapshot;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the gtk4 trading suite workstation controller operation used by this module and
 * its client applications.
 */
UmiTradingUiController *umi_gtk4_trading_suite_workstation_controller(
    UmiGtk4TradingSuiteWorkstation *workstation)
{
    return workstation != NULL ? &workstation->controller : NULL;
}

/* Keep native publication with the established Framework workspace owner;
 * copied observations support product acceptance without exposing its model. */
UmiStatus umi_gtk4_trading_suite_workstation_library_snapshot(
    UmiGtk4TradingSuiteWorkstation *workstation, UmiUiWorkspaceLibrarySnapshot *out_snapshot)
{
    if (workstation == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_library_snapshot(workstation->suite, out_snapshot);
}

/* Reuse the existing transactional UI path, retaining panel bodies and all
 * application-specific ownership instead of building another layout manager. */
UmiStatus umi_gtk4_trading_suite_workstation_library_apply(
    UmiGtk4TradingSuiteWorkstation *workstation, const UmiUiWorkspaceLibraryRequest *request)
{
    if (workstation == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_application_suite_gtk4_workstation_library_apply(workstation->suite, request);
    return status;
}

/* Backend selection stays with the existing Framework storage owner. */
UmiStatus umi_gtk4_trading_suite_workstation_bind_checkpoint_storage(
    UmiGtk4TradingSuiteWorkstation *workstation, UmiDataServer *server)
{
    return workstation != NULL ? umi_application_suite_gtk4_workstation_bind_checkpoint_storage(workstation->suite, server) : UMI_STATUS_INVALID_ARGUMENT;
}

/* Preview stays with the established Framework storage owner and never
 * publishes a candidate, changes trade state or adopts a competing Save CAS. */
UmiStatus umi_gtk4_trading_suite_workstation_library_preview(
    UmiGtk4TradingSuiteWorkstation *workstation, UmiUiWorkspaceLibraryPreview *out_preview)
{
    if (workstation == NULL || out_preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_library_preview(workstation->suite, out_preview);
}

/* Keep profile persistence with the established Framework layout owner. */
UmiStatus UmiGtk4TradingSuiteEnableProfileStorage(UmiGtk4TradingSuiteWorkstation *workstation, const char *profile, int restore_saved)
{
/* Chart and layout storage now use the same explicit profile identity. Chart restore remains a separate reviewed action. The previous implementation remains for engineering review. */
#if 0
    return workstation != NULL ? UmiApplicationSuiteEnableProfileStorage(workstation->suite,profile,restore_saved)
        : UMI_STATUS_INVALID_ARGUMENT;
#endif
    if (profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return TradingSuiteEnableStorage(workstation, profile, restore_saved);
}

UmiTradingChartPersistence *UmiGtk4TradingSuiteChartPersistence(UmiGtk4TradingSuiteWorkstation *workstation)
{ return workstation != NULL ? workstation->chart_persistence : NULL; }

UmiStatus UmiGtk4TradingSuiteBindChartStorage(UmiGtk4TradingSuiteWorkstation *workstation,
    UmiDataServer *server, const char *scope)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiTradingChartPersistenceBind(workstation->chart_persistence, server, scope);
    if (status != UMI_STATUS_OK) return status;
    if (workstation->owned_chart_server != server) {
        umi_data_server_destroy(workstation->owned_chart_server); workstation->owned_chart_server = NULL;
    }
    if (workstation->panel_observers != NULL) {
        for (guint i = 0U; i < workstation->panel_observers->len; ++i) {
            GWeakRef *observer = g_ptr_array_index(workstation->panel_observers, i);
            GtkWidget *panel = g_weak_ref_get(observer);
            if (panel != NULL) { UmiGtk4TradingPanelBindChartPersistence(panel, workstation->chart_persistence); g_object_unref(panel); }
        }
    }
    return UMI_STATUS_OK;
}

static UmiStatus TradingSuiteEnableStorage(UmiGtk4TradingSuiteWorkstation *workstation,
    const char *profile, int restore_saved)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char storage_id[256]; const char *identity = workstation->config.application_id;
    UmiStatus status = UMI_STATUS_OK;
    if (profile != NULL) {
        status = UmiGtk4WorkspaceProfileStorageId(identity, profile, storage_id, sizeof(storage_id));
        if (status != UMI_STATUS_OK) return status;
        identity = storage_id;
    }
    UmiDataServer *server = NULL;
    status = umi_gtk4_workspace_storage_open(identity, &server);
    if (status != UMI_STATUS_OK) return status;
    status = profile != NULL ? UmiApplicationSuiteEnableProfileStorage(workstation->suite, profile, restore_saved)
        : umi_application_suite_gtk4_workstation_enable_checkpoint_storage(workstation->suite, restore_saved);
    if (status == UMI_STATUS_OK)
        status = UmiGtk4TradingSuiteBindChartStorage(workstation, server, workstation->config.application_id);
    if (status == UMI_STATUS_OK) workstation->owned_chart_server = server;
    else {
        umi_data_server_destroy(server);
        /* A layout restore can fail after changing its backend. Disable chart
         * persistence rather than leave a writable chart in the old profile. */
        (void)UmiGtk4TradingSuiteBindChartStorage(workstation, NULL, NULL);
    }
    return status;
}

/* Keep archive ownership and native publication in the shared layout host. */
UmiStatus umi_gtk4_trading_suite_workstation_library_export(
    UmiGtk4TradingSuiteWorkstation *workstation, char *bytes, size_t capacity, size_t *out_size)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_library_export(workstation->suite, bytes, capacity, out_size);
}

/* Keep archive ownership and native publication in the shared layout host. */
UmiStatus umi_gtk4_trading_suite_workstation_library_import_review(
    UmiGtk4TradingSuiteWorkstation *workstation, const void *bytes, size_t size, UmiUiWorkspaceLibraryImport **out_review)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_library_import_review(workstation->suite, bytes, size, out_review);
}

/* Keep archive ownership and native publication in the shared layout host. */
UmiStatus umi_gtk4_trading_suite_workstation_library_import_apply(
    UmiGtk4TradingSuiteWorkstation *workstation, const UmiUiWorkspaceLibraryImport *review)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_library_import_apply(workstation->suite, review);
}

/* Layout recovery stays with Framework. This owner-thread call neither
 * saves product data nor runs a trade, payment or project command. */
UmiStatus umi_gtk4_trading_suite_workstation_library_history_read(UmiGtk4TradingSuiteWorkstation *workstation,
    UmiUiWorkspaceLibraryHistoryState *out_state)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_library_history_read(workstation->suite, out_state);
}

/* Layout recovery stays with Framework. This owner-thread call neither
 * saves product data nor runs a trade, payment or project command. */
UmiStatus umi_gtk4_trading_suite_workstation_library_history_navigate(UmiGtk4TradingSuiteWorkstation *workstation,
    UmiUiWorkspaceLibraryHistoryDirection direction, uint64_t expected_revision)
{
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_suite_gtk4_workstation_library_history_navigate(workstation->suite, direction, expected_revision);
}
