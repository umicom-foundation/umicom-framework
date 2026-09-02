/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/saga_plan.c
 *
 * PURPOSE:
 *   Maintain unique saga steps in execution order for deterministic reverse compensation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/saga_plan.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric saga plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_fabric_saga_plan_init(UmiFabricSagaPlan *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
/* Add fabric saga plan only after its inputs and available capacity have been checked. */
UmiStatus umi_fabric_saga_plan_add(UmiFabricSagaPlan *registry,const UmiFabricSagaStep *item) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry==NULL || item==NULL || item->step_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<registry->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].step_id,item->step_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
/*
 * Find fabric saga plan while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_fabric_saga_plan_find(const UmiFabricSagaPlan *registry,const char *id,UmiFabricSagaStep *out_item) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<registry->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].step_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
/*
 * Return the number of records represented by fabric saga plan without changing their
 * state.
 */
size_t umi_fabric_saga_plan_count(const UmiFabricSagaPlan *registry) { return registry!=NULL?registry->count:0U; }
