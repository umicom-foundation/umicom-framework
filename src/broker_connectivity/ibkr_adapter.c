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
#include "umicom/broker_connectivity/order_observation.h"
#include "../base/value_archive_internal.h"

#include <math.h>
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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->host, '\0', sizeof(config->host)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->account, '\0', sizeof(config->account)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (config == NULL || config->host[0] == '\0' ||
        config->port == 0U || config->clientId < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Fixed buffers must terminate before a later adapter uses them. The
     * boolean policy values are exact flags, not arbitrary integers. */
    if (memchr(config->host, 0, sizeof config->host) == NULL ||
        memchr(config->account, 0, sizeof config->account) == NULL ||
        (config->paperOnly != 0 && config->paperOnly != 1) ||
        (config->readOnly != 0 && config->readOnly != 1)) {
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
    /* Validate the whole candidate before touching caller-owned output.
     * NaN defeats ordinary comparisons, and an invalid side must never be
     * translated into SELL. This is validation only, not trading authority. */
    if (!isfinite(request->quantity) || !isfinite(request->limit_price) ||
        !isfinite(request->stop_price) ||
        (request->side != UMI_SIDE_BUY && request->side != UMI_SIDE_SELL) ||
        (request->environment != UMI_TRADING_SIMULATION &&
         request->environment != UMI_TRADING_PAPER &&
         request->environment != UMI_TRADING_LIVE) ||
        ((request->type == UMI_ORDER_LIMIT || request->type == UMI_ORDER_STOP_LIMIT) &&
         request->limit_price <= 0.0) ||
        ((request->type == UMI_ORDER_STOP || request->type == UMI_ORDER_STOP_LIMIT) &&
         request->stop_price <= 0.0)) {
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

/* Preserve the earlier status translation for review. It treated Inactive as
 * rejected, although this status alone does not establish an order rejection.
 * The shared phase classifier below keeps ambiguous and pending states out of
 * terminal local transitions; use the observation API when quantities exist. */
#if 0
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


#endif
UmiStatus umi_ibkr_adapter_map_status(const char *providerStatus, UmiOrderStatus *outStatus)
{
    if (outStatus == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrOrderPhase phase;
    UmiStatus status = UmiIbkrClassifyOrderPhase(providerStatus, &phase);
    if (status != UMI_STATUS_OK) return status;
    UmiOrderStatus candidate;
    switch (phase) {
    case UMI_IBKR_ORDER_PENDING_SUBMIT:
    case UMI_IBKR_ORDER_PRE_SUBMITTED: candidate = UMI_ORDER_VALIDATED; break;
    case UMI_IBKR_ORDER_WORKING: candidate = UMI_ORDER_ACCEPTED; break;
    case UMI_IBKR_ORDER_FILLED: candidate = UMI_ORDER_FILLED; break;
    case UMI_IBKR_ORDER_CANCELLED: candidate = UMI_ORDER_CANCELLED; break;
    case UMI_IBKR_ORDER_UNRECOGNIZED: return UMI_STATUS_NOT_FOUND;
    default: return UMI_STATUS_UNAVAILABLE;
    }
    *outStatus = candidate;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiIbkrAdapterConfigArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xce63d388927cd89d);
    schema = (schema ^ (uint64_t)sizeof(((UmiIbkrAdapterConfig *)0)->host)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiIbkrAdapterConfig *)0)->account)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiIbkrAdapterConfigArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiIbkrAdapterConfig *)0)->host) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiIbkrAdapterConfig *)0)->account) - 1U +
        8U +
        8U;
}
static void UmiIbkrAdapterConfigArchiveWrite(UmiArchiveWriter *writer, const UmiIbkrAdapterConfig *value)
{
    UmiArchiveWriteText(writer, value->host, sizeof(value->host));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->port);
    UmiArchiveWriteSigned(writer, (int64_t)value->clientId);
    UmiArchiveWriteText(writer, value->account, sizeof(value->account));
    UmiArchiveWriteSigned(writer, (int64_t)value->paperOnly);
    UmiArchiveWriteSigned(writer, (int64_t)value->readOnly);
}
static void UmiIbkrAdapterConfigArchiveRead(UmiArchiveReader *reader, UmiIbkrAdapterConfig *value)
{
    UmiArchiveReadText(reader, value->host, sizeof(value->host));
    value->port = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->clientId = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->account, sizeof(value->account));
    value->paperOnly = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->readOnly = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiIbkrAdapterConfigArchiveValidate(const UmiIbkrAdapterConfig *value)
{
    return umi_ibkr_adapter_config_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ibkr_adapter_config_archive_encode, umi_ibkr_adapter_config_archive_decode,
    UmiIbkrAdapterConfig, UmiIbkrAdapterConfigArchiveSchema, UmiIbkrAdapterConfigArchiveBound, UmiIbkrAdapterConfigArchiveWrite, UmiIbkrAdapterConfigArchiveRead, UmiIbkrAdapterConfigArchiveValidate)
