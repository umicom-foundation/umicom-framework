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

/*
 * Initialise fabric connector binding from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_connector_binding_init(UmiFabricConnectorBinding *item, const char *binding_id, const char *connector_id, const char *endpoint_id, const char *transport_profile_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->binding_id,sizeof(item->binding_id),binding_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->connector_id,sizeof(item->connector_id),connector_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->endpoint_id,sizeof(item->endpoint_id),endpoint_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->transport_profile_id,sizeof(item->transport_profile_id),transport_profile_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->enabled=true;
    return umi_fabric_connector_binding_validate(item);
}
/*
 * Check that fabric connector binding satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_connector_binding_validate(const UmiFabricConnectorBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->binding_id[0]!='\0' && item->connector_id[0]!='\0' && item->endpoint_id[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
