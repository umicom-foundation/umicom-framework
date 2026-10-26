/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_control_boundaries.c
 *
 * PURPOSE:
 *   Check ledger net arithmetic and aggregate control predicates on malformed public values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/ledger_balance.h"
#include "umicom/finance/accounting/trial_balance.h"
#include "umicom/finance/accounting/double_entry_validation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 2); const char *mode = argv[1];
    if (strncmp(mode, "ledger-", 7U) == 0) {
        UmiAccountingLedgerBalance value;
        CHECK(umi_accounting_ledger_balance_init(&value, "balance", "cash", UMI_ACCOUNTING_NORMAL_DEBIT,
            INT64_MAX, 0) == UMI_STATUS_OK);
        int valid = 0;
        if (!strcmp(mode, "ledger-debit")) valid = 1;
        else if (!strcmp(mode, "ledger-credit")) { value.normal_side = UMI_ACCOUNTING_NORMAL_CREDIT; valid = 1; }
        else if (!strcmp(mode, "ledger-zero")) { value.credit_minor = INT64_MAX; valid = 1; }
        else if (!strcmp(mode, "ledger-negative")) value.credit_minor = INT64_MIN;
        else if (!strcmp(mode, "ledger-side")) value.normal_side = (UmiAccountingNormalSide)99;
        else if (!strcmp(mode, "ledger-id")) memset(value.id.value, 'a', sizeof value.id.value);
        else if (!strcmp(mode, "ledger-account")) value.account_id.value[0] = '\0';
        else if (!strcmp(mode, "ledger-init-atomic") || !strcmp(mode, "ledger-init-alias")) {
            UmiAccountingLedgerBalance before; memcpy(&before, &value, sizeof value);
            UmiStatus status = umi_accounting_ledger_balance_init(&value, value.id.value, value.account_id.value,
                UMI_ACCOUNTING_NORMAL_CREDIT, !strcmp(mode, "ledger-init-atomic") ? -1 : 7, 0);
            if (!strcmp(mode, "ledger-init-atomic")) CHECK(status != UMI_STATUS_OK && memcmp(&value, &before, sizeof value) == 0);
            else CHECK(status == UMI_STATUS_OK && !strcmp(value.id.value, before.id.value) &&
                !strcmp(value.account_id.value, before.account_id.value) && value.debit_minor == 7);
            return 0;
        } else if (strcmp(mode, "ledger-null") && strcmp(mode, "ledger-output-null")) return 2;
        int64_t result = 71;
        UmiStatus status = umi_accounting_ledger_balance_net_minor_checked(!strcmp(mode, "ledger-null") ? NULL : &value,
            !strcmp(mode, "ledger-output-null") ? NULL : &result);
        if (valid) CHECK(status == UMI_STATUS_OK && result == (!strcmp(mode, "ledger-debit") ? INT64_MAX :
            !strcmp(mode, "ledger-credit") ? -INT64_MAX : 0));
        else {
            CHECK(status == UMI_STATUS_INVALID_ARGUMENT && result == 71);
            if (strcmp(mode, "ledger-output-null"))
                CHECK(umi_accounting_ledger_balance_net_minor(!strcmp(mode, "ledger-null") ? NULL : &value) == 0);
        }
    } else if (strncmp(mode, "trial-", 6U) == 0) {
        UmiAccountingTrialBalance value;
        CHECK(umi_accounting_trial_balance_init(&value, "trial", 5, 5, 2U) == UMI_STATUS_OK);
        if (!strcmp(mode, "trial-negative")) { value.debit_minor = INT64_MIN; value.credit_minor = INT64_MIN; }
        else if (!strcmp(mode, "trial-id")) memset(value.id.value, 'a', sizeof value.id.value);
        else if (!strcmp(mode, "trial-zero")) { value.debit_minor = 0; value.credit_minor = 0; value.account_count = 0U; }
        else if (!strcmp(mode, "trial-unequal")) value.credit_minor = 4;
        else if (!strcmp(mode, "trial-init-atomic")) {
            UmiAccountingTrialBalance before; memcpy(&before, &value, sizeof value);
            CHECK(umi_accounting_trial_balance_init(&value, value.id.value, -1, -1, 2U) != UMI_STATUS_OK);
            CHECK(memcmp(&before, &value, sizeof value) == 0); return 0;
        } else return 2;
        CHECK(umi_accounting_trial_balance_balanced(&value) == (!strcmp(mode, "trial-zero")));
    } else if (strncmp(mode, "double-", 7U) == 0) {
        UmiAccountingDoubleEntryValidation value;
        CHECK(umi_accounting_double_entry_validation_init(&value, "control", 5, 5, 2U) == UMI_STATUS_OK);
        if (!strcmp(mode, "double-negative")) { value.debit_minor = INT64_MIN; value.credit_minor = INT64_MIN; }
        else if (!strcmp(mode, "double-id")) value.id.value[0] = '\0';
        else if (!strcmp(mode, "double-count")) value.line_count = 1U;
        else if (!strcmp(mode, "double-zero")) { value.debit_minor = 0; value.credit_minor = 0; }
        else if (!strcmp(mode, "double-unequal")) value.credit_minor = 4;
        else if (!strcmp(mode, "double-init-atomic")) {
            UmiAccountingDoubleEntryValidation before; memcpy(&before, &value, sizeof value);
            CHECK(umi_accounting_double_entry_validation_init(&value, value.id.value, -1, -1, 2U) != UMI_STATUS_OK);
            CHECK(memcmp(&before, &value, sizeof value) == 0); return 0;
        } else return 2;
        CHECK(umi_accounting_double_entry_validation_balanced(&value) == (!strcmp(mode, "double-zero")));
    } else return 2;
    return 0;
}
