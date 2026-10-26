/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/journal_posting/main.c
 *
 * PURPOSE:
 *   Demonstrate exact local journal review through the installed finance SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/posting_review.h"
#include "umicom/finance/accounting/posting_engine.h"
#include "umicom/finance/accounting/ledger_balance.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "Expectation refused at line %d: %s\n", __LINE__, #x); goto done; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "apply") && strcmp(argv[1], "stale-account") &&
        strcmp(argv[1], "closed-period") && strcmp(argv[1], "amount-boundary") &&
        strcmp(argv[1], "stale-book"))) {
        fputs("Usage: journal-posting apply|stale-account|closed-period|amount-boundary|stale-book\n", stderr);
        return 2;
    }
    int result = 1;
    UmiAccountingPostingReview *review = NULL;
    UmiAccountingJournalBook *book = calloc(1U, sizeof *book);
    UmiAccountingChartOfAccounts chart = {0};
    UmiAccountingAccountingCalendar calendar = {0};
    UmiAccountingJournalEntry *entry = malloc(sizeof *entry), *saved = malloc(sizeof *saved);
    REQUIRE(book && entry && saved);
    UmiAccountingLedgerAccount account;
    REQUIRE(umi_accounting_ledger_account_init(&account, "cash", "Practice cash", UMI_ACCOUNTING_ASSET,
        UMI_ACCOUNTING_NORMAL_DEBIT, true) == UMI_STATUS_OK);
    REQUIRE(umi_accounting_chart_of_accounts_add(&chart, &account) == UMI_STATUS_OK);
    REQUIRE(umi_accounting_ledger_account_init(&account, "income", "Practice income", UMI_ACCOUNTING_REVENUE,
        UMI_ACCOUNTING_NORMAL_CREDIT, true) == UMI_STATUS_OK);
    REQUIRE(umi_accounting_chart_of_accounts_add(&chart, &account) == UMI_STATUS_OK);
    UmiAccountingAccountingPeriod period;
    REQUIRE(umi_accounting_accounting_period_init(&period, "practice-period",
        (UmiFinancialDate){2026, 10U, 1U}, (UmiFinancialDate){2026, 10U, 31U},
        UMI_ACCOUNTING_PERIOD_OPEN) == UMI_STATUS_OK);
    REQUIRE(umi_accounting_accounting_calendar_add(&calendar, &period) == UMI_STATUS_OK);

    /* These synthetic accounts use GBP pennies by the example's explicit
     * convention. The journal structure itself does not enforce that metadata. */
    UmiAccountingAccountingEvent event;
    UmiAccountingPostingRule rule;
    UmiAccountingPostingEngine engine;
    umi_accounting_posting_engine_init(&engine);
    REQUIRE(umi_accounting_accounting_event_init(&event, "practice-event", "RECEIPT",
        (UmiFinancialDate){2026, 10U, 26U}, 1250, "GBP") == UMI_STATUS_OK);
    REQUIRE(umi_accounting_posting_rule_init(&rule, "receipt-rule", "RECEIPT",
        "cash", "income", true) == UMI_STATUS_OK);
    REQUIRE(umi_accounting_posting_engine_generate(&engine, &event, &rule, "practice-journal", entry) == UMI_STATUS_OK);
    int64_t debit = 0, credit = 0;
    REQUIRE(umi_accounting_journal_entry_totals(entry, &debit, &credit) == UMI_STATUS_OK);
    REQUIRE(debit == 1250 && credit == 1250);
    int64_t signed_line = 0;
    REQUIRE(umi_accounting_journal_line_signed_minor_checked(&entry->lines[1], &signed_line) == UMI_STATUS_OK);
    REQUIRE(signed_line == -1250);
    UmiAccountingLedgerBalance balance;
    REQUIRE(umi_accounting_ledger_balance_init(&balance, "practice-balance", "cash",
        UMI_ACCOUNTING_NORMAL_DEBIT, debit, 0) == UMI_STATUS_OK);
    int64_t net = 0;
    REQUIRE(umi_accounting_ledger_balance_net_minor_checked(&balance, &net) == UMI_STATUS_OK && net == debit);
    REQUIRE(umi_accounting_chart_of_accounts_validate(&chart) == UMI_STATUS_OK);
    REQUIRE(umi_accounting_accounting_calendar_validate(&calendar) == UMI_STATUS_OK);

    if (!strcmp(argv[1], "amount-boundary")) {
        memcpy(saved, entry, sizeof *saved);
        event.amount_minor = INT64_MIN;
        REQUIRE(umi_accounting_posting_engine_generate(&engine, &event, &rule,
            "refused-journal", entry) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(saved, entry, sizeof *saved) == 0);
        REQUIRE(engine.generated_entries == 1U && engine.rejected_events == 1U);
        puts("The unrepresentable magnitude was refused; the previous journal remains intact.");
    } else if (!strcmp(argv[1], "closed-period")) {
        calendar.items[0].status = UMI_ACCOUNTING_PERIOD_CLOSED;
        UmiAccountingPostingPreview preview;
        REQUIRE(umi_accounting_posting_inspect(entry, &chart, &calendar, book, &preview) == UMI_STATUS_OK);
        REQUIRE(preview.blocker == UMI_ACCOUNTING_POSTING_CLOSED_PERIOD);
        REQUIRE(umi_accounting_posting_review_create(entry, &chart, &calendar, book, &review) == UMI_STATUS_PERMISSION_DENIED);
        REQUIRE(review == NULL && book->count == 0U);
        puts("The closed period blocks local posting.");
    } else {
        REQUIRE(umi_accounting_posting_review_create(entry, &chart, &calendar, book, &review) == UMI_STATUS_OK);
        if (!strcmp(argv[1], "stale-account")) chart.items[0].posting_allowed = false;
        if (!strcmp(argv[1], "stale-book")) {
            *saved = *entry;
            REQUIRE(umi_accounting_id_assign(&saved->id, "another-journal") == UMI_STATUS_OK);
            REQUIRE(umi_accounting_journal_book_post(book, saved) == UMI_STATUS_OK);
        }
        UmiStatus status = umi_accounting_posting_review_apply(review, entry, &chart, &calendar, book);
        if (!strcmp(argv[1], "apply")) {
            REQUIRE(status == UMI_STATUS_OK && book->count == 1U);
            REQUIRE(umi_accounting_journal_book_validate(book) == UMI_STATUS_OK);
            REQUIRE(umi_accounting_posting_review_apply(review, entry, &chart, &calendar, book) == UMI_STATUS_INVALID_STATE);
            printf("Stored one local journal: debit=%" PRId64 ", credit=%" PRId64 " minor units.\n", debit, credit);
        } else {
            REQUIRE(status == UMI_STATUS_INVALID_STATE);
            REQUIRE(book->count == (!strcmp(argv[1], "stale-book") ? 1U : 0U));
            REQUIRE(umi_accounting_journal_book_find(book, "practice-journal") == NULL);
            puts("Changed local prerequisites require a fresh review.");
        }
    }
    puts("Synthetic in-memory example only; no bank, payment rail or durable ledger was contacted.");
    result = ferror(stdout) ? 1 : 0;
done:
    umi_accounting_posting_review_destroy(review);
    free(saved);
    free(entry);
    free(book);
    return result;
}
