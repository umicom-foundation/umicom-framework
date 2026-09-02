/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/types.c
 *
 * PURPOSE:
 *   Implement shared HTTP/API/Web/Cloud workbench text and enum helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/types.h"

#include <string.h>

/*
 * Provide the web workbench copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench phase text operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_phase_text(UmiWebWorkbenchPhase phase)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (phase) {
        case UMI_WEB_WORKBENCH_IDLE: return "idle";
        case UMI_WEB_WORKBENCH_PREPARING: return "preparing";
        case UMI_WEB_WORKBENCH_RUNNING: return "running";
        case UMI_WEB_WORKBENCH_COMPLETED: return "completed";
        case UMI_WEB_WORKBENCH_CANCELLED: return "cancelled";
        case UMI_WEB_WORKBENCH_FAILED: return "failed";
        default: return "unknown";
    }
}

/*
 * Provide the web workbench auth kind text operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_auth_kind_text(UmiWebWorkbenchAuthKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_WEB_WORKBENCH_AUTH_NONE: return "none";
        case UMI_WEB_WORKBENCH_AUTH_BEARER: return "bearer";
        case UMI_WEB_WORKBENCH_AUTH_BASIC: return "basic";
        case UMI_WEB_WORKBENCH_AUTH_API_KEY_HEADER: return "api-key-header";
        case UMI_WEB_WORKBENCH_AUTH_API_KEY_QUERY: return "api-key-query";
        case UMI_WEB_WORKBENCH_AUTH_OAUTH_ACCESS_TOKEN: return "oauth-access-token";
        default: return "unknown";
    }
}

/*
 * Provide the web workbench body kind text operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_body_kind_text(UmiWebWorkbenchBodyKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_WEB_WORKBENCH_BODY_NONE: return "none";
        case UMI_WEB_WORKBENCH_BODY_TEXT: return "text";
        case UMI_WEB_WORKBENCH_BODY_JSON: return "json";
        case UMI_WEB_WORKBENCH_BODY_XML: return "xml";
        case UMI_WEB_WORKBENCH_BODY_FORM: return "form";
        case UMI_WEB_WORKBENCH_BODY_BINARY_REFERENCE: return "binary-reference";
        default: return "unknown";
    }
}

/*
 * Provide the web workbench stream kind text operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_stream_kind_text(UmiWebWorkbenchStreamKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_WEB_WORKBENCH_STREAM_WEBSOCKET: return "websocket";
        case UMI_WEB_WORKBENCH_STREAM_SSE: return "sse";
        default: return "unknown";
    }
}

/*
 * Provide the web workbench cloud provider text operation used by this module and its
 * client applications.
 */
const char *umi_web_workbench_cloud_provider_text(
    UmiWebWorkbenchCloudProvider provider)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (provider) {
        case UMI_WEB_WORKBENCH_CLOUD_GENERIC: return "generic";
        case UMI_WEB_WORKBENCH_CLOUD_AWS: return "aws";
        case UMI_WEB_WORKBENCH_CLOUD_AZURE: return "azure";
        case UMI_WEB_WORKBENCH_CLOUD_GCP: return "gcp";
        case UMI_WEB_WORKBENCH_CLOUD_S3_COMPATIBLE: return "s3-compatible";
        case UMI_WEB_WORKBENCH_CLOUD_LOCAL: return "local";
        default: return "unknown";
    }
}
