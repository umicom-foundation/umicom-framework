/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/request.h
 *
 * PURPOSE:
 *   Model editable REST/API requests with bounded headers, bodies, TLS policy,
 *   timeout and optimistic revision tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_REQUEST_H
#define UMICOM_WEB_WORKBENCH_REQUEST_H

#include "umicom/web/header.h"
#include "umicom/web/url.h"
#include "umicom/web/workbench/environment.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the web workbench request data shared with callers of this public contract.
 */
typedef struct UmiWebWorkbenchRequest {
    char request_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char folder[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    UmiHttpMethod method;
    char url[UMI_WEB_WORKBENCH_URL_CAPACITY];
    UmiWebHeader headers[UMI_WEB_MAX_HEADERS];
    size_t header_count;
    UmiWebWorkbenchBodyKind body_kind;
    char body[UMI_WEB_WORKBENCH_PAYLOAD_CAPACITY];
    size_t body_length;
    char auth_profile_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    uint32_t timeout_ms;
    bool follow_redirects;
    bool verify_tls;
    bool enabled;
    uint64_t revision;
} UmiWebWorkbenchRequest;

/**
 * Initialise web workbench request from caller-provided values so later operations receive
 * a known state.
 */
void umi_web_workbench_request_init(
    UmiWebWorkbenchRequest *request,
    const char *request_id,
    const char *name,
    UmiHttpMethod method,
    const char *url);
/**
 * Provide the web workbench request set header operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_request_set_header(
    UmiWebWorkbenchRequest *request,
    const char *name,
    const char *value);
/**
 * Provide the web workbench request remove header operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_request_remove_header(
    UmiWebWorkbenchRequest *request,
    const char *name);
/**
 * Provide the web workbench request header operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_request_header(
    const UmiWebWorkbenchRequest *request,
    const char *name);
/**
 * Provide the web workbench request set body operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_request_set_body(
    UmiWebWorkbenchRequest *request,
    UmiWebWorkbenchBodyKind kind,
    const char *body);
/**
 * Check that web workbench request satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_web_workbench_request_validate(
    const UmiWebWorkbenchRequest *request,
    char *out_message,
    size_t message_capacity);
/**
 * Provide the web workbench request resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_request_resolve(
    const UmiWebWorkbenchRequest *request,
    const UmiWebWorkbenchEnvironment *environment,
    UmiWebWorkbenchRequest *out_request,
    size_t *out_unresolved_count);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_REQUEST_H */
