/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/designer_types.c
 *
 * PURPOSE:
 *   Implement the designer types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Integration designer primitives | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/designer_types.h"
#include <string.h>

UmiStatus umi_integration_designer_copy(char *destination,size_t capacity,const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

const char *umi_integration_designer_transport_text(UmiIntegrationDesignerTransport transport)
{
    switch (transport) {
        case UMI_INTEGRATION_DESIGNER_REST: return "REST";
        case UMI_INTEGRATION_DESIGNER_GRAPHQL: return "GraphQL";
        case UMI_INTEGRATION_DESIGNER_WEBSOCKET: return "WebSocket";
        case UMI_INTEGRATION_DESIGNER_MESSAGE_BUS: return "Message Bus";
        default: return "Unknown";
    }
}

const char *umi_integration_designer_run_status_text(UmiIntegrationDesignerRunStatus status)
{
    switch (status) {
        case UMI_INTEGRATION_DESIGNER_RUN_PENDING: return "pending";
        case UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED: return "succeeded";
        case UMI_INTEGRATION_DESIGNER_RUN_FAILED: return "failed";
        case UMI_INTEGRATION_DESIGNER_RUN_CANCELLED: return "cancelled";
        default: return "unknown";
    }
}

UmiStatus umi_integration_designer_record_set(UmiIntegrationDesignerRecord *record,const char *key,const char *value)
{
    size_t index;
    UmiStatus status;
    if (record == NULL || key == NULL || value == NULL || key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < record->count; ++index) {
        if (strcmp(record->fields[index].key, key) == 0) {
            return umi_integration_designer_copy(record->fields[index].value,sizeof(record->fields[index].value),value);
        }
    }
    if (record->count >= UMI_INTEGRATION_DESIGNER_MAX_FIELDS) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_integration_designer_copy(record->fields[record->count].key,sizeof(record->fields[record->count].key),key);
    if (status != UMI_STATUS_OK) return status;
    status = umi_integration_designer_copy(record->fields[record->count].value,sizeof(record->fields[record->count].value),value);
    if (status == UMI_STATUS_OK) record->count += 1U;
    return status;
}

const char *umi_integration_designer_record_get(const UmiIntegrationDesignerRecord *record,const char *key)
{
    size_t index;
    if (record == NULL || key == NULL) return NULL;
    for (index = 0U; index < record->count; ++index) {
        if (strcmp(record->fields[index].key, key) == 0) return record->fields[index].value;
    }
    return NULL;
}

UmiStatus umi_integration_designer_validation_add(UmiIntegrationDesignerValidation *validation,const char *location,const char *message)
{
    UmiStatus status;
    if (validation == NULL || location == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (validation->count >= UMI_INTEGRATION_DESIGNER_MAX_ISSUES) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_integration_designer_copy(validation->issues[validation->count].location,sizeof(validation->issues[validation->count].location),location);
    if (status != UMI_STATUS_OK) return status;
    status = umi_integration_designer_copy(validation->issues[validation->count].message,sizeof(validation->issues[validation->count].message),message);
    if (status == UMI_STATUS_OK) validation->count += 1U;
    return status;
}
