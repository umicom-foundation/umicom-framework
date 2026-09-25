/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/finance_operations_panel_gtk4.c
 *
 * PURPOSE:
 *   Build beginner-facing command forms that delegate parsing, financial validation and persistence to Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "finance_operations_internal.h"
#include <inttypes.h>
#include <string.h>

#define FIELD_BIT(field) (UINT64_C(1) << (unsigned)(field))
static const char *FIELD_LABELS[FINANCE_FIELD_COUNT] = {
    "Request identifier", "Actor label (practice role)", "Record identifier",
    "Display name", "Currency (three capitals)", "Decimal scale", "Account class",
    "Reference identifier", "Accounting period identifier", "Date (YYYY-MM-DD)", "Last date (YYYY-MM-DD)",
    "Ledger account identifier", "Participant identifier", "Instrument identifier", "Side",
    "Limit price in ticks", "Quantity in whole lots", "Minor units per tick", "Units per lot",
    "Maximum lots per order", "External balance in minor units", "Market state",
    "Journal lines: account,debit-minor,credit-minor"
};
static const char *FIELD_NAMES[FINANCE_FIELD_COUNT] = {
    "request", "actor", "record", "name", "currency", "scale", "class", "reference",
    "period", "date", "end-date", "account", "participant", "instrument", "side", "price",
    "lots", "tick", "units", "max-lots", "amount", "open", "lines"
};
static const char *HINTS[] = {
    "Create a ledger account. Participant cash accounts must use Liability; a cash-control account uses Asset.",
    "Choose the first and last dates of a new accounting period. Periods cannot overlap.",
    "Reference is the open accounting period. Enter one balanced journal line per row; this creates a draft, not a posting.",
    "Record is a draft journal. Use an actor label different from its maker to approve it.",
    "Record is an approved journal. Use its checker actor to post it once to the ledger.",
    "Record is a new correcting journal; Reference is the old posted manual journal. Approve and post the correction separately.",
    "Link a participant to a unique liability cash account. This is a practice identity, not customer authentication.",
    "List an instrument in a halted state. Tick value, units per lot and maximum order size must all be positive.",
    "Record is an existing instrument. Opening enables matching; halting blocks new orders but still allows cancellation.",
    "Record is a participant. Add explicit simulated custody lots; this does not import real assets or post cash.",
    "Create a new limit/GTC order. The whole buy limit must be funded, and a sell must have available custody lots.",
    "Record is an order with quantity remaining. Participant must match its owner; fills are retained.",
    "Record is a generated fill identifier. Clearing records a local obligation; cash and custody have not moved yet.",
    "Record is a cleared fill. Use an actor different from the clearing actor and a date in an open accounting period.",
    "Record is a new reconciliation identifier. Supply the externally observed normal-side account balance, in minor units.",
    "Record is a period. Finish its orders, settlements and journals; reconcile every posted account before preparing close.",
    "Record is a prepared period. Use an actor different from the preparer to confirm its final close.",
    "Record is an unconfirmed close review. Reopening permits corrections; a finally closed period stays closed."
};
UmiFinanceOperationsGtkPanel *FinanceGtkPanelFromRoot(gpointer root)
{
    return root != NULL ? g_object_get_data(G_OBJECT(root), FINANCE_PANEL_KEY) : NULL;
}
static GtkWidget *Label(const char *text)
{
    GtkWidget *label = gtk_label_new(text); gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE); return label;
}
static GtkWidget *Entry(const char *value, const char *name)
{
    GtkWidget *entry = gtk_entry_new(); gtk_editable_set_text(GTK_EDITABLE(entry), value);
    gtk_widget_set_name(entry, name); return entry;
}
static void NewRequest(UmiFinanceOperationsGtkPanel *panel)
{
    char *id = g_uuid_string_random();
    gtk_editable_set_text(GTK_EDITABLE(panel->fields[FINANCE_FIELD_REQUEST]), id); g_free(id);
}
static uint64_t FieldsForKind(UmiFinanceOperationKind kind)
{
    uint64_t mask = FIELD_BIT(FINANCE_FIELD_REQUEST) | FIELD_BIT(FINANCE_FIELD_ACTOR) | FIELD_BIT(FINANCE_FIELD_ID);
    switch (kind) {
    case UMI_FINANCE_CREATE_ACCOUNT: return mask | FIELD_BIT(FINANCE_FIELD_NAME) | FIELD_BIT(FINANCE_FIELD_CURRENCY) | FIELD_BIT(FINANCE_FIELD_SCALE) | FIELD_BIT(FINANCE_FIELD_CLASS);
    case UMI_FINANCE_OPEN_PERIOD: return mask | FIELD_BIT(FINANCE_FIELD_DATE) | FIELD_BIT(FINANCE_FIELD_END_DATE);
    case UMI_FINANCE_ENTER_JOURNAL: return mask | FIELD_BIT(FINANCE_FIELD_REFERENCE) | FIELD_BIT(FINANCE_FIELD_DATE) | FIELD_BIT(FINANCE_FIELD_LINES);
    case UMI_FINANCE_REVERSE_JOURNAL: return mask | FIELD_BIT(FINANCE_FIELD_REFERENCE) | FIELD_BIT(FINANCE_FIELD_PERIOD) | FIELD_BIT(FINANCE_FIELD_DATE);
    case UMI_FINANCE_REGISTER_PARTICIPANT: return mask | FIELD_BIT(FINANCE_FIELD_ACCOUNT) | FIELD_BIT(FINANCE_FIELD_NAME);
    case UMI_FINANCE_LIST_INSTRUMENT: return mask | FIELD_BIT(FINANCE_FIELD_NAME) | FIELD_BIT(FINANCE_FIELD_CURRENCY) | FIELD_BIT(FINANCE_FIELD_SCALE) | FIELD_BIT(FINANCE_FIELD_TICK) | FIELD_BIT(FINANCE_FIELD_UNITS) | FIELD_BIT(FINANCE_FIELD_MAX_LOTS);
    case UMI_FINANCE_SET_MARKET_STATE: return mask | FIELD_BIT(FINANCE_FIELD_OPEN);
    case UMI_FINANCE_DEPOSIT_LOTS: return mask | FIELD_BIT(FINANCE_FIELD_INSTRUMENT) | FIELD_BIT(FINANCE_FIELD_LOTS);
    case UMI_FINANCE_PLACE_ORDER: return mask | FIELD_BIT(FINANCE_FIELD_PARTICIPANT) | FIELD_BIT(FINANCE_FIELD_INSTRUMENT) | FIELD_BIT(FINANCE_FIELD_SIDE) | FIELD_BIT(FINANCE_FIELD_PRICE) | FIELD_BIT(FINANCE_FIELD_LOTS) | FIELD_BIT(FINANCE_FIELD_DATE);
    case UMI_FINANCE_CANCEL_ORDER: return mask | FIELD_BIT(FINANCE_FIELD_PARTICIPANT);
    case UMI_FINANCE_SETTLE_FILL: return mask | FIELD_BIT(FINANCE_FIELD_PERIOD) | FIELD_BIT(FINANCE_FIELD_DATE);
    case UMI_FINANCE_RECONCILE_ACCOUNT: return mask | FIELD_BIT(FINANCE_FIELD_ACCOUNT) | FIELD_BIT(FINANCE_FIELD_AMOUNT);
    default: return mask;
    }
}
static void KindChanged(GObject *object, GParamSpec *property, gpointer root)
{
    UmiFinanceOperationsGtkPanel *panel = FinanceGtkPanelFromRoot(root);
    guint choice;
    (void)object; (void)property;
    if (panel == NULL) return;
    choice = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->kind));
    if (choice >= 18U) return;
    panel->mask = FieldsForKind((UmiFinanceOperationKind)(choice + 1U));
    for (unsigned field = 0U; field < FINANCE_FIELD_COUNT; ++field)
        gtk_widget_set_visible(panel->rows[field], (panel->mask & FIELD_BIT(field)) != 0U);
    gtk_label_set_text(GTK_LABEL(panel->hint), HINTS[choice]); NewRequest(panel);
}
static UmiStatus CopyText(char *destination, size_t capacity, const char *source)
{
    size_t length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(destination, 0, capacity); memcpy(destination, source, length); return UMI_STATUS_OK;
}
static UmiStatus ReadCommand(UmiFinanceOperationsGtkPanel *panel, UmiFinanceOperationCommand *command)
{
    UmiFinanceOperationCounts counts;
    UmiStatus status = UmiFinanceOperationsCounts(panel->operations, &counts);
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->kind));
    if (status != UMI_STATUS_OK) return status;
    if (selected >= 18U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiFinanceOperationCommandInit(command);
    command->kind = (UmiFinanceOperationKind)(selected + 1U); command->expectedRevision = counts.revision;
    for (unsigned field = 0U; field < FINANCE_FIELD_COUNT; ++field) {
        const char *text;
        int64_t number;
        char *destination = NULL; size_t capacity = 0U;
        if ((panel->mask & FIELD_BIT(field)) == 0U) continue;
        if (field == FINANCE_FIELD_CLASS) {
            guint choice = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->fields[field]));
            if (choice > 4U) return UMI_STATUS_INVALID_ARGUMENT;
            command->accountClass = (UmiAccountingAccountClass)choice; continue;
        }
        if (field == FINANCE_FIELD_SIDE) {
            guint choice = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->fields[field]));
            if (choice > 1U) return UMI_STATUS_INVALID_ARGUMENT;
            command->side = choice == 0U ? UMI_SIDE_BUY : UMI_SIDE_SELL; continue;
        }
        if (field == FINANCE_FIELD_OPEN) {
            guint choice = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->fields[field]));
            if (choice > 1U) return UMI_STATUS_INVALID_ARGUMENT;
            command->enabled = choice == 1U; continue;
        }
        if (field == FINANCE_FIELD_LINES) {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->fields[field]));
            GtkTextIter first, last; char *rows;
            gtk_text_buffer_get_bounds(buffer, &first, &last);
            rows = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
            status = UmiFinanceOperationsParseLines(rows, command->lines, UMI_FINANCE_OPERATIONS_JOURNAL_LINES, &command->lineCount);
            g_free(rows); if (status != UMI_STATUS_OK) return status; continue;
        }
        text = gtk_editable_get_text(GTK_EDITABLE(panel->fields[field]));
        switch ((FinanceField)field) {
        case FINANCE_FIELD_REQUEST: destination = command->requestId.value; capacity = sizeof(command->requestId.value); break;
        case FINANCE_FIELD_ACTOR: destination = command->actorId.value; capacity = sizeof(command->actorId.value); break;
        case FINANCE_FIELD_ID: destination = command->id.value; capacity = sizeof(command->id.value); break;
        case FINANCE_FIELD_REFERENCE: destination = command->referenceId.value; capacity = sizeof(command->referenceId.value); break;
        case FINANCE_FIELD_PERIOD: destination = command->periodId.value; capacity = sizeof(command->periodId.value); break;
        case FINANCE_FIELD_ACCOUNT: destination = command->accountId.value; capacity = sizeof(command->accountId.value); break;
        case FINANCE_FIELD_PARTICIPANT: destination = command->participantId.value; capacity = sizeof(command->participantId.value); break;
        case FINANCE_FIELD_INSTRUMENT: destination = command->instrumentId.value; capacity = sizeof(command->instrumentId.value); break;
        case FINANCE_FIELD_NAME: destination = command->name; capacity = sizeof(command->name); break;
        case FINANCE_FIELD_CURRENCY: destination = command->currency.code; capacity = sizeof(command->currency.code); break;
        default: break;
        }
        if (destination != NULL) { status = CopyText(destination, capacity, text); if (status != UMI_STATUS_OK) return status; continue; }
        if (field == FINANCE_FIELD_DATE || field == FINANCE_FIELD_END_DATE) {
            status = UmiFinanceOperationsParseDate(text, field == FINANCE_FIELD_DATE ? &command->date : &command->endDate);
            if (status != UMI_STATUS_OK) return status;
            continue;
        }
        status = UmiFinanceOperationsParseInteger(text, &number); if (status != UMI_STATUS_OK) return status;
        switch ((FinanceField)field) {
        case FINANCE_FIELD_SCALE: if (number < 0 || number > 9) return UMI_STATUS_INVALID_ARGUMENT; command->scale = (uint8_t)number; break;
        case FINANCE_FIELD_PRICE: command->priceTicks = number; break;
        case FINANCE_FIELD_LOTS: command->lots = number; break;
        case FINANCE_FIELD_TICK: command->minorPerTick = number; break;
        case FINANCE_FIELD_UNITS: command->unitsPerLot = number; break;
        case FINANCE_FIELD_MAX_LOTS: command->maxOrderLots = number; break;
        case FINANCE_FIELD_AMOUNT: command->amountMinor = number; break;
        default: return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}
static void ApplyClicked(GtkButton *button, gpointer root)
{
    UmiFinanceOperationsGtkPanel *panel = FinanceGtkPanelFromRoot(root);
    UmiFinanceOperationCommand command; UmiFinanceOperationReceipt receipt;
    UmiStatus status; char *message;
    (void)button; if (panel == NULL) return;
    status = ReadCommand(panel, &command);
    if (status == UMI_STATUS_OK) status = UmiFinanceOperationsApply(panel->operations, &command, &receipt);
    if (status == UMI_STATUS_OK) message = g_strdup_printf("%s Receipt %s, revision %" PRIu64 ".",
        receipt.duplicate ? "This request was already saved; nothing was posted twice." : "Operation saved.", receipt.requestId.value, receipt.revision);
    else message = g_strdup_printf("%s (Status %d)", UmiFinanceOperationsExplainStatus(status), (int)status);
    gtk_label_set_text(GTK_LABEL(panel->status), message); g_free(message); FinanceGtkRefresh(panel);
}
static void NewClicked(GtkButton *button, gpointer root)
{
    UmiFinanceOperationsGtkPanel *panel = FinanceGtkPanelFromRoot(root); (void)button;
    if (panel != NULL) { NewRequest(panel); gtk_label_set_text(GTK_LABEL(panel->status), "A new request is ready. Review the fields before applying it."); }
}
static void ReloadClicked(GtkButton *button, gpointer root)
{
    UmiFinanceOperationsGtkPanel *panel = FinanceGtkPanelFromRoot(root); UmiStatus status; (void)button;
    if (panel == NULL) return;
    status = UmiFinanceOperationsReload(panel->operations);
    gtk_label_set_text(GTK_LABEL(panel->status), status == UMI_STATUS_OK ? "Shared book reloaded. Review the current records before your next operation." : UmiFinanceOperationsExplainStatus(status));
    FinanceGtkRefresh(panel);
}
static void ViewChanged(GObject *object, GParamSpec *property, gpointer root)
{ (void)object; (void)property; FinanceGtkRefresh(FinanceGtkPanelFromRoot(root)); }
static void RefreshClicked(GtkButton *button, gpointer root)
{ (void)button; FinanceGtkRefresh(FinanceGtkPanelFromRoot(root)); }
static void CopyClicked(GtkButton *button, gpointer root)
{
    UmiFinanceOperationsGtkPanel *panel = FinanceGtkPanelFromRoot(root); GtkTextIter first, last;
    GtkTextBuffer *buffer; char *text; (void)button; if (panel == NULL) return;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->report)); gtk_text_buffer_get_bounds(buffer, &first, &last);
    text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    gdk_clipboard_set_text(gtk_widget_get_clipboard(panel->root), text); g_free(text);
    gtk_label_set_text(GTK_LABEL(panel->status), "The displayed report was copied. It contains practice records only.");
}
static void SaveClicked(GtkButton *button, gpointer root)
{
    UmiFinanceOperationsGtkPanel *panel = FinanceGtkPanelFromRoot(root); GtkTextIter first, last;
    GtkTextBuffer *buffer; const char *path; char *text; GFile *file; GFileOutputStream *stream;
    GError *error = NULL; gboolean written = FALSE; (void)button; if (panel == NULL) return;
    path = gtk_editable_get_text(GTK_EDITABLE(panel->exportPath));
    if (!g_path_is_absolute(path)) { gtk_label_set_text(GTK_LABEL(panel->status), "Enter an absolute path for a new report file. Existing files are never overwritten."); return; }
    file = g_file_new_for_path(path); stream = g_file_create(file, G_FILE_CREATE_PRIVATE, NULL, &error);
    if (stream != NULL) {
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->report)); gtk_text_buffer_get_bounds(buffer, &first, &last);
        text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
        written = g_output_stream_write_all(G_OUTPUT_STREAM(stream), text, strlen(text), NULL, NULL, &error);
        g_free(text);
        if (written) written = g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, &error);
        else (void)g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, NULL);
        g_object_unref(stream);
        if (!written) (void)g_file_delete(file, NULL, NULL);
    }
    gtk_label_set_text(GTK_LABEL(panel->status), written ? "The displayed report was saved to the new file." :
        (error != NULL ? error->message : "The report could not be saved."));
    g_clear_error(&error); g_object_unref(file);
}
static void AddButton(UmiFinanceOperationsGtkPanel *panel, GtkWidget *box,
    const char *text, const char *name, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(text); gtk_widget_set_name(button, name);
    gtk_box_append(GTK_BOX(box), button);
    /* The weak object-bound callback resolves current data from the root. A
     * retained child can never call into a freed panel after its owner closes. */
    g_signal_connect_object(button, "clicked", callback, G_OBJECT(panel->root), 0);
}
UmiStatus UmiFinanceOperationsGtkPanelCreate(UmiFinanceOperations *operations,
    const char *description, UmiFinanceOperationsGtkPanel **outPanel)
{
    UmiFinanceOperationsGtkPanel *panel;
    GtkWidget *body, *form, *formScroll, *right, *reportScroll, *buttons, *options, *exportBox;
    GtkWidget *descriptionLabel;
    const char *kinds[19] = {0};
    const char *classes[] = {"Asset", "Liability", "Equity", "Income", "Expense", NULL};
    const char *sides[] = {"Buy", "Sell", NULL}; const char *markets[] = {"Halted", "Open", NULL};
    const char *views[] = {"Overview", "Ledger accounts", "Accounting periods", "Journals", "Order book and history",
        "Fills and settlement", "Custody lots", "Reconciliation", "Accepted operations", "Trial balance", NULL};
    if (outPanel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outPanel = NULL; if (operations == NULL || description == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    panel = g_try_new0(UmiFinanceOperationsGtkPanel, 1); if (panel == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    panel->operations = operations; panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    g_object_ref_sink(panel->root); g_object_set_data(G_OBJECT(panel->root), FINANCE_PANEL_KEY, panel);
    gtk_widget_set_name(panel->root, "umicom-finance-operations");
    gtk_widget_set_margin_start(panel->root, 12); gtk_widget_set_margin_end(panel->root, 12);
    gtk_widget_set_margin_top(panel->root, 12); gtk_widget_set_margin_bottom(panel->root, 12);
    gtk_box_append(GTK_BOX(panel->root), Label("Local financial practice — no real funds, broker connection or authenticated users"));
    descriptionLabel = Label(description); gtk_label_set_selectable(GTK_LABEL(descriptionLabel), TRUE);
    gtk_box_append(GTK_BOX(panel->root), descriptionLabel);
    body = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL); gtk_widget_set_vexpand(body, TRUE);
    gtk_paned_set_position(GTK_PANED(body), 390); gtk_box_append(GTK_BOX(panel->root), body);
    form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_widget_set_margin_end(form, 12);
    formScroll = gtk_scrolled_window_new(); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(formScroll), form);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(formScroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_paned_set_start_child(GTK_PANED(body), formScroll);
    for (unsigned index = 0U; index < 18U; ++index) kinds[index] = UmiFinanceOperationKindText((UmiFinanceOperationKind)(index + 1U));
    panel->kind = gtk_drop_down_new_from_strings(kinds); gtk_widget_set_name(panel->kind, "finance-operation");
    gtk_box_append(GTK_BOX(form), Label("Operation")); gtk_box_append(GTK_BOX(form), panel->kind);
    panel->hint = Label(""); gtk_box_append(GTK_BOX(form), panel->hint);
    for (unsigned field = 0U; field < FINANCE_FIELD_COUNT; ++field) {
        GtkWidget *input, *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3); char *name;
        panel->rows[field] = row; gtk_box_append(GTK_BOX(row), Label(FIELD_LABELS[field]));
        if (field == FINANCE_FIELD_CLASS) input = gtk_drop_down_new_from_strings(classes);
        else if (field == FINANCE_FIELD_SIDE) input = gtk_drop_down_new_from_strings(sides);
        else if (field == FINANCE_FIELD_OPEN) input = gtk_drop_down_new_from_strings(markets);
        else if (field == FINANCE_FIELD_LINES) {
            input = gtk_text_view_new(); gtk_text_view_set_monospace(GTK_TEXT_VIEW(input), TRUE);
            gtk_widget_set_size_request(input, -1, 115);
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(input)), "control,50000,0\nbuyer.cash,0,50000", -1);
        } else {
            const char *value = "";
            if (field == FINANCE_FIELD_ACTOR) value = "operator";
            else if (field == FINANCE_FIELD_CURRENCY) value = "GBP";
            else if (field == FINANCE_FIELD_SCALE) value = "2";
            else if (field == FINANCE_FIELD_DATE) value = "2026-09-25";
            else if (field == FINANCE_FIELD_END_DATE) value = "2026-09-30";
            else if (field == FINANCE_FIELD_PERIOD || field == FINANCE_FIELD_REFERENCE) value = "september";
            else if (field == FINANCE_FIELD_TICK || field == FINANCE_FIELD_UNITS) value = "1";
            else if (field == FINANCE_FIELD_MAX_LOTS) value = "100";
            input = Entry(value, "finance-field");
        }
        name = g_strconcat("finance-", FIELD_NAMES[field], NULL); gtk_widget_set_name(input, name); g_free(name);
        gtk_widget_set_tooltip_text(input, FIELD_LABELS[field]);
        panel->fields[field] = input; gtk_box_append(GTK_BOX(row), input); gtk_box_append(GTK_BOX(form), row);
    }
    buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(form), buttons);
    AddButton(panel, buttons, "New request", "finance-new-request", G_CALLBACK(NewClicked));
    AddButton(panel, buttons, "Apply operation", "finance-apply", G_CALLBACK(ApplyClicked));
    right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_widget_set_margin_start(right, 12); gtk_paned_set_end_child(GTK_PANED(body), right);
    panel->view = gtk_drop_down_new_from_strings(views); gtk_widget_set_name(panel->view, "finance-report-view"); gtk_box_append(GTK_BOX(right), panel->view);
    options = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(right), options);
    panel->reportPeriod = Entry("september", "finance-report-period"); panel->reportCurrency = Entry("GBP", "finance-report-currency"); panel->reportScale = Entry("2", "finance-report-scale");
    gtk_widget_set_tooltip_text(panel->reportPeriod, "Trial balance accounting period identifier");
    gtk_widget_set_tooltip_text(panel->reportCurrency, "Trial balance currency"); gtk_widget_set_tooltip_text(panel->reportScale, "Trial balance decimal scale");
    gtk_editable_set_width_chars(GTK_EDITABLE(panel->reportCurrency), 4); gtk_editable_set_width_chars(GTK_EDITABLE(panel->reportScale), 2);
    gtk_box_append(GTK_BOX(options), panel->reportPeriod); gtk_box_append(GTK_BOX(options), panel->reportCurrency); gtk_box_append(GTK_BOX(options), panel->reportScale);
    AddButton(panel, options, "Refresh report", "finance-refresh-report", G_CALLBACK(RefreshClicked));
    panel->report = gtk_text_view_new(); gtk_widget_set_name(panel->report, "finance-report");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->report), FALSE); gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(panel->report), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->report), TRUE); gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->report), GTK_WRAP_WORD_CHAR);
    reportScroll = gtk_scrolled_window_new(); gtk_widget_set_vexpand(reportScroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(reportScroll), panel->report); gtk_box_append(GTK_BOX(right), reportScroll);
    buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(right), buttons);
    AddButton(panel, buttons, "Reload shared book", "finance-reload", G_CALLBACK(ReloadClicked));
    AddButton(panel, buttons, "Copy report", "finance-copy-report", G_CALLBACK(CopyClicked));
    exportBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(right), exportBox);
    panel->exportPath = Entry("", "finance-report-path"); gtk_entry_set_placeholder_text(GTK_ENTRY(panel->exportPath), "Absolute path for a new .txt report");
    gtk_widget_set_hexpand(panel->exportPath, TRUE); gtk_box_append(GTK_BOX(exportBox), panel->exportPath);
    AddButton(panel, exportBox, "Save report", "finance-save-report", G_CALLBACK(SaveClicked));
    panel->status = Label("Choose an operation. Preparing a form does not change the book.");
    gtk_widget_set_name(panel->status, "finance-status"); gtk_label_set_selectable(GTK_LABEL(panel->status), TRUE);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    g_signal_connect_object(panel->kind, "notify::selected", G_CALLBACK(KindChanged), G_OBJECT(panel->root), 0);
    g_signal_connect_object(panel->view, "notify::selected", G_CALLBACK(ViewChanged), G_OBJECT(panel->root), 0);
    KindChanged(NULL, NULL, panel->root); FinanceGtkRefresh(panel); *outPanel = panel; return UMI_STATUS_OK;
}
GtkWidget *UmiFinanceOperationsGtkPanelWidget(UmiFinanceOperationsGtkPanel *panel)
{ return panel != NULL ? panel->root : NULL; }
void UmiFinanceOperationsGtkPanelDestroy(UmiFinanceOperationsGtkPanel *panel)
{
    if (panel == NULL) return;
    g_object_set_data(G_OBJECT(panel->root), FINANCE_PANEL_KEY, NULL);
    panel->operations = NULL; g_object_unref(panel->root); g_free(panel);
}
