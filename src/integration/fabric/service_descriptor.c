/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_descriptor.c
 *
 * PURPOSE:
 *   Describe a reusable integration service, version and advertised capability mask.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_descriptor.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric service descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_service_descriptor_init(UmiFabricServiceDescriptor *item, const char *service_id, const char *name, UmiFabricVersion version, uint64_t capability_mask) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->service_id,sizeof(item->service_id),service_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->name,sizeof(item->name),name); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    item->version=version; item->capability_mask=capability_mask;
    return umi_fabric_service_descriptor_validate(item);
}
/*
 * Check that fabric service descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_service_descriptor_validate(const UmiFabricServiceDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->service_id[0]!='\0' && item->name[0]!='\0' && item->version.major>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
