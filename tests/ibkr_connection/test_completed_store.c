/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_completed_store.c
 * PURPOSE: Check completed-order identity, replay handling, raw evidence and bounded retention.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "completed_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    /* Reject misspelled case names so a registration cannot silently run a default. */
    static const char *const cases[] = {"duplicate", "duplicate-overflow", "conflict",  "capacity",
                                        "age",       "disconnect",         "raw",       "raw-short",
                                        "raw-query", "raw-invalid",        "raw-empty", "invalid-copy"};
    bool registered = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(argv[1], cases[i]))
            registered = true;
    if (!registered)
        return 2;
    CompletedFixtureReferences();
    const char *mode = argv[1];
    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 10U) == UMI_STATUS_OK);
    CompletedFields fields = CompletedExample();
    CHECK(CompletedFeed(f, &fields) == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    if (!strcmp(mode, "duplicate") || !strcmp(mode, "duplicate-overflow") || !strcmp(mode, "conflict"))
    {
        if (!strcmp(mode, "duplicate-overflow"))
            f->c->completed.rows[0].duplicateCount = UINT64_MAX;
        if (!strcmp(mode, "conflict"))
            fields.fields[CF_FILLED] = "11";
        CHECK(CompletedFeed(f, &fields) == 0);
        UmiStatus expected = !strcmp(mode, "duplicate-overflow") ? UMI_STATUS_CAPACITY_EXCEEDED
                             : !strcmp(mode, "conflict")         ? UMI_STATUS_PARSE_ERROR
                                                                 : UMI_STATUS_OK;
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == expected);
        CHECK(f->c->completed.snapshot.count == 1U);
        CHECK(!strcmp(f->c->completed.rows[0].filledQuantity.reportedText, "10"));
        CHECK(f->c->completed.rows[0].receivedAtMilliseconds == 11U);
        if (!strcmp(mode, "duplicate"))
            CHECK(f->c->completed.rows[0].duplicateCount == 1U);
    }
    else if (!strcmp(mode, "capacity"))
    {
        char id[32];
        for (size_t i = 1U; i < UMI_IBKR_COMPLETED_ORDER_LIMIT; ++i)
        {
            snprintf(id, sizeof id, "%u", 800U + (unsigned)i);
            fields.fields[CF_PERMANENT] = id;
            CHECK(CompletedFeed(f, &fields) == 0);
            CHECK(UmiIbkrConnectionPump(f->c, 11U + i) == UMI_STATUS_OK);
        }
        fields.fields[CF_PERMANENT] = "9999";
        CHECK(CompletedFeed(f, &fields) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 100U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->c->completed.snapshot.count == UMI_IBKR_COMPLETED_ORDER_LIMIT);
    }
    else if (!strcmp(mode, "age") || !strcmp(mode, "disconnect"))
    {
        FEED(f, "102");
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
        if (!strcmp(mode, "disconnect"))
            UmiIbkrConnectionClose(f->c);
        UmiIbkrCompletedOrder row;
        CHECK(UmiIbkrCompletedOrderCopy(f->c, 0U, 15012U, 15000U, &row) == UMI_STATUS_OK);
        CHECK(row.stale && row.permanentId == 800U);
    }
    else if (!strcmp(mode, "raw") || !strcmp(mode, "raw-short") || !strcmp(mode, "raw-query") ||
             !strcmp(mode, "raw-invalid") || !strcmp(mode, "raw-empty"))
    {
        char buffer[64] = "unchanged";
        size_t needed = 999U;
        size_t index = !strcmp(mode, "raw-empty") ? CF_EXPIRY : CF_REFERENCE;
        if (!strcmp(mode, "raw-short"))
        {
            CHECK(UmiIbkrCompletedOrderFieldCopy(f->c, 0U, index, buffer, 2U, &needed) ==
                  UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(!strcmp(buffer, "unchanged") && needed == 13U);
        }
        else if (!strcmp(mode, "raw-query"))
        {
            CHECK(UmiIbkrCompletedOrderFieldCopy(f->c, 0U, index, NULL, 0U, &needed) ==
                  UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(needed == 13U);
        }
        else if (!strcmp(mode, "raw-invalid"))
        {
            CHECK(UmiIbkrCompletedOrderFieldCopy(f->c, 0U, 999U, buffer, sizeof buffer, &needed) ==
                  UMI_STATUS_NOT_FOUND);
            CHECK(!strcmp(buffer, "unchanged") && needed == 999U);
        }
        else
        {
            CHECK(UmiIbkrCompletedOrderFieldCopy(f->c, 0U, index, buffer, sizeof buffer, &needed) ==
                  UMI_STATUS_OK);
            CHECK(!strcmp(buffer, fields.fields[index]) && needed == strlen(fields.fields[index]) + 1U);
        }
    }
    else if (!strcmp(mode, "invalid-copy"))
    {
        UmiIbkrCompletedOrder row = {0};
        row.permanentId = 9000U;
        CHECK(UmiIbkrCompletedOrderCopy(f->c, 1U, 11U, 15000U, &row) == UMI_STATUS_NOT_FOUND);
        CHECK(row.permanentId == 9000U);
    }
    else
        return 2;
    Delete(f);
    return 0;
}
