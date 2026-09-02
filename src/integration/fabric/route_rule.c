/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/route_rule.c
 *
 * PURPOSE:
 *   Describe ordered route matching by source, destination and message pattern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/route_rule.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric route rule from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_route_rule_init(UmiFabricRouteRule *item, const char *route_id, const char *source_pattern, const char *message_pattern, const char *destination_id, uint32_t priority) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->route_id,sizeof(item->route_id),route_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->source_pattern,sizeof(item->source_pattern),source_pattern);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->message_pattern,sizeof(item->message_pattern),message_pattern);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->destination_id,sizeof(item->destination_id),destination_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->priority=priority;item->enabled=true;
    return umi_fabric_route_rule_validate(item);
}
/* Check that fabric route rule satisfies its contract before another service relies on it. */
UmiStatus umi_fabric_route_rule_validate(const UmiFabricRouteRule *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->route_id[0]!='\0' && item->message_pattern[0]!='\0' && item->destination_id[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
