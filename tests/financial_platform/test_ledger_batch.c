/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_ledger_batch.c
 *
 * PURPOSE:
 *   Verify double-entry balancing and approval-before-posting controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/financial_platform/ledger_batch.h"

int main(void)
{
    UmiFinancialLedgerBatch batch;
    UmiFinancialLedgerControlLine debit = {0};
    UmiFinancialLedgerControlLine credit = {0};

    umi_financial_ledger_batch_init(&batch, "batch-1");
    (void)strcpy(debit.accountId, "cash");
    (void)strcpy(debit.currency, "GBP");
    (void)strcpy(debit.referenceId, "trade-1");
    debit.debit = 100.0;
    credit = debit;
    credit.debit = 0.0;
    credit.credit = 100.0;
    (void)strcpy(credit.accountId, "receivable");

    assert(umi_financial_ledger_batch_add(&batch, &debit) == UMI_STATUS_OK);
    assert(umi_financial_ledger_batch_add(&batch, &credit) == UMI_STATUS_OK);
    assert(umi_financial_ledger_batch_validate(&batch, 0.000001) == UMI_STATUS_OK);
    assert(umi_financial_ledger_batch_mark_approved(&batch) == UMI_STATUS_OK);
    assert(umi_financial_ledger_batch_mark_posted(&batch) == UMI_STATUS_OK);
    return 0;
}
