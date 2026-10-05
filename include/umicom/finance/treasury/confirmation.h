/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/confirmation.h
 *
 * PURPOSE:
 *   Represent trade confirmation terms and confirmation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CONFIRMATION_H
#define UMICOM_FINANCE_TREASURY_CONFIRMATION_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury confirmation data shared with callers of this public contract.
 */
typedef struct UmiTreasuryConfirmation {
    char id[UMI_TREASURY_ID_CAPACITY];
    char trade_id[UMI_TREASURY_ID_CAPACITY];
    bool sent;
    bool acknowledged;
} UmiTreasuryConfirmation;
/**
 * Initialise treasury confirmation from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_confirmation_init(UmiTreasuryConfirmation *value,
    const char *id,
    const char *trade_id,
    bool sent,
    bool acknowledged);
/**
 * Check that treasury confirmation satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_confirmation_valid(const UmiTreasuryConfirmation *value);
/**
 * Provide the treasury confirmation complete operation used by this module and its client
 * applications.
 */
bool umi_treasury_confirmation_complete(const UmiTreasuryConfirmation *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_confirmation_archive_encode(const UmiTreasuryConfirmation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_confirmation_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryConfirmation *value);

#ifdef __cplusplus
}
#endif
#endif
