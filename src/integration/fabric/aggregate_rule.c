/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/aggregate_rule.c
 *
 * PURPOSE:
 *   Describe bounded correlation-based aggregation windows before the canonical messaging aggregator executes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/aggregate_rule.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric aggregate rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_aggregate_rule_init(UmiFabricAggregateRule *item, const char *rule_id, const char *correlation_field, size_t expected_count, uint64_t timeout_ms, bool partial_allowed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->rule_id,sizeof(item->rule_id),rule_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->correlation_field,sizeof(item->correlation_field),correlation_field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->expected_count=expected_count;item->timeout_ms=timeout_ms;item->partial_allowed=partial_allowed;
    return umi_fabric_aggregate_rule_validate(item);
}
/*
 * Check that fabric aggregate rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_aggregate_rule_validate(const UmiFabricAggregateRule *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->rule_id[0]!='\0' && item->correlation_field[0]!='\0' && item->expected_count>0U && item->timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
