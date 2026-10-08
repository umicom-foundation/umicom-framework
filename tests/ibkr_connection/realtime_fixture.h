/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/realtime_fixture.h
 * PURPOSE: Supply independent five-second packets through the injected transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_REALTIME_FIXTURE_H
#define UMICOM_TEST_REALTIME_FIXTURE_H
#include "historical_fixture.h"
#include "umicom/broker_connectivity/realtime_chart.h"
#include "umicom/broker_connectivity/realtime_report.h"
static UmiIbkrRealtimeQuery StreamQuery(void)
{
    UmiIbkrRealtimeQuery q = {0};
    q.contract.contractId = 123U;
    strcpy(q.contract.exchange, "SMART");
    q.dataKind = UMI_IBKR_HISTORY_TRADES;
    q.regularHours = true;
    return q;
}
static int StreamFeed(Fixture *f, uint32_t request, uint64_t seconds)
{
    char id[32], time[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    (void)snprintf(time, sizeof time, "%llu", (unsigned long long)seconds);
    const char *fields[] = {"50", "3", id, time, "10", "12", "9", "11", "100", "10.5", "8"};
    return Feed(f, fields, 11U);
}
/* Long pacing cases answer only heartbeats already requested by the session.
 * This is simulated transport data, never a network connection to a broker. */
static int StreamPump(Fixture *f, uint64_t now)
{
    if (f->c->pingPending)
        FEED(f, "49", "1", "1791363600");
    CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
    return 0;
}
static void StreamReferences(void)
{
    HistoricalReferences();
    (void)StreamQuery;
    (void)StreamFeed;
    (void)StreamPump;
}
#endif
