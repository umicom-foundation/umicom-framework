/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/finance_operations_report_gtk4.c
 *
 * PURPOSE:
 *   Present copied ledger, order, settlement and report records without maintaining a second financial model.
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

static const char *AccountClass(UmiAccountingAccountClass kind)
{
    switch (kind) {
    case UMI_ACCOUNTING_ASSET: return "Asset";
    case UMI_ACCOUNTING_LIABILITY: return "Liability";
    case UMI_ACCOUNTING_EQUITY: return "Equity";
    case UMI_ACCOUNTING_REVENUE: return "Income";
    case UMI_ACCOUNTING_EXPENSE: return "Expense";
    default: return "Unknown";
    }
}
static void Money(GString *text, int64_t minor, UmiCurrency currency, uint8_t scale)
{
    char amount[64];
    UmiStatus status = UmiFinanceOperationsFormatAmount(minor, scale, amount, sizeof(amount));
    if (status == UMI_STATUS_OK) g_string_append_printf(text, "%s %s", currency.code, amount);
    else g_string_append(text, "Amount unavailable");
}
static void Date(GString *text, UmiFinancialDate date)
{
    g_string_append_printf(text, "%04d-%02u-%02u", (int)date.year, (unsigned)date.month, (unsigned)date.day);
}
static const char *JournalState(UmiAccountingJournalStatus state)
{
    switch (state) {
    case UMI_ACCOUNTING_JOURNAL_DRAFT: return "Draft";
    case UMI_ACCOUNTING_JOURNAL_VALIDATED: return "Approved";
    case UMI_ACCOUNTING_JOURNAL_POSTED: return "Posted";
    case UMI_ACCOUNTING_JOURNAL_REVERSED: return "Reversed by compensation";
    default: return "Unknown";
    }
}
static const char *OrderState(UmiOrderStatus state)
{
    switch (state) {
    case UMI_ORDER_ACCEPTED: return "Resting";
    case UMI_ORDER_PARTIALLY_FILLED: return "Partly filled";
    case UMI_ORDER_FILLED: return "Filled";
    case UMI_ORDER_CANCELLED: return "Remainder cancelled";
    default: return "Unknown";
    }
}
static UmiStatus AppendReport(UmiFinanceOperationsGtkPanel *panel, guint view,
    const UmiFinanceOperationCounts *counts, GString *text)
{
    UmiStatus status;
    if (view == 0U) {
        g_string_append(text, "This is a local practice book, not a connected bank or exchange.\n\n");
        g_string_append_printf(text, "Accounts: %zu    Periods: %zu    Journals: %zu\nParticipants: %zu    Instruments: %zu    Holdings: %zu\nOrders: %zu    Fills: %zu    Reconciliation records: %zu\n\n",
            counts->accounts, counts->periods, counts->journals, counts->participants,
            counts->instruments, counts->holdings, counts->orders, counts->fills, counts->reconciliations);
        for (size_t index = 0U; index < counts->participants; ++index) {
            UmiFinanceOperationParticipant participant;
            UmiFinanceOperationAvailability cash;
            status = UmiFinanceOperationsParticipantAt(panel->operations, index, &participant);
            if (status == UMI_STATUS_OK) status = UmiFinanceOperationsCash(panel->operations, participant.id.value, &cash);
            if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s (%s) — cash account %s\n  Booked %" PRId64 "  Reserved %" PRId64 "  Available %" PRId64 " minor units\n",
                participant.id.value, participant.name, participant.cashAccountId.value, cash.booked, cash.reserved, cash.available);
        }
        g_string_append(text, "\nChoose a report above, or prepare an operation on the left.\nNew request gives a different operation its own receipt.\nReload shared book reads changes made in the other application.\n");
    } else if (view == 1U) {
        g_string_append(text, "LEDGER ACCOUNTS — posted normal-side balances\n\n");
        for (size_t index = 0U; index < counts->accounts; ++index) {
            UmiFinanceOperationAccount account; int64_t balance;
            status = UmiFinanceOperationsAccountAt(panel->operations, index, &account);
            if (status == UMI_STATUS_OK) status = UmiFinanceOperationsAccountBalance(panel->operations, account.id.value, &balance);
            if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s — %s [%s]\n  Balance ", account.id.value, account.name, AccountClass(account.accountClass));
            Money(text, balance, account.currency, account.scale);
            g_string_append_printf(text, "\n  Debits %" PRId64 "  Credits %" PRId64 " minor units; last posting revision %" PRIu64 "\n\n",
                account.debitMinor, account.creditMinor, account.lastPostingRevision);
        }
    } else if (view == 2U) {
        g_string_append(text, "ACCOUNTING PERIODS\n\n");
        for (size_t index = 0U; index < counts->periods; ++index) {
            UmiFinanceOperationPeriod period;
            status = UmiFinanceOperationsPeriodAt(panel->operations, index, &period); if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s: ", period.id.value); Date(text, period.startDate); g_string_append(text, " to "); Date(text, period.endDate);
            g_string_append_printf(text, "\n  %s; prepared by %s; closed by %s\n\n",
                period.status == UMI_ACCOUNTING_PERIOD_OPEN ? "Open" : period.status == UMI_ACCOUNTING_PERIOD_SOFT_CLOSED ? "Close awaiting review" : "Closed",
                period.preparedBy.value[0] ? period.preparedBy.value : "—", period.closedBy.value[0] ? period.closedBy.value : "—");
        }
    } else if (view == 3U) {
        g_string_append(text, "JOURNALS — amounts below are minor units\n\n");
        for (size_t index = 0U; index < counts->journals; ++index) {
            UmiFinanceOperationJournal journal;
            status = UmiFinanceOperationsJournalAt(panel->operations, index, &journal); if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s — %s, %s, period %s\n  ", journal.entry.id.value,
                JournalState(journal.entry.status), journal.currency.code, journal.periodId.value);
            Date(text, journal.entry.accounting_date);
            g_string_append_printf(text, "; maker %s; checker %s; posting revision %" PRIu64 "\n",
                journal.makerId.value, journal.checkerId.value, journal.postingRevision);
            for (size_t line = 0U; line < journal.entry.line_count; ++line)
                g_string_append_printf(text, "  %s  debit %" PRId64 "  credit %" PRId64 "\n",
                    journal.entry.lines[line].account_id.value, journal.entry.lines[line].debit_minor, journal.entry.lines[line].credit_minor);
            if (journal.reverses.value[0]) g_string_append_printf(text, "  Compensates %s\n", journal.reverses.value);
            if (journal.reversedBy.value[0]) g_string_append_printf(text, "  Compensated by %s\n", journal.reversedBy.value);
            if (journal.settlement) g_string_append_printf(text, "  Cash leg of local fill %s\n", journal.fillId.value);
            g_string_append_c(text, '\n');
        }
    } else if (view == 4U) {
        g_string_append(text, "ORDER BOOK AND HISTORY — limit/GTC, whole lots\n\n");
        for (size_t index = 0U; index < counts->instruments; ++index) {
            UmiFinanceOperationInstrument instrument;
            status = UmiFinanceOperationsInstrumentAt(panel->operations, index, &instrument); if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s — %s (%s), %s\n  Tick value %" PRId64 " minor units; units/lot %" PRId64 "; order limit %" PRId64 " lots\n",
                instrument.id.value, instrument.name, instrument.currency.code,
                instrument.marketState == UMI_MARKET_OPEN ? "Open" : "Halted", instrument.minorPerTick, instrument.unitsPerLot, instrument.maxOrderLots);
        }
        g_string_append_c(text, '\n');
        for (size_t index = 0U; index < counts->orders; ++index) {
            UmiFinanceOperationOrder order;
            status = UmiFinanceOperationsOrderAt(panel->operations, index, &order); if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s — %s %s, participant %s\n  Limit %" PRId64 " ticks; original %" PRId64 ", remaining %" PRId64 ", filled %" PRId64 " lots\n  %s; priority sequence %" PRIu64 "\n\n",
                order.id.value, order.side == UMI_SIDE_BUY ? "Buy" : "Sell", order.instrumentId.value, order.participantId.value,
                order.limitTicks, order.originalLots, order.remainingLots, order.filledLots, OrderState(order.status), order.sequence);
        }
    } else if (view == 5U) {
        g_string_append(text, "FILLS AND LOCAL SETTLEMENT\n\n");
        for (size_t index = 0U; index < counts->fills; ++index) {
            UmiFinanceOperationFill fill;
            status = UmiFinanceOperationsFillAt(panel->operations, index, &fill); if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s — %s\n  Buyer %s (%s); seller %s (%s)\n  %" PRId64 " lots at %" PRId64 " ticks; cash ",
                fill.id.value, fill.instrumentId.value, fill.buyerId.value, fill.buyOrderId.value,
                fill.sellerId.value, fill.sellOrderId.value, fill.lots, fill.executionTicks);
            Money(text, fill.cash.minor_units, fill.cash.currency, fill.cash.scale);
            g_string_append_printf(text, "\n  %s; clearing actor %s\n",
                fill.state == UMI_SETTLEMENT_PENDING ? "Awaiting clearing" : fill.state == UMI_SETTLEMENT_MATCHED ? "Cleared, awaiting settlement" : "Settled locally",
                fill.clearingActor.value[0] ? fill.clearingActor.value : "—");
            if (fill.settlementJournalId.value[0]) { g_string_append_printf(text, "  Journal %s; settled ", fill.settlementJournalId.value); Date(text, fill.settlementDate); g_string_append_c(text, '\n'); }
            g_string_append_c(text, '\n');
        }
    } else if (view == 6U) {
        g_string_append(text, "SIMULATED CUSTODY — whole lots, not exchange-owned inventory\n\n");
        for (size_t index = 0U; index < counts->holdings; ++index) {
            UmiFinanceOperationHolding holding; UmiFinanceOperationAvailability lots;
            status = UmiFinanceOperationsHoldingAt(panel->operations, index, &holding);
            if (status == UMI_STATUS_OK) status = UmiFinanceOperationsLots(panel->operations, holding.participantId.value, holding.instrumentId.value, &lots);
            if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s / %s\n  Booked %" PRId64 "; reserved %" PRId64 "; available %" PRId64 " lots\n\n",
                holding.participantId.value, holding.instrumentId.value, lots.booked, lots.reserved, lots.available);
        }
    } else if (view == 7U) {
        g_string_append(text, "RECONCILIATION HISTORY — observations do not alter balances\n\n");
        for (size_t index = 0U; index < counts->reconciliations; ++index) {
            UmiFinanceOperationReconciliation record;
            status = UmiFinanceOperationsReconciliationAt(panel->operations, index, &record); if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%s — %s, %s\n  External %" PRId64 "; ledger %" PRId64 " minor units\n  Covers posting revision %" PRIu64 "; recorded by %s at revision %" PRIu64 "\n\n",
                record.id.value, record.accountId.value, record.matched ? "Matched at observation" : "Difference to investigate",
                record.externalMinor, record.ledgerMinor, record.postingRevision, record.actorId.value, record.revision);
        }
    } else if (view == 8U) {
        g_string_append(text, "ACCEPTED OPERATIONS — actor labels are not authenticated identities\n\n");
        for (size_t index = 0U; index < counts->events; ++index) {
            UmiFinanceOperationAudit audit;
            status = UmiFinanceOperationsAuditAt(panel->operations, index, &audit); if (status != UMI_STATUS_OK) return status;
            g_string_append_printf(text, "%" PRIu64 "  %s\n  %s; record %s; actor %s\n\n", audit.revision,
                audit.requestId.value, UmiFinanceOperationKindText(audit.kind), audit.entityId.value, audit.actorId.value);
        }
    } else if (view == 9U) {
        UmiFinanceTrialBalance report;
        UmiCurrency currency = {{0}};
        const char *code = gtk_editable_get_text(GTK_EDITABLE(panel->reportCurrency));
        int64_t scale;
        if (strlen(code) != 3U) return UMI_STATUS_INVALID_ARGUMENT;
        memcpy(currency.code, code, 3U);
        status = UmiFinanceOperationsParseInteger(gtk_editable_get_text(GTK_EDITABLE(panel->reportScale)), &scale);
        if (status != UMI_STATUS_OK || scale < 0 || scale > 9) return UMI_STATUS_INVALID_ARGUMENT;
        status = UmiFinanceOperationsTrialBalance(panel->operations,
            gtk_editable_get_text(GTK_EDITABLE(panel->reportPeriod)), currency, (uint8_t)scale, &report);
        if (status != UMI_STATUS_OK) return status;
        g_string_append_printf(text, "TRIAL BALANCE — %s, %s, scale %u\n\nPositive opening/closing amounts are debit balances; negative amounts are credit balances.\n\n",
            report.periodId.value, report.currency.code, (unsigned)report.scale);
        for (size_t index = 0U; index < report.lineCount; ++index) {
            const UmiFinanceTrialBalanceLine *line = &report.lines[index];
            g_string_append_printf(text, "%s [%s]\n  Opening ", line->accountId.value, AccountClass(line->accountClass));
            Money(text, line->openingDebitNet, currency, report.scale); g_string_append(text, "; movement debit ");
            Money(text, line->periodDebitMinor, currency, report.scale); g_string_append(text, "; credit ");
            Money(text, line->periodCreditMinor, currency, report.scale); g_string_append(text, "; closing ");
            Money(text, line->closingDebitNet, currency, report.scale); g_string_append_c(text, '\n');
        }
        g_string_append(text, "\nPeriod debit total "); Money(text, report.periodDebitMinor, currency, report.scale);
        g_string_append(text, "; credit total "); Money(text, report.periodCreditMinor, currency, report.scale);
        g_string_append(text, "\nClosing debit total "); Money(text, report.closingDebitMinor, currency, report.scale);
        g_string_append(text, "; credit total "); Money(text, report.closingCreditMinor, currency, report.scale);
        g_string_append_printf(text, "\n%s\n", report.balanced ? "The trial balance agrees." : "The trial balance does not agree.");
    } else return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
void FinanceGtkRefresh(UmiFinanceOperationsGtkPanel *panel)
{
    UmiFinanceOperationCounts counts;
    GString *text;
    UmiStatus status;
    if (panel == NULL || panel->operations == NULL) return;
    text = g_string_new("UMICOM — LOCAL FINANCIAL PRACTICE\n");
    status = UmiFinanceOperationsCounts(panel->operations, &counts);
    if (status == UMI_STATUS_OK) {
        g_string_append_printf(text, "Book revision %" PRIu64 "; %s. No real transactions.\n\n",
            counts.revision, counts.durable ? "SQLite storage" : "memory-only storage");
        status = AppendReport(panel, gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->view)), &counts, text);
    }
    if (status != UMI_STATUS_OK) {
        g_string_truncate(text, 0U);
        g_string_append_printf(text, "Report unavailable: %s\nStatus %d\n", UmiFinanceOperationsExplainStatus(status), (int)status);
    }
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->report)), text->str, -1);
    g_string_free(text, TRUE);
}
