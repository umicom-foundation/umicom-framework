/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/confirmation_matching.h
 *
 * PURPOSE:
 *   Score confirmation field matching and expose exact-match status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CONFIRMATION_MATCHING_H
#define UMICOM_FINANCE_TREASURY_CONFIRMATION_MATCHING_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury confirmation matching data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryConfirmationMatching {
    char id[UMI_TREASURY_ID_CAPACITY];
    uint32_t matched_fields;
    uint32_t total_fields;
} UmiTreasuryConfirmationMatching;
/**
 * Initialise treasury confirmation matching from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_confirmation_matching_init(UmiTreasuryConfirmationMatching *value,
    const char *id,
    uint32_t matched_fields,
    uint32_t total_fields);
/**
 * Check that treasury confirmation matching satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_confirmation_matching_valid(const UmiTreasuryConfirmationMatching *value);
/**
 * Provide the treasury confirmation matching exact operation used by this module and its
 * client applications.
 */
bool umi_treasury_confirmation_matching_exact(const UmiTreasuryConfirmationMatching *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_confirmation_matching_archive_encode(const UmiTreasuryConfirmationMatching *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_confirmation_matching_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryConfirmationMatching *value);

#ifdef __cplusplus
}
#endif
#endif
