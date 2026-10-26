/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_account_catalogue_boundaries.c
 *
 * PURPOSE:
 *   Exercise account eligibility and restored chart bounds without trusting malformed records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/chart_of_accounts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)

static void Account(UmiAccountingLedgerAccount *value, const char *id)
{
    CHECK(umi_accounting_ledger_account_init(value, id, "Cash", UMI_ACCOUNTING_ASSET,
        UMI_ACCOUNTING_NORMAL_DEBIT, true) == UMI_STATUS_OK);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiAccountingLedgerAccount account;
    Account(&account, "cash");
    if (!strncmp(mode, "chart-", 6U)) {
        UmiAccountingChartOfAccounts *chart = malloc(sizeof *chart);
        UmiAccountingChartOfAccounts *saved = malloc(sizeof *saved);
        CHECK(chart && saved);
        umi_accounting_chart_of_accounts_init(chart);
        CHECK(umi_accounting_chart_of_accounts_add(chart, &account) == UMI_STATUS_OK);
        if (!strcmp(mode, "chart-find")) {
            CHECK(umi_accounting_chart_of_accounts_find(chart, "cash") == &chart->items[0]);
            CHECK(umi_accounting_chart_of_accounts_find(chart, "missing") == NULL);
        } else if (!strcmp(mode, "chart-null")) {
            CHECK(umi_accounting_chart_of_accounts_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_accounting_chart_of_accounts_find(NULL, "cash") == NULL);
            CHECK(umi_accounting_chart_of_accounts_add(NULL, &account) == UMI_STATUS_INVALID_ARGUMENT);
        } else if (!strcmp(mode, "chart-null-key")) {
            CHECK(umi_accounting_chart_of_accounts_find(chart, NULL) == NULL);
        } else {
            if (!strcmp(mode, "chart-count")) chart->count = SIZE_MAX;
            else if (!strcmp(mode, "chart-invalid-item")) chart->items[0].id.value[0] = '\0';
            else if (!strcmp(mode, "chart-restored-duplicate")) {
                chart->items[1] = chart->items[0];
                chart->count = 2U;
            } else if (!strcmp(mode, "chart-invalid-tail")) {
                Account(&account, "income");
                CHECK(umi_accounting_chart_of_accounts_add(chart, &account) == UMI_STATUS_OK);
                memset(chart->items[1].name, 'x', sizeof chart->items[1].name);
            } else if (!strcmp(mode, "chart-full")) {
                for (size_t i = 1U; i < UMI_ACCOUNTING_MAX_ITEMS; ++i) {
                    char id[32];
                    int n = snprintf(id, sizeof id, "account-%zu", i);
                    CHECK(n > 0 && (size_t)n < sizeof id);
                    Account(&account, id);
                    CHECK(umi_accounting_chart_of_accounts_add(chart, &account) == UMI_STATUS_OK);
                }
                Account(&account, "extra");
            } else if (strcmp(mode, "chart-duplicate") && strcmp(mode, "chart-alias") &&
                       strcmp(mode, "chart-null-item")) return 2;
            memcpy(saved, chart, sizeof *saved);
            const UmiAccountingLedgerAccount *input = !strcmp(mode, "chart-alias") ?
                &chart->items[0] : !strcmp(mode, "chart-null-item") ? NULL : &account;
            CHECK(umi_accounting_chart_of_accounts_add(chart, input) != UMI_STATUS_OK);
            CHECK(memcmp(saved, chart, sizeof *saved) == 0);
            if (!strcmp(mode, "chart-count") || !strcmp(mode, "chart-invalid-item") ||
                !strcmp(mode, "chart-restored-duplicate") || !strcmp(mode, "chart-invalid-tail")) {
                CHECK(umi_accounting_chart_of_accounts_validate(chart) != UMI_STATUS_OK);
                CHECK(umi_accounting_chart_of_accounts_find(chart, "cash") == NULL);
            }
        }
        free(saved);
        free(chart);
        return 0;
    }
    if (!strcmp(mode, "named-classes")) {
        const UmiAccountingAccountClass classes[] = {UMI_ACCOUNTING_ASSET, UMI_ACCOUNTING_LIABILITY,
            UMI_ACCOUNTING_EQUITY, UMI_ACCOUNTING_REVENUE, UMI_ACCOUNTING_EXPENSE};
        for (size_t i = 0U; i < sizeof classes / sizeof classes[0]; ++i) {
            account.account_class = classes[i];
            CHECK(umi_accounting_ledger_account_valid(&account));
            CHECK(umi_accounting_ledger_account_postable(&account));
        }
        return 0;
    }
    if (!strcmp(mode, "init-atomic") || !strcmp(mode, "init-alias")) {
        UmiAccountingLedgerAccount saved;
        memcpy(&saved, &account, sizeof saved);
        UmiStatus status = umi_accounting_ledger_account_init(&account, account.id.value,
            account.name, !strcmp(mode, "init-atomic") ? (UmiAccountingAccountClass)99 : UMI_ACCOUNTING_EXPENSE,
            UMI_ACCOUNTING_NORMAL_CREDIT, false);
        if (!strcmp(mode, "init-atomic")) {
            CHECK(status != UMI_STATUS_OK && memcmp(&saved, &account, sizeof saved) == 0);
        } else {
            CHECK(status == UMI_STATUS_OK && !strcmp(account.id.value, saved.id.value) &&
                !strcmp(account.name, saved.name) && account.account_class == UMI_ACCOUNTING_EXPENSE);
        }
        return 0;
    }
    bool valid = false;
    if (!strcmp(mode, "blocked")) {
        account.posting_allowed = false;
        valid = true;
    } else if (!strcmp(mode, "empty-id") || !strcmp(mode, "archive-invalid-id")) account.id.value[0] = '\0';
    else if (!strcmp(mode, "id-unterminated")) memset(account.id.value, 'x', sizeof account.id.value);
    else if (!strcmp(mode, "name-unterminated")) memset(account.name, 'x', sizeof account.name);
    else if (!strcmp(mode, "empty-name")) account.name[0] = '\0';
    else if (!strcmp(mode, "invalid-class")) account.account_class = (UmiAccountingAccountClass)99;
    else if (!strcmp(mode, "negative-class")) account.account_class = (UmiAccountingAccountClass)-1;
    else if (!strcmp(mode, "invalid-side")) account.normal_side = (UmiAccountingNormalSide)99;
    else if (!strcmp(mode, "null")) {
        CHECK(!umi_accounting_ledger_account_valid(NULL) && !umi_accounting_ledger_account_postable(NULL));
        return 0;
    } else return 2;
    CHECK(umi_accounting_ledger_account_valid(&account) == valid);
    CHECK(!umi_accounting_ledger_account_postable(&account));
    if (!strcmp(mode, "archive-invalid-id")) {
        unsigned char bytes[32], saved[32];
        memset(bytes, 0xA5, sizeof bytes);
        memcpy(saved, bytes, sizeof saved);
        size_t size = 77U;
        CHECK(umi_accounting_ledger_account_archive_encode(&account, bytes, sizeof bytes, &size) != UMI_STATUS_OK);
        CHECK(size == 77U && memcmp(saved, bytes, sizeof bytes) == 0);
    }
    return 0;
}
