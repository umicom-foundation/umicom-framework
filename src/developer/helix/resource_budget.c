/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/resource_budget.c
 *
 * PURPOSE:
 *   Bound files, lines, attempts and execution time consumed by one Helix engineering session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/resource_budget.h"
#include <string.h>

/* Initialise stable API metadata and conservative default policy/evidence. */
void umi_helix_resource_budget_init(UmiHelixResourceBudget *record,const char *id){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL){
        return;
    }
    memset(record,0,sizeof(*record));
    record->structure_size=(uint32_t)sizeof(*record);
    record->api_version=UMI_HELIX_API_VERSION;
    record->state=UMI_HELIX_IDLE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id!=NULL){
        (void)umi_helix_copy_text(record->id,sizeof(record->id),id);
    }
    record->limit=1U; record->used=0U;
}
/* Reject malformed or future-incompatible records before orchestration consumes them. */
UmiStatus umi_helix_resource_budget_validate(const UmiHelixResourceBudget *record){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL || record->structure_size!=sizeof(*record) || record->api_version!=UMI_HELIX_API_VERSION || !umi_helix_valid_id(record->id)){
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
/* Reduce this record to one deterministic normalised score without executing side effects. */
double umi_helix_resource_budget_score(const UmiHelixResourceBudget *record){
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_helix_resource_budget_validate(record)!=UMI_STATUS_OK){
        return 0.0;
    }
    return record->limit==0U?0.0:umi_helix_clamp_score(1.0-umi_helix_safe_ratio((double)record->used,(double)record->limit));
}
