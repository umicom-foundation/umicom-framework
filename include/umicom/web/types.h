/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/types.h
 *
 * PURPOSE:
 *   Define stable HTTP, routing, server and frontend-neutral web types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These small enums and fixed capacities are shared by the web modules. Keeping them toolkit neutral lets the same web core serve Studio, Trader, TMS and future applications.
 */

#ifndef UMICOM_WEB_TYPES_H
#define UMICOM_WEB_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WEB_METHOD_CAPACITY 16U
#define UMI_WEB_PATH_CAPACITY 512U
#define UMI_WEB_QUERY_CAPACITY 512U
#define UMI_WEB_HEADER_NAME_CAPACITY 96U
#define UMI_WEB_HEADER_VALUE_CAPACITY 512U
#define UMI_WEB_BODY_CAPACITY 16384U
#define UMI_WEB_MAX_HEADERS 64U
#define UMI_WEB_MAX_QUERY_ITEMS 64U
#define UMI_WEB_MAX_ROUTES 128U
#define UMI_WEB_MAX_MIDDLEWARE 32U
#define UMI_WEB_MAX_SESSIONS 128U
#define UMI_WEB_MAX_ORIGINS 64U
#define UMI_WEB_MAX_ENDPOINTS 128U

typedef enum UmiHttpMethod {
    UMI_HTTP_METHOD_UNKNOWN = 0,
    UMI_HTTP_METHOD_GET = 1,
    UMI_HTTP_METHOD_POST = 2,
    UMI_HTTP_METHOD_PUT = 3,
    UMI_HTTP_METHOD_PATCH = 4,
    UMI_HTTP_METHOD_DELETE = 5,
    UMI_HTTP_METHOD_OPTIONS = 6,
    UMI_HTTP_METHOD_HEAD = 7
} UmiHttpMethod;

typedef enum UmiHttpStatus {
    UMI_HTTP_STATUS_OK = 200,
    UMI_HTTP_STATUS_CREATED = 201,
    UMI_HTTP_STATUS_NO_CONTENT = 204,
    UMI_HTTP_STATUS_BAD_REQUEST = 400,
    UMI_HTTP_STATUS_UNAUTHORIZED = 401,
    UMI_HTTP_STATUS_FORBIDDEN = 403,
    UMI_HTTP_STATUS_NOT_FOUND = 404,
    UMI_HTTP_STATUS_METHOD_NOT_ALLOWED = 405,
    UMI_HTTP_STATUS_CONFLICT = 409,
    UMI_HTTP_STATUS_PAYLOAD_TOO_LARGE = 413,
    UMI_HTTP_STATUS_INTERNAL_ERROR = 500,
    UMI_HTTP_STATUS_SERVICE_UNAVAILABLE = 503
} UmiHttpStatus;

typedef enum UmiWebServerPhase {
    UMI_WEB_SERVER_STOPPED = 0,
    UMI_WEB_SERVER_STARTING = 1,
    UMI_WEB_SERVER_READY = 2,
    UMI_WEB_SERVER_STOPPING = 3,
    UMI_WEB_SERVER_FAILED = 4
} UmiWebServerPhase;

UmiHttpMethod umi_http_method_from_text(const char *text);
const char *umi_http_method_text(UmiHttpMethod method);
const char *umi_http_status_reason(int status);
UmiStatus umi_web_copy_text(char *destination, size_t capacity, const char *source);

#ifdef __cplusplus
}
#endif

#endif
