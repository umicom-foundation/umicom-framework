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

UmiStatus umi_fabric_protocol_contract_init(UmiFabricProtocolContract *item, const char *contract_id, UmiFabricProtocol protocol, UmiFabricVersion minimum_version, UmiFabricVersion maximum_version, bool streaming, bool bidirectional) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->contract_id,sizeof(item->contract_id),contract_id);if(s!=UMI_STATUS_OK)return s;item->protocol=protocol;item->minimum_version=minimum_version;item->maximum_version=maximum_version;item->streaming=streaming;item->bidirectional=bidirectional;
    return umi_fabric_protocol_contract_validate(item);
}
UmiStatus umi_fabric_protocol_contract_validate(const UmiFabricProtocolContract *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->contract_id[0]!='\0' && item->protocol>=UMI_FABRIC_PROTOCOL_INPROC && item->protocol<=UMI_FABRIC_PROTOCOL_FILE && umi_fabric_version_compare(item->minimum_version,item->maximum_version)<=0)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
