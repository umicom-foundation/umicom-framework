/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_limit.h
 *
 * PURPOSE:
 *   Define a hard treasury risk limit and warning threshold.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_LIMIT_H
#define UMICOM_FINANCE_TREASURY_RISK_LIMIT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury risk limit data shared with callers of this public contract.
 */
typedef struct UmiTreasuryRiskLimit {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t hard_limit_minor;
    int64_t warning_limit_minor;
} UmiTreasuryRiskLimit;
/**
 * Initialise treasury risk limit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_treasury_risk_limit_init(UmiTreasuryRiskLimit *value,
    const char *id,
    int64_t hard_limit_minor,
    int64_t warning_limit_minor);
/**
 * Check that treasury risk limit satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_risk_limit_valid(const UmiTreasuryRiskLimit *value);
/**
 * Provide the treasury risk limit buffer minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_risk_limit_buffer_minor(const UmiTreasuryRiskLimit *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_risk_limit_archive_encode(const UmiTreasuryRiskLimit *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_risk_limit_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryRiskLimit *value);

#ifdef __cplusplus
}
#endif
#endif
