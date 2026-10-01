/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/drawing_appearance_gtk4.inc
 * PURPOSE: Edit a copied drawing appearance without rebinding an unfinished draft during chart refresh.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included after ChartSelectedDrawing. The editor deliberately loads a copied
 * row on command. Quote timers never rewrite entries or silently retarget them. */
static void ChartAppearanceLoad(GtkButton *button, gpointer root)
{
    (void)button;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
    UmiChartDrawingSnapshot drawing;
    UmiStatus status = ChartSelectedDrawing(state, &drawing);
    if (status != UMI_STATUS_OK) {
        state->appearance_id[0] = '\0';
        gtk_widget_set_sensitive(state->appearance_fields, FALSE);
        gtk_label_set_text(GTK_LABEL(state->appearance_note), "Select a current drawing, then choose Load selected.");
        return;
    }
    UmiChartPlotStyle theme;
    umi_chart_plot_style_dark(&theme);
    UmiChartDrawingResolvedAppearance resolved;
    status = UmiChartDrawingAppearanceResolve(&drawing, &theme, &resolved);
    if (status != UMI_STATUS_OK) return;
    UmiChartDrawingAppearance appearance;
    UmiStatus decoded = UmiChartDrawingAppearanceDecode(drawing.style, sizeof drawing.style, &appearance);
    if (decoded != UMI_STATUS_OK) {
        appearance.red = (uint32_t)lround(resolved.outline.red * 255.0);
        appearance.green = (uint32_t)lround(resolved.outline.green * 255.0);
        appearance.blue = (uint32_t)lround(resolved.outline.blue * 255.0);
        appearance.width_tenths = (uint32_t)lround(resolved.width * 10.0);
        appearance.fill_percent = (uint32_t)lround(resolved.fill.alpha * 100.0);
    }
    char color[8], note[480];
    (void)snprintf(color, sizeof color, "#%02X%02X%02X", (unsigned)appearance.red, (unsigned)appearance.green, (unsigned)appearance.blue);
    gtk_editable_set_text(GTK_EDITABLE(state->appearance_color), color);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->appearance_width), appearance.width_tenths / 10.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->appearance_fill), appearance.fill_percent);
    (void)snprintf(state->appearance_id, sizeof state->appearance_id, "%s", drawing.id);
    (void)snprintf(state->appearance_pane, sizeof state->appearance_pane, "%s", drawing.pane_id);
    state->appearance_revision = drawing.revision;
    int box = strcmp(drawing.tool, "range") == 0 || strcmp(drawing.tool, "liquidity-zone") == 0;
    gtk_widget_set_sensitive(state->appearance_fields, TRUE);
    gtk_widget_set_sensitive(state->appearance_fill, box);
    (void)snprintf(note, sizeof note, "%s | %s. %s",
        drawing.id, drawing.tool,
        decoded == UMI_STATUS_OK ? "Edit the fields, then Apply. Save chart keeps the change." :
        decoded == UMI_STATUS_NOT_FOUND ? "Tool defaults loaded. Apply stores a custom appearance; Use tool defaults keeps theme colours." :
        "Unrecognised saved style retained. Apply or Use tool defaults explicitly replaces it. Leave this editor to keep it.");
    /* Imported identities are bounded bytes, not necessarily UTF-8. Match the
     * object list's display conversion without changing the canonical ID. */
    char *display = g_utf8_make_valid(note, -1);
    gtk_label_set_text(GTK_LABEL(state->appearance_note), display);
    g_free(display);
}

static void ChartAppearanceApply(GtkButton *button, gpointer root)
{
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
    UmiChartDrawingSnapshot selected;
    UmiStatus status = ChartSelectedDrawing(state, &selected);
    if (status != UMI_STATUS_OK || state->appearance_id[0] == '\0' ||
        strcmp(selected.id, state->appearance_id) != 0 ||
        strcmp(selected.pane_id, state->appearance_pane) != 0 ||
        selected.revision != state->appearance_revision) {
        gtk_label_set_text(GTK_LABEL(state->appearance_note),
            "Drawing or selection changed. Your fields were kept; choose Load selected before applying a new edit.");
        return;
    }
    int defaults = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "appearance-defaults"));
    UmiChartDrawingAppearance appearance;
    if (!defaults) {
        /* Convert displayed logical pixels to exact integer tenths. The shared
         * parser checks all bounds again before the model can change. */
        gtk_spin_button_update(GTK_SPIN_BUTTON(state->appearance_width));
        gtk_spin_button_update(GTK_SPIN_BUTTON(state->appearance_fill));
        status = UmiChartDrawingAppearanceFromHex(
            gtk_editable_get_text(GTK_EDITABLE(state->appearance_color)),
            (uint32_t)lround(gtk_spin_button_get_value(GTK_SPIN_BUTTON(state->appearance_width)) * 10.0),
            (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(state->appearance_fill)), &appearance);
        if (status != UMI_STATUS_OK) {
            gtk_label_set_text(GTK_LABEL(state->appearance_note), "Use a colour such as #33AAFF, width 1.0 to 8.0, and fill 0 to 80 percent. No change was made.");
            return;
        }
    }
    status = UmiTradingWorkspaceSetChartDrawingAppearance(state->context->workspace,
        state->appearance_pane, state->appearance_id, state->appearance_revision, defaults ? NULL : &appearance);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(state->appearance_note), "Appearance unchanged. The instrument or drawing changed; Load selected and review again.");
        return;
    }
    ChartFinishMove(state);
    state->have_first = 0;
    ChartRefresh(state);
    ChartAppearanceLoad(NULL, root);
    ChartMessage(state, defaults ? "Tool appearance restored. Save chart to keep this choice." :
        "Drawing appearance applied. Save chart to keep this choice.");
}

static void ChartAppearanceCreate(InteractiveChart *state)
{
    GtkWidget *expander = gtk_expander_new("Drawing appearance");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *loadRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    (void)ChartButton(loadRow, "Load selected", "trading.chart.appearance-load", G_CALLBACK(ChartAppearanceLoad), state->root);
    gtk_box_append(GTK_BOX(box), loadRow);
    state->appearance_note = gtk_label_new("Select a drawing and choose Load selected. Unsaved fields stay unchanged during price updates.");
    gtk_label_set_wrap(GTK_LABEL(state->appearance_note), TRUE);
    gtk_label_set_xalign(GTK_LABEL(state->appearance_note), 0);
    (void)umi_gtk4_automation_tag_widget(state->appearance_note, "trading.chart.appearance-note");
    gtk_box_append(GTK_BOX(box), state->appearance_note);
    state->appearance_fields = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *fields = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    state->appearance_color = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(state->appearance_color), 7);
    gtk_editable_set_width_chars(GTK_EDITABLE(state->appearance_color), 8);
    state->appearance_width = gtk_spin_button_new_with_range(1, 8, 0.1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(state->appearance_width), 1);
    state->appearance_fill = gtk_spin_button_new_with_range(0, 80, 1);
    GtkWidget *inputs[] = {state->appearance_color, state->appearance_width, state->appearance_fill};
    const char *labels[] = {"Colour", "Width", "Fill %"};
    const char *tags[] = {"trading.chart.appearance-color", "trading.chart.appearance-width", "trading.chart.appearance-fill"};
    for (size_t i = 0U; i < 3U; ++i) {
        gtk_box_append(GTK_BOX(fields), gtk_label_new(labels[i]));
        gtk_box_append(GTK_BOX(fields), inputs[i]);
        (void)umi_gtk4_automation_tag_widget(inputs[i], tags[i]);
        gtk_widget_set_tooltip_text(inputs[i], i == 0U ? "RGB hexadecimal colour, for example #33AAFF." :
            i == 1U ? "Outline width in logical chart pixels, 1.0 to 8.0." :
            "Fill opacity for range and liquidity boxes only. Zero means no fill.");
    }
    gtk_box_append(GTK_BOX(state->appearance_fields), fields);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    (void)ChartButton(actions, "Apply appearance", "trading.chart.appearance-apply", G_CALLBACK(ChartAppearanceApply), state->root);
    GtkWidget *reset = ChartButton(actions, "Use tool defaults", "trading.chart.appearance-defaults", G_CALLBACK(ChartAppearanceApply), state->root);
    g_object_set_data(G_OBJECT(reset), "appearance-defaults", GINT_TO_POINTER(1));
    gtk_box_append(GTK_BOX(state->appearance_fields), actions);
    gtk_widget_set_sensitive(state->appearance_fields, FALSE);
    gtk_box_append(GTK_BOX(box), state->appearance_fields);
    gtk_expander_set_child(GTK_EXPANDER(expander), box);
    gtk_box_append(GTK_BOX(state->root), expander);
}
