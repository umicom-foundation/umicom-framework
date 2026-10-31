/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_completed_snapshot.c
 *
 * PURPOSE:
 *   Verify owned completed-order report fidelity, age, scope and atomic failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "completed_fixture.h"

/* Independent wire fixtures exercise the real decoder and the owned report.
 * No native window, broker socket or order mutation is used by these cases. */
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    static const char *cases[] = {"invalid", "backward", "capacity", "no-request", "empty", "partial", "rows", "all-scope", "row-age", "age", "disconnected", "owned", "no-io"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0) known = true;
    if (!known) return 2;
    CompletedFixtureReferences();
    Fixture *f = New(); CHECK(f != NULL); CHECK(Connect(f) == 0);
    UmiIbkrCompletedOrderReport *out = malloc(sizeof(*out));
    UmiIbkrCompletedOrderReport *before = malloc(sizeof(*before));
    CHECK(out != NULL && before != NULL);
    memset(out, 0x5a, sizeof(*out)); memcpy(before, out, sizeof(*out));
    if (strcmp(mode, "invalid") == 0 || strcmp(mode, "backward") == 0 || strcmp(mode, "capacity") == 0) {
        if (strcmp(mode, "invalid") == 0) {
            CHECK(UmiIbkrCompletedOrderReportCopy(NULL, 10U, 100U, out) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrCompletedOrderReportCopy(f->c, 10U, 0U, out) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrCompletedOrderReportCopy(f->c, 10U, 100U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        } else if (strcmp(mode, "backward") == 0) {
            CHECK(UmiIbkrCompletedOrderReportCopy(f->c, 4U, 100U, out) == UMI_STATUS_INVALID_ARGUMENT);
        } else {
            f->c->completed.snapshot.count = UMI_IBKR_COMPLETED_ORDER_LIMIT + 1U;
            CHECK(UmiIbkrCompletedOrderReportCopy(f->c, 10U, 100U, out) == UMI_STATUS_CAPACITY_EXCEEDED);
            f->c->completed.snapshot.count = 0U;
        }
        CHECK(memcmp(out, before, sizeof(*out)) == 0);
    } else if (strcmp(mode, "no-request") == 0) {
        CHECK(UmiIbkrCompletedOrderReportCopy(f->c, 10U, 100U, out) == UMI_STATUS_OK);
        CHECK(!out->capture.requested && out->capture.stale && out->capture.count == 0U);
        UmiIbkrCompletedOrder zero = {0};
        CHECK(memcmp(&out->rows[0], &zero, sizeof(zero)) == 0);
    } else {
        bool apiOnly = strcmp(mode, "all-scope") != 0;
        CHECK(UmiIbkrCompletedOrdersRequest(f->c, apiOnly, 10U) == UMI_STATUS_OK);
        if (strcmp(mode, "empty") != 0) {
            CompletedFields fields = CompletedExample();
            fields.fields[CF_QUANTITY] = "9007199254740993.123456789";
            fields.fields[CF_FILLED] = "0.000000001";
            CHECK(CompletedFeed(f, &fields) == 0);
            CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        }
        uint64_t now = 13U, age = 100U;
        if (strcmp(mode, "partial") != 0) {
            FEED(f, "102");
            CHECK(UmiIbkrConnectionPump(f->c, strcmp(mode, "row-age") == 0 ? 40U : 12U) == UMI_STATUS_OK);
        }
        if (strcmp(mode, "row-age") == 0) { now = 50U; age = 15U; }
        if (strcmp(mode, "age") == 0) now = 113U;
        if (strcmp(mode, "disconnected") == 0) UmiIbkrConnectionClose(f->c);
        size_t bytesSent = f->outSize, queued = f->c->txSize;
        CHECK(UmiIbkrCompletedOrderReportCopy(f->c, now, age, out) == UMI_STATUS_OK);
        CHECK(out->capture.requested && out->capture.apiOnly == apiOnly);
        CHECK(out->capture.complete == (strcmp(mode, "partial") != 0));
        CHECK(out->capture.stale == (strcmp(mode, "partial") == 0 || strcmp(mode, "age") == 0 ||
            strcmp(mode, "disconnected") == 0));
        CHECK(f->outSize == bytesSent && f->c->txSize == queued);
        if (strcmp(mode, "empty") == 0) CHECK(out->capture.count == 0U);
        else {
            CHECK(out->capture.count == 1U && out->rows[0].permanentId == 800U);
            CHECK(strcmp(out->rows[0].order.totalQuantity.reportedText, "9007199254740993.123456789") == 0);
            CHECK(strcmp(out->rows[0].filledQuantity.reportedText, "0.000000001") == 0);
            CHECK(strcmp(out->rows[0].completedStatus, "Cancelled") == 0);
            CHECK(out->rows[0].stale == (strcmp(mode, "row-age") == 0 || strcmp(mode, "age") == 0 ||
                strcmp(mode, "disconnected") == 0));
        }
        UmiIbkrCompletedOrder zero = {0};
        CHECK(memcmp(&out->rows[out->capture.count], &zero, sizeof(zero)) == 0);
        if (strcmp(mode, "owned") == 0) {
            Delete(f); f = NULL;
            CHECK(strcmp(out->rows[0].order.account, "DU123") == 0);
            CHECK(strcmp(out->rows[0].completedTime, "20261007 13:25:10 UTC") == 0);
        }
    }
    if (f != NULL) Delete(f);
    free(before); free(out);
    return 0;
}
