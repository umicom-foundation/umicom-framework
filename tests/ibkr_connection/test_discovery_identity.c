/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_discovery_identity.c
 * PURPOSE: Prevent cross-type ID reuse after finite or streaming chart requests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "discovery_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    DiscoveryReferences();
    bool history = !strcmp(argv[1], "history-first"), stream = !strcmp(argv[1], "stream-first");
    if (!history && !stream)
        return 2;
    Fixture *f = New();
    CHECK(f && Connect(f) == 0);
    uint32_t ids[5] = {0};
    UmiIbkrRealtimeQuery bars = StreamQuery();
    UmiIbkrHistoricalQuery finite = HistoricalQuery();
    if (history)
        CHECK(UmiIbkrHistoricalRequest(f->c, &finite, 10U, &ids[0]) == UMI_STATUS_OK);
    else
        CHECK(UmiIbkrRealtimeRequest(f->c, &bars, 10U, &ids[0]) == UMI_STATUS_OK);
    UmiIbkrQuoteContract contract = {123U, "SMART"};
    CHECK(UmiIbkrQuoteSubscribe(f->c, &contract, 11U, &ids[1]) == UMI_STATUS_OK);
    CHECK(UmiIbkrSymbolSearchRequest(f->c, "WORK", 12U, &ids[2]) == UMI_STATUS_OK);
    UmiIbkrScannerQuery scanner = ScanQuery();
    UmiIbkrOptionChainQuery chain = ChainQuery();
    CHECK(UmiIbkrScannerRequest(f->c, &scanner, 13U, &ids[3]) == UMI_STATUS_OK);
    CHECK(UmiIbkrOptionChainRequest(f->c, &chain, 14U, &ids[4]) == UMI_STATUS_OK);
    for (size_t i = 0; i < 5U; ++i)
    {
        CHECK(ids[i] >= 36000U && ids[i] < f->c->nextQuoteRequest);
        for (size_t j = 0; j < i; ++j)
            CHECK(ids[i] != ids[j]);
    }
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)ids[0]);
    FEED(f, "4", "2", id, "200", "Observation rejected");
    CHECK(StreamPump(f, 15U) == 0);
    UmiIbkrQuoteSnapshot quote;
    CHECK(UmiIbkrQuoteCopy(f->c, ids[1], 15U, 60000U, &quote) == UMI_STATUS_OK && !quote.failed);
    if (history)
    {
        UmiIbkrHistoricalSnapshot snapshot;
        CHECK(UmiIbkrHistoricalCopy(f->c, ids[0], 15U, 60000U, &snapshot) == UMI_STATUS_OK &&
              snapshot.failed);
    }
    else
    {
        UmiIbkrRealtimeSnapshot snapshot;
        CHECK(UmiIbkrRealtimeCopy(f->c, ids[0], 15U, 60000U, &snapshot) == UMI_STATUS_OK && snapshot.failed);
    }
    CHECK(f->c->snapshot.state == UMI_IBKR_READY);
    Delete(f);
    return 0;
}
