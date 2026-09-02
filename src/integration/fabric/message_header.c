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

/*
 * Initialise fabric message header from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_message_header_init(UmiFabricMessageHeader *item, const char *message_id, const char *correlation_id, const char *tenant_id, const char *content_type, uint64_t created_ms) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->message_id,sizeof(item->message_id),message_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->correlation_id,sizeof(item->correlation_id),correlation_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->tenant_id,sizeof(item->tenant_id),tenant_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->content_type,sizeof(item->content_type),content_type);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->created_ms=created_ms;
    return umi_fabric_message_header_validate(item);
}
/*
 * Check that fabric message header satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_message_header_validate(const UmiFabricMessageHeader *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->message_id[0]!='\0' && item->correlation_id[0]!='\0' && item->content_type[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
