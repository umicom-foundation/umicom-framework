/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/eligibility_rule.h
 *
 * PURPOSE:
 *   Evaluate collateral eligibility using minimum value and maximum maturity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_ELIGIBILITY_RULE_H
#define UMICOM_FINANCE_TREASURY_ELIGIBILITY_RULE_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury eligibility rule data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryEligibilityRule {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t minimum_value_minor;
    uint32_t maximum_maturity_days;
} UmiTreasuryEligibilityRule;
/**
 * Initialise treasury eligibility rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_eligibility_rule_init(UmiTreasuryEligibilityRule *value,
    const char *id,
    int64_t minimum_value_minor,
    uint32_t maximum_maturity_days);
/**
 * Check that treasury eligibility rule satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_eligibility_rule_valid(const UmiTreasuryEligibilityRule *value);
/**
 * Provide the treasury eligibility rule usable operation used by this module and its
 * client applications.
 */
bool umi_treasury_eligibility_rule_usable(const UmiTreasuryEligibilityRule *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_eligibility_rule_archive_encode(const UmiTreasuryEligibilityRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_eligibility_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryEligibilityRule *value);

#ifdef __cplusplus
}
#endif
#endif
