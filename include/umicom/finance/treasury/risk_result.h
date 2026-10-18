/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_result.h
 *
 * PURPOSE:
 *   Record observed, stressed and limit risk values for governance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_RESULT_H
#define UMICOM_FINANCE_TREASURY_RISK_RESULT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury risk result data shared with callers of this public contract.
 */
typedef struct UmiTreasuryRiskResult {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t observed_minor;
    int64_t stressed_minor;
    int64_t limit_minor;
} UmiTreasuryRiskResult;
/**
 * Initialise treasury risk result from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_risk_result_init(UmiTreasuryRiskResult *value,
    const char *id,
    int64_t observed_minor,
    int64_t stressed_minor,
    int64_t limit_minor);
/**
 * Check that treasury risk result satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_risk_result_valid(const UmiTreasuryRiskResult *value);
/**
 * Provide the treasury risk result within limit operation used by this module and its
 * client applications.
 */
bool umi_treasury_risk_result_within_limit(const UmiTreasuryRiskResult *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_risk_result_archive_encode(const UmiTreasuryRiskResult *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_risk_result_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryRiskResult *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTreasuryRiskResult object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_treasury_risk_result_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTreasuryRiskResult *workspace, UmiTreasuryRiskResult *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_risk_result_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasuryRiskResult *expected, UmiTreasuryRiskResult *value);


#ifdef __cplusplus
}
#endif
#endif
