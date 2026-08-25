/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_audit_event.c
 *
 * PURPOSE:
 *   Record immutable data-operation audit evidence without storing application payloads.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_audit_event.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_data_audit_event_init(UmiDataAuditEvent *item, const char *event_id, const char *operation_id, const char *principal_id, const char *action, uint64_t timestamp, UmiStatus outcome) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->event_id,sizeof(item->event_id),event_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->principal_id,sizeof(item->principal_id),principal_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->action,sizeof(item->action),action);if(s!=UMI_STATUS_OK)return s;item->timestamp=timestamp;item->outcome=outcome;
    return umi_data_data_audit_event_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_data_audit_event_validate(const UmiDataAuditEvent *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->event_id[0] != '\0' && item->operation_id[0] != '\0' && item->principal_id[0] != '\0' && item->action[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
