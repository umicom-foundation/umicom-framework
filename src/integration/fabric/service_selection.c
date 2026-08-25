/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_selection.c
 *
 * PURPOSE:
 *   Select the best healthy service instance using deterministic priority and weight scoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_selection.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_service_select(const UmiFabricServiceRegistry *registry,const char *service_id,UmiFabricServiceInstance *out_instance) {
    size_t i; bool found=false; uint64_t best=0U;
    if(registry==NULL || service_id==NULL || out_instance==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<registry->count;++i){ const UmiFabricServiceInstance *v=&registry->items[i]; uint64_t score;
        if(!v->healthy || strcmp(v->service_id,service_id)!=0) continue;
        score=(UINT64_C(1000000)-(uint64_t)(v->priority<1000000U?v->priority:999999U))+(uint64_t)v->weight;
        if(!found || score>best || (score==best && strcmp(v->instance_id,out_instance->instance_id)<0)){*out_instance=*v;best=score;found=true;}
    }
    return found?UMI_STATUS_OK:UMI_STATUS_NOT_FOUND;
}
