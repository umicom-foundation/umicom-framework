/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/cash_plan_gtk4.c
 * PURPOSE: Render shared cash assumptions with reversible edits and explicit file review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "cash_plan_private.h"
#include "umicom/ui/gtk4/drop_down.h"
#include <stdlib.h>
/* Retain controls as well as the root-owned model. A notification handler may
 * destroy the window while an action is publishing labels; its in-flight
 * callback still has valid controls until the root reference is released. */
static void Destroy(gpointer value)
{
    CashPanel *panel = value;
    UmiCashPlanDestroy(panel->plan);
    UmiCashPlanDestroy(panel->previous);
    UmiCashPlanDestroy(panel->pending);
    g_clear_object(&panel->names);
    g_ptr_array_unref(panel->retained);
    g_free(panel);
}
static GtkWidget *Keep(CashPanel *panel, GtkWidget *widget, const char *id)
{
    g_ptr_array_add(panel->retained, g_object_ref_sink(widget));
    (void)umi_gtk4_automation_tag_widget(widget, id);
    return widget;
}
void UmiCashPanelInvalidate(CashPanel *panel)
{
    UmiCashPlanDestroy(panel->pending);
    panel->pending = NULL;
    if (panel->generation != UINT64_MAX)
        ++panel->generation;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
}
static void Changed(GObject *control, gpointer root)
{
    CashPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-cash-plan");
    if (panel == NULL || panel->updating)
        return;
    g_object_ref(root);
    ++panel->updating;
    UmiCashPanelInvalidate(panel);
    int field = GPOINTER_TO_INT(g_object_get_data(control, "cash-field")) - 1;
    if (field >= CASH_TITLE && field <= CASH_END)
        panel->configDirty = 1;
    if (field >= CASH_ID && field <= CASH_AMOUNT)
        panel->entryDirty = 1;
    gtk_label_set_text(
        GTK_LABEL(panel->note),
        "Fields changed. Apply settings or add/update the entry. Reports and saves use stored assumptions.");
    --panel->updating;
    g_object_unref(root);
}
static const char *Text(CashPanel *panel, int field)
{
    return gtk_editable_get_text(GTK_EDITABLE(panel->fields[field]));
}
static UmiStatus MoneyText(const char *text, uint8_t scale, int64_t *out)
{
    UmiDecimal d;
    UmiStatus status = UmiDecimalParse(text, strlen(text), scale, &d);
    if (status == UMI_STATUS_OK)
        *out = d.coefficient;
    return status;
}
static void SetMoney(GtkWidget *entry, int64_t value, uint8_t scale)
{
    char text[UMI_DECIMAL_TEXT_MAX];
    if (UmiDecimalFormat((UmiDecimal){value, scale}, text, sizeof text) == UMI_STATUS_OK)
        gtk_editable_set_text(GTK_EDITABLE(entry), text);
}
static void SetDate(GtkWidget *entry, UmiFinancialDate value)
{
    char text[11];
    if (UmiCashPlanDateFormat(value, text, sizeof text) == UMI_STATUS_OK)
        gtk_editable_set_text(GTK_EDITABLE(entry), text);
}
static void EntryFields(CashPanel *panel)
{
    UmiCashPlanEntry entry = {0};
    UmiCashPlanConfig config;
    (void)UmiCashPlanRead(panel->plan, &config);
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
    if (UmiCashPlanAt(panel->plan, (size_t)selected, &entry) != UMI_STATUS_OK)
    {
        entry.date = config.start;
        entry.amount = config.opening;
        entry.amount.minor_units = 0;
        entry.enabled = 1;
        entry.direction = UMI_FINANCIAL_DIRECTION_RECEIVE;
    }
    ++panel->updating;
    gtk_editable_set_text(GTK_EDITABLE(panel->fields[CASH_ID]), entry.id);
    gtk_editable_set_text(GTK_EDITABLE(panel->fields[CASH_LABEL]), entry.label);
    SetDate(panel->fields[CASH_DATE], entry.date);
    SetMoney(panel->fields[CASH_AMOUNT], entry.amount.minor_units, config.opening.scale);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->receive),
                                entry.direction == UMI_FINANCIAL_DIRECTION_RECEIVE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->enabled), entry.enabled);
    --panel->updating;
    panel->entryDirty = 0;
}
void UmiCashPanelSync(CashPanel *panel)
{
    UmiCashPlanConfig config;
    (void)UmiCashPlanRead(panel->plan, &config);
    ++panel->updating;
    gtk_editable_set_text(GTK_EDITABLE(panel->fields[CASH_TITLE]), config.title);
    gtk_editable_set_text(GTK_EDITABLE(panel->fields[CASH_CURRENCY]), config.opening.currency.code);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->scale), (double)config.opening.scale);
    SetMoney(panel->fields[CASH_OPENING], config.opening.minor_units, config.opening.scale);
    SetMoney(panel->fields[CASH_BUFFER], config.buffer_minor, config.opening.scale);
    SetDate(panel->fields[CASH_START], config.start);
    SetDate(panel->fields[CASH_END], config.end);
    gtk_string_list_splice(panel->names, 0U, g_list_model_get_n_items(G_LIST_MODEL(panel->names)), NULL);
    for (size_t i = 0U; i < UmiCashPlanCount(panel->plan); ++i)
    {
        UmiCashPlanEntry entry;
        (void)UmiCashPlanAt(panel->plan, i, &entry);
        char *name = g_strdup_printf("%s | %s%s", entry.id, entry.label, entry.enabled ? "" : " (disabled)");
        gtk_string_list_append(panel->names, name);
        g_free(name);
    }
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->selector), GTK_INVALID_LIST_POSITION);
    --panel->updating;
    panel->configDirty = 0;
    EntryFields(panel);
}
void UmiCashPanelRender(CashPanel *panel)
{
    UmiCashPlanForecast *forecast = g_new0(UmiCashPlanForecast, 1);
    UmiStatus status = UmiCashPlanProject(panel->plan, forecast);
    GString *text = g_string_new("Stored assumptions - local planning only\n");
    if (status != UMI_STATUS_OK)
        g_string_append_printf(text, "Projection refused: %s. Correct the stored amounts before exporting.\n",
                               umi_status_text(status));
    else
    {
        char start[11], end[11], closing[96], minimum[96], date[11];
        (void)UmiCashPlanDateFormat(forecast->config.start, start, sizeof start);
        (void)UmiCashPlanDateFormat(forecast->config.end, end, sizeof end);
        (void)UmiDecimalFormat((UmiDecimal){forecast->closing_minor, forecast->config.opening.scale}, closing,
                               sizeof closing);
        (void)UmiDecimalFormat((UmiDecimal){forecast->minimum_minor, forecast->config.opening.scale}, minimum,
                               sizeof minimum);
        g_string_append_printf(text, "%s\n%s to %s | %s | scale %u\nClosing: %s | Minimum: %s\n",
                               forecast->config.title, start, end, forecast->config.opening.currency.code,
                               (unsigned)forecast->config.opening.scale, closing, minimum);
        if (forecast->has_shortfall)
        {
            (void)UmiCashPlanDateFormat(forecast->first_shortfall_date, date, sizeof date);
            g_string_append_printf(text, "First buffer shortfall: %s\n", date);
        }
        else
            g_string_append(text, "No opening or end-of-day buffer shortfall in these assumptions.\n");
        g_string_append_printf(text, "Included: %zu | Disabled: %zu | Before range: %zu | After range: %zu\n",
                               forecast->included, forecast->disabled, forecast->before_start,
                               forecast->after_end);
        for (size_t i = 0U; i < forecast->day_count; ++i)
        {
            char in[96], out[96], balance[96], headroom[96];
            UmiCashPlanDay *d = &forecast->days[i];
            (void)UmiCashPlanDateFormat(d->date, date, sizeof date);
            (void)UmiDecimalFormat((UmiDecimal){d->inflow_minor, forecast->config.opening.scale}, in,
                                   sizeof in);
            (void)UmiDecimalFormat((UmiDecimal){d->outflow_minor, forecast->config.opening.scale}, out,
                                   sizeof out);
            (void)UmiDecimalFormat((UmiDecimal){d->closing_minor, forecast->config.opening.scale}, balance,
                                   sizeof balance);
            (void)UmiDecimalFormat((UmiDecimal){d->headroom_minor, forecast->config.opening.scale}, headroom,
                                   sizeof headroom);
            g_string_append_printf(text, "%s | in %s | out %s | closing %s | headroom %s\n", date, in, out,
                                   balance, headroom);
        }
    }
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->output)), text->str, -1);
    g_string_free(text, TRUE);
    g_free(forecast);
}
static void Selected(GObject *selector, GParamSpec *spec, gpointer root)
{
    (void)selector;
    (void)spec;
    CashPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-cash-plan");
    if (panel == NULL || panel->updating)
        return;
    g_object_ref(root);
    ++panel->updating;
    UmiCashPanelInvalidate(panel);
    EntryFields(panel);
    gtk_label_set_text(GTK_LABEL(panel->note),
                       "Selected entry copied to the editor. Update stores changes; Add needs a unique ID.");
    --panel->updating;
    g_object_unref(root);
}
static UmiStatus ConfigFields(CashPanel *panel, UmiCashPlan *next)
{
    UmiCashPlanConfig config = {0};
    g_strlcpy(config.title, Text(panel, CASH_TITLE), sizeof config.title);
    g_strlcpy(config.opening.currency.code, Text(panel, CASH_CURRENCY), sizeof config.opening.currency.code);
    if (strlen(Text(panel, CASH_TITLE)) >= sizeof config.title || strlen(Text(panel, CASH_CURRENCY)) != 3U)
        return UMI_STATUS_INVALID_ARGUMENT;
    config.opening.scale = (uint8_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->scale));
    UmiStatus status =
        MoneyText(Text(panel, CASH_OPENING), config.opening.scale, &config.opening.minor_units);
    if (status == UMI_STATUS_OK)
        status = MoneyText(Text(panel, CASH_BUFFER), config.opening.scale, &config.buffer_minor);
    if (status == UMI_STATUS_OK)
        status =
            UmiCashPlanDateParse(Text(panel, CASH_START), strlen(Text(panel, CASH_START)), &config.start);
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanDateParse(Text(panel, CASH_END), strlen(Text(panel, CASH_END)), &config.end);
    return status == UMI_STATUS_OK ? UmiCashPlanSetConfig(next, &config) : status;
}
static UmiStatus ReadEntry(CashPanel *panel, UmiCashPlanEntry *entry)
{
    UmiCashPlanConfig config;
    (void)UmiCashPlanRead(panel->plan, &config);
    memset(entry, 0, sizeof *entry);
    entry->amount = config.opening;
    if (strlen(Text(panel, CASH_ID)) >= sizeof entry->id ||
        strlen(Text(panel, CASH_LABEL)) >= sizeof entry->label)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    g_strlcpy(entry->id, Text(panel, CASH_ID), sizeof entry->id);
    g_strlcpy(entry->label, Text(panel, CASH_LABEL), sizeof entry->label);
    entry->enabled = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->enabled));
    entry->direction = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->receive))
                           ? UMI_FINANCIAL_DIRECTION_RECEIVE
                           : UMI_FINANCIAL_DIRECTION_PAY;
    UmiStatus status = MoneyText(Text(panel, CASH_AMOUNT), config.opening.scale, &entry->amount.minor_units);
    return status == UMI_STATUS_OK
               ? UmiCashPlanDateParse(Text(panel, CASH_DATE), strlen(Text(panel, CASH_DATE)), &entry->date)
               : status;
}
/* Edit a private copy first, then transfer ownership and retain one undo state.
 * Products do not duplicate financial validation or partially commit fields. */
static void Edit(GtkButton *button, gpointer root)
{
    CashPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-cash-plan");
    if (panel == NULL || panel->updating)
        return;
    g_object_ref(root);
    ++panel->updating;
    const char *action = g_object_get_data(G_OBJECT(button), "cash-action");
    UmiStatus status = UMI_STATUS_OK;
    if (strcmp(action, "reset") == 0)
    {
        UmiCashPanelInvalidate(panel);
        UmiCashPanelSync(panel);
        UmiCashPanelRender(panel);
    }
    else if (strcmp(action, "undo") == 0)
    {
        if (panel->previous == NULL)
            status = UMI_STATUS_NOT_FOUND;
        else
        {
            UmiCashPanelInvalidate(panel);
            UmiCashPlan *old = panel->plan;
            panel->plan = panel->previous;
            panel->previous = NULL;
            UmiCashPlanDestroy(old);
            UmiCashPanelSync(panel);
            UmiCashPanelRender(panel);
        }
    }
    else
    {
        int config = strcmp(action, "settings") == 0;
        if ((config && panel->entryDirty) || (!config && panel->configDirty))
            status = UMI_STATUS_INVALID_STATE;
        UmiCashPlan *next = NULL;
        if (status == UMI_STATUS_OK)
            status = UmiCashPlanCopy(panel->plan, &next);
        if (status == UMI_STATUS_OK && config)
            status = ConfigFields(panel, next);
        else if (status == UMI_STATUS_OK && strcmp(action, "remove") == 0)
            status =
                UmiCashPlanRemove(next, (size_t)gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector)));
        else if (status == UMI_STATUS_OK)
        {
            UmiCashPlanEntry entry;
            status = ReadEntry(panel, &entry);
            if (status == UMI_STATUS_OK)
                status = strcmp(action, "add") == 0
                             ? UmiCashPlanAdd(next, &entry)
                             : UmiCashPlanReplace(
                                   next, (size_t)gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector)),
                                   &entry);
        }
        if (status == UMI_STATUS_OK)
        {
            UmiCashPanelInvalidate(panel);
            UmiCashPlanDestroy(panel->previous);
            panel->previous = panel->plan;
            panel->plan = next;
            next = NULL;
            UmiCashPanelSync(panel);
            UmiCashPanelRender(panel);
        }
        UmiCashPlanDestroy(next);
    }
    gtk_label_set_text(GTK_LABEL(panel->note),
                       status == UMI_STATUS_OK
                           ? "Stored plan updated. Save a new file to keep it after closing."
                           : umi_status_text(status));
    --panel->updating;
    g_object_unref(root);
}
static GtkWidget *Button(CashPanel *panel, GtkWidget *row, GtkWidget *root, const char *label,
                         const char *action, GCallback callback)
{
    char *id = g_strdup_printf("cash.plan.%s", action);
    GtkWidget *button = Keep(panel, gtk_button_new_with_label(label), id);
    g_free(id);
    g_object_set_data_full(G_OBJECT(button), "cash-action", g_strdup(action), g_free);
    g_signal_connect_object(button, "clicked", callback, root, 0);
    gtk_box_append(GTK_BOX(row), button);
    return button;
}
UmiStatus UmiGtk4CashPlanCreate(GtkWidget **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    CashPanel *panel = g_new0(CashPanel, 1);
    panel->retained = g_ptr_array_new_with_free_func(g_object_unref);
    panel->generation = 1U;
    UmiCashPlanConfig config = {.title = "Cash plan", .opening = {.scale = 2U, .currency = {"GBP"}}};
    GDateTime *now = g_date_time_new_now_local();
    if (now == NULL)
    {
        Destroy(panel);
        return UMI_STATUS_UNAVAILABLE;
    }
    GDateTime *end = g_date_time_add_days(now, 30);
    if (end == NULL)
    {
        g_date_time_unref(now);
        Destroy(panel);
        return UMI_STATUS_UNAVAILABLE;
    }
    config.start = (UmiFinancialDate){g_date_time_get_year(now), (uint8_t)g_date_time_get_month(now),
                                      (uint8_t)g_date_time_get_day_of_month(now)};
    config.end = (UmiFinancialDate){g_date_time_get_year(end), (uint8_t)g_date_time_get_month(end),
                                    (uint8_t)g_date_time_get_day_of_month(end)};
    g_date_time_unref(now);
    g_date_time_unref(end);
    UmiStatus status = UmiCashPlanCreate(&config, &panel->plan);
    if (status != UMI_STATUS_OK)
    {
        Destroy(panel);
        return status;
    }
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    g_object_set_data_full(G_OBJECT(root), "umicom-cash-plan", panel, Destroy);
    ++panel->updating;
    GtkWidget *intro = gtk_label_new("Local cash assumptions. No bank feed, payment or trade is executed. "
                                     "Dates are calendar days; projections are end-of-day.");
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
    gtk_box_append(GTK_BOX(root), intro);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4U);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8U);
    gtk_box_append(GTK_BOX(root), grid);
    const char *labels[] = {"Plan title",     "Currency (3 letters)", "Opening amount",
                            "Minimum buffer", "Start (YYYY-MM-DD)",   "End (YYYY-MM-DD)",
                            "Entry ID",       "Entry label",          "Entry date",
                            "Entry amount",   "Absolute file path"};
    const char *ids[] = {"title", "currency", "opening", "buffer", "start", "end",
                         "id",    "label",    "date",    "amount", "path"};
    for (int i = 0; i < CASH_FIELDS; ++i)
    {
        char *id = g_strdup_printf("cash.plan.%s", ids[i]);
        panel->fields[i] = Keep(panel, gtk_entry_new(), id);
        g_free(id);
        gtk_widget_set_hexpand(panel->fields[i], TRUE);
        gtk_grid_attach(GTK_GRID(grid), gtk_label_new(labels[i]), 0, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), panel->fields[i], 1, i, 1, 1);
        g_object_set_data(G_OBJECT(panel->fields[i]), "cash-field", GINT_TO_POINTER(i + 1));
        g_signal_connect_object(panel->fields[i], "changed", G_CALLBACK(Changed), root, 0);
    }
    panel->scale = Keep(panel, gtk_spin_button_new_with_range(0, 9, 1), "cash.plan.scale");
    g_object_set_data(G_OBJECT(panel->scale), "cash-field", GINT_TO_POINTER(CASH_OPENING + 1));
    g_signal_connect_object(panel->scale, "value-changed", G_CALLBACK(Changed), root, 0);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Decimal places"), 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), panel->scale, 3, 2, 1, 1);
    panel->receive =
        Keep(panel, gtk_check_button_new_with_label("Inflow (unchecked = outflow)"), "cash.plan.receive");
    panel->enabled = Keep(panel, gtk_check_button_new_with_label("Include entry"), "cash.plan.enabled");
    GtkWidget *options = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(root), options);
    GtkWidget *checks[] = {panel->receive, panel->enabled};
    for (size_t i = 0U; i < 2U; ++i)
    {
        gtk_box_append(GTK_BOX(options), checks[i]);
        g_object_set_data(G_OBJECT(checks[i]), "cash-field", GINT_TO_POINTER(CASH_AMOUNT + 1));
        g_signal_connect_object(checks[i], "toggled", G_CALLBACK(Changed), root, 0);
    }
    /* Keep our editing reference and transfer a separate reference through
     * the canonical helper, so selector disposal cannot release the model twice. */
    panel->names = gtk_string_list_new(NULL);
    panel->selector = Keep(panel, umi_ui_gtk4_drop_down_new_take_string_list(g_object_ref(panel->names)),
                           "cash.plan.entries");
    gtk_box_append(GTK_BOX(root), panel->selector);
    g_signal_connect_object(panel->selector, "notify::selected", G_CALLBACK(Selected), root, 0);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_append(GTK_BOX(root), actions);
    const char *actionsIds[] = {"settings", "add", "update", "remove", "undo", "reset"};
    const char *actionLabels[] = {"Apply settings",  "Add entry",        "Update selected",
                                  "Remove selected", "Undo last change", "Reset fields"};
    for (size_t i = 0U; i < 6U; ++i)
        (void)Button(panel, actions, root, actionLabels[i], actionsIds[i], G_CALLBACK(Edit));
    GtkWidget *files = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_append(GTK_BOX(root), files);
    (void)Button(panel, files, root, "Save new JSON", "save", G_CALLBACK(UmiCashPanelFile));
    (void)Button(panel, files, root, "Load for review", "load", G_CALLBACK(UmiCashPanelFile));
    (void)Button(panel, files, root, "Export new CSV", "export", G_CALLBACK(UmiCashPanelFile));
    panel->approval = Keep(
        panel,
        gtk_check_button_new_with_label("I reviewed the loaded assumptions and want to replace this plan"),
        "cash.plan.approval");
    gtk_box_append(GTK_BOX(root), panel->approval);
    (void)Button(panel, files, root, "Apply reviewed plan", "apply", G_CALLBACK(UmiCashPanelApplyLoaded));
    panel->note = Keep(panel, gtk_label_new("Save explicitly to retain your local plan."), "cash.plan.note");
    gtk_label_set_wrap(GTK_LABEL(panel->note), TRUE);
    gtk_box_append(GTK_BOX(root), panel->note);
    panel->output = Keep(panel, gtk_text_view_new(), "cash.plan.output");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->output), TRUE);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->output);
    gtk_widget_set_size_request(scroll, -1, 220);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(root), scroll);
    --panel->updating;
    UmiCashPanelSync(panel);
    UmiCashPanelRender(panel);
    *out = root;
    return UMI_STATUS_OK;
}
static void Open(GtkButton *button, gpointer unused)
{
    (void)unused;
    /* A retained launcher from a closed product window must not open an
     * orphan workspace. Resolve its live parent for this click only. */
    GtkRoot *parent = gtk_widget_get_root(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(parent))
        return;
    GtkWidget *content = NULL;
    if (UmiGtk4CashPlanCreate(&content) != UMI_STATUS_OK)
        return;
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Cash planning");
    gtk_window_set_default_size(GTK_WINDOW(window), 1040, 780);
    if (GTK_IS_WINDOW(parent))
    {
        gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(parent));
        gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    }
    gtk_window_set_child(GTK_WINDOW(window), content);
    gtk_window_present(GTK_WINDOW(window));
}
GtkWidget *UmiGtk4CashPlanLauncherCreate(void)
{
    GtkWidget *button = gtk_button_new_with_label("Cash planning");
    (void)umi_gtk4_automation_tag_widget(button, "cash.plan.launch");
    g_signal_connect(button, "clicked", G_CALLBACK(Open), NULL);
    return button;
}
