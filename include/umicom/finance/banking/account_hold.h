/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/account_hold.h
 *
 * PURPOSE:
 *   Represent ring-fenced account funds and explicit release state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_ACCOUNT_HOLD_H
#define UMICOM_FINANCE_BANKING_ACCOUNT_HOLD_H
#include "umicom/finance/banking/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking account hold struct shared with callers of this public
 * contract.
 */
typedef struct UmiBankingAccountHold {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t amount_minor;
    bool active;
} UmiBankingAccountHold;
/**
 * Initialise banking account hold from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_banking_account_hold_init(UmiBankingAccountHold *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    bool active);
/**
 * Check that banking account hold satisfies its contract before another
 * service relies on it.
 */
bool umi_banking_account_hold_valid(const UmiBankingAccountHold *value);
/**
 * Provide the banking account hold releasable operation used by this module
 * and its client applications.
 */
bool umi_banking_account_hold_releasable(const UmiBankingAccountHold *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_banking_account_hold_archive_encode(const UmiBankingAccountHold *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_banking_account_hold_archive_decode(const void *bytes, size_t byte_count,
    UmiBankingAccountHold *value);

#ifdef __cplusplus
}
#endif
#endif
