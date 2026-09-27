/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/market_tape_gtk4.c
 *
 * PURPOSE:
 *   Render one copied market-tape observation across all linked practice views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/market_tape.h"
#include "umicom/trading/market_tape_practice.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/* Reuse the existing canonical, embedded Umicom logo and icon. */
#include "desktop_system_brand.inc"

typedef struct MarketTapeView {
    UmiMarketTapePractice *practice;
    UmiMarketTapeSnapshot snapshot;
    GtkWidget *watch[2];
    GtkWidget *summary, *status, *chart, *sales, *depth, *bars;
    GtkWidget *next, *gap;
    bool closed;
} MarketTapeView;

static MarketTapeView *Owner(GtkWindow *window)
{
    MarketTapeView *view = g_object_get_data(G_OBJECT(window), "umicom-market-tape-owner");
    return view != NULL && !view->closed ? view : NULL;
}
static void ReleaseView(gpointer data)
{
    MarketTapeView *view = data;
    if (view == NULL) return;
    UmiMarketTapePracticeDestroy(view->practice);
    free(view);
}
static void CloseView(GtkWidget *widget, gpointer unused)
{
    (void)unused;
    MarketTapeView *view = g_object_get_data(G_OBJECT(widget), "umicom-market-tape-owner");
    if (view != NULL) view->closed = true;
}
static GtkWidget *Label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    return label;
}
static GtkWidget *ReadOnlyText(void)
{
    GtkWidget *text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text), TRUE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text), 10);
    return text;
}
static const char *Fresh(bool present, bool fresh)
{
    return !present ? "not observed" : fresh ? "fresh" : "NOT current";
}
static void SetText(GtkWidget *widget, GString *text)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget)), text->str, -1);
    g_string_free(text, TRUE);
}
/* One read supplies every child. There is no second selection, quote cache or
 * separate timer that could attach one instrument's ladder to another chart. */
static void Refresh(MarketTapeView *view)
{
    UmiStatus result = UmiMarketTapePracticeRead(view->practice, &view->snapshot);
    if (result != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(view->status), umi_status_text(result));
        return;
    }
    UmiMarketTapeSnapshot *s = &view->snapshot;
    const UmiMarketTapeRow *row = &s->rows[s->selectedIndex];
    for (size_t i = 0U; i < 2U; ++i) {
        const UmiMarketTapeRow *item = &s->rows[i];
        char *line = item->haveQuote ? g_strdup_printf("%s%s | bid %.3f / ask %.3f | %s | quote %s",
            i == s->selectedIndex ? "> " : "", item->instrument.symbol,
            item->quote.bid, item->quote.ask, UmiMarketTapeStateText(item->state),
            Fresh(item->haveQuote, item->quoteFresh)) :
            g_strdup_printf("%s%s | no quote | %s", i == s->selectedIndex ? "> " : "",
                item->instrument.symbol, UmiMarketTapeStateText(item->state));
        gtk_button_set_label(GTK_BUTTON(view->watch[i]), line);
        g_free(line);
    }
    char *summary = g_strdup_printf(
        "%s / %s | generation %" PRIu64 " | revision %" PRIu64 " | practice clock %" PRId64 " ms\n"
        "Stream: %s | expected sequence %" PRIu64 " | last accepted %" PRIu64 " | gap observed %" PRIu64 "\n"
        "Quote: %s | last trade: %s | two-sided depth: %s\n"
        "Retained %zu trades / %zu bars; retired by capacity %" PRIu64 " trades / %" PRIu64 " bars.",
        row->instrument.symbol, row->instrument.venue, row->generation, s->revision, s->observedAtMs,
        UmiMarketTapeStateText(row->state), row->expectedSequence, row->lastSequence, row->gapObservedSequence,
        Fresh(row->haveQuote, row->quoteFresh), Fresh(row->haveTrade, row->tradeFresh),
        Fresh(row->haveDepth, row->depthFresh), s->tradeCount, s->barCount,
        row->discardedTrades, row->discardedBars);
    gtk_label_set_text(GTK_LABEL(view->summary), summary);
    g_free(summary);
    GString *trades = g_string_new("Source time (ms)       Price        Quantity\n");
    for (size_t i = 0U; i < s->tradeCount; ++i)
        g_string_append_printf(trades, "%16" PRId64 "  %12.3f  %12.3f\n",
            s->trades[i].event_time_ms, s->trades[i].price, s->trades[i].size);
    SetText(view->sales, trades);
    GString *ladder = g_string_new("Full retained snapshot; NOT resting orders or executable liquidity.\n\nBids (highest first)\n    Price          Quantity\n");
    for (size_t i = 0U; i < s->depth.bid_count; ++i)
        g_string_append_printf(ladder, "%12.3f  %12.3f\n", s->depth.bids[i].price, s->depth.bids[i].size);
    g_string_append(ladder, "\nAsks (lowest first)\n    Price          Quantity\n");
    for (size_t i = 0U; i < s->depth.ask_count; ++i)
        g_string_append_printf(ladder, "%12.3f  %12.3f\n", s->depth.asks[i].price, s->depth.asks[i].size);
    SetText(view->depth, ladder);
    GString *bars = g_string_new("Buckets [start,end); last bucket may still be forming. No empty bars are invented.\n\nStart        End          Open       High        Low       Close       Volume\n");
    for (size_t i = 0U; i < s->barCount; ++i) {
        const UmiBar *bar = &s->bars[i];
        g_string_append_printf(bars, "%10" PRId64 " %10" PRId64 " %10.3f %10.3f %10.3f %10.3f %10.3f\n",
            bar->start_time_ms, bar->end_time_ms, bar->open, bar->high, bar->low, bar->close, bar->volume);
    }
    SetText(view->bars, bars);
    bool enabled = row->connected && !row->gapLatched && !row->sequenceExhausted;
    gtk_widget_set_sensitive(view->next, enabled);
    gtk_widget_set_sensitive(view->gap, enabled);
    gtk_widget_queue_draw(view->chart);
}
/* Drawing keeps only a weak reference. A retained drawing area may survive the
 * window, so its draw function must not retain a raw pointer to the tape. */
static void ReleaseWeak(gpointer data)
{
    GWeakRef *weak = data;
    g_weak_ref_clear(weak);
    g_free(weak);
}
static double PriceY(double value, double minimum, double span, double height)
{
    return 20.0 + (1.0 - (value - minimum) / span) * height;
}
static void Draw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data)
{
    GObject *object = g_weak_ref_get(data);
    if (object == NULL) return;
    MarketTapeView *view = Owner(GTK_WINDOW(object));
    if (view == NULL || width < 160 || height < 100) { g_object_unref(object); return; }
    GdkRGBA foreground;
    gtk_widget_get_color(GTK_WIDGET(area), &foreground);
    gdk_cairo_set_source_rgba(cr, &foreground);
    cairo_set_font_size(cr, 12.0);
    UmiMarketTapeSnapshot *s = &view->snapshot;
    if (s->barCount == 0U) {
        cairo_move_to(cr, 14.0, 34.0);
        cairo_show_text(cr, "No trades observed in this epoch.");
        g_object_unref(object);
        return;
    }
    double low = s->bars[0].low, high = s->bars[0].high;
    for (size_t i = 1U; i < s->barCount; ++i) {
        low = fmin(low, s->bars[i].low); high = fmax(high, s->bars[i].high);
    }
    double span = high - low;
    if (!isfinite(span)) { g_object_unref(object); return; }
    if (span == 0.0) { low -= 0.5; span = 1.0; }
    double plotWidth = (double)width - 102.0;
    double plotHeight = (double)height - 62.0;
    int64_t first = s->bars[0].start_time_ms;
    int64_t last = s->bars[s->barCount - 1U].end_time_ms;
    double duration = (double)(last - first);
    if (duration <= 0.0) { g_object_unref(object); return; }
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, 78.0, 20.0); cairo_line_to(cr, 78.0, 20.0 + plotHeight);
    cairo_line_to(cr, 78.0 + plotWidth, 20.0 + plotHeight); cairo_stroke(cr);
    for (size_t i = 0U; i < s->barCount; ++i) {
        const UmiBar *bar = &s->bars[i];
        double left = 78.0 + (double)(bar->start_time_ms - first) / duration * plotWidth;
        double bucket = (double)(bar->end_time_ms - bar->start_time_ms) / duration * plotWidth;
        double centre = left + bucket * 0.5;
        double bodyWidth = fmax(1.0, bucket * 0.62);
        double top = PriceY(fmax(bar->open, bar->close), low, span, plotHeight);
        double bottom = PriceY(fmin(bar->open, bar->close), low, span, plotHeight);
        cairo_move_to(cr, centre, PriceY(bar->high, low, span, plotHeight));
        cairo_line_to(cr, centre, PriceY(bar->low, low, span, plotHeight)); cairo_stroke(cr);
        cairo_rectangle(cr, centre - bodyWidth * 0.5, top, bodyWidth, fmax(1.0, bottom - top));
        if (bar->close < bar->open) cairo_fill(cr); else cairo_stroke(cr);
    }
    char label[100];
    (void)snprintf(label, sizeof(label), "%.3f", high);
    cairo_move_to(cr, 5.0, 27.0); cairo_show_text(cr, label);
    (void)snprintf(label, sizeof(label), "%.3f", low);
    cairo_move_to(cr, 5.0, 20.0 + plotHeight); cairo_show_text(cr, label);
    (void)snprintf(label, sizeof(label), "[%" PRId64 ", %" PRId64 ") ms; hollow = non-falling", first, last);
    cairo_move_to(cr, 78.0, (double)height - 14.0); cairo_show_text(cr, label);
    g_object_unref(object);
}
static void Action(GtkButton *button, gpointer data)
{
    GtkWindow *window = data;
    MarketTapeView *view = Owner(window);
    if (view == NULL || gtk_widget_get_root(GTK_WIDGET(button)) != GTK_ROOT(window)) return;
    UmiMarketTapePracticeAction action = (UmiMarketTapePracticeAction)GPOINTER_TO_INT(
        g_object_get_data(G_OBJECT(button), "umicom-market-tape-action"));
    UmiMarketTapeRejection reason = UMI_MARKET_TAPE_ACCEPTED;
    UmiStatus status = UmiMarketTapePracticeAct(view->practice, action, &reason);
    const char *message = "Observation accepted; every pane reads the same copied snapshot.";
    if (status != UMI_STATUS_OK) message = UmiMarketTapeRejectionText(reason);
    else if (action == UMI_MARKET_PRACTICE_NEW_EPOCH)
        message = "New epoch: this instrument's retained observations were discarded, not repaired.";
    else if (action == UMI_MARKET_PRACTICE_AGE)
        message = "Practice clock advanced by three seconds. No feed event was added.";
    else if (action == UMI_MARKET_PRACTICE_DISCONNECT)
        message = "Disconnected. Retained observations remain visible but are not current.";
    gtk_label_set_text(GTK_LABEL(view->status), message);
    Refresh(view);
}
static void SelectInstrument(GtkButton *button, gpointer data)
{
    GtkWindow *window = data;
    MarketTapeView *view = Owner(window);
    if (view == NULL || gtk_widget_get_root(GTK_WIDGET(button)) != GTK_ROOT(window)) return;
    size_t index = (size_t)GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "umicom-market-tape-index"));
    UmiStatus status = UmiMarketTapePracticeSelect(view->practice, index);
    gtk_label_set_text(GTK_LABEL(view->status), status == UMI_STATUS_OK ?
        "Selected instrument changed across the chart, tape and depth panes." : umi_status_text(status));
    Refresh(view);
}
static GtkWidget *ActionButton(GtkWindow *window, GtkWidget *box, const char *text,
    UmiMarketTapePracticeAction action, const char *hint)
{
    GtkWidget *button = gtk_button_new_with_label(text);
    gtk_widget_set_name(button, text);
    gtk_widget_set_tooltip_text(button, hint);
    g_object_set_data(G_OBJECT(button), "umicom-market-tape-action", GINT_TO_POINTER((int)action));
    g_signal_connect_object(button, "clicked", G_CALLBACK(Action), window, 0);
    gtk_box_append(GTK_BOX(box), button);
    return button;
}
static void AddTab(GtkWidget *notebook, GtkWidget *content, const char *name)
{
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), content);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, gtk_label_new(name));
}
static GtkWidget *Brand(void)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GBytes *bytes = g_bytes_new_static(UMICOM_SYSTEM_LOGO, sizeof(UMICOM_SYSTEM_LOGO));
    GdkTexture *texture = gdk_texture_new_from_bytes(bytes, NULL);
    if (texture != NULL) {
        GtkWidget *picture = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture));
        gtk_widget_set_size_request(picture, 160, 44);
        gtk_box_append(GTK_BOX(row), picture);
        g_object_unref(texture);
    }
    g_bytes_unref(bytes);
    /* Use the existing icon bytes too; no path lookup through the CWD. */
    bytes = g_bytes_new_static(UMICOM_SYSTEM_ICON, sizeof(UMICOM_SYSTEM_ICON));
    texture = gdk_texture_new_from_bytes(bytes, NULL);
    if (texture != NULL) {
        GtkWidget *picture = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture));
        gtk_widget_set_size_request(picture, 32, 32);
        gtk_box_append(GTK_BOX(row), picture);
        g_object_unref(texture);
    }
    g_bytes_unref(bytes);
    gtk_box_append(GTK_BOX(row), Label("Linked market tape\nFictional practice — no live provider or order entry"));
    return row;
}
GtkWindow *UmiMarketTapeGtkCreate(GtkWindow *parent)
{
    MarketTapeView *view = calloc(1U, sizeof(*view));
    if (view == NULL) return NULL;
    if (UmiMarketTapePracticeCreate(&view->practice) != UMI_STATUS_OK) { free(view); return NULL; }
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    g_object_set_data_full(G_OBJECT(window), "umicom-market-tape-owner", view, ReleaseView);
    g_signal_connect(window, "destroy", G_CALLBACK(CloseView), NULL);
    gtk_window_set_title(window, "Umicom — Linked market tape (practice)");
    gtk_window_set_default_size(window, 1040, 800);
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent);
        gtk_window_set_destroy_with_parent(window, TRUE);
        GtkApplication *app = gtk_window_get_application(parent);
        if (app != NULL) gtk_window_set_application(window, app);
    }
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(box, 14); gtk_widget_set_margin_end(box, 14);
    gtk_widget_set_margin_top(box, 14); gtk_widget_set_margin_bottom(box, 14);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    gtk_window_set_child(window, scroll);
    gtk_box_append(GTK_BOX(box), Brand());
    for (size_t i = 0U; i < 2U; ++i) {
        view->watch[i] = gtk_button_new();
        gtk_widget_set_name(view->watch[i], i == 0U ? "Select ALPHA" : "Select BETA");
        g_object_set_data(G_OBJECT(view->watch[i]), "umicom-market-tape-index", GUINT_TO_POINTER((guint)i));
        g_signal_connect_object(view->watch[i], "clicked", G_CALLBACK(SelectInstrument), window, 0);
        gtk_box_append(GTK_BOX(box), view->watch[i]);
    }
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *secondary = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(box), actions); gtk_box_append(GTK_BOX(box), secondary);
    view->next = ActionButton(window, actions, "Next observation", UMI_MARKET_PRACTICE_NEXT,
        "Add a fictional quote, complete depth snapshot and trade for the selected instrument.");
    (void)ActionButton(window, actions, "Advance 3 seconds", UMI_MARKET_PRACTICE_AGE,
        "Advance the shared practice clock without receiving another event.");
    view->gap = ActionButton(window, secondary, "Introduce gap", UMI_MARKET_PRACTICE_GAP,
        "Skip the expected sequence. The later quote is rejected and continuity remains lost.");
    (void)ActionButton(window, secondary, "Disconnect", UMI_MARKET_PRACTICE_DISCONNECT,
        "Stop accepting observations while retaining the visible history.");
    (void)ActionButton(window, secondary, "New epoch", UMI_MARKET_PRACTICE_NEW_EPOCH,
        "Discard this instrument's history and begin a new generation. Missing events are not recovered.");
    view->status = Label("Select an instrument, then add observations. Time advances only through these controls.");
    view->summary = Label("");
    gtk_box_append(GTK_BOX(box), view->status); gtk_box_append(GTK_BOX(box), view->summary);
    view->chart = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(view->chart), 520);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(view->chart), 230);
    gtk_widget_set_hexpand(view->chart, TRUE);
    gtk_widget_set_tooltip_text(view->chart, "Canonical OHLC bars. Empty time buckets stay empty; see Bars for exact values.");
    GWeakRef *weak = g_new0(GWeakRef, 1);
    g_weak_ref_init(weak, G_OBJECT(window));
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(view->chart), Draw, weak, ReleaseWeak);
    gtk_box_append(GTK_BOX(box), view->chart);
    GtkWidget *tabs = gtk_notebook_new();
    gtk_widget_set_size_request(tabs, -1, 220);
    gtk_widget_set_vexpand(tabs, TRUE);
    view->sales = ReadOnlyText(); view->depth = ReadOnlyText(); view->bars = ReadOnlyText();
    AddTab(tabs, view->sales, "Time & Sales");
    AddTab(tabs, view->depth, "Depth ladder");
    AddTab(tabs, view->bars, "Bars");
    gtk_box_append(GTK_BOX(box), tabs);
    Refresh(view);
    return window;
}
static void OpenPractice(GtkButton *button, gpointer data)
{
    GtkWindow *parent = data;
    if (gtk_widget_get_root(GTK_WIDGET(button)) != GTK_ROOT(parent) ||
        !gtk_widget_get_sensitive(GTK_WIDGET(button))) return;
    GtkWindow *window = UmiMarketTapeGtkCreate(parent);
    if (window != NULL) gtk_window_present(window);
    else gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Could not allocate the practice workspace. No trading state was changed.");
}
static void DisableEntry(GtkWidget *parent, gpointer data)
{
    (void)parent;
    gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
}
GtkWidget *UmiMarketTapeGtkWrap(GtkWidget *content, GtkWindow *parent)
{
    if (content == NULL || parent == NULL || gtk_widget_get_parent(content) != NULL) return content;
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *button = gtk_button_new_with_label("Open linked market tape");
    gtk_widget_set_tooltip_text(button, "An independent fictional market-data workspace. Existing trading controls stay unchanged.");
    g_signal_connect_object(button, "clicked", G_CALLBACK(OpenPractice), parent, 0);
    g_signal_connect_object(parent, "destroy", G_CALLBACK(DisableEntry), button, 0);
    gtk_box_append(GTK_BOX(box), button);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_box_append(GTK_BOX(box), content);
    return box;
}
