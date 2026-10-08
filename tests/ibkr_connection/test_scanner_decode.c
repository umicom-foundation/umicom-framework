/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_scanner_decode.c
 * PURPOSE: Verify whole-refresh publication, generation-pinned selection and retained stale evidence.
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
    static const char *const names[] = {
        "valid",     "empty",          "bad-version", "count",   "capacity",
        "rank",      "contract",       "symbol",      "utf8",    "type",
        "number",    "precision",      "exchange",    "control", "short",
        "extra",     "foreign",        "fragmented",  "refresh", "generation",
        "cancelled", "provider-error", "age",         "atomic",  "generation-overflow"};
    if (!HistoricalKnownCase(mode, names, sizeof names / sizeof names[0]))
        return 2;
    Fixture *f = New();
    CHECK(f && Connect(f) == 0);
    UmiIbkrScannerQuery q = ScanQuery();
    uint32_t request;
    CHECK(UmiIbkrScannerRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    CHECK(ScanFeed(f, request, "valid") == 0 && StreamPump(f, 11U) == 0);
    UmiIbkrScannerSnapshot s;
    UmiIbkrScannerRow row;
    CHECK(UmiIbkrScannerCopy(f->c, request, 11U, 60000U, &s) == UMI_STATUS_OK && s.generation == 1U &&
          s.count == 1U && !s.stale);
    if (!strcmp(mode, "cancelled"))
    {
        CHECK(UmiIbkrScannerCancel(f->c, request, 12U) == UMI_STATUS_OK);
        CHECK(ScanFeed(f, request, "empty") == 0 && StreamPump(f, 13U) == 0);
        CHECK(UmiIbkrScannerCopy(f->c, request, 13U, 60000U, &s) == UMI_STATUS_OK && s.count == 1U &&
              s.generation == 1U && s.stale);
    }
    else if (!strcmp(mode, "provider-error"))
    {
        char id[32];
        (void)snprintf(id, sizeof id, "%u", (unsigned)request);
        FEED(f, "4", "2", id, "354", "Filter subscription unavailable");
        CHECK(StreamPump(f, 12U) == 0);
        CHECK(UmiIbkrScannerCopy(f->c, request, 12U, 60000U, &s) == UMI_STATUS_OK && s.failed &&
              s.needsCancel && s.count == 1U);
        CHECK(f->c->snapshot.state == UMI_IBKR_READY);
    }
    else if (!strcmp(mode, "age"))
    {
        CHECK(UmiIbkrScannerCopy(f->c, request, 60011U, 60000U, &s) == UMI_STATUS_OK && !s.stale);
        CHECK(UmiIbkrScannerCopy(f->c, request, 60012U, 60000U, &s) == UMI_STATUS_OK && s.stale);
    }
    else
    {
        if (!strcmp(mode, "fragmented"))
            f->readStep = 1U;
        if (!strcmp(mode, "generation-overflow"))
            UmiIbkrScannerFind(f->c, request)->snapshot.generation = UINT64_MAX;
        CHECK(ScanFeed(f, !strcmp(mode, "foreign") ? request + 99U : request,
                       !strcmp(mode, "atomic") ? "number" : mode) == 0);
        UmiStatus status = UmiIbkrConnectionPump(f->c, 12U);
        bool accepted = !strcmp(mode, "valid") || !strcmp(mode, "empty") || !strcmp(mode, "precision") ||
                        !strcmp(mode, "foreign") || !strcmp(mode, "fragmented") || !strcmp(mode, "refresh") ||
                        !strcmp(mode, "generation");
        CHECK((status == UMI_STATUS_OK) == accepted);
        CHECK(UmiIbkrScannerCopy(f->c, request, 12U, 60000U, &s) == UMI_STATUS_OK);
        if (!accepted)
            CHECK(s.count == 1U && s.failed && s.stale);
        else if (!strcmp(mode, "foreign"))
            CHECK(s.generation == 1U && s.count == 1U);
        else
        {
            CHECK(s.generation == 2U && s.count == (!strcmp(mode, "empty") ? 0U : 1U));
            if (!strcmp(mode, "generation"))
            {
                memset(&row, 0, sizeof row);
                row.contractId = 999U;
                CHECK(UmiIbkrScannerRowCopy(f->c, request, 1U, 0U, &row) == UMI_STATUS_BUSY &&
                      row.contractId == 999U);
            }
            if (s.count)
            {
                CHECK(UmiIbkrScannerRowCopy(f->c, request, s.generation, 0U, &row) == UMI_STATUS_OK &&
                      row.contractId == 123U);
                CHECK(row.strike.exact == (strcmp(mode, "precision") != 0));
                CHECK(!strcmp(row.distance, "2.5"));
            }
        }
    }
    Delete(f);
    return 0;
}
