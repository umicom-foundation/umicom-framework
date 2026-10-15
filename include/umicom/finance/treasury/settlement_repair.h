/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_repair.h
 *
 * PURPOSE:
 *   Represent a settlement repair action with bounded attempt governance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_REPAIR_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_REPAIR_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury settlement repair data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasurySettlementRepair {
    char id[UMI_TREASURY_ID_CAPACITY];
    uint32_t attempt;
    uint32_t maximum_attempts;
} UmiTreasurySettlementRepair;
/**
 * Initialise treasury settlement repair from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_repair_init(UmiTreasurySettlementRepair *value,
    const char *id,
    uint32_t attempt,
    uint32_t maximum_attempts);
/**
 * Check that treasury settlement repair satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_repair_valid(const UmiTreasurySettlementRepair *value);
/**
 * Provide the treasury settlement repair retry allowed operation used by this module and
 * its client applications.
 */
bool umi_treasury_settlement_repair_retry_allowed(const UmiTreasurySettlementRepair *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_settlement_repair_archive_encode(const UmiTreasurySettlementRepair *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_settlement_repair_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasurySettlementRepair *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_settlement_repair_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasurySettlementRepair *expected, UmiTreasurySettlementRepair *value);


#ifdef __cplusplus
}
#endif
#endif
