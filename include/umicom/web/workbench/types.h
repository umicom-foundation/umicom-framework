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

typedef enum UmiWebWorkbenchPhase {
    UMI_WEB_WORKBENCH_IDLE = 0,
    UMI_WEB_WORKBENCH_PREPARING = 1,
    UMI_WEB_WORKBENCH_RUNNING = 2,
    UMI_WEB_WORKBENCH_COMPLETED = 3,
    UMI_WEB_WORKBENCH_CANCELLED = 4,
    UMI_WEB_WORKBENCH_FAILED = 5
} UmiWebWorkbenchPhase;

typedef enum UmiWebWorkbenchAuthKind {
    UMI_WEB_WORKBENCH_AUTH_NONE = 0,
    UMI_WEB_WORKBENCH_AUTH_BEARER = 1,
    UMI_WEB_WORKBENCH_AUTH_BASIC = 2,
    UMI_WEB_WORKBENCH_AUTH_API_KEY_HEADER = 3,
    UMI_WEB_WORKBENCH_AUTH_API_KEY_QUERY = 4,
    UMI_WEB_WORKBENCH_AUTH_OAUTH_ACCESS_TOKEN = 5
} UmiWebWorkbenchAuthKind;

typedef enum UmiWebWorkbenchBodyKind {
    UMI_WEB_WORKBENCH_BODY_NONE = 0,
    UMI_WEB_WORKBENCH_BODY_TEXT = 1,
    UMI_WEB_WORKBENCH_BODY_JSON = 2,
    UMI_WEB_WORKBENCH_BODY_XML = 3,
    UMI_WEB_WORKBENCH_BODY_FORM = 4,
    UMI_WEB_WORKBENCH_BODY_BINARY_REFERENCE = 5
} UmiWebWorkbenchBodyKind;

typedef enum UmiWebWorkbenchStreamKind {
    UMI_WEB_WORKBENCH_STREAM_WEBSOCKET = 0,
    UMI_WEB_WORKBENCH_STREAM_SSE = 1
} UmiWebWorkbenchStreamKind;

typedef enum UmiWebWorkbenchMessageDirection {
    UMI_WEB_WORKBENCH_MESSAGE_INBOUND = 0,
    UMI_WEB_WORKBENCH_MESSAGE_OUTBOUND = 1
} UmiWebWorkbenchMessageDirection;

typedef enum UmiWebWorkbenchCloudProvider {
    UMI_WEB_WORKBENCH_CLOUD_GENERIC = 0,
    UMI_WEB_WORKBENCH_CLOUD_AWS = 1,
    UMI_WEB_WORKBENCH_CLOUD_AZURE = 2,
    UMI_WEB_WORKBENCH_CLOUD_GCP = 3,
    UMI_WEB_WORKBENCH_CLOUD_S3_COMPATIBLE = 4,
    UMI_WEB_WORKBENCH_CLOUD_LOCAL = 5
} UmiWebWorkbenchCloudProvider;

UmiStatus umi_web_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source);
const char *umi_web_workbench_phase_text(UmiWebWorkbenchPhase phase);
const char *umi_web_workbench_auth_kind_text(UmiWebWorkbenchAuthKind kind);
const char *umi_web_workbench_body_kind_text(UmiWebWorkbenchBodyKind kind);
const char *umi_web_workbench_stream_kind_text(UmiWebWorkbenchStreamKind kind);
const char *umi_web_workbench_cloud_provider_text(
    UmiWebWorkbenchCloudProvider provider);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_TYPES_H */
