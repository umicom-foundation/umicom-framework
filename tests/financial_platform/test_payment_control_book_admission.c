/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_payment_control_book_admission.c
 *
 * PURPOSE:
 *   Verify payment control book bounded storage, mutation and domain-specific observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_platform/service.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

static UmiFinancialPaymentControl ValidRow(void)
{
    UmiFinancialPaymentControl row = {"payment", "account", "creditor", "GBP", 100, UMI_FINANCIAL_PAYMENT_DRAFT, 1U, 0};
    return row;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    static UmiFinancialPaymentControlBook owner, before;
    UmiFinancialPaymentControl row = ValidRow();
    umi_financial_payment_control_book_init(&owner);
    REQUIRE(umi_financial_payment_control_book_add(&owner, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        REQUIRE(owner.count == 1 && owner.revision == 2);
        REQUIRE(umi_financial_payment_control_book_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_payment_control_book_find(&owner, "payment") == &owner.payments[0]);
    } else if (strcmp(test, "fields") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialPaymentControl, paymentId), offsetof(UmiFinancialPaymentControl, debtorAccountId), offsetof(UmiFinancialPaymentControl, creditorPartyId), offsetof(UmiFinancialPaymentControl, currency)};
        const size_t lengths[] = {sizeof row.paymentId, sizeof row.debtorAccountId, sizeof row.creditorPartyId, sizeof row.currency};
        const int required[] = {1, 1, 1, 1};
        for (size_t i = 0; i < sizeof offsets / sizeof offsets[0]; ++i) {
            row = ValidRow(); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &owner, sizeof owner);
            REQUIRE(umi_financial_payment_control_book_add(&owner, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
            ((char *)&row)[offsets[i]] = '\0';
            REQUIRE(umi_financial_payment_control_validate(&row) == (required[i] ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0 || strcmp(test, "stored-field") == 0) {
        if (strcmp(test, "count") == 0) owner.count = SIZE_MAX;
        if (strcmp(test, "duplicate") == 0) { owner.payments[1] = row; owner.count = 2; }
        if (strcmp(test, "stored-field") == 0) memset(owner.payments[0].paymentId, 'x', sizeof row.paymentId);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_payment_control_book_find(&owner, "payment") == NULL);
        REQUIRE(umi_financial_payment_control_book_add(&owner, &row) != UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "revision") == 0) {
        owner.revision = 0; REQUIRE(umi_financial_payment_control_book_validate(&owner) == UMI_STATUS_INVALID_STATE);
        owner.revision = UINT64_MAX; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_payment_control_book_add(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY; ++i) {
            owner.payments[i] = row;
            snprintf(owner.payments[i].paymentId, 64, "record.%zu", i);
        }
        owner.count = UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_payment_control_book_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_payment_control_book_add(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "query") == 0) {
        char key[64]; memset(key, 'x', sizeof key);
        REQUIRE(umi_financial_payment_control_book_find(&owner, key) == NULL);
        REQUIRE(umi_financial_payment_control_book_find(&owner, "") == NULL);
    } else if (strcmp(test, "numbers") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialPaymentControl, amount)};
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < sizeof offsets / sizeof offsets[0]; ++i) for (size_t j = 0; j < 3; ++j) {
            row = ValidRow(); memcpy((char *)&row + offsets[i], &bad[j], sizeof(double));
            REQUIRE(umi_financial_payment_control_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
        row = ValidRow(); row.amount = -1.0;
        REQUIRE(umi_financial_payment_control_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "flags") == 0) {
        row = ValidRow(); row.approved = 2;
        REQUIRE(umi_financial_payment_control_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "states") == 0) {
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_payment_control_book_transition(&owner, "payment", (UmiFinancialPaymentControlState)99, 2) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        row.state = (UmiFinancialPaymentControlState)99;
        REQUIRE(umi_financial_payment_control_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "transitions") == 0) {
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_payment_control_book_transition(&owner, "payment", UMI_FINANCIAL_PAYMENT_DRAFT, 1) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        REQUIRE(umi_financial_payment_control_book_transition(&owner, "missing", UMI_FINANCIAL_PAYMENT_DRAFT, 2) == UMI_STATUS_NOT_FOUND);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        REQUIRE(umi_financial_payment_control_book_transition(&owner, "payment", UMI_FINANCIAL_PAYMENT_SENT, 2) == UMI_STATUS_PERMISSION_DENIED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        REQUIRE(umi_financial_payment_control_book_transition(&owner, "payment", UMI_FINANCIAL_PAYMENT_SETTLED, 2) == UMI_STATUS_PERMISSION_DENIED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        owner.payments[0].approved = 1; /* Local observation; no authentication claim. */
        REQUIRE(umi_financial_payment_control_book_transition(&owner, "payment", UMI_FINANCIAL_PAYMENT_SETTLED, 2) == UMI_STATUS_OK);
        REQUIRE(owner.payments[0].version == 2 && owner.revision == 3);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_payment_control_book_transition(&owner, "payment", UMI_FINANCIAL_PAYMENT_DRAFT, 3) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        owner.payments[0].state = UMI_FINANCIAL_PAYMENT_REJECTED;
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_payment_control_book_transition(&owner, "payment", UMI_FINANCIAL_PAYMENT_DRAFT, 3) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_financial_payment_control_book_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_financial_payment_control_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_payment_control_book_add(&owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else return 2;
    return 0;
}
