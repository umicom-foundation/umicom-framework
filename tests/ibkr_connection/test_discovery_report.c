/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_discovery_report.c
 * PURPOSE: Check discovery report ownership, incomplete scope, formula escaping and separate option sets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "discovery_fixture.h"
#include "realtime_csv_fixture.h"
#include "umicom/broker_connectivity/discovery_report.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    DiscoveryReferences();
    const char *mode = argv[1];
    static const char *const names[] = {"scanner",           "scanner-pending",
                                        "scanner-cancelled", "scanner-owned",
                                        "scanner-frozen",    "scanner-filter",
                                        "scanner-formula",   "scanner-quoted",
                                        "scanner-age",       "option",
                                        "option-partial",    "option-owned",
                                        "option-abandoned",  "option-disconnect",
                                        "option-precision",  "option-separate-sets",
                                        "invalid",           "no-request"};
    if (!HistoricalKnownCase(mode, names, sizeof names / sizeof names[0]))
        return 2;
    Fixture *f = New();
    CHECK(f && Connect(f) == 0);
    uint32_t request = 0U;
    UmiCsvDocument *document = NULL;
    if (!strcmp(mode, "invalid") || !strcmp(mode, "no-request"))
    {
        CHECK(UmiIbkrScannerExportCsv(!strcmp(mode, "invalid") ? NULL : f->c, 999U, 10U, 100U, &document) !=
                  UMI_STATUS_OK &&
              !document);
        CHECK(UmiIbkrOptionChainExportCsv(!strcmp(mode, "invalid") ? NULL : f->c, 999U, 10U, &document) !=
                  UMI_STATUS_OK &&
              !document);
        Delete(f);
        return 0;
    }
    bool scanner = !strncmp(mode, "scanner", 7U);
    if (scanner)
    {
        UmiIbkrScannerQuery q = ScanQuery();
        if (!strcmp(mode, "scanner-formula"))
            strcpy(q.instrument, "=STK");
        if (!strcmp(mode, "scanner-quoted"))
            strcpy(q.instrument, "STK,\"example\"");
        if (!strcmp(mode, "scanner-filter"))
        {
            q.filterCount = 1U;
            strcpy(q.filters[0].tag, "usdMarketCapAbove");
            strcpy(q.filters[0].value, "10000");
        }
        CHECK(UmiIbkrScannerRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
        if (strcmp(mode, "scanner-pending"))
            CHECK(ScanFeed(f, request, "valid") == 0);
        CHECK(StreamPump(f, 11U) == 0);
        if (!strcmp(mode, "scanner-cancelled"))
            CHECK(UmiIbkrScannerCancel(f->c, request, 12U) == UMI_STATUS_OK);
        CHECK(UmiIbkrScannerExportCsv(f->c, request, !strcmp(mode, "scanner-age") ? 112U : 12U, 100U,
                                      &document) == UMI_STATUS_OK);
        CHECK(UmiCsvDocumentRows(document) == (!strcmp(mode, "scanner-pending")  ? 19U
                                               : !strcmp(mode, "scanner-filter") ? 21U
                                                                                 : 20U));
    }
    else
    {
        UmiIbkrOptionChainQuery q = ChainQuery();
        CHECK(UmiIbkrOptionChainRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
        CHECK(ChainFeed(f, request, !strcmp(mode, "option-precision") ? "precision" : "complete") == 0);
        if (strcmp(mode, "option-partial"))
            CHECK(ChainEnd(f, request) == 0);
        CHECK(StreamPump(f, 11U) == 0);
        if (!strcmp(mode, "option-abandoned"))
            CHECK(UmiIbkrOptionChainAbandon(f->c, request) == UMI_STATUS_OK);
        if (!strcmp(mode, "option-disconnect"))
            UmiIbkrConnectionClose(f->c);
        CHECK(UmiIbkrOptionChainExportCsv(f->c, request, 12U, &document) == UMI_STATUS_OK);
        CHECK(UmiCsvDocumentRows(document) == 7U);
    }
    if (strstr(mode, "owned"))
    {
        Delete(f);
        f = NULL;
    }
    if (!strcmp(mode, "scanner-frozen"))
    {
        CHECK(ScanFeed(f, request, "empty") == 0 && StreamPump(f, 13U) == 0);
        CHECK(UmiCsvDocumentRows(document) == 20U);
    }
    char value[512];
    CHECK(NamedCell(document, 1U, "record_kind", value, sizeof value) == 0 && !strcmp(value, "metadata"));
    CHECK(NamedCell(document, 1U, "environment_attested", value, sizeof value) == 0 &&
          !strcmp(value, "false"));
    if (!strcmp(mode, "scanner-formula") || !strcmp(mode, "scanner-quoted"))
    {
        CHECK(NamedCell(document, 2U, "instrument", value, sizeof value) == 0);
        CHECK(!strcmp(value, !strcmp(mode, "scanner-formula") ? "'=STK" : "STK,\"example\""));
    }
    if (!strcmp(mode, "scanner-filter"))
    {
        CHECK(NamedCell(document, 20U, "record_kind", value, sizeof value) == 0 &&
              !strcmp(value, "generic_filter"));
        CHECK(NamedCell(document, 20U, "property", value, sizeof value) == 0 &&
              !strcmp(value, "usdMarketCapAbove"));
        CHECK(NamedCell(document, 20U, "value", value, sizeof value) == 0 && !strcmp(value, "10000"));
    }
    if (!strcmp(mode, "option-precision"))
    {
        CHECK(NamedCell(document, 5U, "value", value, sizeof value) == 0 && !strcmp(value, "0.12345678901"));
        CHECK(NamedCell(document, 5U, "exact", value, sizeof value) == 0 && !strcmp(value, "false"));
    }
    bool stale = strstr(mode, "pending") || strstr(mode, "cancelled") || strstr(mode, "age") ||
                 strstr(mode, "partial") || strstr(mode, "abandoned") || strstr(mode, "disconnect");
    CHECK(NamedCell(document, 1U, "stale", value, sizeof value) == 0 &&
          !strcmp(value, stale ? "true" : "false"));
    if (!scanner)
    {
        CHECK(NamedCell(document, 2U, "record_kind", value, sizeof value) == 0 && !strcmp(value, "chain"));
        CHECK(NamedCell(document, 3U, "record_kind", value, sizeof value) == 0 && !strcmp(value, "expiry"));
        CHECK(NamedCell(document, 5U, "record_kind", value, sizeof value) == 0 && !strcmp(value, "strike"));
        /* Every set member repeats underlying/class scope, even if a spreadsheet
         * later filters away the metadata and chain summary rows. */
        for (size_t i = 3U; i < 7U; ++i)
            CHECK(NamedCell(document, i, "underlying_contract_id", value, sizeof value) == 0 &&
                  !strcmp(value, "123"));
    }
    UmiCsvDocumentDestroy(document);
    Delete(f);
    return 0;
}
