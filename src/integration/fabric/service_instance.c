/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_instance.c
 *
 * PURPOSE:
 *   Represent a live service instance advertised to the Fabric registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_instance.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_service_instance_init(UmiFabricServiceInstance *item, const char *instance_id, const char *service_id, const char *endpoint_id, uint32_t priority, uint32_t weight) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->instance_id,sizeof(item->instance_id),instance_id); if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->service_id,sizeof(item->service_id),service_id); if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->endpoint_id,sizeof(item->endpoint_id),endpoint_id); if(s!=UMI_STATUS_OK)return s;
    item->priority=priority; item->weight=weight; item->healthy=true;
    return umi_fabric_service_instance_validate(item);
}
UmiStatus umi_fabric_service_instance_validate(const UmiFabricServiceInstance *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->instance_id[0]!='\0' && item->service_id[0]!='\0' && item->endpoint_id[0]!='\0' && item->weight>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
