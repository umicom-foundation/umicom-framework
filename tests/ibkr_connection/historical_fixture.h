/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/historical_fixture.h
 * PURPOSE: Supply independent intraday packets to the injected broker transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_HISTORICAL_FIXTURE_H
#define UMICOM_TEST_HISTORICAL_FIXTURE_H
#include "observation_fixture.h"
#include "umicom/broker_connectivity/historical_chart.h"
static UmiIbkrHistoricalQuery HistoricalQuery(void)
{
    UmiIbkrHistoricalQuery q = {0};
    q.contract.contractId = 123U;
    strcpy(q.contract.exchange, "SMART");
    q.durationSeconds = 14400U;
    q.barSeconds = 60U;
    q.dataKind = UMI_IBKR_HISTORY_TRADES;
    q.regularHours = true;
    return q;
}
/* Fixture fields are written explicitly so a changed production offset cannot
 * silently change the expected protocol layout as well. */
static int HistoricalFeed(Fixture *f, uint32_t request)
{
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"17",
                            id,
                            "20261007 09:00:00 UTC",
                            "20261007 13:00:00 UTC",
                            "2",
                            "1791363600",
                            "10",
                            "12",
                            "9",
                            "11",
                            "100",
                            "10.5",
                            "8",
                            "1791363660",
                            "11",
                            "13",
                            "10",
                            "12",
                            "200",
                            "11.5",
                            "9"};
    return Feed(f, fields, sizeof fields / sizeof fields[0]);
}
static bool HistoricalKnownCase(const char *mode, const char *const *cases, size_t count)
{
    for (size_t i = 0U; i < count; ++i)
        if (!strcmp(mode, cases[i]))
            return true;
    return false;
}
static void HistoricalReferences(void)
{
    (void)ObserveWire;
    (void)PositionFeed;
    (void)HistoricalFeed;
    (void)HistoricalQuery;
}
#endif
