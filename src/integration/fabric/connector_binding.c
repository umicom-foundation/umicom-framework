/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/connector_binding.c
 *
 * PURPOSE:
 *   Bind a connector to one endpoint/profile without embedding product-specific connection logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_binding.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_connector_binding_init(UmiFabricConnectorBinding *item, const char *binding_id, const char *connector_id, const char *endpoint_id, const char *transport_profile_id) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->binding_id,sizeof(item->binding_id),binding_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->connector_id,sizeof(item->connector_id),connector_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->endpoint_id,sizeof(item->endpoint_id),endpoint_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->transport_profile_id,sizeof(item->transport_profile_id),transport_profile_id);if(s!=UMI_STATUS_OK)return s;item->enabled=true;
    return umi_fabric_connector_binding_validate(item);
}
UmiStatus umi_fabric_connector_binding_validate(const UmiFabricConnectorBinding *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->binding_id[0]!='\0' && item->connector_id[0]!='\0' && item->endpoint_id[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
