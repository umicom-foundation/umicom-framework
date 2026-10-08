/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_completed_report.c
 * PURPOSE: Check completed-order report scope, fidelity and independence from the connection lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "completed_fixture.h"
#include "umicom/broker_connectivity/completed_report.h"
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
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    CompletedFixtureReferences();
    const char *mode = argv[1];
    const char *const cases[] = {"valid",   "no-request", "null",    "backward",   "zero-age",  "empty",
                                 "partial", "timeout",    "age",     "disconnect", "all-scope", "precision",
                                 "unicode", "quoted",     "formula", "owned"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(mode, cases[i]))
            known = true;
    CHECK(known);
    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    UmiCsvDocument *document = NULL;
    if (!strcmp(mode, "no-request") || !strcmp(mode, "null"))
    {
        UmiStatus expected = !strcmp(mode, "null") ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_INVALID_STATE;
        CHECK(UmiIbkrCompletedOrdersExportCsv(!strcmp(mode, "null") ? NULL : f->c, 10U, 15000U, &document) ==
              expected);
        CHECK(document == NULL);
        Delete(f);
        return 0;
    }
    CHECK(UmiIbkrCompletedOrdersRequest(f->c, strcmp(mode, "all-scope") != 0, 10U) == UMI_STATUS_OK);
    CompletedFields fields = CompletedExample();
    if (!strcmp(mode, "precision"))
        fields.fields[CF_FILLED] = "0.0000000001";
    if (!strcmp(mode, "unicode"))
        fields.fields[CF_REFERENCE] = "caf\xc3\xa9";
    if (!strcmp(mode, "quoted"))
        fields.fields[CF_REFERENCE] = "first,\"quoted\"";
    if (!strcmp(mode, "formula"))
        fields.fields[CF_REFERENCE] = "=1+2";
    if (strcmp(mode, "empty"))
        CHECK(CompletedFeed(f, &fields) == 0);
    if (strcmp(mode, "partial") && strcmp(mode, "timeout"))
        FEED(f, "102");
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    if (!strcmp(mode, "disconnect"))
        UmiIbkrConnectionClose(f->c);
    if (!strcmp(mode, "timeout"))
        CHECK(UmiIbkrConnectionPump(f->c, 10010U) == UMI_STATUS_OK);
    uint64_t now = !strcmp(mode, "timeout")    ? 10010U
                   : !strcmp(mode, "age")      ? 15012U
                   : !strcmp(mode, "backward") ? 10U
                                               : 12U;
    UmiStatus status =
        UmiIbkrCompletedOrdersExportCsv(f->c, now, !strcmp(mode, "zero-age") ? 0U : 15000U, &document);
    if (!strcmp(mode, "backward") || !strcmp(mode, "zero-age"))
    {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && document == NULL);
        Delete(f);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK);
    if (!strcmp(mode, "owned"))
    {
        Delete(f);
        f = NULL;
    }
    CHECK(UmiCsvDocumentRows(document) == (!strcmp(mode, "empty") ? 2U : 3U));
    char value[512];
    CHECK(Cell(document, 0U, 25U, value, sizeof value) == 0 && !strcmp(value, "reported_filled_quantity"));
    CHECK(Cell(document, 1U, 0U, value, sizeof value) == 0 && !strcmp(value, "metadata"));
    CHECK(Cell(document, 1U, 1U, value, sizeof value) == 0 &&
          !strcmp(value, !strcmp(mode, "all-scope") ? "all-visible" : "api-origin-visible"));
    bool incomplete = !strcmp(mode, "partial") || !strcmp(mode, "timeout");
    CHECK(Cell(document, 1U, 2U, value, sizeof value) == 0 && !strcmp(value, incomplete ? "false" : "true"));
    CHECK(Cell(document, 1U, 3U, value, sizeof value) == 0 &&
          !strcmp(value, !strcmp(mode, "timeout") ? "true" : "false"));
    bool stale = incomplete || !strcmp(mode, "age") || !strcmp(mode, "disconnect");
    CHECK(Cell(document, 1U, 4U, value, sizeof value) == 0 && !strcmp(value, stale ? "true" : "false"));
    CHECK(Cell(document, 1U, 8U, value, sizeof value) == 0 && !strcmp(value, "false"));
    if (strcmp(mode, "empty"))
    {
        CHECK(Cell(document, 2U, 1U, value, sizeof value) == 0 &&
              !strcmp(value, !strcmp(mode, "all-scope") ? "all-visible" : "api-origin-visible"));
        CHECK(Cell(document, 2U, 14U, value, sizeof value) == 0 && !strcmp(value, "800"));
        CHECK(Cell(document, 2U, 16U, value, sizeof value) == 0 && !strcmp(value, "DU123"));
        CHECK(Cell(document, 2U, 24U, value, sizeof value) == 0 && !strcmp(value, "7000"));
        CHECK(Cell(document, 2U, 25U, value, sizeof value) == 0 &&
              !strcmp(value, !strcmp(mode, "precision") ? "0.0000000001" : "10"));
        CHECK(Cell(document, 2U, 26U, value, sizeof value) == 0 &&
              !strcmp(value, !strcmp(mode, "precision") ? "false" : "true"));
        CHECK(Cell(document, 2U, 30U, value, sizeof value) == 0 && !strcmp(value, "Cancelled"));
        CHECK(Cell(document, 2U, 47U, value, sizeof value) == 0);
        CHECK(!strcmp(value, !strcmp(mode, "formula") ? "'=1+2" : fields.fields[CF_REFERENCE]));
    }
    UmiCsvDocumentDestroy(document);
    Delete(f);
    return 0;
}
