/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/haircut_rule.h
 *
 * PURPOSE:
 *   Define collateral valuation haircut in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_HAIRCUT_RULE_H
#define UMICOM_FINANCE_TREASURY_HAIRCUT_RULE_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury haircut rule data shared with callers of this public contract.
 */
typedef struct UmiTreasuryHaircutRule {
    char id[UMI_TREASURY_ID_CAPACITY];
    uint32_t haircut_bps;
} UmiTreasuryHaircutRule;
/**
 * Initialise treasury haircut rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_haircut_rule_init(UmiTreasuryHaircutRule *value,
    const char *id,
    uint32_t haircut_bps);
/**
 * Check that treasury haircut rule satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_haircut_rule_valid(const UmiTreasuryHaircutRule *value);
/**
 * Provide the treasury haircut rule remaining bps operation used by this module and its
 * client applications.
 */
uint32_t umi_treasury_haircut_rule_remaining_bps(const UmiTreasuryHaircutRule *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_haircut_rule_archive_encode(const UmiTreasuryHaircutRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_haircut_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryHaircutRule *value);

#ifdef __cplusplus
}
#endif
#endif
