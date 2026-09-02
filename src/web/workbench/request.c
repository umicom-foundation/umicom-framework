/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/request.c
 *
 * PURPOSE:
 *   Implement editable API requests, validation and public-variable expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/request.h"

#include <string.h>

/* Provide the header index operation used by this module and its client applications. */
static size_t header_index(
    const UmiWebWorkbenchRequest *request,
    const char *name)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->header_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_web_header_name_equal(&request->headers[index], name)) return index;
    }
    return request->header_count;
}

/* Provide the body content type operation used by this module and its client applications. */
static const char *body_content_type(UmiWebWorkbenchBodyKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_WEB_WORKBENCH_BODY_JSON: return "application/json";
        case UMI_WEB_WORKBENCH_BODY_XML: return "application/xml";
        case UMI_WEB_WORKBENCH_BODY_FORM: return "application/x-www-form-urlencoded";
        case UMI_WEB_WORKBENCH_BODY_TEXT: return "text/plain";
        default: return NULL;
    }
}

/*
 * Initialise web workbench request from caller-provided values so later operations receive
 * a known state.
 */
void umi_web_workbench_request_init(
    UmiWebWorkbenchRequest *request,
    const char *request_id,
    const char *name,
    UmiHttpMethod method,
    const char *url)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    memset(request, 0, sizeof(*request));
    (void)umi_web_workbench_copy_text(request->request_id,
        sizeof(request->request_id), request_id != NULL ? request_id : "request");
    (void)umi_web_workbench_copy_text(request->name,
        sizeof(request->name), name != NULL ? name : "Request");
    (void)umi_web_workbench_copy_text(request->url,
        sizeof(request->url), url != NULL ? url : "https://localhost/");
    request->method = method;
    request->body_kind = UMI_WEB_WORKBENCH_BODY_NONE;
    request->timeout_ms = 30000U;
    request->follow_redirects = true;
    request->verify_tls = true;
    request->enabled = true;
    request->revision = 1U;
}

/*
 * Provide the web workbench request set header operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_request_set_header(
    UmiWebWorkbenchRequest *request,
    const char *name,
    const char *value)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || name == NULL || value == NULL || name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = header_index(request, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == request->header_count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index >= UMI_WEB_MAX_HEADERS) return UMI_STATUS_CAPACITY_EXCEEDED;
        request->header_count++;
    }
    status = umi_web_header_set(&request->headers[index], name, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index + 1U == request->header_count) request->header_count--;
        return status;
    }
    request->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench request remove header operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_request_remove_header(
    UmiWebWorkbenchRequest *request,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = header_index(request, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == request->header_count) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < request->header_count) {
        memmove(&request->headers[index], &request->headers[index + 1U],
            (request->header_count - index - 1U) * sizeof(request->headers[0]));
    }
    request->header_count--;
    memset(&request->headers[request->header_count], 0, sizeof(request->headers[0]));
    request->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench request header operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_request_header(
    const UmiWebWorkbenchRequest *request,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || name == NULL) return NULL;
    index = header_index(request, name);
    return index < request->header_count ? request->headers[index].value : NULL;
}

/*
 * Provide the web workbench request set body operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_request_set_body(
    UmiWebWorkbenchRequest *request,
    UmiWebWorkbenchBodyKind kind,
    const char *body)
{
    size_t length;
    UmiStatus status;
    const char *content_type;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || body == NULL || kind > UMI_WEB_WORKBENCH_BODY_BINARY_REFERENCE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(body);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(request->body)) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(request->body, body, length + 1U);
    request->body_length = length;
    request->body_kind = kind;
    content_type = body_content_type(kind);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (content_type != NULL && umi_web_workbench_request_header(request, "Content-Type") == NULL) {
        status = umi_web_workbench_request_set_header(request, "Content-Type", content_type);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    request->revision++;
    return UMI_STATUS_OK;
}

/*
 * Check that web workbench request satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_web_workbench_request_validate(
    const UmiWebWorkbenchRequest *request,
    char *out_message,
    size_t message_capacity)
{
    UmiWebUrl parsed;
    UmiStatus status = UMI_STATUS_OK;
    const char *message = "Request is valid.";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (request->request_id[0] == '\0' || request->name[0] == '\0' ||
        request->method == UMI_HTTP_METHOD_UNKNOWN) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Request identity, name or method is invalid.";
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_web_url_parse(request->url, &parsed) != UMI_STATUS_OK ||
               (strcmp(parsed.scheme, "http") != 0 && strcmp(parsed.scheme, "https") != 0)) {
        status = UMI_STATUS_PARSE_ERROR;
        message = "Request URL must be an absolute HTTP or HTTPS URL.";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (request->timeout_ms == 0U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Request timeout must be greater than zero.";
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(parsed.scheme, "https") == 0 && !request->verify_tls) {
        message = "Request is valid, but TLS verification is disabled.";
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) {
        UmiStatus copy_status = umi_web_workbench_copy_text(
            out_message, message_capacity, message);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (copy_status != UMI_STATUS_OK && status == UMI_STATUS_OK) return copy_status;
    }
    return status;
}

/*
 * Provide the web workbench request resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_request_resolve(
    const UmiWebWorkbenchRequest *request,
    const UmiWebWorkbenchEnvironment *environment,
    UmiWebWorkbenchRequest *out_request,
    size_t *out_unresolved_count)
{
    size_t unresolved = 0U;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || out_request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_request = *request;
    status = umi_web_workbench_environment_resolve(environment, request->url,
        out_request->url, sizeof(out_request->url), &unresolved);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->header_count; ++index) {
        size_t item_unresolved = 0U;
        status = umi_web_workbench_environment_resolve(environment,
            request->headers[index].value, out_request->headers[index].value,
            sizeof(out_request->headers[index].value), &item_unresolved);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        unresolved += item_unresolved;
    }
    {
        size_t body_unresolved = 0U;
        status = umi_web_workbench_environment_resolve(environment, request->body,
            out_request->body, sizeof(out_request->body), &body_unresolved);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        unresolved += body_unresolved;
        out_request->body_length = strlen(out_request->body);
    }
    out_request->revision++;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_unresolved_count != NULL) *out_unresolved_count = unresolved;
    return UMI_STATUS_OK;
}
