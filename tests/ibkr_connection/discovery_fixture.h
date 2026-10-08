/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/discovery_fixture.h
 * PURPOSE: Supply independent discovery packets through the fake transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_DISCOVERY_FIXTURE_H
#define UMICOM_TEST_DISCOVERY_FIXTURE_H
#include "realtime_fixture.h"
#include "umicom/broker_connectivity/option_contract.h"
static UmiIbkrScannerQuery ScanQuery(void)
{
    UmiIbkrScannerQuery q = {0};
    q.numberOfRows = 25U;
    strcpy(q.instrument, "STK");
    strcpy(q.locationCode, "STK.US.MAJOR");
    strcpy(q.scanCode, "TOP_PERC_GAIN");
    return q;
}
static UmiIbkrOptionChainQuery ChainQuery(void)
{
    UmiIbkrOptionChainQuery q = {0};
    q.underlyingContractId = 123U;
    strcpy(q.underlyingSymbol, "WORKSHOP");
    strcpy(q.underlyingSecurityType, "STK");
    return q;
}
static int ScanFeed(Fixture *f, uint32_t request, const char *mode)
{
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"20",  "3",     id,   "1",     "0",    "123",      "WORKSHOP",       "STK",
                            "",    "0",     "",   "SMART", "USD",  "WORKSHOP", "Example market", "CLASS",
                            "2.5", "BENCH", "up", "",      "extra"};
    size_t count = 20U;
    if (!strcmp(mode, "empty"))
    {
        fields[3] = "0";
        count = 4U;
    }
    if (!strcmp(mode, "bad-version"))
        fields[1] = "2";
    if (!strcmp(mode, "count"))
        fields[3] = "2";
    if (!strcmp(mode, "capacity"))
        fields[3] = "51";
    if (!strcmp(mode, "rank"))
        fields[4] = "1";
    if (!strcmp(mode, "contract"))
        fields[5] = "0";
    if (!strcmp(mode, "symbol"))
        fields[6] = "";
    if (!strcmp(mode, "utf8"))
        fields[6] = "\xc0\xaf";
    if (!strcmp(mode, "type"))
        fields[7] = "";
    if (!strcmp(mode, "number"))
        fields[9] = "NaN";
    if (!strcmp(mode, "precision"))
        fields[9] = "0.12345678901";
    if (!strcmp(mode, "exchange"))
        fields[11] = "";
    if (!strcmp(mode, "control"))
        fields[18] = "line\nbreak";
    if (!strcmp(mode, "short"))
        --count;
    if (!strcmp(mode, "extra"))
        ++count;
    return Feed(f, fields, count);
}
static int ChainFeed(Fixture *f, uint32_t request, const char *mode)
{
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"75",       id,         "SMART", "123", "CLASS", "100",  "2",
                            "20261218", "20270115", "2",     "100", "110",   "extra"};
    size_t count = 12U;
    if (!strcmp(mode, "underlying"))
        fields[3] = "456";
    if (!strcmp(mode, "exchange"))
        fields[2] = "";
    if (!strcmp(mode, "class"))
        fields[4] = "";
    if (!strcmp(mode, "utf8"))
        fields[4] = "\xc0\xaf";
    if (!strcmp(mode, "expiries"))
        fields[6] = "129";
    if (!strcmp(mode, "date"))
        fields[7] = "20260229";
    if (!strcmp(mode, "duplicate-expiry"))
        fields[8] = fields[7];
    if (!strcmp(mode, "strikes"))
        fields[9] = "513";
    if (!strcmp(mode, "number"))
        fields[10] = "NaN";
    if (!strcmp(mode, "precision"))
        fields[10] = "0.12345678901";
    if (!strcmp(mode, "negative"))
        fields[10] = "-10";
    if (!strcmp(mode, "duplicate-strike"))
        fields[11] = "100.0";
    if (!strcmp(mode, "short"))
        --count;
    if (!strcmp(mode, "extra"))
        ++count;
    return Feed(f, fields, count);
}
static int ChainEnd(Fixture *f, uint32_t request)
{
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    FEED(f, "76", id);
    return 0;
}
static void DiscoveryReferences(void)
{
    StreamReferences();
    (void)ScanQuery;
    (void)ChainQuery;
    (void)ScanFeed;
    (void)ChainFeed;
    (void)ChainEnd;
}
#endif
