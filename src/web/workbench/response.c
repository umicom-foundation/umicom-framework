/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/response.c
 *
 * PURPOSE:
 *   Implement bounded API-response capture and completion evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/response.h"

#include <string.h>

static size_t response_header_index(
    const UmiWebWorkbenchResponse *response,
    const char *name)
{
    size_t index;
    for (index = 0U; index < response->header_count; ++index) {
        if (umi_web_header_name_equal(&response->headers[index], name)) return index;
    }
    return response->header_count;
}

void umi_web_workbench_response_init(UmiWebWorkbenchResponse *response)
{
    if (response == NULL) return;
    memset(response, 0, sizeof(*response));
    response->phase = UMI_WEB_WORKBENCH_IDLE;
    response->status = UMI_STATUS_OK;
    response->revision = 1U;
}

UmiStatus umi_web_workbench_response_set_header(
    UmiWebWorkbenchResponse *response,
    const char *name,
    const char *value)
{
    size_t index;
    UmiStatus status;
    if (response == NULL || name == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = response_header_index(response, name);
    if (index == response->header_count) {
        if (index >= UMI_WEB_MAX_HEADERS) return UMI_STATUS_CAPACITY_EXCEEDED;
        response->header_count++;
    }
    status = umi_web_header_set(&response->headers[index], name, value);
    if (status != UMI_STATUS_OK) {
        if (index + 1U == response->header_count) response->header_count--;
        return status;
    }
    response->revision++;
    return UMI_STATUS_OK;
}

const char *umi_web_workbench_response_header(
    const UmiWebWorkbenchResponse *response,
    const char *name)
{
    size_t index;
    if (response == NULL || name == NULL) return NULL;
    index = response_header_index(response, name);
    return index < response->header_count ? response->headers[index].value : NULL;
}

UmiStatus umi_web_workbench_response_set_body(
    UmiWebWorkbenchResponse *response,
    const void *body,
    size_t length,
    size_t received_bytes)
{
    size_t copied;
    if (response == NULL || (body == NULL && length > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    copied = length < sizeof(response->body) - 1U
        ? length : sizeof(response->body) - 1U;
    if (copied > 0U) memcpy(response->body, body, copied);
    response->body[copied] = '\0';
    response->body_length = copied;
    response->received_bytes = received_bytes;
    response->body_truncated = length > copied || received_bytes > copied;
    response->revision++;
    return UMI_STATUS_OK;
}

void umi_web_workbench_response_complete(
    UmiWebWorkbenchResponse *response,
    UmiStatus status,
    int http_status,
    const UmiWebWorkbenchTiming *timing,
    bool tls_verified,
    const char *error_message)
{
    if (response == NULL) return;
    response->status = status;
    response->http_status = http_status;
    response->tls_verified = tls_verified;
    if (timing != NULL) response->timing = *timing;
    response->phase = status == UMI_STATUS_OK
        ? UMI_WEB_WORKBENCH_COMPLETED
        : (status == UMI_STATUS_CANCELLED
            ? UMI_WEB_WORKBENCH_CANCELLED : UMI_WEB_WORKBENCH_FAILED);
    (void)umi_web_workbench_copy_text(response->error_message,
        sizeof(response->error_message), error_message != NULL ? error_message : "");
    response->revision++;
}
