/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_scanner_catalog.c
 * PURPOSE: Exercise catalogue ownership, atomic copies and inert provider text through injected I/O.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static const char catalog[] = "<ScannerParameters><scanCode>TOP_PERC_GAIN</scanCode></ScannerParameters>";
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *cases[] = {
        "wire",         "not-ready",   "null",       "backward",  "protocol",    "queue-full", "pending",
        "repeat",       "valid",       "whitespace", "unicode",   "inert",       "empty",      "version",
        "extra",        "utf8",        "control",    "duplicate", "unsolicited", "timeout",    "late-timeout",
        "close-before", "close-after", "copy-small", "copy-null", "retained"};
    bool known = false;
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(argv[1], cases[i]))
            known = true;
    if (!known)
        return 2;
    (void)PositionFeed;
    const char *name = argv[1];
    Fixture *f = New();
    CHECK(f);
    if (!strcmp(name, "not-ready"))
    {
        CHECK(UmiIbkrScannerCatalogRequest(f->c, 0) == UMI_STATUS_INVALID_STATE);
        Delete(f);
        return 0;
    }
    CHECK(!Connect(f));
    if (!strcmp(name, "null"))
    {
        CHECK(UmiIbkrScannerCatalogRequest(NULL, 10) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrScannerCatalogCopy(f->c, 10, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "backward"))
    {
        CHECK(UmiIbkrScannerCatalogRequest(f->c, 4) == UMI_STATUS_INVALID_ARGUMENT);
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "protocol"))
    {
        f->c->snapshot.protocolVersion = 177;
        CHECK(UmiIbkrScannerCatalogRequest(f->c, 10) == UMI_STATUS_NOT_IMPLEMENTED);
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "queue-full"))
    {
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrScannerCatalogRequest(f->c, 10) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->scannerCatalog.snapshot.requested);
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "unsolicited"))
    {
        FEED(f, "19", "1", catalog);
        CHECK(UmiIbkrConnectionPump(f->c, 10) == UMI_STATUS_OK);
        CHECK(!f->c->scannerCatalog.xml);
        Delete(f);
        return 0;
    }
    CHECK(UmiIbkrScannerCatalogRequest(f->c, 10) == UMI_STATUS_OK);
    UmiIbkrScannerCatalogSnapshot s;
    CHECK(UmiIbkrScannerCatalogCopy(f->c, 10, &s) == UMI_STATUS_OK);
    if (!strcmp(name, "wire"))
    {
        static const unsigned char expected[] = {0, 0, 0, 5, '2', '4', 0, '1', 0};
        CHECK(f->c->txSize == sizeof expected && !memcmp(f->c->tx, expected, sizeof expected));
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "pending"))
    {
        CHECK(s.requested && !s.complete && s.stale && !s.failed && !s.byteCount);
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "repeat"))
    {
        CHECK(UmiIbkrScannerCatalogRequest(f->c, 11) == UMI_STATUS_INVALID_STATE);
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "timeout") || !strcmp(name, "late-timeout"))
    {
        CHECK(UmiIbkrScannerCatalogCopy(f->c, 60010, &s) == UMI_STATUS_OK);
        CHECK(s.failed && s.stale && !s.complete);
        if (!strcmp(name, "late-timeout"))
        {
            FEED(f, "19", "1", catalog);
            CHECK(UmiIbkrConnectionPump(f->c, 60010) == UMI_STATUS_OK);
            CHECK(!f->c->scannerCatalog.xml);
        }
        CHECK(UmiIbkrScannerCatalogRequest(f->c, 60010) == UMI_STATUS_INVALID_STATE);
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "close-before"))
    {
        UmiIbkrConnectionClose(f->c);
        CHECK(UmiIbkrScannerCatalogCopy(f->c, 10, &s) == UMI_STATUS_OK);
        CHECK(s.failed && s.stale && !s.complete);
        Delete(f);
        return 0;
    }
    const char *text = catalog, *version = "1";
    bool bad = false;
    if (!strcmp(name, "whitespace"))
        text = "<Scanner>\r\n\t<scan>GAIN</scan>\n</Scanner>";
    if (!strcmp(name, "unicode"))
        text = "<Scanner>caf\xc3\xa9 \xe6\x97\xa5\xe6\x9c\xac</Scanner>";
    if (!strcmp(name, "inert"))
        text = "<!DOCTYPE x SYSTEM \"https://invalid.example/none\"><x>&external;</x>";
    if (!strcmp(name, "empty"))
    {
        text = "";
        bad = true;
    }
    if (!strcmp(name, "version"))
    {
        version = "9";
        bad = true;
    }
    if (!strcmp(name, "utf8"))
    {
        text = "<x>\xc0\xaf</x>";
        bad = true;
    }
    if (!strcmp(name, "control"))
    {
        text = "<x>\x01</x>";
        bad = true;
    }
    if (!strcmp(name, "extra"))
    {
        FEED(f, "19", "1", text, "extra");
        bad = true;
    }
    else
    {
        FEED(f, "19", version, text);
    }
    UmiStatus status = UmiIbkrConnectionPump(f->c, 11);
    if (bad)
    {
        CHECK(status == UMI_STATUS_PARSE_ERROR);
        CHECK(!f->c->scannerCatalog.xml);
        CHECK(UmiIbkrScannerCatalogCopy(f->c, 11, &s) == UMI_STATUS_OK);
        CHECK(s.failed && !s.complete);
        Delete(f);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiIbkrScannerCatalogCopy(f->c, 11, &s) == UMI_STATUS_OK);
    CHECK(s.complete && !s.stale && s.byteCount == strlen(text));
    char output[512] = "unchanged";
    if (!strcmp(name, "copy-small"))
    {
        CHECK(UmiIbkrScannerCatalogTextCopy(f->c, output, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!strcmp(output, "unchanged"));
    }
    else if (!strcmp(name, "copy-null"))
    {
        CHECK(UmiIbkrScannerCatalogTextCopy(f->c, NULL, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrScannerCatalogTextCopy(NULL, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
    {
        if (!strcmp(name, "duplicate"))
        {
            FEED(f, "19", "1", "<changed/>");
            CHECK(UmiIbkrConnectionPump(f->c, 12) == UMI_STATUS_OK);
        }
        if (!strcmp(name, "close-after"))
        {
            UmiIbkrConnectionClose(f->c);
            CHECK(UmiIbkrScannerCatalogCopy(f->c, 12, &s) == UMI_STATUS_OK);
            CHECK(s.complete && s.stale && !s.failed);
        }
        CHECK(UmiIbkrScannerCatalogTextCopy(f->c, output, sizeof output) == UMI_STATUS_OK);
        CHECK(!strcmp(output, text));
        if (!strcmp(name, "retained"))
        {
            Delete(f);
            f = NULL;
            CHECK(!strcmp(output, text));
        }
    }
    Delete(f);
    return 0;
}
