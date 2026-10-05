/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/collateral_agreement.h
 *
 * PURPOSE:
 *   Model a collateral agreement threshold and minimum transfer amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COLLATERAL_AGREEMENT_H
#define UMICOM_FINANCE_TREASURY_COLLATERAL_AGREEMENT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury collateral agreement data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCollateralAgreement {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t threshold_minor;
    int64_t minimum_transfer_minor;
} UmiTreasuryCollateralAgreement;
/**
 * Initialise treasury collateral agreement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_collateral_agreement_init(UmiTreasuryCollateralAgreement *value,
    const char *id,
    int64_t threshold_minor,
    int64_t minimum_transfer_minor);
/**
 * Check that treasury collateral agreement satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_agreement_valid(const UmiTreasuryCollateralAgreement *value);
/**
 * Provide the treasury collateral agreement secured threshold minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_collateral_agreement_secured_threshold_minor(const UmiTreasuryCollateralAgreement *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_collateral_agreement_archive_encode(const UmiTreasuryCollateralAgreement *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_collateral_agreement_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryCollateralAgreement *value);

#ifdef __cplusplus
}
#endif
#endif
