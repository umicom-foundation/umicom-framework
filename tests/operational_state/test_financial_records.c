/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/operational_state/test_financial_records.c
 * PURPOSE: Check bounded identity and rejected banking updates without real money.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/finance/identifier.h"
#include "umicom/finance/core/types.h"
#include "umicom/finance/banking/deposit_account_book.h"
#include "umicom/finance/banking/account_hold.h"
#include "umicom/finance/banking/deposit_transaction.h"
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)

static int Identifiers(void)
{
    UmiFinancialId valid = {{0}}, other = {{0}};
    UmiFinancialId *broken = malloc(sizeof(*broken));
    CHECK(broken != NULL);
    memset(broken, 'z', sizeof(*broken));
    CHECK(!umi_financial_id_valid(broken));
    CHECK(!umi_financial_id_is_valid(broken));
    CHECK(!umi_financial_id_is_valid(NULL));
    CHECK(!umi_financial_id_is_valid(&valid));
    CHECK(umi_financial_id_assign(&valid, "ACCOUNT-1") == UMI_STATUS_OK);
    CHECK(umi_financial_id_assign(&other, "ACCOUNT-2") == UMI_STATUS_OK);
    CHECK(umi_financial_id_is_valid(&valid));
    CHECK(umi_financial_id_compare(&valid, &other) < 0);
    CHECK(umi_financial_id_compare(NULL, &valid) < 0 && umi_financial_id_compare(&valid, NULL) > 0);
    CHECK(umi_financial_id_compare(NULL, NULL) == 0);
    CHECK(umi_financial_id_compare(broken, broken) == 0); /* Bounded damaged-record inspection. */
    CHECK(umi_financial_id_compare(&valid, broken) < 0);
    broken->value[sizeof(broken->value)-1U] = '\0';
    CHECK(umi_financial_id_is_valid(broken));
    free(broken);
    return EXIT_SUCCESS;
}
static int Accounts(void)
{
    UmiBankingDepositAccount account, before;
    int64_t reserved = 7;
    CHECK(umi_banking_deposit_account_init(&account, "ACCOUNT", "CUSTOMER", "CURRENT", "GBP",
        12500, 10000, UMI_BANKING_ACCOUNT_OPEN) == UMI_STATUS_OK);
    CHECK(UmiBankingDepositAccountReserved(&account, &reserved) == UMI_STATUS_OK && reserved == 2500);
    CHECK(umi_banking_deposit_account_reserved_minor(&account) == 2500);
    before = account;
    CHECK(umi_banking_deposit_account_init(&account, "", "CUSTOMER", "CURRENT", "GBP",
        1, 0, UMI_BANKING_ACCOUNT_OPEN) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&account, &before, sizeof(account)) == 0);
    CHECK(umi_banking_deposit_account_init(&account, "ACCOUNT", "CUSTOMER", "CURRENT", "gbp",
        1, 0, UMI_BANKING_ACCOUNT_OPEN) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&account, &before, sizeof(account)) == 0);
    account = before; account.currency.code[3] = 'X';
    CHECK(!umi_banking_deposit_account_valid(&account));
    account = before; memset(&account.id, 'X', sizeof(account.id));
    CHECK(!umi_banking_deposit_account_valid(&account));
    reserved = 7;
    CHECK(UmiBankingDepositAccountReserved(&account, &reserved) == UMI_STATUS_INVALID_ARGUMENT && reserved == 7);
    account = before; account.ledger_minor = INT64_MAX; account.available_minor = -1;
    CHECK(!umi_banking_deposit_account_valid(&account));
    CHECK(UmiBankingDepositAccountReserved(&account, &reserved) == UMI_STATUS_INVALID_ARGUMENT && reserved == 7);
    CHECK(umi_banking_deposit_account_reserved_minor(&account) == 0);
    CHECK(umi_banking_deposit_account_init(&account, "ACCOUNT", "CUSTOMER", "CURRENT", "GBP",
        -1, INT64_MIN, UMI_BANKING_ACCOUNT_OPEN) == UMI_STATUS_OK);
    CHECK(UmiBankingDepositAccountReserved(&account, &reserved) == UMI_STATUS_OK && reserved == INT64_MAX);
    CHECK(umi_banking_deposit_account_init(&account, "ACCOUNT", "CUSTOMER", "CURRENT", "GBP",
        INT64_MIN, INT64_MIN, UMI_BANKING_ACCOUNT_OPEN) == UMI_STATUS_OK);
    CHECK(umi_banking_deposit_account_reserved_minor(&account) == 0);
    before = account;
    CHECK(umi_banking_deposit_account_init(&account, "ACCOUNT", "CUSTOMER", "CURRENT", "GBP",
        0, INT64_MIN, UMI_BANKING_ACCOUNT_OPEN) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&account, &before, sizeof(account)) == 0);
    return EXIT_SUCCESS;
}
static int Book(void)
{
    UmiBankingDepositAccountBook *book = calloc(1U, sizeof(*book));
    UmiBankingDepositAccount account;
    CHECK(book != NULL);
    CHECK(umi_banking_deposit_account_init(&account, "ACCOUNT", "CUSTOMER", "CURRENT", "GBP",
        100, 50, UMI_BANKING_ACCOUNT_OPEN) == UMI_STATUS_OK);
    CHECK(umi_banking_deposit_account_book_add(book, &account) == UMI_STATUS_OK);
    CHECK(umi_banking_deposit_account_book_find(book, "ACCOUNT") != NULL);
    CHECK(umi_banking_deposit_account_book_add(book, &account) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(book->count == 1U);
    memset(&account.id, 'X', sizeof(account.id));
    CHECK(umi_banking_deposit_account_book_add(book, &account) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(book->count == 1U);
    book->count = SIZE_MAX;
    CHECK(umi_banking_deposit_account_book_find(book, "MISSING") == NULL);
    free(book);
    return EXIT_SUCCESS;
}
static int Movements(void)
{
    UmiBankingAccountHold hold, oldHold;
    UmiBankingDepositTransaction movement, oldMovement;
    UmiFinancialDate date = {2026, 9U, 16U};
    CHECK(umi_banking_account_hold_init(&hold, "HOLD", "ACCOUNT", 100, true) == UMI_STATUS_OK);
    oldHold = hold;
    CHECK(umi_banking_account_hold_init(&hold, "", "ACCOUNT", 100, true) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&hold, &oldHold, sizeof(hold)) == 0);
    CHECK(umi_banking_account_hold_init(&hold, "HOLD", "", 100, true) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&hold, &oldHold, sizeof(hold)) == 0);
    memset(&hold.account_id, 'X', sizeof(hold.account_id));
    CHECK(!umi_banking_account_hold_valid(&hold) && !umi_banking_account_hold_releasable(&hold));
    CHECK(umi_banking_deposit_transaction_init(&movement, "TX", "ACCOUNT", -100, date, date) == UMI_STATUS_OK);
    oldMovement = movement;
    CHECK(umi_banking_deposit_transaction_init(&movement, "", "ACCOUNT", -100, date, date) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&movement, &oldMovement, sizeof(movement)) == 0);
    date.day = 32U;
    CHECK(umi_banking_deposit_transaction_init(&movement, "TX", "ACCOUNT", 100, date, date) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&movement, &oldMovement, sizeof(movement)) == 0);
    memset(&movement.id, 'X', sizeof(movement.id));
    CHECK(!umi_banking_deposit_transaction_valid(&movement));
    return EXIT_SUCCESS;
}
int main(int argc, char **argv)
{
    if (argc != 2) return EXIT_FAILURE;
    if (strcmp(argv[1], "identifiers") == 0) return Identifiers();
    if (strcmp(argv[1], "accounts") == 0) return Accounts();
    if (strcmp(argv[1], "book") == 0) return Book();
    if (strcmp(argv[1], "movements") == 0) return Movements();
    return EXIT_FAILURE;
}
