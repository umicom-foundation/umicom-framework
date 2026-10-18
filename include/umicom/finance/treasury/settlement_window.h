/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_window.h
 *
 * PURPOSE:
 *   Define operational settlement opening and cut-off timestamps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_WINDOW_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_WINDOW_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury settlement window data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasurySettlementWindow {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t opens_epoch_millis;
    int64_t closes_epoch_millis;
} UmiTreasurySettlementWindow;
/**
 * Initialise treasury settlement window from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_window_init(UmiTreasurySettlementWindow *value,
    const char *id,
    int64_t opens_epoch_millis,
    int64_t closes_epoch_millis);
/**
 * Check that treasury settlement window satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_window_valid(const UmiTreasurySettlementWindow *value);
/**
 * Provide the treasury settlement window duration millis operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_settlement_window_duration_millis(const UmiTreasurySettlementWindow *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_settlement_window_archive_encode(const UmiTreasurySettlementWindow *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_settlement_window_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasurySettlementWindow *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTreasurySettlementWindow object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_treasury_settlement_window_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTreasurySettlementWindow *workspace, UmiTreasurySettlementWindow *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_settlement_window_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasurySettlementWindow *expected, UmiTreasurySettlementWindow *value);


#ifdef __cplusplus
}
#endif
#endif
