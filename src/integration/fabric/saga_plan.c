/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/saga_plan.c
 *
 * PURPOSE:
 *   Maintain unique saga steps in execution order for deterministic reverse compensation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/saga_plan.h"
#include <string.h>
#include <limits.h>

void umi_fabric_saga_plan_init(UmiFabricSagaPlan *registry) { if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
UmiStatus umi_fabric_saga_plan_add(UmiFabricSagaPlan *registry,const UmiFabricSagaStep *item) {
    size_t i;
    if (registry==NULL || item==NULL || item->step_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].step_id,item->step_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
UmiStatus umi_fabric_saga_plan_find(const UmiFabricSagaPlan *registry,const char *id,UmiFabricSagaStep *out_item) {
    size_t i;
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].step_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_fabric_saga_plan_count(const UmiFabricSagaPlan *registry) { return registry!=NULL?registry->count:0U; }
