/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_deposit_summary.c
 *
 * PURPOSE:
 *   Verify customer and currency scope, bounded records and failure-atomic balance summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_account_book.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "Requirement failed at line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

static int append(UmiBankingDepositAccountBook *book, const char *id,
    const char *customer, const char *currency, int64_t ledger, int64_t available,
    UmiBankingAccountStatus status)
{
    UmiBankingDepositAccount account = {0};
    return umi_banking_deposit_account_init(&account, id, customer, "deposit",
        currency, ledger, available, status) == UMI_STATUS_OK &&
        umi_banking_deposit_account_book_add(book, &account) == UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiBankingDepositAccountBook book = {0};
    UmiBankingDepositSummary summary, before;
    UmiCurrency gbp = {{'G','B','P','\0'}};
    UmiStatus expected = UMI_STATUS_OK;
    REQUIRE(argc == 2);
    memset(&summary, 0xa5, sizeof summary);
    memcpy(&before, &summary, sizeof before);
    if (strcmp(argv[1], "empty") == 0) {
        REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "alice", &gbp, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.account_count == 0 && summary.ledger_minor == 0 &&
            summary.available_minor == 0 && summary.reserved_minor == 0);
        REQUIRE(strcmp(summary.customer_id.value, "alice") == 0);
        REQUIRE(strcmp(summary.currency.code, "GBP") == 0);
        return 0;
    }
    if (strcmp(argv[1], "arguments") == 0) {
        UmiCurrency malformed = {{'G','B','P','X'}};
        REQUIRE(UmiBankingDepositAccountBookSummarise(NULL, "alice", &gbp, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiBankingDepositAccountBookSummarise(&book, NULL, &gbp, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "", &gbp, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "alice", NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "alice", &malformed, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "alice", &gbp, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&summary, &before, sizeof summary) == 0);
        return 0;
    }
    if (strcmp(argv[1], "overflow") == 0 || strcmp(argv[1], "underflow") == 0) {
        int negative = strcmp(argv[1], "underflow") == 0;
        int64_t limit = negative ? INT64_MIN : INT64_MAX;
        int64_t increment = negative ? -1 : 1;
        REQUIRE(append(&book, "a", "alice", "GBP", limit, limit, UMI_BANKING_ACCOUNT_OPEN));
        REQUIRE(append(&book, "b", "alice", "GBP", increment, increment, UMI_BANKING_ACCOUNT_OPEN));
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(argv[1], "reserved-overflow") == 0) {
        REQUIRE(append(&book, "a", "alice", "GBP", 0, -INT64_MAX, UMI_BANKING_ACCOUNT_OPEN));
        REQUIRE(append(&book, "b", "alice", "GBP", 1, 0, UMI_BANKING_ACCOUNT_OPEN));
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(argv[1], "capacity") == 0) {
        for (size_t i = 0; i < UMI_BANKING_MAX_ITEMS; ++i) {
            char id[32];
            int count = snprintf(id, sizeof id, "account-%zu", i);
            REQUIRE(count > 0 && (size_t)count < sizeof id);
            REQUIRE(append(&book, id, "alice", "GBP", 10, 9, UMI_BANKING_ACCOUNT_OPEN));
        }
        REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "alice", &gbp, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.account_count == UMI_BANKING_MAX_ITEMS);
        REQUIRE(summary.ledger_minor == 640 && summary.available_minor == 576 && summary.reserved_minor == 64);
        return 0;
    } else {
        REQUIRE(append(&book, "a", "alice", "GBP", 10000, 8500, UMI_BANKING_ACCOUNT_OPEN));
        REQUIRE(append(&book, "b", "alice", "GBP", 2000, 1000, UMI_BANKING_ACCOUNT_BLOCKED));
        REQUIRE(append(&book, "c", "bob", "GBP", 500, 500, UMI_BANKING_ACCOUNT_OPEN));
        REQUIRE(append(&book, "d", "alice", "USD", 400, 300, UMI_BANKING_ACCOUNT_OPEN));
        if (strcmp(argv[1], "scope") == 0) {
            REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "alice", &gbp, &summary) == UMI_STATUS_OK);
            REQUIRE(summary.account_count == 2 && summary.open_count == 1 && summary.blocked_count == 1);
            REQUIRE(summary.pending_count == 0 && summary.closed_count == 0);
            REQUIRE(summary.ledger_minor == 12000 && summary.available_minor == 9500 && summary.reserved_minor == 2500);
            REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "unknown", &gbp, &summary) == UMI_STATUS_OK);
            REQUIRE(summary.account_count == 0 && summary.ledger_minor == 0);
            return 0;
        } else if (strcmp(argv[1], "statuses-and-negative") == 0) {
            REQUIRE(append(&book, "e", "alice", "GBP", -100, -200, UMI_BANKING_ACCOUNT_PENDING));
            REQUIRE(append(&book, "f", "alice", "GBP", 0, 0, UMI_BANKING_ACCOUNT_CLOSED));
            REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "alice", &gbp, &summary) == UMI_STATUS_OK);
            REQUIRE(summary.account_count == 4 && summary.open_count == 1 &&
                summary.blocked_count == 1 && summary.pending_count == 1 && summary.closed_count == 1);
            REQUIRE(summary.ledger_minor == 11900 && summary.available_minor == 9300 && summary.reserved_minor == 2600);
            return 0;
        } else if (strcmp(argv[1], "corrupt-count") == 0) {
            book.count = SIZE_MAX;
        } else if (strcmp(argv[1], "invalid-unselected") == 0) {
            /* Corrupt records cannot be hidden by selecting a different customer. */
            book.items[2].available_minor = 501;
        } else if (strcmp(argv[1], "duplicate") == 0) {
            book.items[2].id = book.items[0].id;
        } else {
            fprintf(stderr, "Unknown scenario: %s\n", argv[1]);
            return 2;
        }
        expected = UMI_STATUS_INVALID_STATE;
    }
    REQUIRE(UmiBankingDepositAccountBookSummarise(&book, "alice", &gbp, &summary) == expected);
    REQUIRE(memcmp(&summary, &before, sizeof summary) == 0);
    return 0;
}
