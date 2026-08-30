/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/transport_profile.c
 *
 * PURPOSE:
 *   Describe bounded transport limits, heartbeat policy and security requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transport_profile.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_transport_profile_init(UmiFabricTransportProfile *item, const char *profile_id, uint64_t max_frame_bytes, uint32_t heartbeat_ms, uint32_t idle_timeout_ms, bool compression_allowed, bool tls_required) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->profile_id,sizeof(item->profile_id),profile_id);if(s!=UMI_STATUS_OK)return s;item->max_frame_bytes=max_frame_bytes;item->heartbeat_ms=heartbeat_ms;item->idle_timeout_ms=idle_timeout_ms;item->compression_allowed=compression_allowed;item->tls_required=tls_required;
    return umi_fabric_transport_profile_validate(item);
}
UmiStatus umi_fabric_transport_profile_validate(const UmiFabricTransportProfile *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->profile_id[0]!='\0' && item->max_frame_bytes>0U && item->heartbeat_ms>0U && item->idle_timeout_ms>=item->heartbeat_ms)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
