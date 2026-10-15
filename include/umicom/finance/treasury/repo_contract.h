/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/repo_contract.h
 *
 * PURPOSE:
 *   Model repo cash principal, collateral value and repo rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_REPO_CONTRACT_H
#define UMICOM_FINANCE_TREASURY_REPO_CONTRACT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury repo contract data shared with callers of this public contract.
 */
typedef struct UmiTreasuryRepoContract {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t cash_principal_minor;
    int64_t collateral_value_minor;
    uint32_t repo_rate_bps;
} UmiTreasuryRepoContract;
/**
 * Initialise treasury repo contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_repo_contract_init(UmiTreasuryRepoContract *value,
    const char *id,
    int64_t cash_principal_minor,
    int64_t collateral_value_minor,
    uint32_t repo_rate_bps);
/**
 * Check that treasury repo contract satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_repo_contract_valid(const UmiTreasuryRepoContract *value);
/**
 * Provide the treasury repo contract haircut minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_repo_contract_haircut_minor(const UmiTreasuryRepoContract *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_repo_contract_archive_encode(const UmiTreasuryRepoContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_repo_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryRepoContract *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_repo_contract_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasuryRepoContract *expected, UmiTreasuryRepoContract *value);


#ifdef __cplusplus
}
#endif
#endif
