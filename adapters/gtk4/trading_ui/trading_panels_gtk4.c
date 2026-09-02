/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/trading_panels_gtk4.c
 *
 * PURPOSE:
 *   Render interactive professional trading controls while routing every state
 *   change through the toolkit-neutral guarded trading UI controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_ui/gtk4/trading_panels.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/trading_ui/trading_ui.h"
#include "umicom/ui/gtk4/drop_down.h"
#include "umicom/ui/gtk4/workstation/view_model_panel.h"

#define UMI_GTK4_TRADING_TEXT_CAPACITY 384U

typedef struct UmiGtk4TradingPanelState {
    UmiGtk4TradingPanelContext *context;
    GtkWidget *environment_dropdown;
    GtkWidget *filter_entry;
    GtkWidget *instrument_dropdown;
    GtkWidget *alert_dropdown;
    GtkWidget *alert_direction_dropdown;
    GtkWidget *alert_threshold_spin;
    GtkWidget *alert_toggle_button;
    GtkWidget *alert_acknowledge_button;
    GtkWidget *alert_remove_button;
    GtkWidget *order_filter_dropdown;
    GtkWidget *order_dropdown;
    GtkWidget *side_dropdown;
    GtkWidget *type_dropdown;
    GtkWidget *tif_dropdown;
    GtkWidget *quantity_spin;
    GtkWidget *limit_spin;
    GtkWidget *stop_spin;
    GtkWidget *risk_label;
    char instrument_ids[UMI_TRADING_MAX_WATCHLIST][UMI_FINANCE_ID_CAPACITY];
    size_t instrument_count;
    char alert_ids[UMI_TRADING_MAX_ALERTS][UMI_FINANCE_ID_CAPACITY];
    size_t alert_count;
    char order_ids[UMI_TRADING_MAX_ORDERS][UMI_FINANCE_ID_CAPACITY];
    size_t order_count;
    int building;
} UmiGtk4TradingPanelState;

/* Provide the new text label operation used by this module and its client applications. */
static GtkWidget *new_text_label(const char *text, int title)
{
    GtkWidget *label = gtk_label_new(text != NULL ? text : "");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    /* Apply this branch only when its contract condition is satisfied. */
    if (title) gtk_widget_add_css_class(label, "title-4");
    return label;
}

/* Provide the new section operation used by this module and its client applications. */
static GtkWidget *new_section(const char *title)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *label = new_text_label(title, 1);
    gtk_widget_add_css_class(box, "umicom-trading-panel");
    gtk_box_append(GTK_BOX(box), label);
    return box;
}

/* Provide the new dropdown operation used by this module and its client applications. */
static GtkWidget *new_dropdown(const char *const *labels,
                               size_t count,
                               size_t selected)
{
    GtkStringList *items = gtk_string_list_new(NULL);
    GtkWidget *dropdown;
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index)
        gtk_string_list_append(items, labels[index]);
    dropdown = umi_ui_gtk4_drop_down_new_take_string_list(items);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > 0U)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), (guint)selected);
    return dropdown;
}

/* Provide the set feedback operation used by this module and its client applications. */
static void set_feedback(UmiGtk4TradingPanelState *state,
                         const char *fallback)
{
    UmiTradingUiControllerSnapshot snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || state->risk_label == NULL || state->context == NULL ||
        state->context->controller == NULL) return;
    snapshot = umi_trading_ui_controller_snapshot(state->context->controller);
    gtk_label_set_text(GTK_LABEL(state->risk_label),
        snapshot.last_message[0] != '\0'
            ? snapshot.last_message
            : (fallback != NULL ? fallback : ""));
}

/*
 * Provide the on refresh clicked operation used by this module and its client
 * applications.
 */
static void on_refresh_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state != NULL && state->context != NULL)
        (void)umi_trading_ui_controller_refresh(state->context->controller);
}

/*
 * Provide the on kill switch clicked operation used by this module and its client
 * applications.
 */
static void on_kill_switch_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state != NULL && state->context != NULL)
        (void)umi_trading_ui_controller_engage_kill_switch(
            state->context->controller,
            "Requested from Framework GTK4 trading workstation");
}

/*
 * Provide the on reset kill switch clicked operation used by this module and its client
 * applications.
 */
static void on_reset_kill_switch_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state != NULL && state->context != NULL)
        (void)umi_trading_ui_controller_reset_kill_switch(
            state->context->controller);
}

/*
 * Find on environment while leaving the underlying catalogue or model owned by this
 * module.
 */
static void on_environment_selected(GObject *object,
                                    GParamSpec *pspec,
                                    gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    guint selected;
    UmiTradingEnvironment environment;
    (void)pspec;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || state->building || state->context == NULL) return;
    selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    environment = selected == 0U ? UMI_TRADING_SIMULATION
        : selected == 1U ? UMI_TRADING_PAPER : UMI_TRADING_LIVE;
    (void)umi_trading_ui_controller_set_environment(
        state->context->controller, environment);
}

/*
 * Provide the create dashboard panel operation used by this module and its client
 * applications.
 */
static GtkWidget *create_dashboard_panel(UmiGtk4TradingPanelContext *context)
{
    static const char *const environment_labels[] = {
        "Simulation", "Paper", "Live"
    };
    UmiTradingWorkspaceSnapshot snapshot;
    UmiGtk4TradingPanelState *state;
    GtkWidget *root;
    GtkWidget *summary;
    GtkWidget *environment_row;
    GtkWidget *button_row;
    GtkWidget *refresh;
    GtkWidget *kill;
    GtkWidget *reset;
    char text[UMI_GTK4_TRADING_TEXT_CAPACITY];
    size_t environment_count;
    size_t selected;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->workspace == NULL ||
        umi_trading_workspace_snapshot(context->workspace, &snapshot) !=
            UMI_STATUS_OK) return NULL;
    state = calloc(1U, sizeof(*state));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return NULL;
    state->context = context;
    state->building = 1;
    root = new_section("Trading Dashboard");
    g_object_set_data_full(G_OBJECT(root), "umicom-trading-panel-state",
                           state, free);

    (void)snprintf(text, sizeof(text),
        "Account %s  •  %s  •  %zu instruments  •  %zu orders  •  "
        "PnL %.2f  •  market %s  •  broker %s  •  risk %s",
        snapshot.account_id.value,
        umi_trading_environment_text(snapshot.environment),
        snapshot.watchlist_count,
        snapshot.order_count,
        snapshot.realised_pnl,
        snapshot.market_data_ready ? "ready" : "waiting",
        snapshot.broker_ready ? "ready" : "offline",
        snapshot.risk_ready ? "ready" : "offline");
    summary = new_text_label(text, 0);
    gtk_box_append(GTK_BOX(root), summary);

    environment_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(environment_row), new_text_label("Environment", 0));
    environment_count = context->allow_live_environment ? 3U : 2U;
    selected = snapshot.environment == UMI_TRADING_PAPER ? 1U
        : snapshot.environment == UMI_TRADING_LIVE ? 2U : 0U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (selected >= environment_count) selected = 0U;
    state->environment_dropdown = new_dropdown(
        environment_labels, environment_count, selected);
    g_signal_connect(state->environment_dropdown, "notify::selected",
                     G_CALLBACK(on_environment_selected), state);
    gtk_box_append(GTK_BOX(environment_row), state->environment_dropdown);
    gtk_box_append(GTK_BOX(root), environment_row);

    button_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    refresh = gtk_button_new_with_label("Refresh");
    kill = gtk_button_new_with_label("Stop Trading");
    reset = gtk_button_new_with_label("Reset Stop");
    gtk_widget_set_sensitive(kill, !snapshot.kill_switch_engaged);
    gtk_widget_set_sensitive(reset, snapshot.can_reset_kill_switch);
    g_signal_connect(refresh, "clicked", G_CALLBACK(on_refresh_clicked), state);
    g_signal_connect(kill, "clicked", G_CALLBACK(on_kill_switch_clicked), state);
    g_signal_connect(reset, "clicked",
                     G_CALLBACK(on_reset_kill_switch_clicked), state);
    gtk_box_append(GTK_BOX(button_row), refresh);
    gtk_box_append(GTK_BOX(button_row), kill);
    gtk_box_append(GTK_BOX(button_row), reset);
    gtk_box_append(GTK_BOX(root), button_row);
    state->building = 0;
    return root;
}

/* Provide the on filter clicked operation used by this module and its client applications. */
static void on_filter_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    const char *text;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || state->context == NULL || state->filter_entry == NULL)
        return;
    text = gtk_editable_get_text(GTK_EDITABLE(state->filter_entry));
    (void)umi_trading_ui_controller_set_instrument_filter(
        state->context->controller, text != NULL ? text : "");
}

/* Find on instrument while leaving the underlying catalogue or model owned by this module. */
static void on_instrument_selected(GObject *object,
                                   GParamSpec *pspec,
                                   gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    guint selected;
    (void)pspec;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || state->building || state->context == NULL) return;
    selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    /* Apply this branch only when its contract condition is satisfied. */
    if ((size_t)selected < state->instrument_count)
        (void)umi_trading_ui_controller_select_instrument(
            state->context->controller, state->instrument_ids[selected]);
}

/*
 * Provide the create watchlist panel operation used by this module and its client
 * applications.
 */
static GtkWidget *create_watchlist_panel(UmiGtk4TradingPanelContext *context)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiGtk4TradingPanelState *state;
    GtkWidget *root;
    GtkWidget *filter_row;
    GtkWidget *apply;
    GtkStringList *items;
    size_t count;
    size_t index;
    size_t selected_index = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->workspace == NULL ||
        umi_trading_workspace_snapshot(context->workspace, &snapshot) !=
            UMI_STATUS_OK) return NULL;
    state = calloc(1U, sizeof(*state));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return NULL;
    state->context = context;
    state->building = 1;
    root = new_section("Watchlist");
    g_object_set_data_full(G_OBJECT(root), "umicom-trading-panel-state",
                           state, free);

    filter_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    state->filter_entry = gtk_search_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(state->filter_entry),
                          snapshot.instrument_filter);
    gtk_widget_set_hexpand(state->filter_entry, TRUE);
    apply = gtk_button_new_with_label("Apply Filter");
    g_signal_connect(apply, "clicked", G_CALLBACK(on_filter_clicked), state);
    gtk_box_append(GTK_BOX(filter_row), state->filter_entry);
    gtk_box_append(GTK_BOX(filter_row), apply);
    gtk_box_append(GTK_BOX(root), filter_row);

    items = gtk_string_list_new(NULL);
    count = snapshot.visible_instrument_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_TRADING_MAX_WATCHLIST) count = UMI_TRADING_MAX_WATCHLIST;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiTradingMarketSnapshot market;
        char label[UMI_GTK4_TRADING_TEXT_CAPACITY];
        /* Apply this operation only while the related capability or state is available. */
        if (umi_trading_workspace_visible_instrument_at(
                context->workspace, index, &market) != UMI_STATUS_OK) continue;
        (void)snprintf(label, sizeof(label),
            "%s  •  %s  •  bid %.5f  ask %.5f  •  %s",
            market.instrument.symbol,
            market.instrument.venue,
            market.has_quote ? market.quote.bid : 0.0,
            market.has_quote ? market.quote.ask : 0.0,
            umi_trading_market_state_text(market.market_state));
        gtk_string_list_append(items, label);
        (void)snprintf(state->instrument_ids[state->instrument_count],
                       sizeof(state->instrument_ids[state->instrument_count]),
                       "%s", market.instrument.instrument_id.value);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(market.instrument.instrument_id.value,
                   snapshot.selected_instrument_id) == 0)
            selected_index = state->instrument_count;
        state->instrument_count += 1U;
    }
    state->instrument_dropdown =
        umi_ui_gtk4_drop_down_new_take_string_list(items);
    /* Apply this branch only when its contract condition is satisfied. */
    if (state->instrument_count > 0U)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(state->instrument_dropdown),
                                   (guint)selected_index);
    gtk_widget_set_hexpand(state->instrument_dropdown, TRUE);
    g_signal_connect(state->instrument_dropdown, "notify::selected",
                     G_CALLBACK(on_instrument_selected), state);
    gtk_box_append(GTK_BOX(root), state->instrument_dropdown);
    /* Apply this branch only when its contract condition is satisfied. */
    if (state->instrument_count == 0U)
        gtk_box_append(GTK_BOX(root),
            new_text_label("No instruments match the current filter.", 0));
    state->building = 0;
    return root;
}

/* Return the stable identifier selected in an Alerts panel. */
static const char *selected_alert_id(const UmiGtk4TradingPanelState *state)
{
    guint selected;

    if (state == NULL || state->alert_dropdown == NULL) {
        return NULL;
    }
    selected = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(state->alert_dropdown));
    if ((size_t)selected >= state->alert_count) {
        return NULL;
    }
    return state->alert_ids[selected];
}

/* Copy the selected Framework alert so callbacks never retain internal state. */
static int selected_price_alert(const UmiGtk4TradingPanelState *state,
                                UmiTradingPriceAlert *out_alert)
{
    const char *alert_id = selected_alert_id(state);
    size_t index;

    if (state == NULL || state->context == NULL ||
        state->context->workspace == NULL || alert_id == NULL ||
        out_alert == NULL) {
        return 0;
    }
    for (index = 0U; index < UMI_TRADING_MAX_ALERTS; ++index) {
        UmiTradingPriceAlert alert;

        /* A missing position means the compact alert collection has ended. */
        if (umi_trading_workspace_price_alert_at(
                state->context->workspace, index, &alert) != UMI_STATUS_OK) {
            break;
        }
        /* Stable identifiers connect display order to the owned rule. */
        if (strcmp(alert.alert_id, alert_id) == 0) {
            *out_alert = alert;
            return 1;
        }
    }
    return 0;
}

/*
 * Keep the alert action buttons consistent with the rule currently selected
 * by the user. An active rule may be acknowledged, while any existing rule
 * may be removed from the workspace.
 */
static void on_alert_selected(GObject *object,
                              GParamSpec *parameter,
                              gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    UmiTradingPriceAlert alert;
    int has_selection;
    int selected_is_active = 0;

    (void)object;
    (void)parameter;
    if (state == NULL) return;
    has_selection = selected_price_alert(state, &alert);
    if (has_selection) selected_is_active = alert.active;
    if (state->alert_toggle_button != NULL) {
        gtk_widget_set_sensitive(state->alert_toggle_button, has_selection);
        gtk_button_set_label(
            GTK_BUTTON(state->alert_toggle_button),
            has_selection && alert.enabled ? "Pause" : "Resume");
    }
    if (state->alert_acknowledge_button != NULL) {
        gtk_widget_set_sensitive(state->alert_acknowledge_button,
                                 selected_is_active);
    }
    if (state->alert_remove_button != NULL) {
        gtk_widget_set_sensitive(state->alert_remove_button,
                                 has_selection);
    }
}

/* Create a rule from the selected instrument, direction and threshold controls. */
static void on_create_price_alert_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    UmiTradingPriceAlertDirection direction;
    guint selected_direction;
    double threshold;
    int64_t now_ms;

    (void)button;
    if (state == NULL || state->context == NULL ||
        state->alert_direction_dropdown == NULL ||
        state->alert_threshold_spin == NULL) {
        return;
    }
    selected_direction = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(state->alert_direction_dropdown));
    if (selected_direction > 1U) {
        return;
    }
    direction = selected_direction == 0U
        ? UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE
        : UMI_TRADING_PRICE_ALERT_CROSSES_BELOW;
    threshold = gtk_spin_button_get_value(
        GTK_SPIN_BUTTON(state->alert_threshold_spin));
    now_ms = (int64_t)(g_get_real_time() / 1000);
    (void)umi_trading_ui_controller_create_price_alert(
        state->context->controller, direction, threshold, now_ms);
    set_feedback(state, "Price alert request completed.");
}

/* Acknowledge the active rule chosen in the Alerts panel. */
static void on_acknowledge_price_alert_clicked(GtkButton *button,
                                               gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    const char *alert_id = selected_alert_id(state);

    (void)button;
    if (state == NULL || state->context == NULL || alert_id == NULL) {
        return;
    }
    (void)umi_trading_ui_controller_acknowledge_price_alert(
        state->context->controller, alert_id);
    set_feedback(state, "Price alert acknowledgement completed.");
}

/* Pause or resume the rule chosen in the Alerts panel. */
static void on_toggle_price_alert_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    UmiTradingPriceAlert alert;

    (void)button;
    if (!selected_price_alert(state, &alert)) return;
    (void)umi_trading_ui_controller_set_price_alert_enabled(
        state->context->controller,
        alert.alert_id,
        !alert.enabled);
    set_feedback(state,
                 alert.enabled
                     ? "Price alert pause request completed."
                     : "Price alert resume request completed.");
}

/* Remove the rule chosen in the Alerts panel. */
static void on_remove_price_alert_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    const char *alert_id = selected_alert_id(state);

    (void)button;
    if (state == NULL || state->context == NULL || alert_id == NULL) {
        return;
    }
    (void)umi_trading_ui_controller_remove_price_alert(
        state->context->controller, alert_id);
    set_feedback(state, "Price alert removal completed.");
}

/* Build the interactive price-alert panel over Framework-owned alert state. */
static GtkWidget *create_alerts_panel(UmiGtk4TradingPanelContext *context)
{
    static const char *const direction_labels[] = {
        "Crosses above", "Crosses below"
    };
    UmiTradingWorkspaceSnapshot snapshot;
    UmiGtk4TradingPanelState *state;
    GtkWidget *root;
    GtkWidget *summary;
    GtkWidget *rule_row;
    GtkWidget *create_button;
    GtkWidget *button_row;
    GtkStringList *items;
    size_t count;
    size_t index;
    size_t selected_index = 0U;
    int selected_is_active = 0;
    char text[UMI_GTK4_TRADING_TEXT_CAPACITY];

    if (context == NULL || context->workspace == NULL ||
        umi_trading_workspace_snapshot(context->workspace, &snapshot) !=
            UMI_STATUS_OK) {
        return NULL;
    }
    state = calloc(1U, sizeof(*state));
    if (state == NULL) {
        return NULL;
    }
    state->context = context;
    root = new_section("Price Alerts");
    g_object_set_data_full(G_OBJECT(root),
                           "umicom-trading-panel-state",
                           state,
                           free);

    (void)snprintf(text,
                   sizeof(text),
                   "%zu rules  •  %zu active  •  %zu need acknowledgement",
                   snapshot.alert_count,
                   snapshot.active_alert_count,
                   snapshot.unacknowledged_alert_count);
    summary = new_text_label(text, 0);
    gtk_box_append(GTK_BOX(root), summary);

    rule_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    state->alert_direction_dropdown = new_dropdown(
        direction_labels, 2U, 0U);
    state->alert_threshold_spin = gtk_spin_button_new_with_range(
        0.00001, 1000000000.0, 0.01);
    gtk_spin_button_set_digits(
        GTK_SPIN_BUTTON(state->alert_threshold_spin), 5U);
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(state->alert_threshold_spin),
        snapshot.has_quote && snapshot.selected_mid > 0.0
            ? snapshot.selected_mid : 1.0);
    create_button = gtk_button_new_with_label("Create Alert");
    gtk_widget_set_sensitive(
        create_button,
        snapshot.has_selected_instrument && snapshot.has_quote);
    g_signal_connect(create_button,
                     "clicked",
                     G_CALLBACK(on_create_price_alert_clicked),
                     state);
    gtk_box_append(GTK_BOX(rule_row), state->alert_direction_dropdown);
    gtk_box_append(GTK_BOX(rule_row), state->alert_threshold_spin);
    gtk_box_append(GTK_BOX(rule_row), create_button);
    gtk_box_append(GTK_BOX(root), rule_row);

    items = gtk_string_list_new(NULL);
    count = snapshot.alert_count < UMI_TRADING_MAX_ALERTS
        ? snapshot.alert_count : UMI_TRADING_MAX_ALERTS;
    for (index = 0U; index < count; ++index) {
        UmiTradingPriceAlert alert;
        const char *state_text;
        size_t identifier_length;

        if (umi_trading_workspace_price_alert_at(
                context->workspace, index, &alert) != UMI_STATUS_OK) {
            continue;
        }
        state_text = alert.active
            ? "ACTIVE"
            : (alert.enabled ? "watching" : "paused");
        (void)snprintf(
            text,
            sizeof(text),
            "%s  •  %s %s %.5f  •  %s",
            alert.alert_id,
            alert.instrument_id,
            umi_trading_price_alert_direction_text(alert.direction),
            alert.threshold,
            state_text);
        gtk_string_list_append(items, text);
        identifier_length = strlen(alert.alert_id);
        (void)memcpy(state->alert_ids[state->alert_count],
                     alert.alert_id,
                     identifier_length + 1U);
        /* Select the first active rule so acknowledgement is one clear action. */
        if (alert.active && !selected_is_active) {
            selected_index = state->alert_count;
            selected_is_active = 1;
        }
        state->alert_count += 1U;
    }
    state->alert_dropdown =
        umi_ui_gtk4_drop_down_new_take_string_list(items);
    if (state->alert_count > 0U) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(state->alert_dropdown),
                                   (guint)selected_index);
    }
    gtk_widget_set_hexpand(state->alert_dropdown, TRUE);
    gtk_box_append(GTK_BOX(root), state->alert_dropdown);

    button_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    state->alert_toggle_button = gtk_button_new_with_label("Pause");
    state->alert_acknowledge_button =
        gtk_button_new_with_label("Acknowledge");
    state->alert_remove_button = gtk_button_new_with_label("Remove");
    gtk_widget_set_sensitive(state->alert_toggle_button,
                             state->alert_count > 0U);
    gtk_widget_set_sensitive(state->alert_acknowledge_button,
                             selected_is_active);
    gtk_widget_set_sensitive(state->alert_remove_button,
                             state->alert_count > 0U);
    g_signal_connect(state->alert_dropdown,
                     "notify::selected",
                     G_CALLBACK(on_alert_selected),
                     state);
    g_signal_connect(state->alert_toggle_button,
                     "clicked",
                     G_CALLBACK(on_toggle_price_alert_clicked),
                     state);
    g_signal_connect(state->alert_acknowledge_button,
                     "clicked",
                     G_CALLBACK(on_acknowledge_price_alert_clicked),
                     state);
    g_signal_connect(state->alert_remove_button,
                     "clicked",
                     G_CALLBACK(on_remove_price_alert_clicked),
                     state);
    /* Synchronize labels and availability after all three buttons exist. */
    on_alert_selected(G_OBJECT(state->alert_dropdown), NULL, state);
    gtk_box_append(GTK_BOX(button_row), state->alert_toggle_button);
    gtk_box_append(GTK_BOX(button_row), state->alert_acknowledge_button);
    gtk_box_append(GTK_BOX(button_row), state->alert_remove_button);
    gtk_box_append(GTK_BOX(root), button_row);

    state->risk_label = new_text_label(
        snapshot.alert_count == 0U
            ? "Create an alert after selecting an instrument with a current quote."
            : "Alert conditions observe market data and never submit an order.",
        0);
    gtk_box_append(GTK_BOX(root), state->risk_label);
    return root;
}

/*
 * Provide the apply order controls operation used by this module and its client
 * applications.
 */
static UmiStatus apply_order_controls(UmiGtk4TradingPanelState *state)
{
    static const UmiSide sides[] = {UMI_SIDE_BUY, UMI_SIDE_SELL};
    static const UmiOrderType types[] = {
        UMI_ORDER_MARKET, UMI_ORDER_LIMIT, UMI_ORDER_STOP, UMI_ORDER_STOP_LIMIT
    };
    static const UmiTimeInForce tifs[] = {
        UMI_TIF_DAY, UMI_TIF_GTC, UMI_TIF_IOC, UMI_TIF_FOK
    };
    guint side_index;
    guint type_index;
    guint tif_index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || state->context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    side_index = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->side_dropdown));
    type_index = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->type_dropdown));
    tif_index = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->tif_dropdown));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (side_index >= 2U || type_index >= 4U || tif_index >= 4U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_ui_controller_set_draft_side(
        state->context->controller, sides[side_index]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_trading_ui_controller_set_draft_type(
            state->context->controller, types[type_index], tifs[tif_index]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_trading_ui_controller_set_draft_quantity(
            state->context->controller,
            gtk_spin_button_get_value(GTK_SPIN_BUTTON(state->quantity_spin)));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_trading_ui_controller_set_draft_prices(
            state->context->controller,
            gtk_spin_button_get_value(GTK_SPIN_BUTTON(state->limit_spin)),
            gtk_spin_button_get_value(GTK_SPIN_BUTTON(state->stop_spin)));
    return status;
}

/*
 * Provide the on preview clicked operation used by this module and its client
 * applications.
 */
static void on_preview_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    UmiRiskDecision decision = {0};
    UmiStatus status;
    (void)button;
    status = apply_order_controls(state);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_trading_ui_controller_preview_order(
            state->context->controller, &decision);
    (void)status;
    set_feedback(state, decision.reason);
}

/* Provide the on submit clicked operation used by this module and its client applications. */
static void on_submit_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    UmiTradingWorkspaceSnapshot snapshot;
    UmiRiskDecision decision = {0};
    UmiStatus status;
    int64_t now_ms;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || state->context == NULL) return;
    status = umi_trading_workspace_snapshot(state->context->workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!snapshot.has_draft_risk || !snapshot.can_submit_order) {
        gtk_label_set_text(GTK_LABEL(state->risk_label),
                           "Preview and pass risk controls before submission.");
        return;
    }
    now_ms = (int64_t)(g_get_real_time() / 1000);
    status = umi_trading_ui_controller_submit_order(
        state->context->controller, now_ms, &decision);
    (void)status;
    set_feedback(state, decision.reason);
}

/* Provide the side index operation used by this module and its client applications. */
static size_t side_index(UmiSide side)
{
    return side == UMI_SIDE_SELL ? 1U : 0U;
}

/* Provide the type index operation used by this module and its client applications. */
static size_t type_index(UmiOrderType type)
{
    return type >= UMI_ORDER_MARKET && type <= UMI_ORDER_STOP_LIMIT
        ? (size_t)type : 1U;
}

/* Provide the tif index operation used by this module and its client applications. */
static size_t tif_index(UmiTimeInForce tif)
{
    return tif >= UMI_TIF_DAY && tif <= UMI_TIF_FOK ? (size_t)tif : 0U;
}

/*
 * Provide the create order entry panel operation used by this module and its client
 * applications.
 */
static GtkWidget *create_order_entry_panel(UmiGtk4TradingPanelContext *context)
{
    static const char *const side_labels[] = {"Buy", "Sell"};
    static const char *const type_labels[] = {
        "Market", "Limit", "Stop", "Stop Limit"
    };
    static const char *const tif_labels[] = {"DAY", "GTC", "IOC", "FOK"};
    UmiTradingWorkspaceSnapshot snapshot;
    UmiGtk4TradingPanelState *state;
    GtkWidget *root;
    GtkWidget *grid;
    GtkWidget *buttons;
    GtkWidget *preview;
    GtkWidget *submit;
    char heading[UMI_GTK4_TRADING_TEXT_CAPACITY];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->workspace == NULL ||
        umi_trading_workspace_snapshot(context->workspace, &snapshot) !=
            UMI_STATUS_OK) return NULL;
    state = calloc(1U, sizeof(*state));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return NULL;
    state->context = context;
    root = new_section("Order Entry");
    g_object_set_data_full(G_OBJECT(root), "umicom-trading-panel-state",
                           state, free);
    (void)snprintf(heading, sizeof(heading), "Instrument: %s",
        snapshot.has_selected_instrument
            ? snapshot.selected_instrument_id : "select an instrument first");
    gtk_box_append(GTK_BOX(root), new_text_label(heading, 0));

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    state->side_dropdown = new_dropdown(
        side_labels, 2U, side_index(snapshot.draft_order.side));
    state->type_dropdown = new_dropdown(
        type_labels, 4U, type_index(snapshot.draft_order.type));
    state->tif_dropdown = new_dropdown(
        tif_labels, 4U, tif_index(snapshot.draft_order.tif));
    state->quantity_spin = gtk_spin_button_new_with_range(0.01, 1000000000.0, 1.0);
    state->limit_spin = gtk_spin_button_new_with_range(0.0, 1000000000.0, 0.01);
    state->stop_spin = gtk_spin_button_new_with_range(0.0, 1000000000.0, 0.01);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(state->quantity_spin), 2U);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(state->limit_spin), 6U);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(state->stop_spin), 6U);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->quantity_spin),
                              snapshot.draft_order.quantity);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->limit_spin),
                              snapshot.draft_order.limit_price);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->stop_spin),
                              snapshot.draft_order.stop_price);

#define ATTACH_ROW(row, label_text, widget) do { \
    GtkWidget *row_label = new_text_label((label_text), 0); \
    gtk_grid_attach(GTK_GRID(grid), row_label, 0, (row), 1, 1); \
    gtk_grid_attach(GTK_GRID(grid), (widget), 1, (row), 1, 1); \
} while (0)
    ATTACH_ROW(0, "Side", state->side_dropdown);
    ATTACH_ROW(1, "Order type", state->type_dropdown);
    ATTACH_ROW(2, "Time in force", state->tif_dropdown);
    ATTACH_ROW(3, "Quantity", state->quantity_spin);
    ATTACH_ROW(4, "Limit price", state->limit_spin);
    ATTACH_ROW(5, "Stop price", state->stop_spin);
#undef ATTACH_ROW
    gtk_box_append(GTK_BOX(root), grid);

    state->risk_label = new_text_label(
        snapshot.has_draft_risk
            ? snapshot.draft_risk.reason
            : "Preview validates the order through Framework pre-trade risk.", 0);
    gtk_widget_add_css_class(state->risk_label, "dim-label");
    gtk_box_append(GTK_BOX(root), state->risk_label);

    buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    preview = gtk_button_new_with_label("Preview Risk");
    submit = gtk_button_new_with_label("Submit Order");
    gtk_widget_set_sensitive(preview, snapshot.can_preview_order);
    gtk_widget_set_sensitive(submit, snapshot.can_submit_order);
    g_signal_connect(preview, "clicked", G_CALLBACK(on_preview_clicked), state);
    g_signal_connect(submit, "clicked", G_CALLBACK(on_submit_clicked), state);
    gtk_box_append(GTK_BOX(buttons), preview);
    gtk_box_append(GTK_BOX(buttons), submit);
    gtk_box_append(GTK_BOX(root), buttons);
    return root;
}

/*
 * Find on order filter while leaving the underlying catalogue or model owned by this
 * module.
 */
static void on_order_filter_selected(GObject *object,
                                     GParamSpec *pspec,
                                     gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    guint selected;
    (void)pspec;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || state->building || state->context == NULL) return;
    selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (selected <= (guint)UMI_TRADING_WORKSPACE_ORDERS_REJECTED)
        (void)umi_trading_ui_controller_set_order_filter(
            state->context->controller,
            (UmiTradingWorkspaceOrderFilter)selected);
}

/* Find on order while leaving the underlying catalogue or model owned by this module. */
static void on_order_selected(GObject *object,
                              GParamSpec *pspec,
                              gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    guint selected;
    (void)pspec;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || state->building || state->context == NULL) return;
    selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    /* Apply this branch only when its contract condition is satisfied. */
    if ((size_t)selected < state->order_count)
        (void)umi_trading_ui_controller_select_order(
            state->context->controller, state->order_ids[selected]);
}

/*
 * Provide the on cancel order clicked operation used by this module and its client
 * applications.
 */
static void on_cancel_order_clicked(GtkButton *button, gpointer data)
{
    UmiGtk4TradingPanelState *state = data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state != NULL && state->context != NULL)
        (void)umi_trading_ui_controller_cancel_selected_order(
            state->context->controller);
}

/*
 * Provide the create orders panel operation used by this module and its client
 * applications.
 */
static GtkWidget *create_orders_panel(UmiGtk4TradingPanelContext *context)
{
    static const char *const filter_labels[] = {
        "All", "Open", "Filled", "Cancelled", "Rejected"
    };
    UmiTradingWorkspaceSnapshot snapshot;
    UmiGtk4TradingPanelState *state;
    GtkWidget *root;
    GtkWidget *filter_row;
    GtkWidget *cancel;
    GtkStringList *orders;
    size_t count;
    size_t index;
    size_t selected_index = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->workspace == NULL ||
        umi_trading_workspace_snapshot(context->workspace, &snapshot) !=
            UMI_STATUS_OK) return NULL;
    state = calloc(1U, sizeof(*state));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return NULL;
    state->context = context;
    state->building = 1;
    root = new_section("Orders / Trade Blotter");
    g_object_set_data_full(G_OBJECT(root), "umicom-trading-panel-state",
                           state, free);

    filter_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(filter_row), new_text_label("Filter", 0));
    state->order_filter_dropdown = new_dropdown(
        filter_labels, 5U, (size_t)snapshot.order_filter);
    g_signal_connect(state->order_filter_dropdown, "notify::selected",
                     G_CALLBACK(on_order_filter_selected), state);
    gtk_box_append(GTK_BOX(filter_row), state->order_filter_dropdown);
    gtk_box_append(GTK_BOX(root), filter_row);

    orders = gtk_string_list_new(NULL);
    count = snapshot.visible_order_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_TRADING_MAX_ORDERS) count = UMI_TRADING_MAX_ORDERS;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiOrder order;
        char label[UMI_GTK4_TRADING_TEXT_CAPACITY];
        /* Apply this operation only while the related capability or state is available. */
        if (umi_trading_workspace_visible_order_at(
                context->workspace, index, &order) != UMI_STATUS_OK) continue;
        (void)snprintf(label, sizeof(label),
            "%s  •  %s %s %.2f  •  %s  •  filled %.2f",
            order.request.client_order_id.value,
            umi_trading_side_text(order.request.side),
            order.request.instrument.symbol,
            order.request.quantity,
            umi_trading_order_status_text(order.status),
            order.filled_quantity);
        gtk_string_list_append(orders, label);
        (void)snprintf(state->order_ids[state->order_count],
                       sizeof(state->order_ids[state->order_count]),
                       "%s", order.request.client_order_id.value);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(order.request.client_order_id.value,
                   snapshot.selected_order_id) == 0)
            selected_index = state->order_count;
        state->order_count += 1U;
    }
    state->order_dropdown =
        umi_ui_gtk4_drop_down_new_take_string_list(orders);
    /* Apply this branch only when its contract condition is satisfied. */
    if (state->order_count > 0U)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(state->order_dropdown),
                                   (guint)selected_index);
    gtk_widget_set_hexpand(state->order_dropdown, TRUE);
    g_signal_connect(state->order_dropdown, "notify::selected",
                     G_CALLBACK(on_order_selected), state);
    gtk_box_append(GTK_BOX(root), state->order_dropdown);

    cancel = gtk_button_new_with_label("Cancel Selected Order");
    gtk_widget_set_sensitive(cancel, snapshot.can_cancel_order);
    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel_order_clicked), state);
    gtk_box_append(GTK_BOX(root), cancel);
    state->building = 0;
    return root;
}

/*
 * Provide the generic action handler operation used by this module and its client
 * applications.
 */
static UmiStatus generic_action_handler(const char *action_id, void *user_data)
{
    UmiGtk4TradingPanelContext *context = user_data;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->controller == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_trading_ui_controller_dispatch(
        context->controller, action_id, NULL, NULL);
}

/*
 * Provide the create generic panel operation used by this module and its client
 * applications.
 */
static GtkWidget *create_generic_panel(const UmiUiWorkspaceWindow *window,
                                       UmiGtk4TradingPanelContext *context)
{
    UmiUiViewModel *view = NULL;
    UmiStatus status = UMI_STATUS_NOT_IMPLEMENTED;
    GtkWidget *widget;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL || context == NULL || context->workspace == NULL)
        return NULL;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(window->tool_id, "depth") == 0)
        status = umi_trading_ui_depth_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "chart") == 0)
        status = umi_trading_ui_chart_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "positions") == 0 ||
             strcmp(window->tool_id, "risk") == 0)
        status = umi_trading_ui_portfolio_risk_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "executions") == 0)
        status = umi_trading_ui_executions_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "dashboard") == 0)
        status = umi_trading_ui_dashboard_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "scanner") == 0)
        status = umi_trading_ui_scanner_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "predictive-lab") == 0)
        status = umi_trading_ui_predictive_lab_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "news") == 0)
        status = umi_trading_ui_news_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "context-inspector") == 0)
        status = umi_trading_ui_context_inspector_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "strategy") == 0 ||
             strcmp(window->tool_id, "strategy-analysis") == 0)
        status = umi_trading_ui_strategy_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "replay") == 0)
        status = umi_trading_ui_replay_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "output") == 0 ||
             strcmp(window->tool_id, "trade-performance") == 0)
        status = umi_trading_ui_research_output_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "time-and-sales") == 0)
        status = umi_trading_ui_time_and_sales_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "economic-calendar") == 0)
        status = umi_trading_ui_economic_calendar_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "fundamentals") == 0)
        status = umi_trading_ui_fundamentals_view_create(
            window->window_id, context->workspace, &view);
    else if (strcmp(window->tool_id, "alerts") == 0)
        status = umi_trading_ui_alerts_view_create(
            window->window_id, context->workspace, &view);
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->tool_id, "price-ladder") == 0)
        status = umi_trading_ui_depth_view_create(
            window->window_id, context->workspace, &view);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || view == NULL) return NULL;
    widget = umi_gtk4_view_model_panel_create(
        view, generic_action_handler, context);
    umi_ui_view_model_destroy(view);
    return widget;
}

/*
 * Initialise gtk4 trading panel from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_trading_panel_create(
    const UmiUiWorkspaceWindow *window,
    UmiGtk4TradingPanelContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL || context == NULL || context->workspace == NULL ||
        context->controller == NULL) return NULL;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(window->tool_id, "account") == 0)
        return create_dashboard_panel(context);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(window->tool_id, "watchlist") == 0)
        return create_watchlist_panel(context);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(window->tool_id, "order-entry") == 0)
        return create_order_entry_panel(context);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(window->tool_id, "blotter") == 0)
        return create_orders_panel(context);
    /* Alerts need native creation and acknowledgement controls, not only rows. */
    if (strcmp(window->tool_id, "alerts") == 0)
        return create_alerts_panel(context);
    return create_generic_panel(window, context);
}
