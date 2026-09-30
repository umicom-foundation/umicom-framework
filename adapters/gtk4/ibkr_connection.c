/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_connection.c
 * PURPOSE:
 *   Paper/Live connection controls. This window never offers order execution.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Paper/Live connection controls. This window never offers order execution.
 *---------------------------------------------------------------------------*/
#include "window_removal_private.h"
#include "umicom/broker_connectivity/connection_gtk4.h"
#include "umicom/broker_connectivity/connection.h"
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include "desktop_system_brand.inc"

#define UI_KEY "umicom-ibkr-connection-owner"
typedef struct ConnectionUi {
    UmiIbkrConnection *connection;
    UmiIbkrConnectionSnapshot snapshot;
    GtkWidget *mode, *program, *port, *client, *ack;
    GtkWidget *connect, *disconnect, *read, *accounts, *status, *output;
    guint source;
    gboolean closed, populated;
    uint64_t lastPaint;
    gchar *lastOutput;
} ConnectionUi;

static ConnectionUi *Owner(GtkWindow *window)
{
    ConnectionUi *ui = g_object_get_data(G_OBJECT(window), UI_KEY);
    return ui != NULL && !ui->closed ? ui : NULL;
}
static GtkWidget *Text(const char *value)
{
    GtkWidget *label = gtk_label_new(value);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_widget_set_hexpand(label, TRUE);
    return label;
}
static GtkWidget *Brand(const unsigned char *bytes, size_t length, int height)
{
    GBytes *data = g_bytes_new_static(bytes, length);
    GError *error = NULL;
    GdkTexture *texture = gdk_texture_new_from_bytes(data, &error);
    g_bytes_unref(data);
    if (texture == NULL) { g_clear_error(&error); return Text("Umicom"); }
    GtkWidget *image = gtk_image_new_from_paintable(GDK_PAINTABLE(texture));
    gtk_image_set_pixel_size(GTK_IMAGE(image), height);
    g_object_unref(texture);
    return image;
}
static gboolean Connected(const ConnectionUi *ui)
{
    return ui->connection != NULL && ui->snapshot.state >= UMI_IBKR_CONNECTING &&
        ui->snapshot.state <= UMI_IBKR_READY;
}
static void Controls(ConnectionUi *ui)
{
    gboolean active = Connected(ui);
    GtkWidget *profile[] = {ui->mode, ui->program, ui->port, ui->client};
    for (size_t i = 0; i < G_N_ELEMENTS(profile); ++i)
        gtk_widget_set_sensitive(profile[i], !active);
    gboolean live = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode)) == 1U;
    gtk_widget_set_sensitive(ui->ack, !active && live);
    gtk_widget_set_sensitive(ui->connect, !active);
    gtk_widget_set_sensitive(ui->disconnect, active);
    gboolean canRead = active && ui->snapshot.state == UMI_IBKR_READY &&
        !ui->snapshot.requestIssued && ui->snapshot.accountCount != 0U;
    gtk_widget_set_sensitive(ui->read, canRead);
    gtk_widget_set_sensitive(ui->accounts, canRead);
}
static void Render(ConnectionUi *ui, uint64_t now)
{
    if (ui->connection == NULL) { Controls(ui); return; }
    (void)UmiIbkrConnectionCopy(ui->connection, &ui->snapshot);
    if (!ui->populated && ui->snapshot.state == UMI_IBKR_READY) {
        GtkStringList *items = gtk_string_list_new(NULL);
        for (size_t i = 0; i < ui->snapshot.accountCount; ++i)
            gtk_string_list_append(items, ui->snapshot.accounts[i]);
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->accounts), G_LIST_MODEL(items));
        g_object_unref(items);
        ui->populated = TRUE;
    }
    GString *text = g_string_new(NULL);
    const UmiIbkrConnectionSnapshot *s = &ui->snapshot;
    g_string_append_printf(text, "Requested mode: %s\nServer mode attested: NO\nOrders: not available\nProtocol version: %d\n\n",
        s->requestedEnvironment == UMI_TRADING_LIVE ? "LIVE" : "PAPER", s->protocolVersion);
    g_string_append_printf(text, "Authorised accounts: %zu\n", s->accountCount);
    for (size_t i = 0; i < s->accountCount; ++i)
        g_string_append_printf(text, "  %s\n", s->accounts[i]);
    if (s->requestIssued) {
        g_string_append_printf(text, "\nSelected account: %s\nSummary: %s; Positions: %s\n",
            s->selectedAccount, s->summaryComplete ? "complete" : "INCOMPLETE",
            s->positionsComplete ? "complete" : "INCOMPLETE");
        if (s->summaryComplete && now >= s->summaryAtMilliseconds)
            g_string_append_printf(text, "Summary received at monotonic time %" PRIu64 " ms.\n", s->summaryAtMilliseconds);
        if (s->positionsComplete && now >= s->positionsAtMilliseconds)
            g_string_append_printf(text, "Positions received at monotonic time %" PRIu64 " ms.\n", s->positionsAtMilliseconds);
        g_string_append(text, "These are separate captured responses, not an atomic or continuously refreshed portfolio.\n\n");
        for (size_t i = 0; i < s->valueCount; ++i) {
            const UmiIbkrAccountValue *v = &s->values[i];
            g_string_append_printf(text, "%s: %s %s\n", v->tag, v->value[0] ? v->value : "(unset)", v->currency);
        }
        g_string_append_printf(text, "\nPositions received: %zu\n", s->positionCount);
        for (size_t i = 0; i < s->positionCount; ++i) {
            const UmiIbkrPositionObservation *p = &s->positions[i];
            g_string_append_printf(text, "%s | %s | %s | %s | %s\n  quantity %s; average cost %s %s\n  expiry %s; strike %s; right %s; multiplier %s\n",
                p->contractId, p->symbol, p->securityType, p->localSymbol, p->exchange,
                p->quantity, p->averageCost, p->currency, p->expiry, p->strike, p->right, p->multiplier);
        }
    }
    if (s->stale)
        g_string_append(text, "\nSTALE: the connection is not ready or has ended. Retained rows are not current.\n");
    /* Preserve selection/copying when only the age label changes. */
    if (ui->lastOutput == NULL || strcmp(ui->lastOutput, text->str) != 0) {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->output)), text->str, -1);
        g_free(ui->lastOutput);
        ui->lastOutput = g_string_free(text, FALSE);
    } else {
        g_string_free(text, TRUE);
    }
    GString *status = g_string_new(UmiIbkrConnectionStateName(s->state));
    g_string_append_printf(status, "\n%s", s->message);
    if (s->summaryComplete && now >= s->summaryAtMilliseconds)
        g_string_append_printf(status, "\nSummary age: %" PRIu64 " ms.", now-s->summaryAtMilliseconds);
    if (s->positionsComplete && now >= s->positionsAtMilliseconds)
        g_string_append_printf(status, " Positions age: %" PRIu64 " ms.", now-s->positionsAtMilliseconds);
    gtk_label_set_text(GTK_LABEL(ui->status), status->str);
    g_string_free(status, TRUE);
    Controls(ui);
}
static gboolean Poll(gpointer data)
{
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL) return G_SOURCE_REMOVE;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrConnectionPump(ui->connection, now);
    (void)UmiIbkrConnectionCopy(ui->connection, &ui->snapshot);
    if (status != UMI_STATUS_OK && Connected(ui))
        UmiIbkrConnectionClose(ui->connection);
    if (status != UMI_STATUS_OK || now-ui->lastPaint >= 250U) {
        Render(ui, now);
        ui->lastPaint = now;
    }
    if (status != UMI_STATUS_OK || !Connected(ui)) {
        ui->source = 0U;
        return G_SOURCE_REMOVE;
    }
    return G_SOURCE_CONTINUE;
}
static void ProfileChanged(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL || Connected(ui)) return;
    UmiTradingEnvironment mode = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode)) == 1U ? UMI_TRADING_LIVE : UMI_TRADING_PAPER;
    UmiIbkrProgram program = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->program)) == 1U ? UMI_IBKR_GATEWAY : UMI_IBKR_TWS;
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->port), UmiIbkrDefaultPort(program, mode));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->ack), FALSE);
    Controls(ui);
}
static void ConnectClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL || Connected(ui)) return;
    UmiIbkrConnectionOptions options = UmiIbkrConnectionOptionsDefault();
    options.environment = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode)) == 1U ? UMI_TRADING_LIVE : UMI_TRADING_PAPER;
    options.program = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->program)) == 1U ? UMI_IBKR_GATEWAY : UMI_IBKR_TWS;
    options.adapter.paperOnly = options.environment == UMI_TRADING_PAPER ? 1 : 0;
    options.adapter.port = (uint16_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->port));
    options.adapter.clientId = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->client));
    options.acknowledgeLive = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->ack)) != FALSE;
    UmiStatus status = UmiIbkrConnectionValidate(&options);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(ui->status), "Connection not opened. Check the profile and acknowledge Live access when selected.");
        return;
    }
    UmiIbkrConnectionDestroy(ui->connection);
    ui->connection = NULL; ui->populated = FALSE;
    memset(&ui->snapshot, 0, sizeof ui->snapshot);
    gtk_drop_down_set_model(GTK_DROP_DOWN(ui->accounts), NULL);
    status = UmiIbkrConnectionCreate(&options, &ui->connection);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(ui->status), umi_status_text(status));
        Controls(ui); return;
    }
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    status = UmiIbkrConnectionOpen(ui->connection, now);
    Render(ui, now);
    if (status == UMI_STATUS_OK) {
        ui->lastPaint = now;
        ui->source = g_timeout_add(25U, Poll, data);
        if (ui->source == 0U) { UmiIbkrConnectionClose(ui->connection); Render(ui, now); }
    }
}
static void DisconnectClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL) return;
    if (ui->source != 0U) { g_source_remove(ui->source); ui->source = 0U; }
    UmiIbkrConnectionClose(ui->connection);
    Render(ui, UmiIbkrMonotonicMilliseconds());
}
static void ReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL || !Connected(ui)) return;
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->accounts));
    if ((size_t)index >= ui->snapshot.accountCount) return;
    UmiStatus status = UmiIbkrConnectionReadAccount(ui->connection, ui->snapshot.accounts[index], UmiIbkrMonotonicMilliseconds());
    Render(ui, UmiIbkrMonotonicMilliseconds());
    if (status != UMI_STATUS_OK) gtk_label_set_text(GTK_LABEL(ui->status), umi_status_text(status));
}
static void Destroyed(GtkWidget *widget, gpointer data)
{
    (void)data;
    ConnectionUi *ui = g_object_get_data(G_OBJECT(widget), UI_KEY);
    if (ui == NULL || ui->closed) return;
    ui->closed = TRUE;
    if (ui->source != 0U) { g_source_remove(ui->source); ui->source = 0U; }
    UmiIbkrConnectionDestroy(ui->connection); ui->connection = NULL;
}
static void FreeOwner(gpointer data)
{
    ConnectionUi *ui = data;
    if (ui->source != 0U) g_source_remove(ui->source);
    UmiIbkrConnectionDestroy(ui->connection);
    g_free(ui->lastOutput);
    g_free(ui);
}
static void Field(GtkGrid *grid, const char *name, GtkWidget *widget, int row, const char *id)
{
    gtk_grid_attach(grid, Text(name), 0, row, 1, 1);
    gtk_grid_attach(grid, widget, 1, row, 1, 1);
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_widget_set_name(widget, id);
}
GtkWindow *UmiIbkrGtkCreate(GtkWindow *parent)
{
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    ConnectionUi *ui = g_new0(ConnectionUi, 1);
    g_object_set_data_full(G_OBJECT(window), UI_KEY, ui, FreeOwner);
/* Close the connection at native window removal, including retained windows. The previous implementation remains for engineering review. */
#if 0
    g_signal_connect(window, "destroy", G_CALLBACK(Destroyed), NULL);
#endif
    UmiGtk4ObserveWindowRemoval(window, G_OBJECT(window), Destroyed, NULL);
    gtk_window_set_title(window, "Umicom Broker Connections — Read-only");
    gtk_window_set_default_size(window, 940, 740);
    gtk_window_set_icon_name(window, "org.umicom.trader");
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent);
        gtk_window_set_destroy_with_parent(window, TRUE);
        UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
        GtkApplication *application = gtk_window_get_application(parent);
        if (application != NULL) gtk_window_set_application(window, application);
    }
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(root, 18); gtk_widget_set_margin_end(root, 18);
    gtk_widget_set_margin_top(root, 18); gtk_widget_set_margin_bottom(root, 18);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), root);
    gtk_window_set_child(window, scroll);
    GtkWidget *brand = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(brand), Brand(UMICOM_SYSTEM_ICON, sizeof UMICOM_SYSTEM_ICON, 40));
    gtk_box_append(GTK_BOX(brand), Brand(UMICOM_SYSTEM_LOGO, sizeof UMICOM_SYSTEM_LOGO, 48));
    gtk_box_append(GTK_BOX(root), brand);
    gtk_box_append(GTK_BOX(root), Text("Choose Paper or Live, then confirm the actual login in TWS / IB Gateway. This window cannot place, cancel or modify orders."));
    gtk_box_append(GTK_BOX(root), Text("Endpoint: 127.0.0.1 only. Keep Read-Only API enabled in TWS. A port number or account prefix does not verify the environment."));
    const char *modes[] = {"Paper", "Live", NULL};
    const char *programs[] = {"TWS", "IB Gateway", NULL};
    ui->mode = gtk_drop_down_new_from_strings(modes);
    ui->program = gtk_drop_down_new_from_strings(programs);
    ui->port = gtk_spin_button_new_with_range(1, 65535, 1);
    ui->client = gtk_spin_button_new_with_range(1, INT_MAX, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->port), 7497);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->client), 35);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8); gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    Field(GTK_GRID(grid), "Requested environment", ui->mode, 0, "ibkr-mode");
    Field(GTK_GRID(grid), "Provider program", ui->program, 1, "ibkr-program");
    Field(GTK_GRID(grid), "Configured API port", ui->port, 2, "ibkr-port");
    Field(GTK_GRID(grid), "Unique client ID", ui->client, 3, "ibkr-client");
    gtk_box_append(GTK_BOX(root), grid);
    ui->ack = gtk_check_button_new_with_label("I intend to access my Live account read-only and have checked the provider login.");
    gtk_widget_set_name(ui->ack, "ibkr-live-ack");
    gtk_box_append(GTK_BOX(root), ui->ack);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    ui->connect = gtk_button_new_with_label("Connect read-only");
    ui->disconnect = gtk_button_new_with_label("Disconnect");
    gtk_widget_set_name(ui->connect, "ibkr-connect");
    gtk_widget_set_name(ui->disconnect, "ibkr-disconnect");
    gtk_box_append(GTK_BOX(buttons), ui->connect); gtk_box_append(GTK_BOX(buttons), ui->disconnect);
    gtk_box_append(GTK_BOX(root), buttons);
    ui->accounts = gtk_drop_down_new(NULL, NULL);
    ui->read = gtk_button_new_with_label("Read selected account once");
    gtk_widget_set_name(ui->accounts, "ibkr-accounts"); gtk_widget_set_name(ui->read, "ibkr-read");
    gtk_box_append(GTK_BOX(root), ui->accounts); gtk_box_append(GTK_BOX(root), ui->read);
    gtk_box_append(GTK_BOX(root), Text("To refresh or choose another account, disconnect and connect again. Only selected-account observations are retained; provider requests may cover all accounts authorised to this login."));
    ui->status = Text("Not connected. Opening this window does not open a socket.");
    gtk_widget_set_name(ui->status, "ibkr-status");
    gtk_box_append(GTK_BOX(root), ui->status);
    ui->output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ui->output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(ui->output), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(ui->output), GTK_WRAP_WORD_CHAR);
    GtkWidget *outputScroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(outputScroll), ui->output);
    gtk_widget_set_size_request(outputScroll, -1, 300);
    gtk_widget_set_vexpand(outputScroll, TRUE);
    gtk_box_append(GTK_BOX(root), outputScroll);
    g_signal_connect_object(ui->mode, "notify::selected", G_CALLBACK(ProfileChanged), window, 0);
    g_signal_connect_object(ui->program, "notify::selected", G_CALLBACK(ProfileChanged), window, 0);
    g_signal_connect_object(ui->connect, "clicked", G_CALLBACK(ConnectClicked), window, 0);
    g_signal_connect_object(ui->disconnect, "clicked", G_CALLBACK(DisconnectClicked), window, 0);
    g_signal_connect_object(ui->read, "clicked", G_CALLBACK(ReadClicked), window, 0);
    Controls(ui);
    return window;
}

typedef struct ParentLink { GWeakRef parent; gulong destroyed; } ParentLink;
static void ParentDestroyed(GtkWidget *widget, gpointer data)
{
    (void)widget;
    ParentLink *link = data;
    link->destroyed = 0U;
    g_weak_ref_set(&link->parent, NULL);
}
static void FreeLink(gpointer data)
{
    ParentLink *link = data;
    GObject *parent = g_weak_ref_get(&link->parent);
    if (parent != NULL) {
        if (link->destroyed != 0U) g_signal_handler_disconnect(parent, link->destroyed);
        g_object_unref(parent);
    }
    g_weak_ref_clear(&link->parent); g_free(link);
}
static void OpenClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ParentLink *link = data;
    GObject *parent = g_weak_ref_get(&link->parent);
    if (parent == NULL) return;
    GtkWindow *window = UmiIbkrGtkCreate(GTK_WINDOW(parent));
    gtk_window_present(window);
    g_object_unref(parent);
}
GtkWidget *UmiIbkrGtkWrap(GtkWidget *child, GtkWindow *parent)
{
    if (!GTK_IS_WIDGET(child) || !GTK_IS_WINDOW(parent) || gtk_widget_get_parent(child) != NULL) return child;
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *button = gtk_button_new_with_label("Open Paper / Live broker connections");
    ParentLink *link = g_new0(ParentLink, 1);
    g_weak_ref_init(&link->parent, parent);
/* A retained parent must lose launcher authority when removed from the native window list. The previous implementation remains for engineering review. */
#if 0
    link->destroyed = g_signal_connect(parent, "destroy", G_CALLBACK(ParentDestroyed), link);
#endif
    UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(button), ParentDestroyed, link);
    g_object_set_data_full(G_OBJECT(button), "umicom-broker-parent", link, FreeLink);
    g_signal_connect(button, "clicked", G_CALLBACK(OpenClicked), link);
    gtk_widget_set_halign(button, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), button); gtk_box_append(GTK_BOX(box), child);
    gtk_widget_set_vexpand(child, TRUE);
    return box;
}
