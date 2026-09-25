/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/ledger_batch.c
 *
 * PURPOSE:
 *   Implement pre-posting double-entry control evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/ledger_batch.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

void umi_financial_ledger_batch_init(
    UmiFinancialLedgerBatch *batch,
    const char *batchId)
{
    if (batch == NULL) return;
    (void)memset(batch, 0, sizeof(*batch));
    if (batchId != NULL) {
        (void)snprintf(batch->batchId, sizeof(batch->batchId), "%s", batchId);
    }
    batch->revision = 1U;
}

UmiStatus umi_financial_ledger_batch_add(
    UmiFinancialLedgerBatch *batch,
    const UmiFinancialLedgerControlLine *line)
{
    if (batch == NULL || line == NULL ||
        batch->batchId[0] == '\0' ||
        line->accountId[0] == '\0' ||
        line->currency[0] == '\0' ||
        line->referenceId[0] == '\0' ||
        line->debit < 0.0 || line->credit < 0.0 ||
        (line->debit > 0.0 && line->credit > 0.0) ||
        (line->debit == 0.0 && line->credit == 0.0) ||
        batch->posted) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (batch->count >= UMI_FINANCIAL_LEDGER_LINE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    batch->lines[batch->count++] = *line;
    batch->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_ledger_batch_totals(
    const UmiFinancialLedgerBatch *batch,
    double *outDebit,
    double *outCredit)
{
    size_t index;
    double debit = 0.0;
    double credit = 0.0;
    if (batch == NULL || outDebit == NULL || outCredit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < batch->count; ++index) {
        debit += batch->lines[index].debit;
        credit += batch->lines[index].credit;
    }
    *outDebit = debit;
    *outCredit = credit;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_ledger_batch_validate(
    const UmiFinancialLedgerBatch *batch,
    double tolerance)
{
    double debit;
    double credit;
    if (batch == NULL || tolerance < 0.0 || batch->count < 2U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_ledger_batch_totals(batch, &debit, &credit) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    return fabs(debit - credit) <= tolerance
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_financial_ledger_batch_mark_approved(
    UmiFinancialLedgerBatch *batch)
{
    if (batch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_financial_ledger_batch_validate(batch, 0.000001) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    batch->approved = 1;
    batch->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_ledger_batch_mark_posted(
    UmiFinancialLedgerBatch *batch)
{
    if (batch == NULL || !batch->approved || batch->posted) {
        return UMI_STATUS_INVALID_STATE;
    }
    batch->posted = 1;
    batch->revision += 1U;
    return UMI_STATUS_OK;
}
