/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/scanner.h
 * PURPOSE: Discover ranked contracts without treating scanner results as executable prices.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_SCANNER_H
#define UMICOM_BROKER_CONNECTIVITY_SCANNER_H
#include "umicom/broker_connectivity/connection.h"
#include "umicom/broker_connectivity/order_recovery.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_SCANNER_LIMIT 10U
#define UMI_IBKR_SCANNER_ROW_LIMIT 50U
#define UMI_IBKR_SCANNER_FILTER_LIMIT 16U
    typedef struct UmiIbkrScannerFilter
    {
        char tag[64], value[96];
    } UmiIbkrScannerFilter;
    typedef struct UmiIbkrScannerQuery
    {
        unsigned numberOfRows; /* 1..50; the broker may return fewer contracts. */
        char instrument[32], locationCode[96], scanCode[64];
        /* Empty numeric text means unset. Exact decimal text avoids locale-dependent
     * formatting; volume fields contain whole, nonnegative numbers. */
        char abovePrice[96], belowPrice[96], aboveVolume[32];
        char marketCapAbove[96], marketCapBelow[96];
        char moodyRatingAbove[32], moodyRatingBelow[32], spRatingAbove[32], spRatingBelow[32];
        char maturityDateAbove[9], maturityDateBelow[9];
        char couponRateAbove[96], couponRateBelow[96];
        bool excludeConvertible;
        char averageOptionVolumeAbove[32], scannerSettingPairs[256], stockTypeFilter[32];
        size_t filterCount;
        UmiIbkrScannerFilter filters[UMI_IBKR_SCANNER_FILTER_LIMIT];
    } UmiIbkrScannerQuery;
    typedef struct UmiIbkrScannerRow
    {
        unsigned rank;
        uint32_t contractId;
        char symbol[96], securityType[24], expiry[96], right[16];
        char exchange[64], currency[16], localSymbol[96], marketName[128], tradingClass[64];
        UmiIbkrOrderNumber strike;
        char distance[256], benchmark[256], projection[256], legs[512];
    } UmiIbkrScannerRow;
    typedef struct UmiIbkrScannerSnapshot
    {
        uint32_t requestId;
        UmiIbkrScannerQuery query;
        size_t count;
        uint64_t generation, requestedAtMilliseconds, receivedAtMilliseconds;
        bool active, needsCancel, cancelled, failed, stale;
        int providerCode;
        char message[256];
    } UmiIbkrScannerSnapshot;
    /* The connection owns ten independent subscriptions. Queue failures leave outputs
 * unchanged. Generic filters are serialized as bounded tag=value pairs; internal
 * subscription options are deliberately empty, as prescribed by the API. */
    UmiStatus UmiIbkrScannerQueryValidate(const UmiIbkrScannerQuery *query);
    UmiStatus UmiIbkrScannerRequest(UmiIbkrConnection *, const UmiIbkrScannerQuery *,
                                    uint64_t nowMilliseconds, uint32_t *outRequest);
    UmiStatus UmiIbkrScannerCancel(UmiIbkrConnection *, uint32_t request, uint64_t nowMilliseconds);
    UmiStatus UmiIbkrScannerCopy(const UmiIbkrConnection *, uint32_t request, uint64_t nowMilliseconds,
                                 uint64_t maximumAgeMilliseconds, UmiIbkrScannerSnapshot *out);
    /* A caller pins the generation shown on screen. A later refresh returns BUSY
 * instead of silently selecting a different contract at the same row index. */
    UmiStatus UmiIbkrScannerRowCopy(const UmiIbkrConnection *, uint32_t request, uint64_t generation,
                                    size_t index, UmiIbkrScannerRow *out);
#ifdef __cplusplus
}
#endif
#endif
