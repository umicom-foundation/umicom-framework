/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_audit.c
 *
 * PURPOSE:
 *   Implement record treasury audit evidence with actor and monotonically increasing sequence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_audit.h"
#include <string.h>
/*
 * Initialise treasury treasury audit from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_audit_init(UmiTreasuryTreasuryAudit *value,
    const char *id,
    const char *actor_id,
    uint64_t sequence) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->actor_id,sizeof value->actor_id,actor_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    value->sequence=sequence;
    return umi_treasury_treasury_audit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury treasury audit satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_treasury_audit_valid(const UmiTreasuryTreasuryAudit *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->actor_id) && value->sequence > 0U);
}

/*
 * Provide the treasury treasury audit sequenced operation used by this module and its
 * client applications.
 */
bool umi_treasury_treasury_audit_sequenced(const UmiTreasuryTreasuryAudit *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->sequence > 0U;
}
