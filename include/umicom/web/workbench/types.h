/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/types.h
 *
 * PURPOSE:
 *   Define bounded, toolkit-neutral types shared by the HTTP/API/Web/Cloud
 *   developer workbench. Existing web and integration engines remain the
 *   transport and contract authorities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_TYPES_H
#define UMICOM_WEB_WORKBENCH_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/web/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WEB_WORKBENCH_API_VERSION 1U
#define UMI_WEB_WORKBENCH_ID_CAPACITY 96U
#define UMI_WEB_WORKBENCH_NAME_CAPACITY 160U
#define UMI_WEB_WORKBENCH_TEXT_CAPACITY 512U
#define UMI_WEB_WORKBENCH_URL_CAPACITY 1024U
#define UMI_WEB_WORKBENCH_PAYLOAD_CAPACITY UMI_WEB_BODY_CAPACITY
#define UMI_WEB_WORKBENCH_MAX_ENVIRONMENT_VALUES 64U
#define UMI_WEB_WORKBENCH_MAX_AUTH_PROFILES 32U
#define UMI_WEB_WORKBENCH_MAX_REQUESTS 64U
#define UMI_WEB_WORKBENCH_MAX_HISTORY 128U
#define UMI_WEB_WORKBENCH_MAX_OPERATIONS 128U
#define UMI_WEB_WORKBENCH_MAX_STREAMS 32U
#define UMI_WEB_WORKBENCH_MAX_STREAM_MESSAGES 128U
#define UMI_WEB_WORKBENCH_MAX_CLOUD_ITEMS 128U
#define UMI_WEB_WORKBENCH_MAX_COMMANDS 48U

/**
 * List the named web workbench phase values accepted by this public contract.
 */
typedef enum UmiWebWorkbenchPhase {
    UMI_WEB_WORKBENCH_IDLE = 0,
    UMI_WEB_WORKBENCH_PREPARING = 1,
    UMI_WEB_WORKBENCH_RUNNING = 2,
    UMI_WEB_WORKBENCH_COMPLETED = 3,
    UMI_WEB_WORKBENCH_CANCELLED = 4,
    UMI_WEB_WORKBENCH_FAILED = 5
} UmiWebWorkbenchPhase;

/**
 * List the named web workbench auth kind values accepted by this public contract.
 */
typedef enum UmiWebWorkbenchAuthKind {
    UMI_WEB_WORKBENCH_AUTH_NONE = 0,
    UMI_WEB_WORKBENCH_AUTH_BEARER = 1,
    UMI_WEB_WORKBENCH_AUTH_BASIC = 2,
    UMI_WEB_WORKBENCH_AUTH_API_KEY_HEADER = 3,
    UMI_WEB_WORKBENCH_AUTH_API_KEY_QUERY = 4,
    UMI_WEB_WORKBENCH_AUTH_OAUTH_ACCESS_TOKEN = 5
} UmiWebWorkbenchAuthKind;

/**
 * List the named web workbench body kind values accepted by this public contract.
 */
typedef enum UmiWebWorkbenchBodyKind {
    UMI_WEB_WORKBENCH_BODY_NONE = 0,
    UMI_WEB_WORKBENCH_BODY_TEXT = 1,
    UMI_WEB_WORKBENCH_BODY_JSON = 2,
    UMI_WEB_WORKBENCH_BODY_XML = 3,
    UMI_WEB_WORKBENCH_BODY_FORM = 4,
    UMI_WEB_WORKBENCH_BODY_BINARY_REFERENCE = 5
} UmiWebWorkbenchBodyKind;

/**
 * List the named web workbench stream kind values accepted by this public contract.
 */
typedef enum UmiWebWorkbenchStreamKind {
    UMI_WEB_WORKBENCH_STREAM_WEBSOCKET = 0,
    UMI_WEB_WORKBENCH_STREAM_SSE = 1
} UmiWebWorkbenchStreamKind;

/**
 * List the named web workbench message direction values accepted by this public contract.
 */
typedef enum UmiWebWorkbenchMessageDirection {
    UMI_WEB_WORKBENCH_MESSAGE_INBOUND = 0,
    UMI_WEB_WORKBENCH_MESSAGE_OUTBOUND = 1
} UmiWebWorkbenchMessageDirection;

/**
 * List the named web workbench cloud provider values accepted by this public contract.
 */
typedef enum UmiWebWorkbenchCloudProvider {
    UMI_WEB_WORKBENCH_CLOUD_GENERIC = 0,
    UMI_WEB_WORKBENCH_CLOUD_AWS = 1,
    UMI_WEB_WORKBENCH_CLOUD_AZURE = 2,
    UMI_WEB_WORKBENCH_CLOUD_GCP = 3,
    UMI_WEB_WORKBENCH_CLOUD_S3_COMPATIBLE = 4,
    UMI_WEB_WORKBENCH_CLOUD_LOCAL = 5
} UmiWebWorkbenchCloudProvider;

/**
 * Provide the web workbench copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source);
/**
 * Provide the web workbench phase text operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_phase_text(UmiWebWorkbenchPhase phase);
/**
 * Provide the web workbench auth kind text operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_auth_kind_text(UmiWebWorkbenchAuthKind kind);
/**
 * Provide the web workbench body kind text operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_body_kind_text(UmiWebWorkbenchBodyKind kind);
/**
 * Provide the web workbench stream kind text operation used by this module and its client
 * applications.
 */
const char *umi_web_workbench_stream_kind_text(UmiWebWorkbenchStreamKind kind);
/**
 * Provide the web workbench cloud provider text operation used by this module and its
 * client applications.
 */
const char *umi_web_workbench_cloud_provider_text(
    UmiWebWorkbenchCloudProvider provider);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_TYPES_H */
