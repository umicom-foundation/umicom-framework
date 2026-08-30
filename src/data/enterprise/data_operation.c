/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_operation.c
 *
 * PURPOSE:
 *   Describe one reviewable Data Server operation for queueing, audit and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_operation.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_data_operation_init(UmiDataOperation *item, const char *operation_id, const char *session_id, const char *operation_kind, uint64_t submitted_at, uint32_t priority) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->session_id,sizeof(item->session_id),session_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->operation_kind,sizeof(item->operation_kind),operation_kind);if(s!=UMI_STATUS_OK)return s;item->submitted_at=submitted_at;item->priority=priority;item->cancellable=true;
    return umi_data_data_operation_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_data_operation_validate(const UmiDataOperation *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->operation_id[0] != '\0' && item->session_id[0] != '\0' && item->operation_kind[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
