/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/margin_call.h
 *
 * PURPOSE:
 *   Represent a margin call amount, agreed amount and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARGIN_CALL_H
#define UMICOM_FINANCE_TREASURY_MARGIN_CALL_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury margin call data shared with callers of this public contract.
 */
typedef struct UmiTreasuryMarginCall {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t called_minor;
    int64_t agreed_minor;
    UmiTreasuryMarginState state;
} UmiTreasuryMarginCall;
/**
 * Initialise treasury margin call from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_margin_call_init(UmiTreasuryMarginCall *value,
    const char *id,
    int64_t called_minor,
    int64_t agreed_minor,
    UmiTreasuryMarginState state);
/**
 * Check that treasury margin call satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_margin_call_valid(const UmiTreasuryMarginCall *value);
/**
 * Provide the treasury margin call unagreed minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_margin_call_unagreed_minor(const UmiTreasuryMarginCall *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_margin_call_archive_encode(const UmiTreasuryMarginCall *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_margin_call_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryMarginCall *value);

#ifdef __cplusplus
}
#endif
#endif
