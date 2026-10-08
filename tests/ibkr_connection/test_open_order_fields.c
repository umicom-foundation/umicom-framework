/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_open_order_fields.c
 * PURPOSE: Verify order-prefix fields and lossless bounded retention of uninterpreted extensions.
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
    size_t change = SIZE_MAX;
    const char *value = NULL;
    bool invalid = false;
    if (!strcmp(mode, "unicode"))
    {
        change = 3;
        value = "caf\xc3\xa9";
    }
    else if (!strcmp(mode, "negative-price"))
    {
        change = 16;
        value = "-1.25";
    }
    else if (!strcmp(mode, "unset"))
    {
        change = 16;
        value = "1.7976931348623157e308";
    }
    else if (!strcmp(mode, "precision"))
    {
        change = 14;
        value = "0.0000000001";
    }
    else if (!strcmp(mode, "bad-quantity"))
    {
        change = 14;
        value = "-1";
        invalid = true;
    }
    else if (!strcmp(mode, "bad-contract"))
    {
        change = 2;
        value = "2147483648";
        invalid = true;
    }
    else if (!strcmp(mode, "bad-client"))
    {
        change = 24;
        value = "-1";
        invalid = true;
    }
    else if (!strcmp(mode, "bad-utf8"))
    {
        change = 3;
        value = "\xc3";
        invalid = true;
    }
    else if (!strcmp(mode, "bad-price"))
    {
        change = 16;
        value = "inf";
        invalid = true;
    }
    CHECK(OpenFeed(f, "42", "35", "800", change, value) == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 11) == (invalid ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK));
    if (invalid)
    {
        CHECK(f->c->orders.snapshot.count == 0U);
        Delete(f);
        return 0;
    }
    UmiIbkrRecoveredOrder row;
    CHECK(UmiIbkrOrderCopy(f->c, 0, 11, 100, &row) == UMI_STATUS_OK);
    CHECK(row.hasOpenOrder && !row.hasStatus && row.open.wireFieldCount == 30U);
    if (!strcmp(mode, "valid"))
    {
        CHECK(row.permanentId == 800U && row.open.contractId == 123U);
        CHECK(!strcmp(row.open.account, "DU123") && !strcmp(row.open.orderType, "LMT"));
        CHECK(row.open.totalQuantity.exact && row.open.totalQuantity.value.coefficient == 7000);
    }
    else if (!strcmp(mode, "unicode"))
        CHECK(!strcmp(row.open.symbol, "caf\xc3\xa9"));
    else if (!strcmp(mode, "negative-price"))
        CHECK(row.open.limitPrice.value.coefficient == -125);
    else if (!strcmp(mode, "unset"))
        CHECK(!row.open.limitPrice.exact && row.open.limitPrice.reportedText[0]);
    else if (!strcmp(mode, "precision"))
        CHECK(!row.open.totalQuantity.exact);
    else if (!strcmp(mode, "raw-field") || !strcmp(mode, "raw-short") || !strcmp(mode, "raw-query") ||
             !strcmp(mode, "raw-empty") || !strcmp(mode, "raw-invalid") || !strcmp(mode, "disconnect"))
    {
        char buffer[64] = "unchanged";
        size_t required = 12345U;
        if (!strcmp(mode, "disconnect"))
            UmiIbkrConnectionClose(f->c);
        if (!strcmp(mode, "raw-short") || !strcmp(mode, "raw-query"))
        {
            CHECK(UmiIbkrOpenOrderFieldCopy(f->c, 0, 28, !strcmp(mode, "raw-query") ? NULL : buffer,
                                            !strcmp(mode, "raw-query") ? 0U : 2U,
                                            &required) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(required == 17U && !strcmp(buffer, "unchanged"));
        }
        else if (!strcmp(mode, "raw-invalid"))
        {
            CHECK(UmiIbkrOpenOrderFieldCopy(f->c, 0, 30, buffer, sizeof buffer, &required) ==
                  UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrOpenOrderFieldCopy(f->c, 1, 0, buffer, sizeof buffer, &required) ==
                  UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrOpenOrderFieldCopy(f->c, 0, 0, NULL, 1U, &required) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(required == 12345U && !strcmp(buffer, "unchanged"));
        }
        else
        {
            bool empty = !strcmp(mode, "raw-empty");
            CHECK(UmiIbkrOpenOrderFieldCopy(f->c, 0, empty ? 29U : 28U, buffer, sizeof buffer, &required) ==
                  UMI_STATUS_OK);
            CHECK(!strcmp(buffer, empty ? "" : "opaque-extension"));
            if (!strcmp(mode, "disconnect"))
            {
                CHECK(UmiIbkrOrderCopy(f->c, 0, 12, 100, &row) == UMI_STATUS_OK);
                CHECK(row.openStale);
            }
        }
    }
    else if (!strcmp(mode, "duplicate") || !strcmp(mode, "replace"))
    {
        CHECK(OpenFeed(f, "42", "35", "800", !strcmp(mode, "replace") ? 16U : SIZE_MAX, "3.50") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12) == UMI_STATUS_OK);
        CHECK(f->c->orders.snapshot.count == 1U);
        CHECK(f->c->orders.rows[0].revision == (!strcmp(mode, "replace") ? 2U : 1U));
        char text[32];
        size_t required;
        CHECK(UmiIbkrOpenOrderFieldCopy(f->c, 0, 16, text, sizeof text, &required) == UMI_STATUS_OK);
        CHECK(!strcmp(text, !strcmp(mode, "replace") ? "3.50" : "3.25"));
    }
    else if (!strcmp(mode, "truncated"))
    {
        FEED(f, "5", "42", "123");
        CHECK(UmiIbkrConnectionPump(f->c, 12) == UMI_STATUS_PARSE_ERROR);
        CHECK(f->c->orders.rows[0].revision == 1U);
    }
    else
        return 2;
    Delete(f);
    return 0;
}
