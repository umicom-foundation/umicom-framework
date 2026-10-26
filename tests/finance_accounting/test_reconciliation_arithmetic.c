/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_reconciliation_arithmetic.c
 *
 * PURPOSE:
 *   Verify exact reconciliation distances and checked subledger subtraction at signed boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/reconciliation_item.h"
#include "umicom/finance/accounting/subledger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)

typedef struct Example {
    const char *name;
    int64_t external, ledger, tolerance;
    uint64_t distance;
    int64_t difference;
    UmiStatus difference_status;
    bool matched;
} Example;

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const Example examples[] = {
        {"equal", 5, 5, 0, 0U, 0, UMI_STATUS_OK, true},
        {"equal-minimum", INT64_MIN, INT64_MIN, 0, 0U, 0, UMI_STATUS_OK, true},
        {"equal-maximum", INT64_MAX, INT64_MAX, 0, 0U, 0, UMI_STATUS_OK, true},
        {"positive", 17, 10, 7, 7U, 7, UMI_STATUS_OK, true},
        {"negative", 10, 17, 7, 7U, -7, UMI_STATUS_OK, true},
        {"negative-values", -17, -10, 7, 7U, -7, UMI_STATUS_OK, true},
        {"outside", 17, 10, 6, 7U, 7, UMI_STATUS_OK, false},
        {"opposite-signs", 5, -7, 12, 12U, 12, UMI_STATUS_OK, true},
        {"maximum-distance", INT64_MAX, INT64_MIN, INT64_MAX, UINT64_MAX, 0, UMI_STATUS_CAPACITY_EXCEEDED, false},
        {"reverse-maximum-distance", INT64_MIN, INT64_MAX, INT64_MAX, UINT64_MAX, 0, UMI_STATUS_CAPACITY_EXCEEDED, false},
        {"minimum-vs-zero", INT64_MIN, 0, INT64_MAX, UINT64_C(9223372036854775808), INT64_MIN, UMI_STATUS_OK, false},
        {"zero-vs-minimum", 0, INT64_MIN, INT64_MAX, UINT64_C(9223372036854775808), 0, UMI_STATUS_CAPACITY_EXCEEDED, false},
        {"maximum-vs-zero", INT64_MAX, 0, INT64_MAX, UINT64_C(9223372036854775807), INT64_MAX, UMI_STATUS_OK, true},
        {"zero-vs-maximum", 0, INT64_MAX, INT64_MAX, UINT64_C(9223372036854775807), -INT64_MAX, UMI_STATUS_OK, true},
        {"positive-overflow", INT64_MAX, -1, INT64_MAX, UINT64_C(9223372036854775808), 0, UMI_STATUS_CAPACITY_EXCEEDED, false},
        {"negative-overflow", INT64_MIN, 1, INT64_MAX, UINT64_C(9223372036854775809), 0, UMI_STATUS_CAPACITY_EXCEEDED, false},
        {"minimum-neighbour", INT64_MIN, INT64_MIN + 1, 1, 1U, -1, UMI_STATUS_OK, true},
        {"maximum-neighbour", INT64_MAX, INT64_MAX - 1, 1, 1U, 1, UMI_STATUS_OK, true}
    };
    for (size_t i = 0U; i < sizeof examples / sizeof examples[0]; ++i) {
        const Example *example = &examples[i];
        if (strcmp(mode, example->name)) continue;
        UmiAccountingReconciliationItem value, saved;
        CHECK(umi_accounting_reconciliation_item_init(&value, "item", example->external,
            example->ledger, example->tolerance) == UMI_STATUS_OK);
        memcpy(&saved, &value, sizeof saved);
        uint64_t distance = 71U;
        int64_t difference = 72;
        CHECK(umi_accounting_reconciliation_item_distance_minor_checked(&value, &distance) == UMI_STATUS_OK);
        CHECK(distance == example->distance);
        CHECK(umi_accounting_reconciliation_item_difference_minor_checked(&value, &difference) == example->difference_status);
        CHECK(difference == (example->difference_status == UMI_STATUS_OK ? example->difference : 72));
        CHECK(umi_accounting_reconciliation_item_matched(&value) == example->matched);
        CHECK(memcmp(&saved, &value, sizeof saved) == 0);
        return 0;
    }
    if (!strncmp(mode, "subledger-", 10U)) {
        UmiAccountingSubledger value;
        CHECK(umi_accounting_subledger_init(&value, "subledger", "cash", INT64_MAX, 0) == UMI_STATUS_OK);
        bool valid = false;
        if (!strcmp(mode, "subledger-positive")) valid = true;
        else if (!strcmp(mode, "subledger-negative")) { value.debit_minor = 0; value.credit_minor = INT64_MAX; valid = true; }
        else if (!strcmp(mode, "subledger-zero")) { value.credit_minor = INT64_MAX; valid = true; }
        else if (!strcmp(mode, "subledger-bad-debit")) value.debit_minor = INT64_MIN;
        else if (!strcmp(mode, "subledger-bad-credit")) value.credit_minor = INT64_MIN;
        else if (!strcmp(mode, "subledger-id")) value.id.value[0] = '\0';
        else if (!strcmp(mode, "subledger-control")) memset(value.control_account_id.value, 'x', sizeof value.control_account_id.value);
        else if (!strcmp(mode, "subledger-init-atomic") || !strcmp(mode, "subledger-init-alias")) {
            UmiAccountingSubledger saved;
            memcpy(&saved, &value, sizeof saved);
            UmiStatus status = umi_accounting_subledger_init(&value, value.id.value, value.control_account_id.value,
                !strcmp(mode, "subledger-init-atomic") ? -1 : 4, 0);
            if (!strcmp(mode, "subledger-init-atomic")) CHECK(status != UMI_STATUS_OK && memcmp(&saved, &value, sizeof saved) == 0);
            else CHECK(status == UMI_STATUS_OK && !strcmp(value.id.value, saved.id.value) &&
                !strcmp(value.control_account_id.value, saved.control_account_id.value) && value.debit_minor == 4);
            return 0;
        } else if (strcmp(mode, "subledger-null") && strcmp(mode, "subledger-output-null")) return 2;
        int64_t amount = 71;
        const UmiAccountingSubledger *input = !strcmp(mode, "subledger-null") ? NULL : &value;
        UmiStatus status = umi_accounting_subledger_net_minor_checked(input,
            !strcmp(mode, "subledger-output-null") ? NULL : &amount);
        if (valid) {
            CHECK(status == UMI_STATUS_OK && amount == (!strcmp(mode, "subledger-positive") ? INT64_MAX :
                !strcmp(mode, "subledger-negative") ? -INT64_MAX : 0));
        } else {
            CHECK(status == UMI_STATUS_INVALID_ARGUMENT && amount == 71);
            if (strcmp(mode, "subledger-output-null")) CHECK(umi_accounting_subledger_net_minor(input) == 0);
        }
        return 0;
    }
    UmiAccountingReconciliationItem value;
    CHECK(umi_accounting_reconciliation_item_init(&value, "item", 1, 1, 0) == UMI_STATUS_OK);
    if (!strcmp(mode, "init-atomic") || !strcmp(mode, "init-alias")) {
        UmiAccountingReconciliationItem saved;
        memcpy(&saved, &value, sizeof saved);
        UmiStatus status = umi_accounting_reconciliation_item_init(&value, value.id.value, 10, 11,
            !strcmp(mode, "init-atomic") ? -1 : 1);
        if (!strcmp(mode, "init-atomic")) CHECK(status != UMI_STATUS_OK && memcmp(&saved, &value, sizeof saved) == 0);
        else CHECK(status == UMI_STATUS_OK && !strcmp(value.id.value, saved.id.value) &&
            umi_accounting_reconciliation_item_matched(&value));
        return 0;
    }
    bool null_output = false;
    if (!strcmp(mode, "invalid-tolerance")) value.tolerance_minor = -1;
    else if (!strcmp(mode, "invalid-id")) value.id.value[0] = '\0';
    else if (!strcmp(mode, "unterminated-id")) memset(value.id.value, 'x', sizeof value.id.value);
    else if (!strcmp(mode, "null-output")) null_output = true;
    else if (strcmp(mode, "null")) return 2;
    const UmiAccountingReconciliationItem *input = !strcmp(mode, "null") ? NULL : &value;
    uint64_t distance = 71U;
    int64_t difference = 72;
    CHECK(umi_accounting_reconciliation_item_distance_minor_checked(input, null_output ? NULL : &distance) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_accounting_reconciliation_item_difference_minor_checked(input, null_output ? NULL : &difference) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(distance == 71U && difference == 72);
    if (!null_output) CHECK(!umi_accounting_reconciliation_item_matched(input));
    return 0;
}
