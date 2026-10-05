/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/loopback_access.h
 * PURPOSE: Admit explicitly addressed local browser requests before a development service exposes routes or selected files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_LOOPBACK_ACCESS_H
#define UMICOM_WEB_LOOPBACK_ACCESS_H
#include "umicom/web/service.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
    /* The host supplies its selected nonzero listening port. Keep this small
 * configuration alive and unchanged while its service is processing requests. */
    typedef struct UmiWebLoopbackAccess
    {
        uint16_t port;
    } UmiWebLoopbackAccess;
    /* Install as the service request gate, with UmiWebLoopbackAccess as context.
 * Accept exactly one Host naming 127.0.0.1 or localhost and this port. If Origin
 * is present it must name the same HTTP origin as Host; duplicate or opaque
 * origins are refused. Browser fetch metadata, when present, must be
 * same-origin or none. Responses to refusals use HTTP 403 and no CORS grant.
 * This reduces browser cross-origin and DNS-rebinding exposure. It is not
 * user authentication: other local processes can send these headers too.
 * The native listener must independently remain restricted to loopback. */
    UmiStatus UmiWebLoopbackRequestGate(const UmiWebRequest *request, UmiWebResponse *response,
                                        bool *accepted, void *context);
#ifdef __cplusplus
}
#endif
#endif
