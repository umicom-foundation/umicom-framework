/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/collateral_valuation.h
 *
 * PURPOSE:
 *   Calculate post-haircut collateral value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COLLATERAL_VALUATION_H
#define UMICOM_FINANCE_TREASURY_COLLATERAL_VALUATION_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury collateral valuation data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCollateralValuation {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t gross_value_minor;
    uint32_t haircut_bps;
} UmiTreasuryCollateralValuation;
/**
 * Initialise treasury collateral valuation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_collateral_valuation_init(UmiTreasuryCollateralValuation *value,
    const char *id,
    int64_t gross_value_minor,
    uint32_t haircut_bps);
/**
 * Check that treasury collateral valuation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_valuation_valid(const UmiTreasuryCollateralValuation *value);
/**
 * Provide the treasury collateral valuation eligible value minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_collateral_valuation_eligible_value_minor(const UmiTreasuryCollateralValuation *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_collateral_valuation_archive_encode(const UmiTreasuryCollateralValuation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_collateral_valuation_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryCollateralValuation *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTreasuryCollateralValuation object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_treasury_collateral_valuation_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTreasuryCollateralValuation *workspace, UmiTreasuryCollateralValuation *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_collateral_valuation_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasuryCollateralValuation *expected, UmiTreasuryCollateralValuation *value);


#ifdef __cplusplus
}
#endif
#endif
