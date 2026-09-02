/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/response.h
 *
 * PURPOSE:
 *   Capture one API response with timing, transfer, TLS and truncation evidence
 *   suitable for workbench inspection and bounded history summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_RESPONSE_H
#define UMICOM_WEB_WORKBENCH_RESPONSE_H

#include "umicom/web/header.h"
#include "umicom/web/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the web workbench timing data shared with callers of this public contract.
 */
typedef struct UmiWebWorkbenchTiming {
    uint64_t dns_us;
    uint64_t connect_us;
    uint64_t tls_us;
    uint64_t first_byte_us;
    uint64_t total_us;
} UmiWebWorkbenchTiming;

/**
 * Represent the web workbench response data shared with callers of this public contract.
 */
typedef struct UmiWebWorkbenchResponse {
    UmiWebWorkbenchPhase phase;
    UmiStatus status;
    int http_status;
    UmiWebHeader headers[UMI_WEB_MAX_HEADERS];
    size_t header_count;
    char body[UMI_WEB_WORKBENCH_PAYLOAD_CAPACITY];
    size_t body_length;
    size_t received_bytes;
    UmiWebWorkbenchTiming timing;
    bool tls_verified;
    bool body_truncated;
    char error_message[UMI_WEB_WORKBENCH_TEXT_CAPACITY];
    uint64_t revision;
} UmiWebWorkbenchResponse;

/**
 * Initialise web workbench response from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_response_init(UmiWebWorkbenchResponse *response);
/**
 * Provide the web workbench response set header operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_response_set_header(
    UmiWebWorkbenchResponse *response,
    const char *name,
    const char *value);
/**
 * Provide the web workbench response header operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_response_header(
    const UmiWebWorkbenchResponse *response,
    const char *name);
/**
 * Provide the web workbench response set body operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_response_set_body(
    UmiWebWorkbenchResponse *response,
    const void *body,
    size_t length,
    size_t received_bytes);
/**
 * Provide the web workbench response complete operation used by this module and its client
 * applications.
 */
void umi_web_workbench_response_complete(
    UmiWebWorkbenchResponse *response,
    UmiStatus status,
    int http_status,
    const UmiWebWorkbenchTiming *timing,
    bool tls_verified,
    const char *error_message);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_RESPONSE_H */
