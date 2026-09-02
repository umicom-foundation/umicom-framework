/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/protocol_contract.c
 *
 * PURPOSE:
 *   Describe versioned protocol expectations and streaming semantics for endpoint negotiation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/protocol_contract.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric protocol contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_protocol_contract_init(UmiFabricProtocolContract *item, const char *contract_id, UmiFabricProtocol protocol, UmiFabricVersion minimum_version, UmiFabricVersion maximum_version, bool streaming, bool bidirectional) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->contract_id,sizeof(item->contract_id),contract_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->protocol=protocol;item->minimum_version=minimum_version;item->maximum_version=maximum_version;item->streaming=streaming;item->bidirectional=bidirectional;
    return umi_fabric_protocol_contract_validate(item);
}
/*
 * Check that fabric protocol contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_protocol_contract_validate(const UmiFabricProtocolContract *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->contract_id[0]!='\0' && item->protocol>=UMI_FABRIC_PROTOCOL_INPROC && item->protocol<=UMI_FABRIC_PROTOCOL_FILE && umi_fabric_version_compare(item->minimum_version,item->maximum_version)<=0)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
