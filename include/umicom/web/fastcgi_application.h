/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/fastcgi_application.h
 *
 * PURPOSE:
 *   Connect one configured application entry point to a verified Framework web response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_FASTCGI_APPLICATION_H
#define UMICOM_WEB_FASTCGI_APPLICATION_H

#include "umicom/web/fastcgi_response.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebFastCgiApplication {
    const char *scriptFilename;
    const char *scriptName;
    const char *serverName;
    uint16_t serverPort;
    uint16_t requestId;
    size_t replyLimit;
} UmiWebFastCgiApplication;

/* The host supplies an immutable deployment record and a connected worker
 * transport for this exchange. The native absolute scriptFilename is a server
 * configuration value. The parsed request must match scriptName exactly.
 * Use UmiWebRequestParseStrict at the HTTP admission boundary. The adapter
 * builds an exact CGI environment from admitted request data and trusted
 * server configuration; optional Content-Type is the only forwarded input
 * header in the current profile. HEAD is outside this response profile.
 *
 * Worker ownership remains with the caller. No worker is launched, and this
 * function does not open, discover or derive script paths from the request.
 * Keep configuration and request storage immutable during the synchronous
 * call. *outBody must start NULL. All failures preserve the response and body
 * slot. A successful body is independently owned by the caller.
 */
UmiStatus UmiWebFastCgiApplicationHandle(const UmiWebFastCgiApplication *application,
    const UmiWebRequest *request, const UmiWebConnectionIo *worker, const UmiCancellationToken *cancel,
    UmiWebResponse *response, UmiWebResponseBody **outBody);

/** A successful open transfers one fresh worker connection to the route.
 * On failure the connector retains ownership of any resources it acquired.
 * Connection settings belong to the connector context and are not supplied by
 * the HTTP request. Bound the waits in all callbacks and honor cancellation.
 * Close receives every successfully opened connection, including those with
 * invalid I/O callbacks or a later exchange failure. */
typedef struct UmiWebFastCgiConnector {
    void *context;
    UmiStatus (*open)(void *context, const UmiCancellationToken *cancel, UmiWebConnectionIo *out);
    void (*close)(void *context, UmiWebConnectionIo *connection);
} UmiWebFastCgiConnector;

typedef struct UmiWebFastCgiRoute {
    UmiWebFastCgiApplication application;
    UmiWebFastCgiConnector connector;
    const UmiCancellationToken *cancel;
} UmiWebFastCgiRoute;

/** Body fallback for UmiWebServiceSetBodyFallback. Context points to the
 * trusted route and stays valid for registered requests. Each admitted
 * exchange opens at most one worker and returns its lease before returning.
 * Unmatched routes return HTTP 404 and unsupported methods return HTTP 501
 * without acquiring a worker. Genuine worker failures retain their status.
 * Keep configuration stable for the synchronous call. The connector host
 * manages its own synchronization if several requests share a context. */
UmiStatus UmiWebFastCgiRouteHandle(const UmiWebRequest *request, UmiWebResponse *response,
    UmiWebResponseBody **outBody, void *context);

#ifdef __cplusplus
}
#endif
#endif
