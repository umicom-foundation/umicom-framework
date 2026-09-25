/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/ibkr_adapter.c
 *
 * PURPOSE:
 *   Implement Interactive Brokers configuration and order/status mapping while
 *   keeping vendor SDK types outside Framework public contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/ibkr_adapter.h"

#include <stdio.h>
#include <string.h>

void umi_ibkr_adapter_config_init(UmiIbkrAdapterConfig *config)
{
    if (config == NULL) return;
    (void)memset(config, 0, sizeof(*config));
    (void)snprintf(config->host, sizeof(config->host), "%s", "127.0.0.1");
    config->port = 7497U;
    config->clientId = 17;
    config->paperOnly = 1;
}

UmiStatus umi_ibkr_adapter_config_validate(const UmiIbkrAdapterConfig *config)
{
    if (config == NULL || config->host[0] == '\0' ||
        config->port == 0U || config->clientId < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static const char *order_type(UmiOrderType type)
{
    switch (type) {
    case UMI_ORDER_MARKET: return "MKT";
    case UMI_ORDER_LIMIT: return "LMT";
    case UMI_ORDER_STOP: return "STP";
    case UMI_ORDER_STOP_LIMIT: return "STP LMT";
    default: return NULL;
    }
}

static const char *tif(UmiTimeInForce value)
{
    switch (value) {
    case UMI_TIF_DAY: return "DAY";
    case UMI_TIF_GTC: return "GTC";
    case UMI_TIF_IOC: return "IOC";
    case UMI_TIF_FOK: return "FOK";
    default: return NULL;
    }
}

UmiStatus umi_ibkr_adapter_map_order(
    const UmiOrderRequest *request,
    const UmiIbkrAdapterConfig *config,
    UmiIbkrOrderMessage *outMessage)
{
    const char *typeText;
    const char *tifText;

    if (request == NULL || outMessage == NULL ||
        umi_ibkr_adapter_config_validate(config) != UMI_STATUS_OK ||
        request->quantity <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (config->readOnly) return UMI_STATUS_PERMISSION_DENIED;
    if (config->paperOnly && request->environment == UMI_TRADING_LIVE) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    typeText = order_type(request->type);
    tifText = tif(request->tif);
    if (typeText == NULL || tifText == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outMessage, 0, sizeof(*outMessage));
    (void)snprintf(outMessage->action, sizeof(outMessage->action), "%s",
                   request->side == UMI_SIDE_BUY ? "BUY" : "SELL");
    (void)snprintf(outMessage->orderType, sizeof(outMessage->orderType), "%s",
                   typeText);
    (void)snprintf(outMessage->timeInForce, sizeof(outMessage->timeInForce), "%s",
                   tifText);
    outMessage->quantity = request->quantity;
    outMessage->limitPrice = request->limit_price;
    outMessage->stopPrice = request->stop_price;
    outMessage->transmit = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_ibkr_adapter_map_status(
    const char *providerStatus,
    UmiOrderStatus *outStatus)
{
    if (providerStatus == NULL || outStatus == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(providerStatus, "PendingSubmit") == 0 ||
        strcmp(providerStatus, "PreSubmitted") == 0) {
        *outStatus = UMI_ORDER_VALIDATED;
    } else if (strcmp(providerStatus, "Submitted") == 0) {
        *outStatus = UMI_ORDER_ACCEPTED;
    } else if (strcmp(providerStatus, "Filled") == 0) {
        *outStatus = UMI_ORDER_FILLED;
    } else if (strcmp(providerStatus, "Cancelled") == 0 ||
               strcmp(providerStatus, "ApiCancelled") == 0) {
        *outStatus = UMI_ORDER_CANCELLED;
    } else if (strcmp(providerStatus, "Inactive") == 0) {
        *outStatus = UMI_ORDER_REJECTED;
    } else {
        return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
