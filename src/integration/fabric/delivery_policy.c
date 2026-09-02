/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/delivery_policy.c
 *
 * PURPOSE:
 *   Describe acknowledgement, attempt and durability requirements for message delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/delivery_policy.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric delivery policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_delivery_policy_init(UmiFabricDeliveryPolicy *item, const char *policy_id, UmiFabricDeliveryMode mode, uint32_t max_attempts, uint64_t acknowledgement_timeout_ms, bool durable) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->policy_id,sizeof(item->policy_id),policy_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->mode=mode;item->max_attempts=max_attempts;item->acknowledgement_timeout_ms=acknowledgement_timeout_ms;item->durable=durable;
    return umi_fabric_delivery_policy_validate(item);
}
/*
 * Check that fabric delivery policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_delivery_policy_validate(const UmiFabricDeliveryPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->policy_id[0]!='\0' && item->mode>=UMI_FABRIC_DELIVERY_AT_MOST_ONCE && item->mode<=UMI_FABRIC_DELIVERY_IDEMPOTENT_EFFECT && item->max_attempts>0U && item->acknowledgement_timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
