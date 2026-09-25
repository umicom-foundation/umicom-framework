/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/ledger_batch.h
 *
 * PURPOSE:
 *   Validate and retain a bounded batch of debit/credit posting evidence before
 *   canonical accounting posting is invoked.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_LEDGER_BATCH_H
#define UMICOM_FINANCIAL_PLATFORM_LEDGER_BATCH_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_LEDGER_LINE_CAPACITY 256U

typedef struct UmiFinancialLedgerControlLine {
    char accountId[64];
    char currency[8];
    double debit;
    double credit;
    char referenceId[64];
} UmiFinancialLedgerControlLine;

typedef struct UmiFinancialLedgerBatch {
    char batchId[64];
    UmiFinancialLedgerControlLine lines[UMI_FINANCIAL_LEDGER_LINE_CAPACITY];
    size_t count;
    uint64_t revision;
    int approved;
    int posted;
} UmiFinancialLedgerBatch;

void umi_financial_ledger_batch_init(
    UmiFinancialLedgerBatch *batch,
    const char *batchId);

UmiStatus umi_financial_ledger_batch_add(
    UmiFinancialLedgerBatch *batch,
    const UmiFinancialLedgerControlLine *line);

UmiStatus umi_financial_ledger_batch_totals(
    const UmiFinancialLedgerBatch *batch,
    double *outDebit,
    double *outCredit);

UmiStatus umi_financial_ledger_batch_validate(
    const UmiFinancialLedgerBatch *batch,
    double tolerance);

UmiStatus umi_financial_ledger_batch_mark_approved(
    UmiFinancialLedgerBatch *batch);

UmiStatus umi_financial_ledger_batch_mark_posted(
    UmiFinancialLedgerBatch *batch);

#ifdef __cplusplus
}
#endif
#endif
