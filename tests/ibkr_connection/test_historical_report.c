/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_historical_report.c
 * PURPOSE: Check historical CSV fidelity, per-row metadata and independence from broker lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "historical_fixture.h"
#include "umicom/broker_connectivity/historical_report.h"
/* Read a quoted CSV cell independently of the production writer. Tests check
 * named business columns, including escaping, rather than only searching bytes. */
static int Cell(const UmiCsvDocument *document, size_t wantedRow, size_t wantedColumn, char *out,
                size_t capacity)
{
    const char *p = UmiCsvDocumentData(document);
    size_t row = 0U, column = 0U;
    CHECK(p && capacity);
    while (*p)
    {
        CHECK(*p++ == '"');
        size_t size = 0U;
        bool selected = row == wantedRow && column == wantedColumn;
        bool closed = false;
        while (*p)
        {
            char value = *p++;
            if (value == '"')
            {
                if (*p == '"')
                    ++p;
                else
                {
                    closed = true;
                    break;
                }
            }
            if (selected)
            {
                CHECK(size + 1U < capacity);
                out[size++] = value;
            }
        }
        CHECK(closed);
        if (selected)
        {
            out[size] = '\0';
            return 0;
        }
        if (*p == ',')
        {
            ++p;
            ++column;
        }
        else
        {
            CHECK(p[0] == '\r' && p[1] == '\n');
            p += 2;
            ++row;
            column = 0U;
        }
    }
    return 1;
}

static int NamedCell(const UmiCsvDocument *document, size_t row, const char *name, char *out, size_t capacity)
{
    char heading[128];
    for (size_t column = 0U; column < 40U; ++column)
    {
        CHECK(Cell(document, 0U, column, heading, sizeof heading) == 0);
        if (!strcmp(heading, name))
            return Cell(document, row, column, out, capacity);
    }
    return 1;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {
        "valid",     "no-request",     "null",   "backward", "zero-age",   "pending",
        "cancelled", "timeout",        "failed", "age",      "disconnect", "empty",
        "precision", "missing-volume", "quoted", "formula",  "owned",      "metadata-every-row"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    HistoricalReferences();
    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    UmiIbkrHistoricalQuery q = HistoricalQuery();
    uint32_t request = 1U;
    UmiCsvDocument *document = NULL;
    if (!strcmp(mode, "no-request") || !strcmp(mode, "null"))
    {
        UmiStatus expected = !strcmp(mode, "null") ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_NOT_FOUND;
        CHECK(UmiIbkrHistoricalExportCsv(!strcmp(mode, "null") ? NULL : f->c, request, 10U, 100U,
                                         &document) == expected);
        CHECK(!document);
        Delete(f);
        return 0;
    }
    if (!strcmp(mode, "quoted"))
        strcpy(q.contract.exchange, "X,\"route\"");
    if (!strcmp(mode, "formula"))
        strcpy(q.contract.exchange, "=route");
    CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    bool rows = strcmp(mode, "pending") && strcmp(mode, "cancelled") && strcmp(mode, "timeout") &&
                strcmp(mode, "failed") && strcmp(mode, "empty");
    if (rows)
        CHECK(HistoricalFeed(f, request) == 0);
    else if (!strcmp(mode, "empty"))
    {
        char id[32];
        (void)snprintf(id, sizeof id, "%u", (unsigned)request);
        FEED(f, "17", id, "", "", "0");
    }
    else if (!strcmp(mode, "failed"))
    {
        char id[32];
        (void)snprintf(id, sizeof id, "%u", (unsigned)request);
        FEED(f, "4", "2", id, "162", "Unavailable");
    }
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    if (!strcmp(mode, "cancelled"))
        CHECK(UmiIbkrHistoricalCancel(f->c, request, 12U) == UMI_STATUS_OK);
    if (!strcmp(mode, "disconnect"))
        UmiIbkrConnectionClose(f->c);
    if (!strcmp(mode, "precision"))
    {
        strcpy(f->c->history->bars[0].open.reportedText, "10.0000000001");
        f->c->history->bars[0].open.exact = false;
    }
    if (!strcmp(mode, "missing-volume"))
    {
        strcpy(f->c->history->bars[0].volume.reportedText, "-1");
        f->c->history->bars[0].volumeAvailable = false;
    }
    uint64_t now = !strcmp(mode, "backward")  ? 10U
                   : !strcmp(mode, "timeout") ? 60010U
                   : !strcmp(mode, "age")     ? 112U
                                              : 12U;
    UmiStatus result =
        UmiIbkrHistoricalExportCsv(f->c, request, now, !strcmp(mode, "zero-age") ? 0U : 100U, &document);
    if (!strcmp(mode, "backward") || !strcmp(mode, "zero-age"))
    {
        CHECK(result == UMI_STATUS_INVALID_ARGUMENT && !document);
        Delete(f);
        return 0;
    }
    CHECK(result == UMI_STATUS_OK && UmiCsvDocumentRows(document) == (rows ? 4U : 2U));
    if (!strcmp(mode, "owned"))
    {
        Delete(f);
        f = NULL;
    }
    char value[512];
    CHECK(NamedCell(document, 1U, "record_kind", value, sizeof value) == 0 && !strcmp(value, "metadata"));
    CHECK(NamedCell(document, 1U, "contract_id", value, sizeof value) == 0 && !strcmp(value, "123"));
    bool stale = !strcmp(mode, "pending") || !strcmp(mode, "cancelled") || !strcmp(mode, "timeout") ||
                 !strcmp(mode, "failed") || !strcmp(mode, "age") || !strcmp(mode, "disconnect");
    CHECK(NamedCell(document, 1U, "capture_stale", value, sizeof value) == 0 &&
          !strcmp(value, stale ? "true" : "false"));
    if (!strcmp(mode, "timeout"))
        CHECK(NamedCell(document, 1U, "capture_failed", value, sizeof value) == 0 && !strcmp(value, "true"));
    if (!strcmp(mode, "cancelled"))
        CHECK(NamedCell(document, 1U, "capture_cancelled", value, sizeof value) == 0 &&
              !strcmp(value, "true"));
    if (rows)
    {
        CHECK(NamedCell(document, 2U, "bar_time_utc_ms", value, sizeof value) == 0 &&
              !strcmp(value, "1791363600000"));
        CHECK(NamedCell(document, 2U, "open", value, sizeof value) == 0 &&
              !strcmp(value, !strcmp(mode, "precision") ? "10.0000000001" : "10"));
        CHECK(NamedCell(document, 2U, "open_exact", value, sizeof value) == 0 &&
              !strcmp(value, !strcmp(mode, "precision") ? "false" : "true"));
        CHECK(NamedCell(document, 2U, "volume_available", value, sizeof value) == 0 &&
              !strcmp(value, !strcmp(mode, "missing-volume") ? "false" : "true"));
        if (!strcmp(mode, "quoted"))
            CHECK(NamedCell(document, 2U, "exchange", value, sizeof value) == 0 &&
                  !strcmp(value, "X,\"route\""));
        if (!strcmp(mode, "formula"))
            CHECK(NamedCell(document, 2U, "exchange", value, sizeof value) == 0 && !strcmp(value, "'=route"));
        if (!strcmp(mode, "metadata-every-row"))
        {
            char meta[512];
            const char *names[] = {"request_id",  "contract_id",      "exchange",     "data_kind",
                                   "bar_seconds", "capture_complete", "capture_stale"};
            for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
            {
                CHECK(NamedCell(document, 1U, names[i], meta, sizeof meta) == 0);
                CHECK(NamedCell(document, 3U, names[i], value, sizeof value) == 0 && !strcmp(meta, value));
            }
        }
    }
    UmiCsvDocumentDestroy(document);
    Delete(f);
    return 0;
}
