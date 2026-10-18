/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/types.h
 *
 * PURPOSE:
 *   Define bounded Open TMS evidence for FX-forward lifecycle, cash/liquidity,
 *   pricing, positions, risk, settlement, accounting and reconciliation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_TYPES_H
#define UMICOM_OPEN_TMS_TYPES_H

#include <stdint.h>
#include "umicom/base/value_archive.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OPEN_TMS_ID_CAPACITY 96U
#define UMI_OPEN_TMS_LABEL_CAPACITY 128U

typedef struct UmiOpenTmsInput {
    const char *label;

    double value;
    double secondaryValue;
    double referenceValue;
    double limitValue;
    double amount;
    double notional;
    double spotRate;
    double domesticRate;
    double foreignRate;
    double yearFraction;
    double forwardRate;
    double discountFactor;
    double presentValue;
    double accruedAmount;
    double pnl;
    double grossExposure;
    double netExposure;
    double concentrationPercent;
    double liquidityInflows;
    double liquidityOutflows;
    double liquidityBuffer;
    double fundingAvailable;
    double fundingRequired;
    double settledCash;
    double projectedCash;
    double minimumCash;

    uint64_t matchedCount;
    uint64_t totalCount;
    uint64_t completedCount;
    uint64_t failedCount;
    uint64_t breakCount;
    uint64_t breachedCount;
    uint64_t ageMilliseconds;
    uint64_t nowMilliseconds;
    uint64_t dueMilliseconds;

    int validIdentity;
    int active;
    int trusted;
    int approved;
    int marketDataReady;
    int curvesReady;
    int fixingsReady;
    int cashReady;
    int liquidityReady;
    int positionReady;
    int riskReady;
    int settlementReady;
    int accountingReady;
    int reconciliationReady;
    int auditReady;
    int transitionAllowed;

    uint64_t revision;
} UmiOpenTmsInput;

typedef struct UmiOpenTmsSnapshot {
    char id[UMI_OPEN_TMS_ID_CAPACITY];
    char label[UMI_OPEN_TMS_LABEL_CAPACITY];
    double value;
    double secondaryValue;
    double score;
    double ratio;
    double pnl;
    int ready;
    int attention;
    int blocked;
    int approvalRequired;
    uint64_t revision;
} UmiOpenTmsSnapshot;

typedef UmiStatus (*UmiOpenTmsEvaluator)(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

void umi_open_tms_input_init(UmiOpenTmsInput *input);
void umi_open_tms_snapshot_init(UmiOpenTmsSnapshot *snapshot);
UmiStatus umi_open_tms_snapshot_validate(const UmiOpenTmsSnapshot *snapshot);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_open_tms_snapshot_archive_encode(const UmiOpenTmsSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_open_tms_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiOpenTmsSnapshot *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiOpenTmsSnapshot object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_open_tms_snapshot_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiOpenTmsSnapshot *workspace, UmiOpenTmsSnapshot *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_open_tms_snapshot_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiOpenTmsSnapshot *expected, UmiOpenTmsSnapshot *value);


#ifdef __cplusplus
}
#endif
#endif
