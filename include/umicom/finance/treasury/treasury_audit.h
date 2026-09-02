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
#ifdef __cplusplus
}
#endif
#endif
