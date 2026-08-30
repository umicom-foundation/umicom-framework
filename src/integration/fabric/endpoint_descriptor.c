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

UmiStatus umi_fabric_endpoint_descriptor_init(UmiFabricEndpointDescriptor *item, const char *endpoint_id, const char *uri, UmiFabricProtocol protocol, bool secure, uint32_t weight) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->endpoint_id,sizeof(item->endpoint_id),endpoint_id);if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->uri,sizeof(item->uri),uri);if(s!=UMI_STATUS_OK)return s; item->protocol=protocol;item->secure=secure;item->weight=weight;
    return umi_fabric_endpoint_descriptor_validate(item);
}
UmiStatus umi_fabric_endpoint_descriptor_validate(const UmiFabricEndpointDescriptor *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->endpoint_id[0]!='\0' && item->uri[0]!='\0' && item->protocol>=UMI_FABRIC_PROTOCOL_INPROC && item->protocol<=UMI_FABRIC_PROTOCOL_FILE && item->weight>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
