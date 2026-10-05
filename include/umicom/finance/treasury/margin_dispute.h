/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/margin_dispute.h
 *
 * PURPOSE:
 *   Track margin dispute amount and resolution state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARGIN_DISPUTE_H
#define UMICOM_FINANCE_TREASURY_MARGIN_DISPUTE_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury margin dispute data shared with callers of this public contract.
 */
typedef struct UmiTreasuryMarginDispute {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t disputed_minor;
    int64_t resolved_minor;
} UmiTreasuryMarginDispute;
/**
 * Initialise treasury margin dispute from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_margin_dispute_init(UmiTreasuryMarginDispute *value,
    const char *id,
    int64_t disputed_minor,
    int64_t resolved_minor);
/**
 * Check that treasury margin dispute satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_margin_dispute_valid(const UmiTreasuryMarginDispute *value);
/**
 * Provide the treasury margin dispute outstanding minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_margin_dispute_outstanding_minor(const UmiTreasuryMarginDispute *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_margin_dispute_archive_encode(const UmiTreasuryMarginDispute *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_margin_dispute_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryMarginDispute *value);

#ifdef __cplusplus
}
#endif
#endif
