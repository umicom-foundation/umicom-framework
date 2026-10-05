/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/custody_account.h
 *
 * PURPOSE:
 *   Model a securities custody account and segregation status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CUSTODY_ACCOUNT_H
#define UMICOM_FINANCE_TREASURY_CUSTODY_ACCOUNT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury custody account data shared with callers of this public contract.
 */
typedef struct UmiTreasuryCustodyAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    char custodian_id[UMI_TREASURY_ID_CAPACITY];
    bool segregated;
} UmiTreasuryCustodyAccount;
/**
 * Initialise treasury custody account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_custody_account_init(UmiTreasuryCustodyAccount *value,
    const char *id,
    const char *custodian_id,
    bool segregated);
/**
 * Check that treasury custody account satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_custody_account_valid(const UmiTreasuryCustodyAccount *value);
/**
 * Provide the treasury custody account is segregated operation used by this module and its
 * client applications.
 */
bool umi_treasury_custody_account_is_segregated(const UmiTreasuryCustodyAccount *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_custody_account_archive_encode(const UmiTreasuryCustodyAccount *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_custody_account_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryCustodyAccount *value);

#ifdef __cplusplus
}
#endif
#endif
