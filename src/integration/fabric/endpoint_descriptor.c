/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/endpoint_descriptor.c
 *
 * PURPOSE:
 *   Describe a protocol endpoint independently from any transport implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/endpoint_descriptor.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric endpoint descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_endpoint_descriptor_init(UmiFabricEndpointDescriptor *item, const char *endpoint_id, const char *uri, UmiFabricProtocol protocol, bool secure, uint32_t weight) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->endpoint_id,sizeof(item->endpoint_id),endpoint_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->uri,sizeof(item->uri),uri);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; item->protocol=protocol;item->secure=secure;item->weight=weight;
    return umi_fabric_endpoint_descriptor_validate(item);
}
/*
 * Check that fabric endpoint descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_endpoint_descriptor_validate(const UmiFabricEndpointDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->endpoint_id[0]!='\0' && item->uri[0]!='\0' && item->protocol>=UMI_FABRIC_PROTOCOL_INPROC && item->protocol<=UMI_FABRIC_PROTOCOL_FILE && item->weight>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
