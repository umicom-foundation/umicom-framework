/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_journal_boundaries.c
 *
 * PURPOSE:
 *   Verify exact journal arithmetic, bounded restored values and failure-atomic publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/journal_book.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static void Pair(UmiAccountingJournalEntry *entry) {
    CHECK(umi_accounting_journal_entry_init(entry, "journal", (UmiFinancialDate){2026, 10U, 26U}) == UMI_STATUS_OK);
    UmiAccountingJournalLine line;
    CHECK(umi_accounting_journal_line_init(&line, "debit", "cash", 10, 0) == UMI_STATUS_OK);
    CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
    CHECK(umi_accounting_journal_line_init(&line, "credit", "income", 0, 10) == UMI_STATUS_OK);
    CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
}
int main(int argc, char **argv) {
    CHECK(argc == 2); const char *mode = argv[1];
    UmiAccountingJournalEntry *entry = malloc(sizeof *entry), *before = malloc(sizeof *before);
    CHECK(entry && before); Pair(entry);
    UmiAccountingJournalLine line = entry->lines[0], originalLine;
    int64_t debit = 71, credit = 72; UmiStatus status;
    if (!strcmp(mode, "named-statuses")) {
        const UmiAccountingJournalStatus statuses[] = {
            UMI_ACCOUNTING_JOURNAL_DRAFT, UMI_ACCOUNTING_JOURNAL_VALIDATED,
            UMI_ACCOUNTING_JOURNAL_POSTED, UMI_ACCOUNTING_JOURNAL_REVERSED
        };
        for (size_t i = 0U; i < sizeof statuses / sizeof statuses[0]; ++i) {
            entry->status = statuses[i];
            CHECK(umi_accounting_journal_entry_valid(entry));
            CHECK(umi_accounting_journal_entry_balanced(entry));
        }
    } else if (!strcmp(mode, "normal") || !strcmp(mode, "empty") || !strcmp(mode, "maximum")) {
        if (!strcmp(mode, "empty")) entry->line_count = 0U;
        if (!strcmp(mode, "maximum")) { entry->lines[0].debit_minor = INT64_MAX; entry->lines[1].credit_minor = INT64_MAX; }
        CHECK(umi_accounting_journal_entry_totals(entry, &debit, &credit) == UMI_STATUS_OK);
        CHECK(debit == credit && debit == (!strcmp(mode, "empty") ? 0 : !strcmp(mode, "maximum") ? INT64_MAX : 10));
        CHECK(umi_accounting_journal_entry_balanced(entry) == (strcmp(mode, "empty") != 0));
    } else if (!strcmp(mode, "signed-credit") || !strcmp(mode, "signed-invalid") || !strcmp(mode, "signed-null")) {
        line = entry->lines[1]; line.credit_minor = INT64_MAX;
        if (!strcmp(mode, "signed-invalid")) line.debit_minor = INT64_MIN;
        status = umi_accounting_journal_line_signed_minor_checked(!strcmp(mode, "signed-null") ? NULL : &line, &debit);
        if (!strcmp(mode, "signed-credit")) CHECK(status == UMI_STATUS_OK && debit == -INT64_MAX);
        else {
            CHECK(status == UMI_STATUS_INVALID_ARGUMENT && debit == 71);
            CHECK(umi_accounting_journal_line_signed_minor(!strcmp(mode, "signed-null") ? NULL : &line) == 0);
        }
    } else if (!strcmp(mode, "line-init-atomic") || !strcmp(mode, "line-init-alias")) {
        /* Capture representation bytes for the failure-preservation assertion. */
#if 0
        originalLine = line;
#endif
        memcpy(&originalLine, &line, sizeof line);
        status = umi_accounting_journal_line_init(&line, line.id.value, line.account_id.value,
            !strcmp(mode, "line-init-atomic") ? INT64_MIN : 5, 0);
        if (!strcmp(mode, "line-init-atomic")) CHECK(status != UMI_STATUS_OK && memcmp(&line, &originalLine, sizeof line) == 0);
        else CHECK(status == UMI_STATUS_OK && !strcmp(line.id.value, originalLine.id.value) &&
            !strcmp(line.account_id.value, originalLine.account_id.value) && line.debit_minor == 5);
    } else if (!strcmp(mode, "entry-init-atomic") || !strcmp(mode, "entry-init-alias")) {
        memcpy(before, entry, sizeof *entry);
        UmiFinancialDate date = entry->accounting_date;
        if (!strcmp(mode, "entry-init-atomic")) date.day = 0U;
        status = umi_accounting_journal_entry_init(entry, entry->id.value, date);
        if (!strcmp(mode, "entry-init-atomic")) CHECK(status != UMI_STATUS_OK && memcmp(entry, before, sizeof *entry) == 0);
        else CHECK(status == UMI_STATUS_OK && !strcmp(entry->id.value, before->id.value) && entry->line_count == 0U);
    } else if (strncmp(mode, "append-", 7U) == 0) {
        if (!strcmp(mode, "append-overflow")) entry->lines[0].debit_minor = INT64_MAX;
        else if (!strcmp(mode, "append-invalid-existing")) entry->lines[1].account_id.value[0] = '\0';
        else if (!strcmp(mode, "append-readonly")) entry->status = UMI_ACCOUNTING_JOURNAL_POSTED;
        else if (!strcmp(mode, "append-full"))
            while (entry->line_count < UMI_ACCOUNTING_MAX_LINES)
                CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
        else if (strcmp(mode, "append-alias")) return 2;
        memcpy(before, entry, sizeof *entry);
        status = umi_accounting_journal_entry_add_line(entry, !strcmp(mode, "append-alias") ? &entry->lines[0] : &line);
        if (!strcmp(mode, "append-alias")) CHECK(status == UMI_STATUS_OK && entry->line_count == 3U && entry->lines[2].debit_minor == 10);
        else CHECK(status != UMI_STATUS_OK && memcmp(entry, before, sizeof *entry) == 0);
    } else if (strncmp(mode, "book-", 5U) == 0) {
        UmiAccountingJournalBook *book = malloc(sizeof *book), *saved = malloc(sizeof *saved); CHECK(book && saved);
        umi_accounting_journal_book_init(book);
        CHECK(umi_accounting_journal_book_post(book, entry) == UMI_STATUS_OK);
        if (!strcmp(mode, "book-corrupt-count")) book->count = SIZE_MAX;
        else if (!strcmp(mode, "book-corrupt-entry")) book->items[0].line_count = SIZE_MAX;
        else if (!strcmp(mode, "book-post-overflow")) {
            entry->lines[0].debit_minor = INT64_MAX;
            entry->lines[2] = entry->lines[0]; entry->lines[2].debit_minor = 1; entry->line_count = 3U;
        } else if (!strcmp(mode, "book-full")) {
            for (size_t i = 1U; i < UMI_ACCOUNTING_MAX_ITEMS; ++i) {
                char id[40]; int n = snprintf(id, sizeof id, "journal-%zu", i); CHECK(n > 0 && (size_t)n < sizeof id);
                CHECK(umi_accounting_id_assign(&entry->id, id) == UMI_STATUS_OK);
                CHECK(umi_accounting_journal_book_post(book, entry) == UMI_STATUS_OK);
            }
        } else if (strcmp(mode, "book-duplicate") && strcmp(mode, "book-null")) return 2;
        memcpy(saved, book, sizeof *saved);
        status = umi_accounting_journal_book_post(!strcmp(mode, "book-null") ? NULL : book, entry);
        CHECK(status != UMI_STATUS_OK && memcmp(saved, book, sizeof *saved) == 0);
        if (!strcmp(mode, "book-corrupt-count") || !strcmp(mode, "book-corrupt-entry"))
            CHECK(umi_accounting_journal_book_find(book, "journal") == NULL);
        if (!strcmp(mode, "book-duplicate")) CHECK(status == UMI_STATUS_ALREADY_EXISTS);
        free(saved); free(book);
    } else {
        if (!strcmp(mode, "invalid-count")) entry->line_count = SIZE_MAX;
        else if (!strcmp(mode, "invalid-date")) entry->accounting_date.month = 0U;
        else if (!strcmp(mode, "invalid-status")) entry->status = (UmiAccountingJournalStatus)99;
        else if (!strcmp(mode, "negative-status")) entry->status = (UmiAccountingJournalStatus)-1;
        else if (!strcmp(mode, "invalid-id")) entry->id.value[0] = '\0';
        else if (!strcmp(mode, "unterminated-id")) memset(entry->id.value, 'a', sizeof entry->id.value);
        else if (!strcmp(mode, "invalid-line-id")) entry->lines[1].id.value[0] = '\0';
        else if (!strcmp(mode, "invalid-account")) entry->lines[1].account_id.value[0] = '\0';
        else if (!strcmp(mode, "unterminated-account")) memset(entry->lines[1].account_id.value, 'a', sizeof entry->lines[1].account_id.value);
        else if (!strcmp(mode, "negative-debit")) entry->lines[0].debit_minor = INT64_MIN;
        else if (!strcmp(mode, "negative-credit")) entry->lines[1].credit_minor = INT64_MIN;
        else if (!strcmp(mode, "both-sides")) entry->lines[0].credit_minor = 1;
        else if (!strcmp(mode, "zero-sides")) entry->lines[0].debit_minor = 0;
        else if (!strcmp(mode, "debit-overflow") || !strcmp(mode, "credit-overflow") || !strcmp(mode, "both-overflow")) {
            entry->lines[0].debit_minor = INT64_MAX; entry->lines[1].credit_minor = INT64_MAX;
            entry->lines[2] = entry->lines[!strcmp(mode, "credit-overflow") ? 1 : 0]; entry->line_count = 3U;
            if (!strcmp(mode, "both-overflow")) { entry->lines[3] = entry->lines[1]; entry->line_count = 4U; }
        } else if (strcmp(mode, "null-totals") && strcmp(mode, "same-output") && strcmp(mode, "null-entry")) return 2;
        memcpy(before, entry, sizeof *entry);
        status = umi_accounting_journal_entry_totals(!strcmp(mode, "null-entry") ? NULL : entry,
            !strcmp(mode, "null-totals") ? NULL : &debit, !strcmp(mode, "same-output") ? &debit : &credit);
        CHECK(status != UMI_STATUS_OK && debit == 71 && credit == 72 && memcmp(entry, before, sizeof *entry) == 0);
        if (strcmp(mode, "null-totals") && strcmp(mode, "same-output")) {
            const UmiAccountingJournalEntry *checked = !strcmp(mode, "null-entry") ? NULL : entry;
            CHECK(!umi_accounting_journal_entry_balanced(checked));
            CHECK(umi_accounting_journal_entry_total_debit(checked) == 0 && umi_accounting_journal_entry_total_credit(checked) == 0);
        }
    }
    free(before); free(entry); return 0;
}
