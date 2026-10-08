/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_order_status.c
 * PURPOSE: Check cumulative order reports, identity isolation and malformed callback atomicity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "order_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    OrderFixtureReferences();
    const char *mode = argv[1];
    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    CHECK(UmiIbkrOrdersRequest(f->c, UMI_IBKR_ORDERS_ALL_CLIENTS, 10) == UMI_STATUS_OK);
    CHECK(StatusFeed(f, "42", "35", !strcmp(mode, "permanent-upgrade") ? "0" : "800", "Submitted", "10",
                     "6990") == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 11) == UMI_STATUS_OK);
    UmiIbkrRecoveredOrder first = f->c->orders.rows[0];
    CHECK(first.hasStatus && !first.hasOpenOrder && first.status.filled.value.coefficient == 10);
    if (!strcmp(mode, "valid"))
    {
        CHECK(first.status.remaining.value.coefficient == 6990 && first.revision == 1U);
    }
    else if (!strcmp(mode, "duplicate") || !strcmp(mode, "duplicate-overflow"))
    {
        if (!strcmp(mode, "duplicate-overflow"))
            f->c->orders.rows[0].duplicateStatuses = UINT64_MAX;
        CHECK(StatusFeed(f, "42", "35", "800", "Submitted", "10", "6990") == 0);
        UmiStatus result = UmiIbkrConnectionPump(f->c, 12);
        CHECK(result == (!strcmp(mode, "duplicate-overflow") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK));
        CHECK(f->c->orders.rows[0].revision == first.revision);
        if (!strcmp(mode, "duplicate"))
            CHECK(f->c->orders.rows[0].duplicateStatuses == 1U);
    }
    else if (!strcmp(mode, "correction") || !strcmp(mode, "filled") || !strcmp(mode, "inactive") ||
             !strcmp(mode, "unknown-state") || !strcmp(mode, "precision") || !strcmp(mode, "scientific") ||
             !strcmp(mode, "revision-overflow"))
    {
        const char *state = !strcmp(mode, "filled")          ? "Filled"
                            : !strcmp(mode, "inactive")      ? "Inactive"
                            : !strcmp(mode, "unknown-state") ? "ProviderPendingReview"
                                                             : "Submitted";
        const char *qty = !strcmp(mode, "correction")   ? "5"
                          : !strcmp(mode, "precision")  ? "0.0000000001"
                          : !strcmp(mode, "scientific") ? "1.25e2"
                                                        : "7000";
        if (!strcmp(mode, "revision-overflow"))
            f->c->orders.rows[0].revision = UINT64_MAX;
        CHECK(StatusFeed(f, "42", "35", "800", state, qty, "0") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12) ==
              (!strcmp(mode, "revision-overflow") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK));
        if (strcmp(mode, "revision-overflow"))
        {
            CHECK(!strcmp(f->c->orders.rows[0].status.filled.reportedText, qty));
            CHECK(!strcmp(f->c->orders.rows[0].status.status, state));
            CHECK(f->c->orders.rows[0].status.filled.exact == (strcmp(mode, "precision") != 0));
            CHECK(f->c->executions.count == 0U);
        }
    }
    else if (!strcmp(mode, "other-client") || !strcmp(mode, "negative-id") ||
             !strcmp(mode, "permanent-upgrade") || !strcmp(mode, "conflicting-permanent"))
    {
        CHECK(StatusFeed(
                  f, !strcmp(mode, "negative-id") ? "-9" : "42", !strcmp(mode, "other-client") ? "99" : "35",
                  !strcmp(mode, "conflicting-permanent") ? "801" : "800", "Submitted", "10", "6990") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12) ==
              (!strcmp(mode, "conflicting-permanent") ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK));
        CHECK(f->c->orders.snapshot.count ==
              (!strcmp(mode, "other-client") || !strcmp(mode, "negative-id") ? 2U : 1U));
        if (!strcmp(mode, "conflicting-permanent"))
            CHECK(f->c->orders.rows[0].permanentId == 800U);
        if (!strcmp(mode, "permanent-upgrade"))
            CHECK(f->c->orders.rows[0].permanentId == 800U);
    }
    else if (!strcmp(mode, "status-before-open") || !strcmp(mode, "status-age"))
    {
        CHECK(OpenFeed(f, "42", "35", "800", SIZE_MAX, NULL) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 20) == UMI_STATUS_OK);
        UmiIbkrRecoveredOrder row;
        CHECK(UmiIbkrOrderCopy(f->c, 0, 21, 10, &row) == UMI_STATUS_OK);
        CHECK(row.hasOpenOrder && row.hasStatus && row.revision == 2U);
        if (!strcmp(mode, "status-age"))
        {
            CHECK(UmiIbkrOrderCopy(f->c, 0, 22, 10, &row) == UMI_STATUS_OK);
            CHECK(row.statusStale && !row.openStale);
        }
    }
    else if (!strcmp(mode, "unbound") || !strcmp(mode, "unbound-missing"))
    {
        CHECK(StatusFeed(f, "0", "0", "801", "Submitted", "0", "1") == 0);
        CHECK(StatusFeed(f, "0", "0", !strcmp(mode, "unbound-missing") ? "0" : "802", "Submitted", "0",
                         "2") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12) ==
              (!strcmp(mode, "unbound-missing") ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK));
        CHECK(f->c->orders.snapshot.count == (!strcmp(mode, "unbound-missing") ? 2U : 3U));
    }
    else if (!strcmp(mode, "capacity"))
    {
        for (unsigned i = 1; i < UMI_IBKR_ORDER_LIMIT; ++i)
        {
            char id[24];
            (void)snprintf(id, sizeof id, "%u", i + 100U);
            CHECK(StatusFeed(f, id, "35", "0", "Submitted", "0", "1") == 0);
        }
        CHECK(UmiIbkrConnectionPump(f->c, 12) == UMI_STATUS_OK);
        CHECK(f->c->orders.snapshot.count == UMI_IBKR_ORDER_LIMIT);
        CHECK(StatusFeed(f, "999", "35", "0", "Submitted", "0", "1") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 13) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->c->orders.snapshot.count == UMI_IBKR_ORDER_LIMIT);
    }
    else if (!strcmp(mode, "invalid-copy"))
    {
        UmiIbkrRecoveredOrder out;
        memset(&out, 0x5a, sizeof out);
        UmiIbkrRecoveredOrder before = out;
        CHECK(UmiIbkrOrderCopy(f->c, 1, 12, 100, &out) == UMI_STATUS_NOT_FOUND);
        CHECK(!memcmp(&out, &before, sizeof out));
        CHECK(UmiIbkrOrderCopy(f->c, 0, 10, 100, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrOrderCopy(f->c, 0, 12, 0, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&out, &before, sizeof out));
    }
    else
    {
        const char *fields[] = {"3",   "42", "Submitted", "10", "6990", "3.25",
                                "800", "0",  "3.25",      "35", "",     "0"};
        size_t count = 12U;
        bool expected = true;
        if (!strcmp(mode, "negative-fill"))
            fields[3] = "-1";
        else if (!strcmp(mode, "nan"))
            fields[5] = "nan";
        else if (!strcmp(mode, "bad-client"))
            fields[9] = "-1";
        else if (!strcmp(mode, "bad-id"))
            fields[1] = "2147483648";
        else if (!strcmp(mode, "bad-permanent"))
            fields[6] = "9223372036854775808";
        else if (!strcmp(mode, "missing-field"))
            count = 11U;
        else if (!strcmp(mode, "invalid-utf8"))
            fields[10] = "\xc3";
        else if (!strcmp(mode, "unset"))
        {
            fields[5] = "1.7976931348623157E+308";
            expected = false;
        }
        else if (!strcmp(mode, "negative-price"))
        {
            fields[5] = "-3.25";
            expected = false;
        }
        else
            return 2;
        CHECK(Feed(f, fields, count) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12) == (expected ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK));
        if (expected)
            CHECK(!memcmp(&first, &f->c->orders.rows[0], sizeof first));
        else
            CHECK(f->c->orders.rows[0].status.averageFillPrice.exact == (strcmp(mode, "unset") != 0));
    }
    Delete(f);
    return 0;
}
