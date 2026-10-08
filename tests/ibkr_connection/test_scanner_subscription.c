/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_scanner_subscription.c
 * PURPOSE: Check request bytes, cancellation rollback, slot limits and mixed request identities.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "discovery_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    DiscoveryReferences();
    const char *mode = argv[1];
    Fixture *f = New();
    CHECK(f);
    UmiIbkrScannerQuery q = ScanQuery();
    uint32_t request = 999U;
    UmiIbkrScannerSnapshot s;
    if (!strcmp(mode, "not-ready"))
    {
        CHECK(UmiIbkrScannerRequest(f->c, &q, 0U, &request) == UMI_STATUS_INVALID_STATE);
        CHECK(request == 999U);
        Delete(f);
        return 0;
    }
    CHECK(Connect(f) == 0);
    if (!strcmp(mode, "backward"))
        CHECK(UmiIbkrScannerRequest(f->c, &q, 4U, &request) == UMI_STATUS_INVALID_ARGUMENT);
    else if (!strcmp(mode, "null-output"))
        CHECK(UmiIbkrScannerRequest(f->c, &q, 10U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    else if (!strcmp(mode, "unsupported"))
    {
        f->c->snapshot.protocolVersion = 177;
        CHECK(UmiIbkrScannerRequest(f->c, &q, 10U, &request) == UMI_STATUS_NOT_IMPLEMENTED);
    }
    else if (!strcmp(mode, "queue-full"))
    {
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrScannerRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(request == 999U && !f->c->scanners[0] && !f->c->scannerRequested);
    }
    else if (!strcmp(mode, "overflow"))
    {
        f->c->nextQuoteRequest = INT32_MAX;
        CHECK(UmiIbkrScannerRequest(f->c, &q, 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else
    {
        if (!strcmp(mode, "filters"))
        {
            q.filterCount = 1U;
            strcpy(q.filters[0].tag, "usdMarketCapAbove");
            strcpy(q.filters[0].value, "10000");
        }
        CHECK(UmiIbkrScannerRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
        CHECK(request >= 36000U);
        size_t offset = f->outSize;
        if (!strcmp(mode, "fragmented"))
            f->writeStep = 1U;
        for (uint64_t now = 11U; f->c->txSize && now < 1000U; ++now)
            CHECK(StreamPump(f, now) == 0);
        CHECK(UmiIbkrScannerCopy(f->c, request, f->c->lastNow, 60000U, &s) == UMI_STATUS_OK && s.active &&
              s.needsCancel && s.stale);
        if (!strcmp(mode, "wire") || !strcmp(mode, "filters") || !strcmp(mode, "fragmented"))
        {
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            const char *fields[] = {"22",
                                    id,
                                    "25",
                                    "STK",
                                    "STK.US.MAJOR",
                                    "TOP_PERC_GAIN",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "0",
                                    "",
                                    "",
                                    "",
                                    !strcmp(mode, "filters") ? "usdMarketCapAbove=10000;" : "",
                                    ""};
            CHECK(ObserveWire(f, offset, fields, 25U) == 0);
        }
        else if (!strcmp(mode, "pacing"))
        {
            uint32_t next = 999U;
            CHECK(UmiIbkrScannerRequest(f->c, &q, 1009U, &next) == UMI_STATUS_BUSY && next == 999U);
            CHECK(UmiIbkrScannerRequest(f->c, &q, 1010U, &next) == UMI_STATUS_OK && next > request);
        }
        else if (!strcmp(mode, "capacity"))
        {
            for (unsigned i = 1U; i < 10U; ++i)
                CHECK(UmiIbkrScannerRequest(f->c, &q, 10U + 1000U * i, &request) == UMI_STATUS_OK);
            CHECK(UmiIbkrScannerRequest(f->c, &q, 10010U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        }
        else if (!strcmp(mode, "cancel") || !strcmp(mode, "reuse"))
        {
            offset = f->outSize;
            uint32_t previous = request;
            CHECK(UmiIbkrScannerCancel(f->c, request, 1010U) == UMI_STATUS_OK);
            CHECK(StreamPump(f, 1011U) == 0);
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            const char *fields[] = {"23", "1", id};
            CHECK(ObserveWire(f, offset, fields, 3U) == 0);
            CHECK(UmiIbkrScannerCopy(f->c, request, 1011U, 60000U, &s) == UMI_STATUS_OK && s.cancelled &&
                  !s.active);
            if (!strcmp(mode, "reuse"))
            {
                CHECK(UmiIbkrScannerRequest(f->c, &q, 1012U, &request) == UMI_STATUS_OK &&
                      request > previous);
                CHECK(UmiIbkrScannerCopy(f->c, previous, 1012U, 60000U, &s) == UMI_STATUS_NOT_FOUND);
            }
        }
        else if (!strcmp(mode, "cancel-full"))
        {
            f->c->txSize = UMI_IBKR_TX_LIMIT;
            CHECK(UmiIbkrScannerCancel(f->c, request, 1010U) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(UmiIbkrScannerCopy(f->c, request, 1010U, 60000U, &s) == UMI_STATUS_OK && s.active &&
                  s.needsCancel);
        }
        else if (!strcmp(mode, "disconnect"))
        {
            UmiIbkrConnectionClose(f->c);
            CHECK(UmiIbkrScannerCopy(f->c, request, 1010U, 60000U, &s) == UMI_STATUS_OK && s.failed &&
                  s.stale && !s.needsCancel);
        }
        else if (!strcmp(mode, "no-mutations"))
        {
            const char *ids[] = {"3", "4", "15", "21", "58"};
            for (size_t i = 0; i < 5U; ++i)
            {
                const char *fields[] = {ids[i], "1"};
                CHECK(UmiIbkrQueueFields(f->c, fields, 2U) == UMI_STATUS_PERMISSION_DENIED);
            }
        }
        else
            return 2;
    }
    Delete(f);
    return 0;
}
