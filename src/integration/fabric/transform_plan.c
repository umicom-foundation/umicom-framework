/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/transform_plan.c
 *
 * PURPOSE:
 *   Maintain an ordered set of unique transform rules before pipeline execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transform_plan.h"
#include <string.h>
#include <limits.h>

void umi_fabric_transform_plan_init(UmiFabricTransformPlan *registry) { if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
UmiStatus umi_fabric_transform_plan_add(UmiFabricTransformPlan *registry,const UmiFabricTransformRule *item) {
    size_t i;
    if (registry==NULL || item==NULL || item->rule_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].rule_id,item->rule_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
UmiStatus umi_fabric_transform_plan_find(const UmiFabricTransformPlan *registry,const char *id,UmiFabricTransformRule *out_item) {
    size_t i;
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].rule_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_fabric_transform_plan_count(const UmiFabricTransformPlan *registry) { return registry!=NULL?registry->count:0U; }
