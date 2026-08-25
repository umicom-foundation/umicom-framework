/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_descriptor.c
 *
 * PURPOSE:
 *   Describe a reusable integration service, version and advertised capability mask.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_descriptor.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_service_descriptor_init(UmiFabricServiceDescriptor *item, const char *service_id, const char *name, UmiFabricVersion version, uint64_t capability_mask) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->service_id,sizeof(item->service_id),service_id); if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->name,sizeof(item->name),name); if(s!=UMI_STATUS_OK)return s;
    item->version=version; item->capability_mask=capability_mask;
    return umi_fabric_service_descriptor_validate(item);
}
UmiStatus umi_fabric_service_descriptor_validate(const UmiFabricServiceDescriptor *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->service_id[0]!='\0' && item->name[0]!='\0' && item->version.major>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
