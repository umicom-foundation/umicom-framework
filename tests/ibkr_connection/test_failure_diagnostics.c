/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_failure_diagnostics.c
 *
 * PURPOSE:
 *   Verify current receive, transport and provider diagnostics through injected broker I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/broker_connectivity/connection_guidance.h"

static int Farm(Fixture *f)
{
    FEED(f, "4", "2", "-1", "2104", "Market data farm is connected", "");
    CHECK(UmiIbkrConnectionPump(f->c, 10U) == UMI_STATUS_OK);
    CHECK(f->c->snapshot.providerCode == 2104);
    return 0;
}
static int Envelope(Fixture *f, size_t size)
{
    CHECK(sizeof(f->input) - f->inSize >= 4U);
    unsigned char *out = f->input + f->inSize;
    out[0] = (unsigned char)(size >> 24U);
    out[1] = (unsigned char)(size >> 16U);
    out[2] = (unsigned char)(size >> 8U);
    out[3] = (unsigned char)size;
    f->inSize += 4U;
    return 0;
}
static int Failed(Fixture *f, UmiStatus status, int provider,
                  UmiIbkrConnectionStep step, const char *text)
{
    UmiIbkrConnectionSnapshot *copy = malloc(sizeof(*copy));
    CHECK(copy != NULL);
    CHECK(UmiIbkrConnectionCopy(f->c, copy) == UMI_STATUS_OK);
    CHECK(copy->state == UMI_IBKR_FAILED && copy->stale);
    CHECK(copy->lastStatus == status && copy->providerCode == provider);
    CHECK(strstr(copy->message, text) != NULL);
    CHECK(copy->accountCount == 2U && strcmp(copy->accounts[0], "DU123") == 0);
    UmiIbkrConnectionGuidance guidance;
    CHECK(UmiIbkrConnectionExplain(copy, &guidance) == UMI_STATUS_OK);
    CHECK(guidance.step == step);
    CHECK(f->closes == 1U && f->c->rxSize == 0U && f->c->txSize == 0U);
    /* A second inspection and explicit close preserve the original failure.
     * Reporting must not reconnect, retry a request or mutate retained rows. */
    UmiIbkrConnectionClose(f->c);
    CHECK(f->closes == 1U);
    CHECK(UmiIbkrConnectionPump(f->c, 50000U) == UMI_STATUS_INVALID_STATE);
    CHECK(f->c->snapshot.lastStatus == copy->lastStatus);
    CHECK(strcmp(f->c->snapshot.message, copy->message) == 0);
    free(copy);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    Fixture *f = New();
    CHECK(f != NULL && Connect(f) == 0 && Farm(f) == 0);
    UmiStatus expected = UMI_STATUS_IO_ERROR;
    UmiIbkrConnectionStep step = UMI_IBKR_STEP_RECONNECT;
    const char *diagnostic = "read failed";
    int provider = 0;
    uint64_t now = 11U;
    if (strcmp(name, "read-error") == 0) {
        f->readStatus = UMI_STATUS_IO_ERROR;
    } else if (strcmp(name, "eof") == 0) {
        f->eof = true;
    } else if (strcmp(name, "write-error") == 0 || strcmp(name, "zero-write") == 0) {
        CHECK(UmiIbkrConnectionReadAccount(f->c, "DU123", now) == UMI_STATUS_OK);
        if (strcmp(name, "zero-write") == 0) {
            f->zeroWrite = true;
            diagnostic = "Invalid transport write";
        } else {
            f->writeStatus = UMI_STATUS_IO_ERROR;
            diagnostic = "write failed";
        }
    } else if (strcmp(name, "heartbeat-timeout") == 0) {
        CHECK(UmiIbkrConnectionPump(f->c, 30005U) == UMI_STATUS_OK);
        CHECK(f->c->pingPending);
        now = 40005U; expected = UMI_STATUS_TIMEOUT; diagnostic = "clock request timed out";
    } else if (strcmp(name, "account-timeout") == 0) {
        CHECK(UmiIbkrConnectionReadAccount(f->c, "DU123", now) == UMI_STATUS_OK);
        now += f->c->options.timeoutMilliseconds;
        expected = UMI_STATUS_TIMEOUT; diagnostic = "account response did not finish";
    } else if (strcmp(name, "zero-frame") == 0 || strcmp(name, "large-frame") == 0) {
        CHECK(Envelope(f, strcmp(name, "zero-frame") == 0 ? 0U : 70000U) == 0);
        expected = UMI_STATUS_CAPACITY_EXCEEDED; step = UMI_IBKR_STEP_REVIEW_PROTOCOL;
        diagnostic = strcmp(name, "zero-frame") == 0 ? "Empty API frame" : "receive bound";
    } else if (strcmp(name, "frame-counter") == 0) {
        f->c->snapshot.framesReceived = UINT64_MAX;
        FEED(f, "999", "ignored");
        expected = UMI_STATUS_CAPACITY_EXCEEDED; step = UMI_IBKR_STEP_REVIEW_PROTOCOL;
        diagnostic = "frame counter exhausted";
    } else if (strcmp(name, "malformed-frame") == 0) {
        FEED(f, "15", "1", "DU123,,DU456");
        expected = UMI_STATUS_PARSE_ERROR; step = UMI_IBKR_STEP_REVIEW_PROTOCOL;
        diagnostic = "malformed";
    } else if (strcmp(name, "deferred-frame") == 0) {
        for (size_t i = 0U; i < 63U; ++i) FEED(f, "999", "ignored");
        FEED(f, "4", "2", "-1", "2104", "Market data farm is connected", "");
        CHECK(Envelope(f, 0U) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, now++) == UMI_STATUS_OK);
        CHECK(f->c->snapshot.providerCode == 2104 && f->c->rxSize >= 4U);
        expected = UMI_STATUS_CAPACITY_EXCEEDED; step = UMI_IBKR_STEP_REVIEW_PROTOCOL;
        diagnostic = "Empty API frame";
    } else if (strcmp(name, "catalog-identity") == 0 ||
               strcmp(name, "catalog-body") == 0 || strcmp(name, "catalog-counter") == 0) {
        CHECK(UmiIbkrScannerCatalogRequest(f->c, now) == UMI_STATUS_OK);
        char *text = malloc(70001U);
        CHECK(text != NULL);
        memset(text, 'x', 70000U); text[70000U] = '\0';
        /* Wrong version exercises the promoted decoder after all bytes arrive;
         * wrong identity must fail before accepting that larger envelope. */
        FEED(f, strcmp(name, "catalog-identity") == 0 ? "79" : "19",
             strcmp(name, "catalog-body") == 0 ? "2" : "1", text);
        free(text);
        expected = UMI_STATUS_PARSE_ERROR; step = UMI_IBKR_STEP_REVIEW_PROTOCOL;
        diagnostic = strcmp(name, "catalog-identity") == 0 ?
                     "Unexpected large API message" : "Invalid scanner catalogue response";
        if (strcmp(name, "catalog-identity") != 0) {
            CHECK(UmiIbkrConnectionPump(f->c, now++) == UMI_STATUS_OK);
            CHECK(f->c->catalogFrame != NULL);
            if (strcmp(name, "catalog-counter") == 0) {
                f->c->snapshot.framesReceived = UINT64_MAX;
                expected = UMI_STATUS_CAPACITY_EXCEEDED; diagnostic = "frame counter exhausted";
            }
        }
    } else if (strcmp(name, "provider-client") == 0 || strcmp(name, "provider-lost") == 0 ||
               strcmp(name, "provider-endpoint") == 0) {
        const char *code = strcmp(name, "provider-client") == 0 ? "326" :
                           strcmp(name, "provider-endpoint") == 0 ? "502" : "1100";
        provider = strcmp(code, "326") == 0 ? 326 : strcmp(code, "502") == 0 ? 502 : 1100;
        FEED(f, "4", "2", "-1", code, "Current provider failure", "");
        expected = UMI_STATUS_UNAVAILABLE; diagnostic = "Current provider failure";
        step = provider == 326 ? UMI_IBKR_STEP_CHANGE_CLIENT :
               provider == 502 ? UMI_IBKR_STEP_CHECK_ENDPOINT : UMI_IBKR_STEP_REVIEW_PROVIDER;
    } else if (strcmp(name, "healthy-farm") == 0) {
        CHECK(UmiIbkrConnectionPump(f->c, now) == UMI_STATUS_OK);
        CHECK(f->c->snapshot.state == UMI_IBKR_READY && f->c->snapshot.providerCode == 2104);
        UmiIbkrConnectionGuidance guidance;
        CHECK(UmiIbkrConnectionExplain(&f->c->snapshot, &guidance) == UMI_STATUS_OK);
        CHECK(guidance.step == UMI_IBKR_STEP_READ_ACCOUNT && f->closes == 0U);
        Delete(f); return 0;
    } else if (strcmp(name, "fragmented-header") == 0) {
        f->input[f->inSize++] = 0U; f->input[f->inSize++] = 0U;
        CHECK(UmiIbkrConnectionPump(f->c, now++) == UMI_STATUS_OK);
        CHECK(f->c->snapshot.state == UMI_IBKR_READY && f->c->snapshot.providerCode == 2104);
        f->input[f->inSize++] = 0U; f->input[f->inSize++] = 0U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; step = UMI_IBKR_STEP_REVIEW_PROTOCOL;
        diagnostic = "Empty API frame";
    } else if (strcmp(name, "refresh-advice") == 0) {
        f->c->snapshot.requestIssued = true;
        f->c->snapshot.summaryComplete = true; f->c->snapshot.positionsComplete = true;
        UmiIbkrConnectionGuidance guidance;
        CHECK(UmiIbkrConnectionExplain(&f->c->snapshot, &guidance) == UMI_STATUS_OK);
        CHECK(guidance.step == UMI_IBKR_STEP_REVIEW_REPORT);
        CHECK(strstr(guidance.detail, "Refresh account") != NULL);
        CHECK(strstr(guidance.detail, "Reconnect to request") == NULL);
        CHECK(f->closes == 0U);
        Delete(f); return 0;
    } else { Delete(f); return 2; }
    CHECK(UmiIbkrConnectionPump(f->c, now) == expected);
    CHECK(Failed(f, expected, provider, step, diagnostic) == 0);
    Delete(f);
    return 0;
}
