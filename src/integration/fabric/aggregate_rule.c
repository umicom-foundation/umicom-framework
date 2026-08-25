/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/aggregate_rule.c
 *
 * PURPOSE:
 *   Describe bounded correlation-based aggregation windows before the canonical messaging aggregator executes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/aggregate_rule.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_aggregate_rule_init(UmiFabricAggregateRule *item, const char *rule_id, const char *correlation_field, size_t expected_count, uint64_t timeout_ms, bool partial_allowed) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->rule_id,sizeof(item->rule_id),rule_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->correlation_field,sizeof(item->correlation_field),correlation_field);if(s!=UMI_STATUS_OK)return s;item->expected_count=expected_count;item->timeout_ms=timeout_ms;item->partial_allowed=partial_allowed;
    return umi_fabric_aggregate_rule_validate(item);
}
UmiStatus umi_fabric_aggregate_rule_validate(const UmiFabricAggregateRule *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->rule_id[0]!='\0' && item->correlation_field[0]!='\0' && item->expected_count>0U && item->timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
