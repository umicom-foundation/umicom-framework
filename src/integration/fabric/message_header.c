/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/message_header.c
 *
 * PURPOSE:
 *   Represent immutable message identity, correlation and tenant metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/message_header.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_message_header_init(UmiFabricMessageHeader *item, const char *message_id, const char *correlation_id, const char *tenant_id, const char *content_type, uint64_t created_ms) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->message_id,sizeof(item->message_id),message_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->correlation_id,sizeof(item->correlation_id),correlation_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->tenant_id,sizeof(item->tenant_id),tenant_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->content_type,sizeof(item->content_type),content_type);if(s!=UMI_STATUS_OK)return s;item->created_ms=created_ms;
    return umi_fabric_message_header_validate(item);
}
UmiStatus umi_fabric_message_header_validate(const UmiFabricMessageHeader *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->message_id[0]!='\0' && item->correlation_id[0]!='\0' && item->content_type[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
