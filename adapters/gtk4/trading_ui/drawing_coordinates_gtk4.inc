/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/drawing_coordinates_gtk4.inc
 * PURPOSE: Edit precise chart anchors while retaining workspace history and instrument guards.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <errno.h>

/* Entry text preserves all 64-bit timestamp digits; a floating-point spin
 * button would round large millisecond values. Prices use GLib's locale-free
 * parser so a dot keeps the same meaning across operating-system locales. */
static UmiStatus ChartCoordinatesParse(const char *time_text, const char *price_text, UmiChartPoint *out)
{
    if (time_text == NULL || price_text == NULL || out == NULL || *time_text == '\0' || *price_text == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t time_bytes = strlen(time_text), price_bytes = strlen(price_text);
    if (time_bytes > 19U || price_bytes > 63U) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < time_bytes; ++index)
        if (time_text[index] < '0' || time_text[index] > '9') return UMI_STATUS_INVALID_ARGUMENT;
    /* Reject special values, hexadecimal syntax and whitespace before parsing.
     * Decimal signs, a decimal point and scientific notation remain supported. */
    for (size_t index = 0U; index < price_bytes; ++index) {
        char c = price_text[index];
        if (!(c >= '0' && c <= '9') && c != '+' && c != '-' && c != '.' && c != 'e' && c != 'E')
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    errno = 0; char *end = NULL;
    gint64 timestamp = g_ascii_strtoll(time_text, &end, 10);
    if (errno == ERANGE || end == time_text || *end != '\0' || timestamp < 0) return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    double value = g_ascii_strtod(price_text, &end);
    if (errno == ERANGE || end == price_text || *end != '\0' || !isfinite(value)) return UMI_STATUS_INVALID_ARGUMENT;
    *out = (UmiChartPoint){(int64_t)timestamp, value}; return UMI_STATUS_OK;
}

/* Loading is explicit so quote refreshes never overwrite a partially typed edit. */
static void ChartCoordinatesLoad(GtkButton *button, gpointer root)
{
    (void)button;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state) || state->geometry_loading) return;
    UmiChartDrawingSnapshot drawing; UmiChartDrawingKind kind;
    UmiStatus status = ChartSelectedDrawing(state, &drawing);
    if (status == UMI_STATUS_OK) status = UmiChartDrawingKindParse(drawing.tool, &kind);
    if (status != UMI_STATUS_OK) {
        state->geometry_id[0] = '\0'; gtk_widget_set_sensitive(state->geometry_fields, FALSE);
        gtk_label_set_text(GTK_LABEL(state->geometry_note), "Select a supported drawing, then Load coordinates."); return;
    }
    state->geometry_loading = 1;
    const int64_t times[] = {drawing.time1, drawing.time2};
    const double prices[] = {drawing.value1, drawing.value2};
    for (size_t index = 0U; index < 2U; ++index) {
        char timestamp_text[32], price[G_ASCII_DTOSTR_BUF_SIZE];
        (void)g_snprintf(timestamp_text, sizeof timestamp_text, "%" G_GINT64_FORMAT, (gint64)times[index]);
        (void)g_ascii_formatd(price, sizeof price, "%.17g", prices[index]);
        gtk_editable_set_text(GTK_EDITABLE(state->geometry_inputs[index * 2U]), timestamp_text);
        gtk_editable_set_text(GTK_EDITABLE(state->geometry_inputs[index * 2U + 1U]), price);
    }
    (void)snprintf(state->geometry_id, sizeof state->geometry_id, "%s", drawing.id);
    (void)snprintf(state->geometry_pane, sizeof state->geometry_pane, "%s", drawing.pane_id);
    state->geometry_revision = drawing.revision;
    int level = kind == UMI_CHART_DRAWING_SUPPORT || kind == UMI_CHART_DRAWING_RESISTANCE;
    gtk_widget_set_sensitive(state->geometry_fields, !drawing.locked);
    gtk_widget_set_sensitive(state->geometry_inputs[2], !level);
    gtk_widget_set_sensitive(state->geometry_inputs[3], !level);
    gtk_label_set_text(GTK_LABEL(state->geometry_note), drawing.locked
        ? "This drawing is locked. Unlock it, then Load coordinates before editing."
        : level ? "Edit the level's price. Times are Unix milliseconds; the second price follows the first. Apply changes only this drawing."
        : "Edit both anchors. Times are Unix milliseconds; prices use a decimal point. Apply creates one drawing Undo step. Save chart remains separate.");
    state->geometry_loading = 0;
}

/* Recheck the displayed drawing before parsing or publishing any geometry. */
static void ChartCoordinatesApply(GtkButton *button, gpointer root)
{
    (void)button;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state) || state->geometry_loading) return;
    UmiChartDrawingSnapshot selected; UmiChartDrawingKind kind;
    UmiStatus status = ChartSelectedDrawing(state, &selected);
    if (status == UMI_STATUS_OK) status = UmiChartDrawingKindParse(selected.tool, &kind);
    if (status != UMI_STATUS_OK || state->geometry_id[0] == '\0' ||
        strcmp(selected.id, state->geometry_id) != 0 || strcmp(selected.pane_id, state->geometry_pane) != 0 ||
        selected.revision != state->geometry_revision || selected.locked) {
        gtk_label_set_text(GTK_LABEL(state->geometry_note), "Drawing, selection or lock changed. Your fields were kept; Load coordinates again before applying."); return;
    }
    UmiChartPoint first, second;
    status = ChartCoordinatesParse(gtk_editable_get_text(GTK_EDITABLE(state->geometry_inputs[0])),
        gtk_editable_get_text(GTK_EDITABLE(state->geometry_inputs[1])), &first);
    int level = kind == UMI_CHART_DRAWING_SUPPORT || kind == UMI_CHART_DRAWING_RESISTANCE;
    /* Horizontal levels share a price, but their stored second timestamp may
     * differ. Keep that unused timestamp so Apply without edits is a true no-op. */
    if (status == UMI_STATUS_OK && level) second = (UmiChartPoint){selected.time2, first.value};
    else if (status == UMI_STATUS_OK) status = ChartCoordinatesParse(
        gtk_editable_get_text(GTK_EDITABLE(state->geometry_inputs[2])),
        gtk_editable_get_text(GTK_EDITABLE(state->geometry_inputs[3])), &second);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(state->geometry_note), "Use nonnegative whole-number milliseconds and finite decimal prices. Spaces, commas and special values are not accepted. No drawing changed."); return;
    }
    /* The existing workspace operation repeats instrument/revision checks and
     * records atomic drawing history. It cannot submit or modify an order. */
    status = UmiTradingWorkspaceMoveChartDrawing(state->context->workspace,
        state->geometry_pane, state->geometry_id, state->geometry_revision, first, second);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(state->geometry_note), "Coordinates were not applied. Check the selected instrument and drawing. Lines need different times; boxes also need different prices."); return;
    }
    ChartFinishMove(state); state->have_first = 0;
    ChartRefresh(state); ChartCoordinatesLoad(NULL, root);
    ChartMessage(state, "Coordinates accepted. Drawing Undo restores an actual change; Save chart stores it. Order drafts are unchanged.");
}

/* Semantic tags keep product tests independent of widget order and captions. */
static void ChartCoordinatesCreate(InteractiveChart *state)
{
    GtkWidget *section = gtk_expander_new("Drawing coordinates");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *load = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    (void)ChartButton(load, "Load coordinates", "trading.chart.coordinates-load", G_CALLBACK(ChartCoordinatesLoad), state->root);
    gtk_box_append(GTK_BOX(box), load);
    state->geometry_note = gtk_label_new("Select a drawing and Load coordinates. Price refreshes preserve your input.");
    gtk_label_set_wrap(GTK_LABEL(state->geometry_note), TRUE); gtk_label_set_xalign(GTK_LABEL(state->geometry_note), 0.0F);
    (void)umi_gtk4_automation_tag_widget(state->geometry_note, "trading.chart.coordinates-note");
    gtk_box_append(GTK_BOX(box), state->geometry_note);
    state->geometry_fields = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    const char *labels[] = {"First time (ms)", "First price", "Second time (ms)", "Second price"};
    const char *tags[] = {"trading.chart.coordinates-time-first", "trading.chart.coordinates-price-first",
        "trading.chart.coordinates-time-second", "trading.chart.coordinates-price-second"};
    for (size_t index = 0U; index < 4U; ++index) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
        state->geometry_inputs[index] = gtk_entry_new();
        gtk_entry_set_max_length(GTK_ENTRY(state->geometry_inputs[index]), index % 2U == 0U ? 19 : 63);
        gtk_widget_set_hexpand(state->geometry_inputs[index], TRUE);
        (void)umi_gtk4_automation_tag_widget(state->geometry_inputs[index], tags[index]);
        gtk_box_append(GTK_BOX(row), gtk_label_new(labels[index]));
        gtk_box_append(GTK_BOX(row), state->geometry_inputs[index]);
        gtk_box_append(GTK_BOX(state->geometry_fields), row);
    }
    (void)ChartButton(state->geometry_fields, "Apply coordinates", "trading.chart.coordinates-apply", G_CALLBACK(ChartCoordinatesApply), state->root);
    gtk_widget_set_sensitive(state->geometry_fields, FALSE);
    gtk_box_append(GTK_BOX(box), state->geometry_fields);
    gtk_expander_set_child(GTK_EXPANDER(section), box); gtk_box_append(GTK_BOX(state->root), section);
}
