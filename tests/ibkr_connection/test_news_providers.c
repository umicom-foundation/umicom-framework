/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_news_providers.c
 *
 * PURPOSE:
 *   Verify news-provider framing, atomic captures, expiry and session boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "observation_fixture.h"
#include "umicom/broker_connectivity/news_providers.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    Fixture *f = New(); CHECK(f != NULL); CHECK(Connect(f) == 0);
    UmiIbkrNewsProvidersSnapshot result = {0};
    if (!strcmp(argv[1], "unsolicited")) {
        FEED(f, "85", "1", "TEST", "Example news");
        CHECK(UmiIbkrConnectionPump(f->c, 6U) == UMI_STATUS_OK);
        CHECK(UmiIbkrNewsProvidersCopy(f->c, 6U, &result) == UMI_STATUS_OK);
        CHECK(!result.requested && !result.complete);
    } else {
        size_t offset = f->outSize;
        CHECK(UmiIbkrNewsProvidersRequest(f->c, 6U) == UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionPump(f->c, 7U) == UMI_STATUS_OK);
        const char *wire[] = {"85"};
        CHECK(ObserveWire(f, offset, wire, 1U) == 0);
        CHECK(UmiIbkrNewsProvidersRequest(f->c, 7U) == UMI_STATUS_BUSY);
        if (!strcmp(argv[1], "wire")) {
            FEED(f, "85", "2", "TEST", "Example news", "OTHER", "Another provider");
            CHECK(UmiIbkrConnectionPump(f->c, 8U) == UMI_STATUS_OK);
            CHECK(UmiIbkrNewsProvidersCopy(f->c, 8U, &result) == UMI_STATUS_OK);
            CHECK(result.complete && result.count == 2U && !result.stale);
            CHECK(!strcmp(result.providers[1].name, "Another provider"));
            FEED(f, "85", "0");
            CHECK(UmiIbkrConnectionPump(f->c, 9U) == UMI_STATUS_OK);
            CHECK(UmiIbkrNewsProvidersCopy(f->c, 9U, &result) == UMI_STATUS_OK);
            CHECK(result.count == 2U);
        } else if (!strcmp(argv[1], "empty")) {
            FEED(f, "85", "0");
            CHECK(UmiIbkrConnectionPump(f->c, 8U) == UMI_STATUS_OK);
            CHECK(UmiIbkrNewsProvidersCopy(f->c, 8U, &result) == UMI_STATUS_OK);
            CHECK(result.complete && result.count == 0U);
        } else if (!strcmp(argv[1], "malformed")) {
            FEED(f, "85", "2", "TEST", "Example news", "TEST", "Duplicate code");
            CHECK(UmiIbkrConnectionPump(f->c, 8U) != UMI_STATUS_OK);
            CHECK(UmiIbkrNewsProvidersCopy(f->c, 8U, &result) == UMI_STATUS_OK);
            CHECK(!result.complete && result.count == 0U && result.stale);
        } else if (!strcmp(argv[1], "timeout")) {
            FEED(f, "85", "1", "TEST", "Late news");
            CHECK(UmiIbkrConnectionPump(f->c, 10006U) == UMI_STATUS_OK);
            CHECK(UmiIbkrNewsProvidersCopy(f->c, 10006U, &result) == UMI_STATUS_OK);
            CHECK(result.timedOut && result.stale && !result.complete);
        } else if (!strcmp(argv[1], "disconnect")) {
            UmiIbkrConnectionClose(f->c);
            CHECK(UmiIbkrNewsProvidersCopy(f->c, 8U, &result) == UMI_STATUS_OK);
            CHECK(result.stale);
        } else CHECK(false);
    }
    Delete(f); return 0;
}
