/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_operations_gtk4.c
 *
 * PURPOSE:
 *   Render operational banking command forms and copied financial views without duplicating domain logic.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/bank_operations.h"
#include "umicom/bank_operations/operations.h"
#include "umicom/finance/money_text.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Presentation-only Slave Controller. The Framework banking service owns all
 * financial state, validation, approvals, journals and persistence. This adapter
 * owns widgets, input conversion and copied projections, never a second ledger. */
typedef struct BankUi {
    UmiBankOperations *operations;
    GtkWindow *window;
    GtkDropDown *action;
    GtkDropDown *identity;
    GtkDropDown *recordState;
    GtkEntry *request, *id, *owner, *source, *destination, *name;
    GtkEntry *minor, *currency, *scale, *date, *statementAccount;
    GtkLabel *message, *hint, *summary;
    GtkButton *submit;
    GtkGrid *customers, *accounts, *transfers, *cards, *ledger, *reconciliation, *audit;
    GtkNotebook *pages;
    int64_t timestampMillis;
    uint64_t displayedRevision;
    char *path;
    bool closed; /* Destroy can precede finalisation when widgets are retained. */
} BankUi;

typedef struct BankLauncher {
    GtkWindow *window; /* Weak; cleared on destroy, disconnected before freeing. */
} BankLauncher;

static const char *StatusText(UmiStatus status)
{
    switch (status) {
    case UMI_STATUS_OK: return "Completed";
    case UMI_STATUS_BUSY: return "State changed or the resource is busy. Reload, review and retry.";
    case UMI_STATUS_PERMISSION_DENIED: return "Capability, maker/checker, ownership or card-limit control denied this request.";
    case UMI_STATUS_INVALID_ARGUMENT: return "Invalid fields, amount, currency, scale or date. Nothing was committed.";
    case UMI_STATUS_INVALID_STATE: return "Invalid lifecycle state or insufficient available funds. Nothing was committed.";
    case UMI_STATUS_ALREADY_EXISTS: return "The entity exists or the request ID was reused with different data.";
    case UMI_STATUS_NOT_FOUND: return "A referenced customer, account, beneficiary, transfer, card or hold was not found.";
    case UMI_STATUS_CAPACITY_EXCEEDED: return "A record limit or integer amount limit was reached. Nothing was committed.";
    case UMI_STATUS_PARSE_ERROR: return "Stored data failed validation. Close this workspace and investigate the database.";
    case UMI_STATUS_UNAVAILABLE: return "SQLite storage is unavailable in this Framework build. No memory fallback was used.";
    default: return "The operation failed. No success has been recorded; investigate storage and application diagnostics.";
    }
}
static void Message(BankUi *ui, UmiStatus status)
{
    gtk_label_set_text(ui->message, StatusText(status));
}
static void UiFree(gpointer data)
{
    BankUi *ui = data;
    UmiBankOperationsDestroy(ui->operations);
    g_free(ui->path);
    g_free(ui);
}
/* Object-bound signal closures disconnect if their owning window is finalised.
 * A separate destroyed flag covers retained widgets before that finalisation. */
static BankUi *SignalUi(gpointer window)
{
    BankUi *ui = g_object_get_data(G_OBJECT(window), "umicom-bank-operations");
    return ui != NULL && !ui->closed ? ui : NULL;
}
static void UiClosed(GtkWidget *widget, gpointer data)
{
    BankUi *ui = data;
    (void)widget;
    ui->closed = true;
    UmiBankOperationsDestroy(ui->operations);
    ui->operations = NULL;
}
static GtkWidget *Cell(GtkGrid *grid, int column, int row, const char *text, bool heading)
{
    char *valid = g_utf8_make_valid(text, -1);
    GtkWidget *label = gtk_label_new(valid);
    g_free(valid);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_widget_set_margin_start(label, 8);
    gtk_widget_set_margin_end(label, 8);
    gtk_widget_set_margin_top(label, 4);
    gtk_widget_set_margin_bottom(label, 4);
    if (heading) gtk_widget_add_css_class(label, "heading");
    gtk_grid_attach(grid, label, column, row, 1, 1);
    return label;
}
static void ClearGrid(GtkGrid *grid)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(GTK_WIDGET(grid))) != NULL) gtk_grid_remove(grid, child);
}
static void Headers(GtkGrid *grid, const char *const *names, size_t count)
{
    ClearGrid(grid);
    for (size_t i = 0U; i < count; ++i) (void)Cell(grid, (int)i, 0, names[i], true);
}
static void NumberCell(GtkGrid *grid, int column, int row, uint64_t value)
{
    char text[32]; (void)snprintf(text, sizeof text, "%" PRIu64, value); (void)Cell(grid, column, row, text, false);
}
static void MinorCell(GtkGrid *grid, int column, int row, int64_t value)
{
    char text[32]; (void)snprintf(text, sizeof text, "%" PRId64, value); (void)Cell(grid, column, row, text, false);
}
/*
 * The earlier view-local formatter is retained below for engineering review.
 * Its signed remainder produced incorrect negative fractions, and its
 * unchecked power-of-ten loop could overflow for a damaged scale. A variable
 * printf width also caused the reported GCC format-truncation warning.
 * UmiMoneyTextFormat now owns exact, bounded presentation for canonical money;
 * the original implementation is disabled, not removed or renamed as an API.
 */
#if 0
static void MoneyCell(GtkGrid *grid, int column, int row, UmiMoney money)
{
    int64_t divisor = 1;
    char text[80];
    for (uint8_t i = 0U; i < money.scale; ++i) divisor *= 10;
    if (money.scale == 0U)
        (void)snprintf(text, sizeof text, "%.3s %" PRId64, money.currency.code, money.minor_units);
    else
        (void)snprintf(text, sizeof text, "%.3s %" PRId64 ".%0*" PRId64, money.currency.code,
            money.minor_units / divisor, (int)money.scale, money.minor_units % divisor);
    (void)Cell(grid, column, row, text, false);
}
#endif

/* Framework owns sign placement and decimal conversion. This view renders
 * either one complete formatted value or a visible invalid-value message,
 * never an apparently valid truncated financial amount. */
static void MoneyCell(GtkGrid *grid, int column, int row, UmiMoney money)
{
    char text[UMI_MONEY_TEXT_CAPACITY];
    UmiStatus status = UmiMoneyTextFormat(&money, text, sizeof text, NULL);
    (void)Cell(grid, column, row,
        status == UMI_STATUS_OK ? text : "Invalid money value", false);
}
static const char *RecordState(UmiBankRecordState state)
{
    switch (state) { case UMI_BANK_RECORD_ACTIVE: return "Active"; case UMI_BANK_RECORD_BLOCKED: return "Blocked";
        case UMI_BANK_RECORD_CLOSED: return "Closed"; default: return "Invalid"; }
}
static const char *TransferState(UmiBankTransferState state)
{
    switch (state) { case UMI_BANK_TRANSFER_PENDING: return "Pending approval";
    case UMI_BANK_TRANSFER_APPROVED: return "Approved"; case UMI_BANK_TRANSFER_REJECTED: return "Rejected";
    case UMI_BANK_TRANSFER_CANCELLED: return "Cancelled"; case UMI_BANK_TRANSFER_EXECUTED: return "Executed locally";
    case UMI_BANK_TRANSFER_REVERSED: return "Reversed"; default: return "Invalid"; }
}
static const char *HoldState(UmiBankHoldState state)
{
    switch (state) { case UMI_BANK_HOLD_ACTIVE: return "Reserved"; case UMI_BANK_HOLD_RELEASED: return "Released";
    case UMI_BANK_HOLD_CAPTURED: return "Captured locally"; case UMI_BANK_HOLD_REFUNDED: return "Refunded locally";
    default: return "Invalid"; }
}

/* Refresh uses copied Framework records. Grid formatting has no persistence
 * side effects and does not authorise any business operation. */
static void Refresh(BankUi *ui)
{
    static const char *const customerHeaders[] = {"Customer ID", "Name", "State"};
    static const char *const accountHeaders[] = {"Account ID", "Customer", "Name", "Booked", "Reserved", "Available", "State"};
    static const char *const transferHeaders[] = {"Transfer ID", "From", "To", "Amount", "Maker", "Checker", "State"};
    static const char *const cardHeaders[] = {"Card / hold ID", "Account", "Card", "Limit / reserved amount", "State"};
    static const char *const journalHeaders[] = {"Revision", "Journal", "Reference", "Currency", "Scale", "Account", "Debit minor", "Credit minor", "Reversal"};
    static const char *const reconHeaders[] = {"Revision", "Reconciliation", "Account", "External", "Booked", "Result"};
    static const char *const auditHeaders[] = {"Revision", "Request", "Actor", "Action", "Entity", "Business date"};
    UmiBankCounts counts;
    UmiStatus status = UmiBankOperationsCounts(ui->operations, &counts);
    char summary[240];
    if (status != UMI_STATUS_OK) { Message(ui, status); gtk_widget_set_sensitive(GTK_WIDGET(ui->submit), FALSE); return; }
    ui->displayedRevision = counts.revision;
    (void)snprintf(summary, sizeof summary,
        "Committed revision %" PRIu64 " | %zu customers | %zu accounts | %zu transfers | %zu journals | SQLite local simulation",
        counts.revision, counts.customers, counts.accounts, counts.transfers, counts.journals);
    gtk_label_set_text(ui->summary, summary);
    Headers(ui->customers, customerHeaders, G_N_ELEMENTS(customerHeaders));
    Headers(ui->accounts, accountHeaders, G_N_ELEMENTS(accountHeaders));
    Headers(ui->transfers, transferHeaders, G_N_ELEMENTS(transferHeaders));
    Headers(ui->cards, cardHeaders, G_N_ELEMENTS(cardHeaders));
    Headers(ui->ledger, journalHeaders, G_N_ELEMENTS(journalHeaders));
    Headers(ui->reconciliation, reconHeaders, G_N_ELEMENTS(reconHeaders));
    Headers(ui->audit, auditHeaders, G_N_ELEMENTS(auditHeaders));
    for (size_t i = 0U; i < counts.customers; ++i) {
        UmiBankCustomer customer;
        if (UmiBankOperationsCustomerAt(ui->operations, i, &customer) != UMI_STATUS_OK) continue;
        (void)Cell(ui->customers, 0, (int)i + 1, customer.party.party_id.value, false);
        (void)Cell(ui->customers, 1, (int)i + 1, customer.party.display_name, false);
        (void)Cell(ui->customers, 2, (int)i + 1, RecordState(customer.state), false);
    }
    for (size_t i = 0U; i < counts.accounts; ++i) {
        UmiBankAccount account; UmiBankBalance balance; int row = (int)i + 1;
        if (UmiBankOperationsAccountAt(ui->operations, i, &account) != UMI_STATUS_OK) continue;
        (void)Cell(ui->accounts, 0, row, account.account.account_id.value, false);
        (void)Cell(ui->accounts, 1, row, account.account.party_id.value, false);
        (void)Cell(ui->accounts, 2, row, account.name, false);
        if (UmiBankOperationsBalance(ui->operations, account.account.account_id.value, &balance) == UMI_STATUS_OK) {
            MoneyCell(ui->accounts, 3, row, balance.booked); MoneyCell(ui->accounts, 4, row, balance.reserved);
            MoneyCell(ui->accounts, 5, row, balance.available);
        }
        (void)Cell(ui->accounts, 6, row, RecordState(account.state), false);
    }
    for (size_t i = 0U; i < counts.transfers; ++i) {
        UmiBankTransfer t; int row = (int)i + 1;
        if (UmiBankOperationsTransferAt(ui->operations, i, &t) != UMI_STATUS_OK) continue;
        (void)Cell(ui->transfers, 0, row, t.id.value, false); (void)Cell(ui->transfers, 1, row, t.sourceAccountId.value, false);
        (void)Cell(ui->transfers, 2, row, t.destinationAccountId.value, false); MoneyCell(ui->transfers, 3, row, t.amount);
        (void)Cell(ui->transfers, 4, row, t.makerId.value, false); (void)Cell(ui->transfers, 5, row, t.checkerId.value, false);
        (void)Cell(ui->transfers, 6, row, TransferState(t.state), false);
    }
    /* Beneficiaries remain visible under transfers without creating a second
     * source of beneficiary or ownership state in the frontend. */
    for (size_t i = 0U; i < counts.beneficiaries; ++i) {
        UmiBankBeneficiary b; int row = (int)(counts.transfers + i) + 2;
        if (UmiBankOperationsBeneficiaryAt(ui->operations, i, &b) != UMI_STATUS_OK) continue;
        (void)Cell(ui->transfers, 0, row, b.id.value, false); (void)Cell(ui->transfers, 1, row, b.customerId.value, false);
        (void)Cell(ui->transfers, 2, row, b.accountId.value, false); (void)Cell(ui->transfers, 3, row, b.name, false);
        (void)Cell(ui->transfers, 4, row, "Beneficiary", false); (void)Cell(ui->transfers, 6, row, RecordState(b.state), false);
    }
    for (size_t i = 0U; i < counts.cards; ++i) {
        UmiBankCard c; int row = (int)i + 1;
        if (UmiBankOperationsCardAt(ui->operations, i, &c) != UMI_STATUS_OK) continue;
        (void)Cell(ui->cards, 0, row, c.id.value, false); (void)Cell(ui->cards, 1, row, c.accountId.value, false);
        (void)Cell(ui->cards, 2, row, "Simulated card", false); MoneyCell(ui->cards, 3, row, c.perTransactionLimit);
        (void)Cell(ui->cards, 4, row, RecordState(c.state), false);
    }
    for (size_t i = 0U; i < counts.holds; ++i) {
        UmiBankHold h; int row = (int)(counts.cards + i) + 2;
        if (UmiBankOperationsHoldAt(ui->operations, i, &h) != UMI_STATUS_OK) continue;
        (void)Cell(ui->cards, 0, row, h.id.value, false); (void)Cell(ui->cards, 1, row, h.accountId.value, false);
        (void)Cell(ui->cards, 2, row, h.cardId.value, false); MoneyCell(ui->cards, 3, row, h.amount);
        (void)Cell(ui->cards, 4, row, HoldState(h.state), false);
    }
    { int row = 1;
      for (size_t i = 0U; i < counts.journals; ++i) {
        UmiBankJournal j;
        if (UmiBankOperationsJournalAt(ui->operations, i, &j) != UMI_STATUS_OK) continue;
        for (size_t n = 0U; n < j.entry.line_count; ++n, ++row) {
            NumberCell(ui->ledger, 0, row, j.revision); (void)Cell(ui->ledger, 1, row, j.entry.id.value, false);
            (void)Cell(ui->ledger, 2, row, j.referenceId.value, false); (void)Cell(ui->ledger, 3, row, j.currency.code, false);
            NumberCell(ui->ledger, 4, row, j.scale); (void)Cell(ui->ledger, 5, row, j.entry.lines[n].account_id.value, false);
            MinorCell(ui->ledger, 6, row, j.entry.lines[n].debit_minor); MinorCell(ui->ledger, 7, row, j.entry.lines[n].credit_minor);
            (void)Cell(ui->ledger, 8, row, j.reversal ? "Yes" : "No", false);
        }
      }
    }
    for (size_t i = 0U; i < counts.reconciliations; ++i) {
        UmiBankReconciliation r; int row = (int)i + 1;
        if (UmiBankOperationsReconciliationAt(ui->operations, i, &r) != UMI_STATUS_OK) continue;
        NumberCell(ui->reconciliation, 0, row, r.revision); (void)Cell(ui->reconciliation, 1, row, r.id.value, false);
        (void)Cell(ui->reconciliation, 2, row, r.accountId.value, false); MoneyCell(ui->reconciliation, 3, row, r.externalBalance);
        MoneyCell(ui->reconciliation, 4, row, r.bookedBalance); (void)Cell(ui->reconciliation, 5, row, r.matched ? "Matched" : "Break - investigate", false);
    }
    for (size_t i = 0U; i < counts.events; ++i) {
        UmiBankAuditEvent e; char date[24]; int row = (int)i + 1;
        if (UmiBankOperationsAuditAt(ui->operations, i, &e) != UMI_STATUS_OK) continue;
        NumberCell(ui->audit, 0, row, e.revision); (void)Cell(ui->audit, 1, row, e.command.requestId.value, false);
        (void)Cell(ui->audit, 2, row, e.actor.id.value, false); (void)Cell(ui->audit, 3, row, UmiBankActionName(e.command.action), false);
        (void)Cell(ui->audit, 4, row, e.command.id.value, false);
        (void)snprintf(date, sizeof date, "%04" PRId32 "-%02u-%02u", e.command.businessDate.year,
            (unsigned)e.command.businessDate.month, (unsigned)e.command.businessDate.day);
        (void)Cell(ui->audit, 5, row, date, false);
    }
}

static bool UnsignedText(const char *text, uint64_t maximum, uint64_t *out)
{
    uint64_t value = 0U;
    if (text == NULL || text[0] == '\0') return false;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned digit;
        if (text[i] < '0' || text[i] > '9') return false;
        digit = (unsigned)(text[i] - '0');
        if (value > maximum / 10U || (value == maximum / 10U && digit > maximum % 10U)) return false;
        value = value * 10U + digit;
    }
    *out = value; return true;
}
static const char *EntryText(GtkEntry *entry) { return gtk_editable_get_text(GTK_EDITABLE(entry)); }
static bool CopyId(GtkEntry *entry, UmiFinancialId *id)
{
    return umi_financial_id_assign(id, EntryText(entry)) == UMI_STATUS_OK;
}
static bool ReadCommand(BankUi *ui, UmiBankActor *actor, UmiBankCommand *command)
{
    static const char *const actors[] = {"test-maker", "test-checker", "test-operator"};
    static const uint32_t capabilities[] = {UMI_BANK_CAP_CUSTOMERS | UMI_BANK_CAP_PAYMENTS,
        UMI_BANK_CAP_APPROVE, UMI_BANK_CAP_OPERATE | UMI_BANK_CAP_TEST_FUNDING};
    guint action = gtk_drop_down_get_selected(ui->action), identity = gtk_drop_down_get_selected(ui->identity);
    uint32_t fields; uint64_t minor = 0U, scale = 0U; int year, month, day, consumed = 0;
    const char *date = EntryText(ui->date);
    if (action >= (guint)UMI_BANK_RECONCILE || identity >= G_N_ELEMENTS(actors)) return false;
    UmiBankCommandInit(command, (UmiBankAction)(action + 1U));
    fields = UmiBankActionFields(command->action);
    memset(actor, 0, sizeof *actor); (void)umi_financial_id_assign(&actor->id, actors[identity]);
    actor->capabilities = capabilities[identity];
    if (!CopyId(ui->request, &command->requestId) || !CopyId(ui->id, &command->id)) return false;
    if ((fields & UMI_BANK_FIELD_OWNER) != 0U && !CopyId(ui->owner, &command->ownerId)) return false;
    if ((fields & UMI_BANK_FIELD_SOURCE) != 0U && !CopyId(ui->source, &command->sourceAccountId)) return false;
    if ((fields & UMI_BANK_FIELD_DESTINATION) != 0U && !CopyId(ui->destination, &command->destinationAccountId)) return false;
    if ((fields & UMI_BANK_FIELD_NAME) != 0U &&
        umi_financial_core_copy(command->name, sizeof command->name, EntryText(ui->name)) != UMI_STATUS_OK) return false;
    if ((fields & UMI_BANK_FIELD_MONEY) != 0U) {
        if (!UnsignedText(EntryText(ui->minor), INT64_MAX, &minor) || !UnsignedText(EntryText(ui->scale), 9U, &scale) ||
            umi_accounting_currency_from_code(EntryText(ui->currency), &command->amount.currency) != UMI_STATUS_OK) return false;
        command->amount.minor_units = (int64_t)minor; command->amount.scale = (uint8_t)scale;
    }
    if ((fields & UMI_BANK_FIELD_STATE) != 0U)
        command->state = (UmiBankRecordState)(gtk_drop_down_get_selected(ui->recordState) + 1U);
    /* Bounded decimal fields prevent scanf overflow; the financial date helper
     * below still checks Gregorian validity (including leap years). */
    if (strlen(date) != 10U || date[4] != '-' || date[7] != '-' ||
        sscanf(date, "%4d-%2d-%2d%n", &year, &month, &day, &consumed) != 3 || consumed != 10 ||
        year < 1600 || year > 9999 || month < 1 || month > 12 || day < 1 || day > 31) return false;
    for (size_t i = 0U; i < 10U; ++i)
        if (i != 4U && i != 7U && (date[i] < '0' || date[i] > '9')) return false;
    command->businessDate = (UmiFinancialDate){(int32_t)year, (uint8_t)month, (uint8_t)day};
    command->timestampMillis = ui->timestampMillis; command->expectedRevision = ui->displayedRevision;
    return umi_financial_date_is_valid(command->businessDate);
}
static void NewRequest(GtkButton *button, gpointer data)
{
    BankUi *ui = SignalUi(data); char *id; GDateTime *now;
    (void)button;
    if (ui == NULL) return;
    id = g_uuid_string_random(); now = g_date_time_new_now_local();
    gtk_editable_set_text(GTK_EDITABLE(ui->request), id); g_free(id);
    ui->timestampMillis = (int64_t)(g_get_real_time() / 1000);
    if (now != NULL) { char *date = g_date_time_format(now, "%Y-%m-%d");
        gtk_editable_set_text(GTK_EDITABLE(ui->date), date); g_free(date); g_date_time_unref(now); }
    gtk_label_set_text(ui->message, "New request prepared. Review fields and the test identity before submitting.");
}
static void Submit(GtkButton *button, gpointer data)
{
    BankUi *ui = SignalUi(data); UmiBankActor actor; UmiBankCommand command; UmiBankReceipt receipt; UmiStatus status;
    (void)button;
    if (ui == NULL) return;
    if (!ReadCommand(ui, &actor, &command)) { Message(ui, UMI_STATUS_INVALID_ARGUMENT); return; }
    status = UmiBankOperationsExecute(ui->operations, &actor, &command, &receipt);
    if (status == UMI_STATUS_OK) {
        char message[220]; Refresh(ui);
        (void)snprintf(message, sizeof message, "%s at revision %" PRIu64
            ". Use New request before changing this command. No network payment occurred.",
            receipt.idempotent ? "Existing receipt returned; no duplicate posting" : "Committed locally", receipt.revision);
        gtk_label_set_text(ui->message, message);
    } else Message(ui, status);
}
static void Reload(GtkButton *button, gpointer data)
{
    BankUi *ui = SignalUi(data); UmiStatus status; (void)button;
    if (ui == NULL) return;
    status = UmiBankOperationsReload(ui->operations); Message(ui, status);
    if (status == UMI_STATUS_OK) Refresh(ui);
}
static void ShowStatement(GtkButton *button, gpointer data)
{
    static const char *const headers[] = {"Revision", "Journal", "Reference", "Debit minor", "Credit minor", "Running balance minor"};
    BankUi *ui = SignalUi(data); UmiBankStatement *statement; UmiStatus status;
    (void)button;
    if (ui == NULL) return;
    statement = g_new0(UmiBankStatement, 1);
    status = UmiBankOperationsStatement(ui->operations, EntryText(ui->statementAccount), 1U, ui->displayedRevision, statement);
    if (status != UMI_STATUS_OK) { Message(ui, status); g_free(statement); return; }
    Headers(ui->ledger, headers, G_N_ELEMENTS(headers));
    for (size_t i = 0U; i < statement->count; ++i) {
        const UmiBankStatementLine *line = &statement->lines[i]; int row = (int)i + 1;
        NumberCell(ui->ledger, 0, row, line->revision); (void)Cell(ui->ledger, 1, row, line->journalId.value, false);
        (void)Cell(ui->ledger, 2, row, line->referenceId.value, false); MinorCell(ui->ledger, 3, row, line->debitMinor);
        MinorCell(ui->ledger, 4, row, line->creditMinor); MinorCell(ui->ledger, 5, row, line->balanceMinor);
    }
    { char message[280];
      (void)snprintf(message, sizeof message, "Statement %.47s | %.3s scale %u | opening %" PRId64
          " minor | closing %" PRId64 " minor | revisions 1-%" PRIu64 ". Reload restores the full journal view.",
          statement->accountId.value, statement->closing.currency.code, (unsigned)statement->closing.scale,
          statement->opening.minor_units, statement->closing.minor_units, statement->lastRevision);
      gtk_label_set_text(ui->message, message);
    }
    gtk_notebook_set_current_page(ui->pages, 4); g_free(statement);
}
static void ActionChanged(GObject *object, GParamSpec *spec, gpointer data)
{
    BankUi *ui = SignalUi(data); UmiBankAction action; uint32_t fields; const char *hint;
    (void)object; (void)spec;
    if (ui == NULL) return;
    action = (UmiBankAction)(gtk_drop_down_get_selected(ui->action) + 1U);
    fields = UmiBankActionFields(action);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->name), (fields & UMI_BANK_FIELD_NAME) != 0U);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->owner), (fields & UMI_BANK_FIELD_OWNER) != 0U);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->source), (fields & UMI_BANK_FIELD_SOURCE) != 0U);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->destination), (fields & UMI_BANK_FIELD_DESTINATION) != 0U);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->minor), (fields & UMI_BANK_FIELD_MONEY) != 0U);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->currency), (fields & UMI_BANK_FIELD_MONEY) != 0U);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->scale), (fields & UMI_BANK_FIELD_MONEY) != 0U);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->recordState), (fields & UMI_BANK_FIELD_STATE) != 0U);
    switch (action) {
    case UMI_BANK_ACCOUNT_OPEN: hint = "Entity = new account ID; owner = customer ID; name, currency and scale required; amount must be 0. Use Test maker."; break;
    case UMI_BANK_BENEFICIARY_CREATE: hint = "Entity = beneficiary ID; owner = paying customer ID; destination = existing recipient account. Use Test maker."; break;
    case UMI_BANK_TRANSFER_SUBMIT: hint = "Entity = transfer ID; owner = beneficiary ID; source = paying account. Destination is optional and must match beneficiary. Use Test maker."; break;
    case UMI_BANK_TRANSFER_APPROVE: case UMI_BANK_TRANSFER_REJECT:
        hint = "Entity = existing transfer ID. Select Test checker; a maker cannot approve or reject their own request."; break;
    case UMI_BANK_TRANSFER_EXECUTE: case UMI_BANK_TRANSFER_REVERSE:
        hint = "Entity = existing transfer ID. Select Test operator. Execution requires prior approval; reversal creates a compensating journal."; break;
    case UMI_BANK_TEST_CREDIT: hint = "Simulation funding only. Source = account receiving test money. Select Test operator. This does not import real funds."; break;
    case UMI_BANK_CARD_ISSUE: hint = "Entity = simulated card ID; source = account; amount = per-transaction limit. Use Test maker. No PAN or provider is created."; break;
    case UMI_BANK_CARD_AUTHORISE: hint = "Entity = new authorisation/hold ID; owner = simulated card ID; amount = requested reservation. Use Test operator."; break;
    case UMI_BANK_CARD_CAPTURE: hint = "Entity = authorisation ID; amount = final capture, no more than authorised. The unused reservation is released. Use Test operator."; break;
    case UMI_BANK_CARD_VOID: case UMI_BANK_CARD_REFUND:
        hint = "Entity = authorisation ID. Void releases an active reservation; refund reverses the full captured amount. Use Test operator."; break;
    case UMI_BANK_RECONCILE: hint = "Entity = new reconciliation ID; source = account; amount = externally observed booked balance. A mismatch records a break, not a ledger adjustment. Use Test operator."; break;
    default: hint = "Supply the entity ID and enabled fields only. Customers/accounts/beneficiaries/card settings use Test maker; manual hold operations use Test operator."; break;
    }
    gtk_label_set_text(ui->hint, hint);
    NewRequest(NULL, ui->window);
}
static GtkEntry *FormEntry(GtkGrid *grid, const char *label, int column, int row, int maxLength, const char *initial)
{
    GtkWidget *entry = gtk_entry_new();
    GtkWidget *caption = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
    gtk_grid_attach(grid, caption, column, row, 1, 1);
    gtk_entry_set_max_length(GTK_ENTRY(entry), maxLength);
    gtk_widget_set_hexpand(entry, TRUE);
    if (initial != NULL) gtk_editable_set_text(GTK_EDITABLE(entry), initial);
    gtk_grid_attach(grid, entry, column + 1, row, 1, 1);
    gtk_widget_set_tooltip_text(entry, label);
    return GTK_ENTRY(entry);
}
static GtkGrid *Page(GtkNotebook *pages, const char *title)
{
    GtkWidget *scroll = gtk_scrolled_window_new(), *grid = gtk_grid_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_notebook_append_page(pages, scroll, gtk_label_new(title));
    return GTK_GRID(grid);
}
static void ChildDestroyed(GtkWidget *widget, gpointer data)
{
    BankLauncher *launcher = data;
    if (launcher->window == GTK_WINDOW(widget)) launcher->window = NULL;
}
static void LauncherFree(gpointer data)
{
    BankLauncher *launcher = data;
    if (launcher->window != NULL) {
        g_signal_handlers_disconnect_by_data(launcher->window, launcher);
        gtk_window_destroy(launcher->window);
    }
    g_free(launcher);
}
static void OpenWorkspace(GtkButton *button, gpointer data)
{
    static const char *const identities[] = {"Test maker", "Test checker", "Test operator", NULL};
    static const char *const states[] = {"Active", "Blocked", "Closed", NULL};
    const char *actions[UMI_BANK_RECONCILE + 1U];
    BankLauncher *launcher = data; BankUi *ui; GtkRoot *root;
    GtkWidget *box, *formScroll, *form, *toolbar, *buttonWidget, *banner;
    char *directory; UmiStatus status;
    if (launcher->window != NULL) { gtk_window_present(launcher->window); return; }
    root = gtk_widget_get_root(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(root)) return;
    ui = g_new0(BankUi, 1);
    ui->window = GTK_WINDOW(gtk_window_new()); launcher->window = ui->window;
    gtk_window_set_application(ui->window, gtk_window_get_application(GTK_WINDOW(root)));
    gtk_window_set_title(ui->window, "Umicom Bank - Local Banking Operations");
    gtk_window_set_default_size(ui->window, 1160, 800);
    gtk_window_set_transient_for(ui->window, GTK_WINDOW(root));
    gtk_window_set_destroy_with_parent(ui->window, TRUE);
    g_object_set_data_full(G_OBJECT(ui->window), "umicom-bank-operations", ui, UiFree);
    g_signal_connect(ui->window, "destroy", G_CALLBACK(UiClosed), ui);
    g_signal_connect(ui->window, "destroy", G_CALLBACK(ChildDestroyed), launcher);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(box, 12); gtk_widget_set_margin_end(box, 12);
    gtk_widget_set_margin_top(box, 12); gtk_widget_set_margin_bottom(box, 12);
    gtk_window_set_child(ui->window, box);
    banner = gtk_label_new("LOCAL SIMULATION - no real payment/card network, no authentication, no real funding. Test identities are not a security boundary.");
    gtk_label_set_wrap(GTK_LABEL(banner), TRUE); gtk_widget_add_css_class(banner, "heading"); gtk_box_append(GTK_BOX(box), banner);
    { GtkWidget *limitLabel = gtk_label_new("Bounded evaluation profile: 64 records per catalogue and 256 accepted commands. Retain databases as evidence; this is not a production banking deployment.");
      gtk_label_set_wrap(GTK_LABEL(limitLabel), TRUE); gtk_box_append(GTK_BOX(box), limitLabel); }
    ui->summary = GTK_LABEL(gtk_label_new("Opening the per-user banking database...")); gtk_box_append(GTK_BOX(box), GTK_WIDGET(ui->summary));
    form = gtk_grid_new(); gtk_grid_set_column_spacing(GTK_GRID(form), 12); gtk_grid_set_row_spacing(GTK_GRID(form), 6);
    formScroll = gtk_scrolled_window_new(); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(formScroll), form);
    gtk_widget_set_size_request(formScroll, -1, 250); gtk_box_append(GTK_BOX(box), formScroll);
    for (unsigned i = 0U; i < (unsigned)UMI_BANK_RECONCILE; ++i) actions[i] = UmiBankActionName((UmiBankAction)(i + 1U));
    actions[UMI_BANK_RECONCILE] = NULL;
    ui->action = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(actions));
    ui->identity = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(identities));
    gtk_grid_attach(GTK_GRID(form), gtk_label_new("Action"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(form), GTK_WIDGET(ui->action), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(form), gtk_label_new("Simulation identity"), 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(form), GTK_WIDGET(ui->identity), 3, 0, 1, 1);
    ui->request = FormEntry(GTK_GRID(form), "Request ID", 0, 1, 47, NULL);
    ui->id = FormEntry(GTK_GRID(form), "Entity ID", 2, 1, 47, NULL);
    ui->owner = FormEntry(GTK_GRID(form), "Owner / beneficiary / card ID", 0, 2, 47, NULL);
    ui->name = FormEntry(GTK_GRID(form), "Name", 2, 2, 95, NULL);
    ui->source = FormEntry(GTK_GRID(form), "Source account", 0, 3, 47, NULL);
    ui->destination = FormEntry(GTK_GRID(form), "Destination account", 2, 3, 47, NULL);
    ui->minor = FormEntry(GTK_GRID(form), "Integer minor units", 0, 4, 19, "0");
    ui->currency = FormEntry(GTK_GRID(form), "Currency", 2, 4, 3, "GBP");
    ui->scale = FormEntry(GTK_GRID(form), "Currency scale", 0, 5, 1, "2");
    ui->date = FormEntry(GTK_GRID(form), "Business date YYYY-MM-DD", 2, 5, 10, NULL);
    ui->recordState = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(states));
    gtk_grid_attach(GTK_GRID(form), gtk_label_new("New lifecycle state"), 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(form), GTK_WIDGET(ui->recordState), 1, 6, 1, 1);
    ui->hint = GTK_LABEL(gtk_label_new(NULL)); gtk_label_set_wrap(ui->hint, TRUE); gtk_box_append(GTK_BOX(box), GTK_WIDGET(ui->hint));
    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), toolbar);
    buttonWidget = gtk_button_new_with_label("New request"); g_signal_connect_object(buttonWidget, "clicked", G_CALLBACK(NewRequest), G_OBJECT(ui->window), 0); gtk_box_append(GTK_BOX(toolbar), buttonWidget);
    ui->submit = GTK_BUTTON(gtk_button_new_with_label("Submit command")); g_signal_connect_object(ui->submit, "clicked", G_CALLBACK(Submit), G_OBJECT(ui->window), 0); gtk_box_append(GTK_BOX(toolbar), GTK_WIDGET(ui->submit));
    buttonWidget = gtk_button_new_with_label("Reload committed data"); g_signal_connect_object(buttonWidget, "clicked", G_CALLBACK(Reload), G_OBJECT(ui->window), 0); gtk_box_append(GTK_BOX(toolbar), buttonWidget);
    ui->statementAccount = GTK_ENTRY(gtk_entry_new()); gtk_entry_set_placeholder_text(ui->statementAccount, "Statement account ID");
    gtk_entry_set_max_length(ui->statementAccount, 47); gtk_box_append(GTK_BOX(toolbar), GTK_WIDGET(ui->statementAccount));
    buttonWidget = gtk_button_new_with_label("Show statement"); g_signal_connect_object(buttonWidget, "clicked", G_CALLBACK(ShowStatement), G_OBJECT(ui->window), 0); gtk_box_append(GTK_BOX(toolbar), buttonWidget);
    ui->message = GTK_LABEL(gtk_label_new(NULL)); gtk_label_set_wrap(ui->message, TRUE); gtk_label_set_selectable(ui->message, TRUE);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(ui->message));
    ui->pages = GTK_NOTEBOOK(gtk_notebook_new()); gtk_widget_set_vexpand(GTK_WIDGET(ui->pages), TRUE); gtk_notebook_set_scrollable(ui->pages, TRUE);
    ui->customers = Page(ui->pages, "Customers"); ui->accounts = Page(ui->pages, "Accounts");
    ui->transfers = Page(ui->pages, "Transfers / beneficiaries"); ui->cards = Page(ui->pages, "Cards / holds");
    ui->ledger = Page(ui->pages, "Ledger / statement"); ui->reconciliation = Page(ui->pages, "Reconciliation"); ui->audit = Page(ui->pages, "Audit");
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(ui->pages));
    directory = g_build_filename(g_get_user_data_dir(), "Umicom", "Bank", NULL);
    ui->path = g_build_filename(directory, "bank-operations-simulation.sqlite", NULL);
    status = g_path_is_absolute(ui->path) && g_mkdir_with_parents(directory, 0700) == 0 ?
        UmiBankOperationsOpenSqlite(ui->path, &ui->operations) : UMI_STATUS_IO_ERROR;
    g_free(directory);
    { GtkWidget *pathLabel = gtk_label_new(ui->path); gtk_label_set_selectable(GTK_LABEL(pathLabel), TRUE);
      gtk_label_set_ellipsize(GTK_LABEL(pathLabel), PANGO_ELLIPSIZE_MIDDLE); gtk_box_append(GTK_BOX(box), pathLabel); }
    g_signal_connect_object(ui->action, "notify::selected", G_CALLBACK(ActionChanged), G_OBJECT(ui->window), 0);
    ActionChanged(NULL, NULL, ui->window);
    if (status == UMI_STATUS_OK) Refresh(ui);
    else { Message(ui, status); gtk_widget_set_sensitive(toolbar, FALSE); }
    gtk_window_present(ui->window);
}

GtkWidget *UmiGtk4BankOperationsLauncherCreate(void)
{
    BankLauncher *launcher = g_new0(BankLauncher, 1);
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *button = gtk_button_new_with_label("Banking operations");
    GtkWidget *label = gtk_label_new("Persistent local simulation: accounts, transfers, approvals, cards, statements and reconciliation");
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_widget_set_margin_start(bar, 8); gtk_widget_set_margin_end(bar, 8);
    gtk_widget_set_margin_top(bar, 6); gtk_widget_set_margin_bottom(bar, 6);
    gtk_box_append(GTK_BOX(bar), button); gtk_box_append(GTK_BOX(bar), label);
    g_object_set_data_full(G_OBJECT(button), "umicom-bank-launcher", launcher, LauncherFree);
    g_signal_connect(button, "clicked", G_CALLBACK(OpenWorkspace), launcher);
    return bar;
}
