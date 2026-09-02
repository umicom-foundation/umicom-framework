/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/dead_letter_policy.c
 *
 * PURPOSE:
 *   Describe bounded dead-letter escalation and retention rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/dead_letter_policy.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric dead letter policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_dead_letter_policy_init(UmiFabricDeadLetterPolicy *item, const char *policy_id, const char *destination, uint32_t after_attempts, uint64_t retention_ms, bool include_payload) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->policy_id,sizeof(item->policy_id),policy_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->destination,sizeof(item->destination),destination);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->after_attempts=after_attempts;item->retention_ms=retention_ms;item->include_payload=include_payload;
    return umi_fabric_dead_letter_policy_validate(item);
}
/*
 * Check that fabric dead letter policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_dead_letter_policy_validate(const UmiFabricDeadLetterPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->policy_id[0]!='\0' && item->destination[0]!='\0' && item->after_attempts>0U && item->retention_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
