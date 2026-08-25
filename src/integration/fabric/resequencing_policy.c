/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/resequencing_policy.c
 *
 * PURPOSE:
 *   Describe ordering window and gap handling for out-of-order integration messages.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/resequencing_policy.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_resequencing_policy_init(UmiFabricResequencingPolicy *item, const char *policy_id, size_t maximum_buffered, uint64_t gap_timeout_ms, bool release_on_timeout) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->policy_id,sizeof(item->policy_id),policy_id);if(s!=UMI_STATUS_OK)return s;item->maximum_buffered=maximum_buffered;item->gap_timeout_ms=gap_timeout_ms;item->release_on_timeout=release_on_timeout;
    return umi_fabric_resequencing_policy_validate(item);
}
UmiStatus umi_fabric_resequencing_policy_validate(const UmiFabricResequencingPolicy *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->policy_id[0]!='\0' && item->maximum_buffered>0U && item->gap_timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
