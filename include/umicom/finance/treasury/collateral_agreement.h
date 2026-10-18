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

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTreasuryCollateralAgreement object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_treasury_collateral_agreement_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTreasuryCollateralAgreement *workspace, UmiTreasuryCollateralAgreement *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_collateral_agreement_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasuryCollateralAgreement *expected, UmiTreasuryCollateralAgreement *value);


#ifdef __cplusplus
}
#endif
#endif
