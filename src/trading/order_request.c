/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_request.c
 *
 * PURPOSE:
 *   Validate canonical order requests before risk evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of order request. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_request.h"
#include "../base/value_archive_internal.h"
#include <math.h>
#include "umicom/finance/identifier.h"
#include "umicom/trading/instrument.h"

/* Check that order request satisfies its contract before another service relies on it. */
UmiStatus umi_order_request_validate(const UmiOrderRequest *request)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->client_order_id.value, '\0', sizeof(request->client_order_id.value)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->account_id.value, '\0', sizeof(request->account_id.value)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->instrument.instrument_id.value, '\0', sizeof(request->instrument.instrument_id.value)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->instrument.symbol, '\0', sizeof(request->instrument.symbol)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->instrument.venue, '\0', sizeof(request->instrument.venue)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->instrument.currency.code, '\0', sizeof(request->instrument.currency.code)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL ||
        !umi_financial_id_valid(&request->client_order_id) ||
        !umi_financial_id_valid(&request->account_id) ||
        !umi_instrument_valid(&request->instrument) ||
        !isfinite(request->quantity) || request->quantity <= 0.0 ||
        !isfinite(request->limit_price) || !isfinite(request->stop_price)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Validate explicit declared values, not enum ranges or truthiness: SELL
     * is -1. This recognises the environment value only; it grants no trading
     * permission, authentication, approval or live-order arming authority.
     */
    if ((request->side != UMI_SIDE_BUY && request->side != UMI_SIDE_SELL) ||
        (request->type != UMI_ORDER_MARKET && request->type != UMI_ORDER_LIMIT &&
         request->type != UMI_ORDER_STOP && request->type != UMI_ORDER_STOP_LIMIT) ||
        (request->tif != UMI_TIF_DAY && request->tif != UMI_TIF_GTC &&
         request->tif != UMI_TIF_IOC && request->tif != UMI_TIF_FOK) ||
        (request->environment != UMI_TRADING_SIMULATION &&
         request->environment != UMI_TRADING_PAPER &&
         request->environment != UMI_TRADING_LIVE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Both price fields must be finite even when inactive. Preserve the
     * established positivity requirements below: limit and stop prices are
     * required only for the order types that use them.
     */
    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->type == UMI_ORDER_LIMIT ||
         request->type == UMI_ORDER_STOP_LIMIT) &&
        request->limit_price <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->type == UMI_ORDER_STOP ||
         request->type == UMI_ORDER_STOP_LIMIT) &&
        request->stop_price <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiOrderRequestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7b02a1f4c3e5b730);
    schema = (schema ^ (uint64_t)sizeof(((UmiOrderRequest *)0)->client_order_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiOrderRequest *)0)->account_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiOrderRequest *)0)->instrument.instrument_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiOrderRequest *)0)->instrument.symbol)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiOrderRequest *)0)->instrument.venue)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiOrderRequest *)0)->instrument.currency.code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiOrderRequestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiOrderRequest *)0)->client_order_id.value) - 1U +
        8U + sizeof(((UmiOrderRequest *)0)->account_id.value) - 1U +
        8U + sizeof(((UmiOrderRequest *)0)->instrument.instrument_id.value) - 1U +
        8U + sizeof(((UmiOrderRequest *)0)->instrument.symbol) - 1U +
        8U + sizeof(((UmiOrderRequest *)0)->instrument.venue) - 1U +
        8U + sizeof(((UmiOrderRequest *)0)->instrument.currency.code) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiOrderRequestArchiveWrite(UmiArchiveWriter *writer, const UmiOrderRequest *value)
{
    UmiArchiveWriteText(writer, value->client_order_id.value, sizeof(value->client_order_id.value));
    UmiArchiveWriteText(writer, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveWriteText(writer, value->instrument.instrument_id.value, sizeof(value->instrument.instrument_id.value));
    UmiArchiveWriteText(writer, value->instrument.symbol, sizeof(value->instrument.symbol));
    UmiArchiveWriteText(writer, value->instrument.venue, sizeof(value->instrument.venue));
    UmiArchiveWriteText(writer, value->instrument.currency.code, sizeof(value->instrument.currency.code));
    UmiArchiveWriteDouble(writer, value->instrument.multiplier);
    UmiArchiveWriteSigned(writer, (int64_t)value->instrument.expiry_yyyymmdd);
    UmiArchiveWriteSigned(writer, (int64_t)value->side);
    UmiArchiveWriteSigned(writer, (int64_t)value->type);
    UmiArchiveWriteSigned(writer, (int64_t)value->tif);
    UmiArchiveWriteDouble(writer, value->quantity);
    UmiArchiveWriteDouble(writer, value->limit_price);
    UmiArchiveWriteDouble(writer, value->stop_price);
    UmiArchiveWriteSigned(writer, (int64_t)value->environment);
}
static void UmiOrderRequestArchiveRead(UmiArchiveReader *reader, UmiOrderRequest *value)
{
    UmiArchiveReadText(reader, value->client_order_id.value, sizeof(value->client_order_id.value));
    UmiArchiveReadText(reader, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveReadText(reader, value->instrument.instrument_id.value, sizeof(value->instrument.instrument_id.value));
    UmiArchiveReadText(reader, value->instrument.symbol, sizeof(value->instrument.symbol));
    UmiArchiveReadText(reader, value->instrument.venue, sizeof(value->instrument.venue));
    UmiArchiveReadText(reader, value->instrument.currency.code, sizeof(value->instrument.currency.code));
    value->instrument.multiplier = UmiArchiveReadDouble(reader);
    value->instrument.expiry_yyyymmdd = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->side = (UmiSide)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->type = (UmiOrderType)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->tif = (UmiTimeInForce)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->quantity = UmiArchiveReadDouble(reader);
    value->limit_price = UmiArchiveReadDouble(reader);
    value->stop_price = UmiArchiveReadDouble(reader);
    value->environment = (UmiTradingEnvironment)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiOrderRequestArchiveValidate(const UmiOrderRequest *value)
{
    return umi_order_request_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_order_request_archive_encode, umi_order_request_archive_decode,
    UmiOrderRequest, UmiOrderRequestArchiveSchema, UmiOrderRequestArchiveBound, UmiOrderRequestArchiveWrite, UmiOrderRequestArchiveRead, UmiOrderRequestArchiveValidate)
