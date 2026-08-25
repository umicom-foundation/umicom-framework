/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/dead_letter_policy.c
 *
 * PURPOSE:
 *   Describe bounded dead-letter escalation and retention rules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/dead_letter_policy.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_dead_letter_policy_init(UmiFabricDeadLetterPolicy *item, const char *policy_id, const char *destination, uint32_t after_attempts, uint64_t retention_ms, bool include_payload) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->policy_id,sizeof(item->policy_id),policy_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->destination,sizeof(item->destination),destination);if(s!=UMI_STATUS_OK)return s;item->after_attempts=after_attempts;item->retention_ms=retention_ms;item->include_payload=include_payload;
    return umi_fabric_dead_letter_policy_validate(item);
}
UmiStatus umi_fabric_dead_letter_policy_validate(const UmiFabricDeadLetterPolicy *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->policy_id[0]!='\0' && item->destination[0]!='\0' && item->after_attempts>0U && item->retention_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
