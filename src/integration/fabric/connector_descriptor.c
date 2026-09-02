/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/connector_descriptor.c
 *
 * PURPOSE:
 *   Describe a connector implementation and supported protocol/capability surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_descriptor.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric connector descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_connector_descriptor_init(UmiFabricConnectorDescriptor *item, const char *connector_id, const char *provider, UmiFabricProtocol protocol, uint64_t capability_mask, bool supports_transactions) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->connector_id,sizeof(item->connector_id),connector_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->provider,sizeof(item->provider),provider);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->protocol=protocol;item->capability_mask=capability_mask;item->supports_transactions=supports_transactions;
    return umi_fabric_connector_descriptor_validate(item);
}
/*
 * Check that fabric connector descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_connector_descriptor_validate(const UmiFabricConnectorDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->connector_id[0]!='\0' && item->provider[0]!='\0' && item->protocol>=UMI_FABRIC_PROTOCOL_INPROC && item->protocol<=UMI_FABRIC_PROTOCOL_FILE)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
