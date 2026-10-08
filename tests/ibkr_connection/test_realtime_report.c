/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_realtime_report.c
 * PURPOSE: Check rolling-report evidence, exact reported values and CSV ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "realtime_csv_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {
        "valid",          "pending", "cancelled", "failed",  "age",    "disconnect",         "precision",
        "missing-volume", "quoted",  "formula",   "owned",   "frozen", "metadata-every-row", "retention",
        "no-request",     "null",    "zero-age",  "backward"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    StreamReferences();

    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    UmiIbkrRealtimeQuery q = StreamQuery();
    uint32_t request = 0U;
    UmiCsvDocument *document = NULL;
    if (!strcmp(mode, "no-request") || !strcmp(mode, "null"))
    {
        CHECK(UmiIbkrRealtimeExportCsv(!strcmp(mode, "null") ? NULL : f->c, 1U, 10U, 100U, &document) ==
              (!strcmp(mode, "null") ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_NOT_FOUND));
        CHECK(!document);
        Delete(f);
        return 0;
    }
    if (!strcmp(mode, "quoted"))
        strcpy(q.contract.exchange, "X,\"route\"");
    if (!strcmp(mode, "formula"))
        strcpy(q.contract.exchange, "=route");
    CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    bool rows = strcmp(mode, "pending") != 0;
    if (rows)
    {
        CHECK(StreamFeed(f, request, 1791363600U) == 0);
        CHECK(StreamFeed(f, request, 1791363605U) == 0);
    }
    CHECK(StreamPump(f, 11U) == 0);
    UmiIbkrRealtimeStore *store = UmiIbkrRealtimeFind(f->c, request);
    CHECK(store);
    if (!strcmp(mode, "cancelled"))
        CHECK(UmiIbkrRealtimeCancel(f->c, request, 12U) == UMI_STATUS_OK);
    if (!strcmp(mode, "disconnect"))
        UmiIbkrConnectionClose(f->c);
    if (!strcmp(mode, "failed"))
    {
        char id[32];
        (void)snprintf(id, sizeof id, "%u", (unsigned)request);
        FEED(f, "4", "2", id, "10225", "Bust");
        CHECK(StreamPump(f, 12U) == 0);
    }
    if (!strcmp(mode, "precision"))
    {
        strcpy(store->bars[0].open.reportedText, "10.0000000001");
        store->bars[0].open.exact = false;
    }
    if (!strcmp(mode, "missing-volume"))
    {
        strcpy(store->bars[0].volume.reportedText, "-1");
        store->bars[0].volumeAvailable = false;
    }
    if (!strcmp(mode, "retention"))
    {
        for (uint64_t i = 2U; i < 514U; ++i)
        {
            CHECK(StreamFeed(f, request, 1791363600U + i * 5U) == 0);
            CHECK(StreamPump(f, 11U + i) == 0);
        }
    }
    uint64_t now = !strcmp(mode, "retention")  ? 600U
                   : !strcmp(mode, "age")      ? 112U
                   : !strcmp(mode, "backward") ? 10U
                                               : 12U;
    UmiStatus result =
        UmiIbkrRealtimeExportCsv(f->c, request, now, !strcmp(mode, "zero-age") ? 0U : 100U, &document);
    if (!strcmp(mode, "zero-age") || !strcmp(mode, "backward"))
    {
        CHECK(result == UMI_STATUS_INVALID_ARGUMENT && !document);
        Delete(f);
        return 0;
    }
    CHECK(result == UMI_STATUS_OK);
    CHECK(UmiCsvDocumentRows(document) == (!strcmp(mode, "retention") ? 514U : rows ? 4U : 2U));
    if (!strcmp(mode, "owned"))
    {
        Delete(f);
        f = NULL;
    }
    if (!strcmp(mode, "frozen"))
    {
        CHECK(StreamFeed(f, request, 1791363610U) == 0);
        CHECK(StreamPump(f, 13U) == 0);
        CHECK(UmiCsvDocumentRows(document) == 4U);
    }
    char value[512];
    CHECK(NamedCell(document, 1U, "record_kind", value, sizeof value) == 0 && !strcmp(value, "metadata"));
    CHECK(NamedCell(document, 1U, "bar_seconds", value, sizeof value) == 0 && !strcmp(value, "5"));
    bool stale = !strcmp(mode, "pending") || !strcmp(mode, "cancelled") || !strcmp(mode, "failed") ||
                 !strcmp(mode, "age") || !strcmp(mode, "disconnect");
    CHECK(NamedCell(document, 1U, "stale", value, sizeof value) == 0 &&
          !strcmp(value, stale ? "true" : "false"));
    if (!strcmp(mode, "quoted") || !strcmp(mode, "formula"))
    {
        CHECK(NamedCell(document, 1U, "exchange", value, sizeof value) == 0);
        CHECK(!strcmp(value, !strcmp(mode, "formula") ? "'=route" : "X,\"route\""));
    }
    if (rows)
    {
        CHECK(NamedCell(document, 2U, "open", value, sizeof value) == 0);
        CHECK(!strcmp(value, !strcmp(mode, "precision") ? "10.0000000001" : "10"));
        CHECK(NamedCell(document, 2U, "open_exact", value, sizeof value) == 0 &&
              !strcmp(value, !strcmp(mode, "precision") ? "false" : "true"));
        CHECK(NamedCell(document, 2U, "volume_available", value, sizeof value) == 0 &&
              !strcmp(value, !strcmp(mode, "missing-volume") ? "false" : "true"));
    }
    if (!strcmp(mode, "failed"))
    {
        CHECK(NamedCell(document, 2U, "provider_code", value, sizeof value) == 0 && !strcmp(value, "10225"));
        CHECK(NamedCell(document, 2U, "failed", value, sizeof value) == 0 && !strcmp(value, "true"));
    }
    if (!strcmp(mode, "metadata-every-row") || !strcmp(mode, "retention"))
    {
        for (size_t row = 1U; row < UmiCsvDocumentRows(document); ++row)
        {
            CHECK(NamedCell(document, row, "contract_id", value, sizeof value) == 0 && !strcmp(value, "123"));
            CHECK(NamedCell(document, row, "dropped_bars", value, sizeof value) == 0 &&
                  !strcmp(value, !strcmp(mode, "retention") ? "2" : "0"));
        }
    }
    UmiCsvDocumentDestroy(document);
    Delete(f);
    return 0;
}
