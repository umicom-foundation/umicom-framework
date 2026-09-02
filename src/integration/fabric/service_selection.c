/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_selection.c
 *
 * PURPOSE:
 *   Select the best healthy service instance using deterministic priority and weight scoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_selection.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric service select operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_service_select(const UmiFabricServiceRegistry *registry,const char *service_id,UmiFabricServiceInstance *out_instance) {
    size_t i; bool found=false; uint64_t best=0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(registry==NULL || service_id==NULL || out_instance==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<registry->count;++i){ const UmiFabricServiceInstance *v=&registry->items[i]; uint64_t score;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(!v->healthy || strcmp(v->service_id,service_id)!=0) continue;
        score=(UINT64_C(1000000)-(uint64_t)(v->priority<1000000U?v->priority:999999U))+(uint64_t)v->weight;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(!found || score>best || (score==best && strcmp(v->instance_id,out_instance->instance_id)<0)){*out_instance=*v;best=score;found=true;}
    }
    return found?UMI_STATUS_OK:UMI_STATUS_NOT_FOUND;
}
