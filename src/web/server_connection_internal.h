/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/server_connection_internal.h
 * PURPOSE: Keep native socket exchange details outside the public web service and routing contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_SERVER_CONNECTION_INTERNAL_H
#define UMICOM_WEB_SERVER_CONNECTION_INTERNAL_H
#include "umicom/web/server.h"
#include "umicom/web/connection.h"
UmiStatus WebNativeServeNext(UmiWebListener *listener, UmiWebService *service, size_t maximum,
                             uint32_t wait_ms, uint32_t exchange_ms, const UmiCancellationToken *cancel,
                             UmiWebExchangeResult *out);
#endif
