/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/treasury_audit.h
 *
 * PURPOSE:
 *   Record treasury audit evidence with actor and monotonically increasing sequence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TREASURY_AUDIT_H
#define UMICOM_FINANCE_TREASURY_TREASURY_AUDIT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury treasury audit data shared with callers of this public contract.
 */
typedef struct UmiTreasuryTreasuryAudit {
    char id[UMI_TREASURY_ID_CAPACITY];
    char actor_id[UMI_TREASURY_ID_CAPACITY];
    uint64_t sequence;
} UmiTreasuryTreasuryAudit;
/**
 * Initialise treasury treasury audit from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_audit_init(UmiTreasuryTreasuryAudit *value,
    const char *id,
    const char *actor_id,
    uint64_t sequence);
/**
 * Check that treasury treasury audit satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_treasury_audit_valid(const UmiTreasuryTreasuryAudit *value);
/**
 * Provide the treasury treasury audit sequenced operation used by this module and its
 * client applications.
 */
bool umi_treasury_treasury_audit_sequenced(const UmiTreasuryTreasuryAudit *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_treasury_audit_archive_encode(const UmiTreasuryTreasuryAudit *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_treasury_audit_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryTreasuryAudit *value);

#ifdef __cplusplus
}
#endif
#endif
