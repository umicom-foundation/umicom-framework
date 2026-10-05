/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/credit_risk_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate credit exposure and expected loss.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CREDIT_RISK_SNAPSHOT_H
#define UMICOM_FINANCE_TREASURY_CREDIT_RISK_SNAPSHOT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury credit risk snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCreditRiskSnapshot {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryRiskClass risk_class;
    int64_t primary_minor;
    int64_t secondary_minor;
} UmiTreasuryCreditRiskSnapshot;
/**
 * Initialise treasury credit risk snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_credit_risk_snapshot_init(UmiTreasuryCreditRiskSnapshot *value,
    const char *id,
    int64_t primary_minor,
    int64_t secondary_minor);
/**
 * Check that treasury credit risk snapshot satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_credit_risk_snapshot_valid(const UmiTreasuryCreditRiskSnapshot *value);
/**
 * Provide the treasury credit risk snapshot combined absolute minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_credit_risk_snapshot_combined_absolute_minor(const UmiTreasuryCreditRiskSnapshot *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_credit_risk_snapshot_archive_encode(const UmiTreasuryCreditRiskSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_credit_risk_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryCreditRiskSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
