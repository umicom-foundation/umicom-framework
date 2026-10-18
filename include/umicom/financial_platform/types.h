/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/types.h
 *
 * PURPOSE:
 *   Define cross-domain evidence for the shared financial platform control
 *   plane spanning reference data, accounts, parties, instruments, trades,
 *   positions, payments, ledger, settlement, reconciliation, workflow and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_TYPES_H
#define UMICOM_FINANCIAL_PLATFORM_TYPES_H

#include <stdint.h>
#include "umicom/base/value_archive.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_PLATFORM_ID_CAPACITY 96U
#define UMI_FINANCIAL_PLATFORM_LABEL_CAPACITY 128U

typedef struct UmiFinancialPlatformInput {
    const char *label;

    double value;
    double secondaryValue;
    double referenceValue;
    double limitValue;
    double tolerance;
    double amount;
    double convertedAmount;
    double rate;
    double debitTotal;
    double creditTotal;
    double grossExposure;
    double netExposure;
    double pnl;
    double concentrationPercent;
    double suspenseAmount;
    double totalAmount;

    uint64_t matchedCount;
    uint64_t totalCount;
    uint64_t mismatchCount;
    uint64_t pendingCount;
    uint64_t approvedCount;
    uint64_t rejectedCount;
    uint64_t completedCount;
    uint64_t exceptionCount;
    uint64_t auditCount;
    uint64_t persistedCount;
    uint64_t gapCount;
    uint64_t ageMilliseconds;
    uint64_t nowMilliseconds;
    uint64_t dueMilliseconds;

    int validIdentity;
    int active;
    int trusted;
    int approved;
    int currencyReady;
    int partyReady;
    int accountReady;
    int instrumentReady;
    int tradeReady;
    int positionReady;
    int paymentReady;
    int ledgerReady;
    int settlementReady;
    int reconciliationReady;
    int workflowReady;
    int auditReady;
    int transitionAllowed;
    int rollbackAvailable;

    uint64_t revision;
} UmiFinancialPlatformInput;

typedef struct UmiFinancialPlatformSnapshot {
    char id[UMI_FINANCIAL_PLATFORM_ID_CAPACITY];
    char label[UMI_FINANCIAL_PLATFORM_LABEL_CAPACITY];
    double value;
    double secondaryValue;
    double score;
    double ratio;
    double delta;
    int ready;
    int attention;
    int blocked;
    int approvalRequired;
    uint64_t revision;
} UmiFinancialPlatformSnapshot;

typedef UmiStatus (*UmiFinancialPlatformEvaluator)(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

void umi_financial_platform_input_init(UmiFinancialPlatformInput *input);
void umi_financial_platform_snapshot_init(UmiFinancialPlatformSnapshot *snapshot);
UmiStatus umi_financial_platform_snapshot_validate(
    const UmiFinancialPlatformSnapshot *snapshot);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_financial_platform_snapshot_archive_encode(const UmiFinancialPlatformSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_financial_platform_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiFinancialPlatformSnapshot *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiFinancialPlatformSnapshot object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_financial_platform_snapshot_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiFinancialPlatformSnapshot *workspace, UmiFinancialPlatformSnapshot *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_financial_platform_snapshot_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFinancialPlatformSnapshot *expected, UmiFinancialPlatformSnapshot *value);


#ifdef __cplusplus
}
#endif
#endif
