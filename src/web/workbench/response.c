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

/*
 * Provide the response header index operation used by this module and its client
 * applications.
 */
static size_t response_header_index(
    const UmiWebWorkbenchResponse *response,
    const char *name)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < response->header_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_web_header_name_equal(&response->headers[index], name)) return index;
    }
    return response->header_count;
}

/*
 * Initialise web workbench response from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_response_init(UmiWebWorkbenchResponse *response)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (response == NULL) return;
    memset(response, 0, sizeof(*response));
    response->phase = UMI_WEB_WORKBENCH_IDLE;
    response->status = UMI_STATUS_OK;
    response->revision = 1U;
}

/*
 * Provide the web workbench response set header operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_response_set_header(
    UmiWebWorkbenchResponse *response,
    const char *name,
    const char *value)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (response == NULL || name == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = response_header_index(response, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == response->header_count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index >= UMI_WEB_MAX_HEADERS) return UMI_STATUS_CAPACITY_EXCEEDED;
        response->header_count++;
    }
    status = umi_web_header_set(&response->headers[index], name, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index + 1U == response->header_count) response->header_count--;
        return status;
    }
    response->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench response header operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_response_header(
    const UmiWebWorkbenchResponse *response,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (response == NULL || name == NULL) return NULL;
    index = response_header_index(response, name);
    return index < response->header_count ? response->headers[index].value : NULL;
}

/*
 * Provide the web workbench response set body operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_response_set_body(
    UmiWebWorkbenchResponse *response,
    const void *body,
    size_t length,
    size_t received_bytes)
{
    size_t copied;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (response == NULL || (body == NULL && length > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    copied = length < sizeof(response->body) - 1U
        ? length : sizeof(response->body) - 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (copied > 0U) memcpy(response->body, body, copied);
    response->body[copied] = '\0';
    response->body_length = copied;
    response->received_bytes = received_bytes;
    response->body_truncated = length > copied || received_bytes > copied;
    response->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench response complete operation used by this module and its client
 * applications.
 */
void umi_web_workbench_response_complete(
    UmiWebWorkbenchResponse *response,
    UmiStatus status,
    int http_status,
    const UmiWebWorkbenchTiming *timing,
    bool tls_verified,
    const char *error_message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (response == NULL) return;
    response->status = status;
    response->http_status = http_status;
    response->tls_verified = tls_verified;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timing != NULL) response->timing = *timing;
    response->phase = status == UMI_STATUS_OK
        ? UMI_WEB_WORKBENCH_COMPLETED
        : (status == UMI_STATUS_CANCELLED
            ? UMI_WEB_WORKBENCH_CANCELLED : UMI_WEB_WORKBENCH_FAILED);
    (void)umi_web_workbench_copy_text(response->error_message,
        sizeof(response->error_message), error_message != NULL ? error_message : "");
    response->revision++;
}
