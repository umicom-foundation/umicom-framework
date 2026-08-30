/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/message_contract.c
 *
 * PURPOSE:
 *   Describe a versioned message schema, payload ceiling and delivery semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/message_contract.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_message_contract_init(UmiFabricMessageContract *item, const char *contract_id, const char *schema_id, UmiFabricVersion version, uint64_t max_payload_bytes, UmiFabricDeliveryMode delivery_mode) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->contract_id,sizeof(item->contract_id),contract_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->schema_id,sizeof(item->schema_id),schema_id);if(s!=UMI_STATUS_OK)return s;item->version=version;item->max_payload_bytes=max_payload_bytes;item->delivery_mode=delivery_mode;
    return umi_fabric_message_contract_validate(item);
}
UmiStatus umi_fabric_message_contract_validate(const UmiFabricMessageContract *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->contract_id[0]!='\0' && item->schema_id[0]!='\0' && item->version.major>0U && item->max_payload_bytes>0U && item->delivery_mode>=UMI_FABRIC_DELIVERY_AT_MOST_ONCE && item->delivery_mode<=UMI_FABRIC_DELIVERY_IDEMPOTENT_EFFECT)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
