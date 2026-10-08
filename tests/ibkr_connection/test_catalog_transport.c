/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_catalog_transport.c
 * PURPOSE: Check large catalogue envelopes preserve small-message limits and incremental receive ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static int Header(Fixture *f, size_t bytes)
{
    CHECK(f->inSize + 4U <= sizeof f->input);
    unsigned char *p = f->input + f->inSize;
    p[0] = (unsigned char)(bytes >> 24U);
    p[1] = (unsigned char)(bytes >> 16U);
    p[2] = (unsigned char)(bytes >> 8U);
    p[3] = (unsigned char)bytes;
    f->inSize += 4U;
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *cases[] = {"large",           "fragmented",     "trailing-frame", "wrong-message",
                           "no-request",      "too-large",      "zero-length",    "close-partial",
                           "timeout-partial", "frame-overflow", "read-error"};
    bool known = false;
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(argv[1], cases[i]))
            known = true;
    if (!known)
        return 2;
    (void)PositionFeed;
    const char *name = argv[1];
    Fixture *f = New();
    CHECK(f && !Connect(f));
    if (strcmp(name, "no-request"))
        CHECK(UmiIbkrScannerCatalogRequest(f->c, 10) == UMI_STATUS_OK);
    if (!strcmp(name, "too-large") || !strcmp(name, "zero-length"))
    {
        CHECK(!Header(f, !strcmp(name, "too-large") ? UMI_IBKR_SCANNER_CATALOG_BYTES + 7U : 0U));
        CHECK(UmiIbkrConnectionPump(f->c, 11) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!f->c->catalogFrame && !f->c->scannerCatalog.xml);
        Delete(f);
        return 0;
    }
    size_t length = 70000U;
    char *text = malloc(length + 1U);
    CHECK(text);
    memset(text, 'x', length);
    memcpy(text, "<Scanner>", 9U);
    memcpy(text + length - 10U, "</Scanner>", 10U);
    text[length] = 0;
    FEED(f, !strcmp(name, "wrong-message") ? "79" : "19", "1", text);
    if (!strcmp(name, "trailing-frame"))
    {
        FEED(f, "15", "1", "DU123,DU456");
    }
    if (!strcmp(name, "fragmented"))
        f->readStep = 1U;
    UmiStatus first = UmiIbkrConnectionPump(f->c, 11);
    if (!strcmp(name, "no-request") || !strcmp(name, "wrong-message"))
    {
        CHECK(first == (!strcmp(name, "no-request") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR));
        CHECK(!f->c->catalogFrame && !f->c->scannerCatalog.xml);
        free(text);
        Delete(f);
        return 0;
    }
    CHECK(first == UMI_STATUS_OK);
    CHECK(f->c->catalogFrame && !f->c->scannerCatalog.xml);
    CHECK(!f->c->scannerCatalog.snapshot.complete);
    if (!strcmp(name, "close-partial"))
    {
        UmiIbkrConnectionClose(f->c);
        CHECK(!f->c->catalogFrame && !f->c->scannerCatalog.xml);
        free(text);
        Delete(f);
        return 0;
    }
    if (!strcmp(name, "frame-overflow"))
        f->c->snapshot.framesReceived = UINT64_MAX;
    if (!strcmp(name, "read-error"))
        f->readStatus = UMI_STATUS_IO_ERROR;
    uint64_t now = !strcmp(name, "timeout-partial") ? 60010U : 12U;
    UmiStatus second = UmiIbkrConnectionPump(f->c, now);
    if (!strcmp(name, "frame-overflow") || !strcmp(name, "read-error"))
    {
        CHECK(second ==
              (!strcmp(name, "frame-overflow") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_IO_ERROR));
        CHECK(!f->c->catalogFrame && !f->c->scannerCatalog.xml);
    }
    else
    {
        CHECK(second == UMI_STATUS_OK);
        CHECK(!f->c->catalogFrame && f->c->rxSize == 0U);
        UmiIbkrScannerCatalogSnapshot s;
        CHECK(UmiIbkrScannerCatalogCopy(f->c, now, &s) == UMI_STATUS_OK);
        if (!strcmp(name, "timeout-partial"))
            CHECK(s.failed && !s.complete && !f->c->scannerCatalog.xml);
        else
        {
            CHECK(s.complete && s.byteCount == length);
            char *copy = malloc(length + 1U);
            CHECK(copy && UmiIbkrScannerCatalogTextCopy(f->c, copy, length + 1U) == UMI_STATUS_OK);
            CHECK(!memcmp(copy, text, length + 1U));
            free(copy);
        }
    }
    free(text);
    Delete(f);
    return 0;
}
