/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_response.c
 *
 * PURPOSE:
 *   Centralize complete JSON admission and current ComfyUI task ownership for native response adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "comfy_response_internal.h"
#include <string.h>
UmiStatus UmiComfyResponseOpen(const void *bytes, size_t length, size_t maximum,
    const UmiCancellationToken *cancel, UmiJsonTree **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL || length == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > maximum) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiJsonTreeLimits limits = {maximum, 65536U, 32U};
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, cancel, out);
    if (status == UMI_STATUS_OK) status = UmiJsonTreeValidateUniqueKeys(*out, 0, cancel);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(*out, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) { UmiJsonTreeDestroy(*out); *out = NULL; }
    return status;
}
UmiStatus UmiComfyResponseMember(const UmiJsonTree *tree, int object, const char *name, int *out)
{
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
}
UmiStatus UmiComfyResponseText(const UmiJsonTree *tree, int object, const char *name,
    char *out, size_t capacity, int allow_empty)
{
    int node = -1;
    UmiStatus status = UmiComfyResponseMember(tree, object, name, &node);
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING) return UMI_STATUS_PARSE_ERROR;
    status = UmiJsonTreeText(tree, node, out, capacity);
    if (status == UMI_STATUS_OK && !allow_empty && out[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    return status;
}
UmiStatus UmiComfyReceiptCheck(const UmiProviderTaskReceipt *receipt, uint64_t revision)
{
    UmiStatus status = UmiProviderTaskValidate(receipt);
    if (status != UMI_STATUS_OK) return status;
    if (receipt->revision != revision) return UMI_STATUS_BUSY;
    if (strcmp(receipt->provider_id, "comfyui") != 0 ||
        strcmp(receipt->context.operation_id, UMI_COMFY_TASK_OPERATION) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* All ComfyUI adapters use this origin contract before binding observations or requests. */
UmiStatus UmiComfyConnectionCheck(const UmiProviderConnection *connection)
{
    UmiStatus status = UmiProviderConnectionValidate(connection);
    if (status != UMI_STATUS_OK) return status;
    if (!connection->enabled || connection->route != UMI_PROVIDER_CONNECTION_LOOPBACK ||
        strcmp(connection->provider_id, "comfyui") != 0)
        return UMI_STATUS_PERMISSION_DENIED;
    /* Validation already proved the fixed literal origin and bounded port.
     * Refuse a saved path rather than guessing whether it is a reverse proxy. */
    if (strchr(connection->endpoint + sizeof("http://127.0.0.1:") - 1U, '/') != NULL)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_OK;
}