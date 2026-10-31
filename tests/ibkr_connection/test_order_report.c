/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_order_report.c
 *
 * PURPOSE:
 *   Verify atomic order report copies preserve exact observations and independent freshness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "order_fixture.h"

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    OrderFixtureReferences();
    Fixture *f = New();
    CHECK(f != NULL && Connect(f) == 0);
    UmiIbkrOrderReport *report = malloc(sizeof(*report));
    UmiIbkrOrderReport *before = malloc(sizeof(*before));
    CHECK(report != NULL && before != NULL);
    memset(report, 0x5a, sizeof(*report));
    *before = *report;
    const char *mode = argv[1];
    if (strcmp(mode, "null") == 0 || strcmp(mode, "age-zero") == 0 ||
        strcmp(mode, "backward") == 0) {
        CHECK(UmiIbkrOrderReportCopy(strcmp(mode, "null") == 0 ? NULL : f->c,
            strcmp(mode, "backward") == 0 ? 4U : 10U,
            strcmp(mode, "age-zero") == 0 ? 0U : 15000U, report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrOrderReportCopy(f->c, 10U, 15000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, report, sizeof(*report)) == 0);
    } else {
        CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_ALL_CLIENTS, 10U) == UMI_STATUS_OK);
        bool empty = strcmp(mode, "empty") == 0;
        bool pending = strcmp(mode, "pending") == 0;
        bool partial = strcmp(mode, "partial") == 0;
        bool timeout = strcmp(mode, "timeout") == 0;
        if (!empty && !pending) {
            CHECK(OpenFeed(f, "12", "99", "800", 14U, "7000.000000001") == 0);
            CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
            if (!partial) {
                CHECK(StatusFeed(f, "12", "99", "800", "Submitted", "0.000000001", "7000") == 0);
                CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
            }
        }
        if (!pending && !partial && !timeout) {
            FEED(f, "53", "1");
            CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
        }
        if (strcmp(mode, "capacity") == 0) {
            f->c->orders.snapshot.count = UMI_IBKR_ORDER_LIMIT + 1U;
            CHECK(UmiIbkrOrderReportCopy(f->c, 14U, 15000U, report) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(memcmp(before, report, sizeof(*report)) == 0);
        } else {
            if (strcmp(mode, "disconnect") == 0) UmiIbkrConnectionClose(f->c);
            size_t queued = f->c->txSize, received = f->inAt, sent = f->outSize;
            uint64_t now = timeout ? 10010U : strcmp(mode, "age") == 0 ? 15014U : 14U;
            CHECK(UmiIbkrOrderReportCopy(f->c, now, 15000U, report) == UMI_STATUS_OK);
            CHECK(f->c->txSize == queued && f->inAt == received && f->outSize == sent);
            CHECK(report->capture.scope == UMI_IBKR_ORDERS_ALL_CLIENTS);
            CHECK(report->capture.count == (empty || pending ? 0U : 1U));
            if (empty) CHECK(report->capture.complete && !report->capture.stale);
            if (pending) CHECK(report->capture.pending && !report->capture.complete && report->capture.stale);
            if (partial) CHECK(report->rows[0].hasOpenOrder && !report->rows[0].hasStatus &&
                report->rows[0].statusStale && report->capture.stale);
            if (timeout) CHECK(report->capture.failed && !report->capture.pending &&
                report->capture.stale && !report->capture.complete);
            if (strcmp(mode, "age") == 0 || strcmp(mode, "disconnect") == 0)
                CHECK(report->capture.stale && report->rows[0].openStale && report->rows[0].statusStale);
            if (!empty && !pending) {
                CHECK(report->rows[0].key.clientId == 99 && report->rows[0].key.orderId == 12);
                CHECK(strcmp(report->rows[0].open.totalQuantity.reportedText, "7000.000000001") == 0);
                if (!partial) CHECK(strcmp(report->rows[0].status.filled.reportedText, "0.000000001") == 0);
            }
            if (strcmp(mode, "complete") == 0)
                CHECK(report->capture.complete && !report->capture.stale &&
                    !report->rows[0].openStale && !report->rows[0].statusStale);
            if (strcmp(mode, "ownership") == 0) {
                *before = *report;
                f->c->orders.rows[0].open.account[0] = 'X';
                CHECK(memcmp(before, report, sizeof(*report)) == 0);
                CHECK(UmiIbkrOrderReportCopy(f->c, 14U, 15000U, report) == UMI_STATUS_OK);
                CHECK(report->rows[0].open.account[0] == 'X');
            }
            UmiIbkrRecoveredOrder zero = {0};
            CHECK(memcmp(&report->rows[report->capture.count], &zero, sizeof(zero)) == 0);
        }
    }
    free(before); free(report); Delete(f);
    return 0;
}
