/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_factor.h
 *
 * PURPOSE:
 *   Describe a named risk factor, class and market shock in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_FACTOR_H
#define UMICOM_FINANCE_TREASURY_RISK_FACTOR_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury risk factor data shared with callers of this public contract.
 */
typedef struct UmiTreasuryRiskFactor {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryRiskClass risk_class;
    int32_t shock_bps;
} UmiTreasuryRiskFactor;
/**
 * Initialise treasury risk factor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_risk_factor_init(UmiTreasuryRiskFactor *value,
    const char *id,
    UmiTreasuryRiskClass risk_class,
    int32_t shock_bps);
/**
 * Check that treasury risk factor satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_risk_factor_valid(const UmiTreasuryRiskFactor *value);
/**
 * Provide the treasury risk factor absolute shock bps operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_risk_factor_absolute_shock_bps(const UmiTreasuryRiskFactor *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_risk_factor_archive_encode(const UmiTreasuryRiskFactor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_risk_factor_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryRiskFactor *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTreasuryRiskFactor object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_treasury_risk_factor_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTreasuryRiskFactor *workspace, UmiTreasuryRiskFactor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_risk_factor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasuryRiskFactor *expected, UmiTreasuryRiskFactor *value);


#ifdef __cplusplus
}
#endif
#endif
