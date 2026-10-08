/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_completed_layout.c
 * PURPOSE: Exercise variable completed-order layouts and reject ambiguous field boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "completed_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    /* Reject misspelled case names so a registration cannot silently run a default. */
    static const char *const cases[] = {"valid",
                                        "legacy",
                                        "peg-threshold",
                                        "precision",
                                        "scientific",
                                        "negative-price",
                                        "scale-unset",
                                        "unicode",
                                        "truncated",
                                        "extra-field",
                                        "delta-order",
                                        "combo",
                                        "leg-prices",
                                        "smart-tags",
                                        "scale",
                                        "hedge",
                                        "delta-contract",
                                        "algorithm",
                                        "peg-benchmark",
                                        "condition-price",
                                        "condition-time",
                                        "condition-margin",
                                        "condition-execution",
                                        "condition-volume",
                                        "condition-percent",
                                        "condition-unknown",
                                        "condition-connector",
                                        "negative-quantity",
                                        "nan-price",
                                        "bad-flag",
                                        "bad-permanent",
                                        "bad-contract",
                                        "bad-utf8",
                                        "missing-account",
                                        "negative-filled",
                                        "combo-limit",
                                        "tag-limit",
                                        "condition-limit",
                                        "scale-inexact"};
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
    CompletedFields value = CompletedExample();
    UmiStatus expected = UMI_STATUS_OK;
    bool known = false;
    struct Invalid
    {
        const char *name;
        size_t field;
        const char *text;
        UmiStatus status;
    };
    static const struct Invalid invalid[] = {
        {"negative-quantity", CF_QUANTITY, "-1", UMI_STATUS_PARSE_ERROR},
        {"nan-price", CF_LIMIT, "NaN", UMI_STATUS_PARSE_ERROR},
        {"bad-flag", CF_ALL_OR_NONE, "2", UMI_STATUS_PARSE_ERROR},
        {"bad-permanent", CF_PERMANENT, "0", UMI_STATUS_PARSE_ERROR},
        {"bad-contract", CF_CONTRACT, "-1", UMI_STATUS_PARSE_ERROR},
        {"bad-utf8", CF_SYMBOL, "\xff", UMI_STATUS_PARSE_ERROR},
        {"missing-account", CF_ACCOUNT, "", UMI_STATUS_PARSE_ERROR},
        {"negative-filled", CF_FILLED, "-0.1", UMI_STATUS_PARSE_ERROR},
        {"combo-limit", CF_COMBO_COUNT, "33", UMI_STATUS_PARSE_ERROR},
        {"tag-limit", CF_SMART_COUNT, "65", UMI_STATUS_PARSE_ERROR},
        {"condition-limit", CF_CONDITIONS, "33", UMI_STATUS_PARSE_ERROR},
        {"scale-inexact", CF_SCALE_INCREMENT, "1e-10", UMI_STATUS_NOT_IMPLEMENTED}};
    for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i)
        if (!strcmp(mode, invalid[i].name))
        {
            value.fields[invalid[i].field] = invalid[i].text;
            expected = invalid[i].status;
            known = true;
            break;
        }
    if (!strcmp(mode, "legacy"))
    {
        value.count = CF_MIN_TRADE;
        f->c->snapshot.protocolVersion = 151;
        known = true;
    }
    else if (!strcmp(mode, "peg-threshold"))
    {
        f->c->snapshot.protocolVersion = 170;
        known = true;
    }
    else if (!strcmp(mode, "precision"))
    {
        value.fields[CF_FILLED] = "0.0000000001";
        known = true;
    }
    else if (!strcmp(mode, "scientific"))
    {
        value.fields[CF_FILLED] = "1e1";
        known = true;
    }
    else if (!strcmp(mode, "negative-price"))
    {
        value.fields[CF_LIMIT] = "-4.25";
        known = true;
    }
    else if (!strcmp(mode, "scale-unset"))
    {
        value.fields[CF_SCALE_INCREMENT] = "1.7976931348623157e+308";
        known = true;
    }
    else if (!strcmp(mode, "unicode"))
    {
        value.fields[CF_REFERENCE] = "caf\xc3\xa9";
        known = true;
    }
    else if (!strcmp(mode, "truncated"))
    {
        --value.count;
        expected = UMI_STATUS_PARSE_ERROR;
        known = true;
    }
    else if (!strcmp(mode, "extra-field"))
    {
        value.fields[value.count++] = "unexpected";
        expected = UMI_STATUS_PARSE_ERROR;
        known = true;
    }
    else if (!strcmp(mode, "delta-order"))
    {
        value.fields[CF_NEUTRAL_TYPE] = "MKT";
        const char *extra[] = {"321", "0", "0", ""};
        CHECK(CompletedInsert(&value, CF_CONTINUOUS, extra, 4U) == 0);
        known = true;
    }
    else if (!strcmp(mode, "combo"))
    {
        value.fields[CF_COMBO_COUNT] = "1";
        const char *extra[] = {"321", "1", "BUY", "SMART", "0", "0", "", "-1"};
        CHECK(CompletedInsert(&value, CF_LEG_PRICE_COUNT, extra, 8U) == 0);
        known = true;
    }
    else if (!strcmp(mode, "leg-prices"))
    {
        value.fields[CF_LEG_PRICE_COUNT] = "2";
        const char *extra[] = {"4.25", "5.25"};
        CHECK(CompletedInsert(&value, CF_SMART_COUNT, extra, 2U) == 0);
        known = true;
    }
    else if (!strcmp(mode, "smart-tags"))
    {
        value.fields[CF_SMART_COUNT] = "1";
        const char *extra[] = {"NonGuaranteed", "1"};
        CHECK(CompletedInsert(&value, CF_SCALE_INITIAL, extra, 2U) == 0);
        known = true;
    }
    else if (!strcmp(mode, "scale"))
    {
        value.fields[CF_SCALE_INCREMENT] = "0.5";
        const char *extra[] = {"0.25", "60", "0.1", "0", "100", "10", "0"};
        CHECK(CompletedInsert(&value, CF_HEDGE, extra, 7U) == 0);
        known = true;
    }
    else if (!strcmp(mode, "hedge"))
    {
        value.fields[CF_HEDGE] = "D";
        const char *extra[] = {"1"};
        CHECK(CompletedInsert(&value, CF_CLEARING_ACCOUNT, extra, 1U) == 0);
        known = true;
    }
    else if (!strcmp(mode, "delta-contract"))
    {
        value.fields[CF_NEUTRAL_PRESENT] = "1";
        const char *extra[] = {"321", "0.5", "4.25"};
        CHECK(CompletedInsert(&value, CF_ALGORITHM, extra, 3U) == 0);
        known = true;
    }
    else if (!strcmp(mode, "algorithm"))
    {
        value.fields[CF_ALGORITHM] = "Adaptive";
        const char *extra[] = {"1", "adaptivePriority", "Normal"};
        CHECK(CompletedInsert(&value, CF_SOLICITED, extra, 3U) == 0);
        known = true;
    }
    else if (!strcmp(mode, "peg-benchmark"))
    {
        value.fields[CF_TYPE] = "PEG BENCH";
        const char *extra[] = {"321", "0", "0.1", "0.2", "LSE"};
        CHECK(CompletedInsert(&value, CF_CONDITIONS, extra, 5U) == 0);
        known = true;
    }
    else if (!strncmp(mode, "condition-", 10U) && strcmp(mode, "condition-limit"))
    {
        const char *kind = mode + 10U;
        value.fields[CF_CONDITIONS] = "1";
        const char *extra[16] = {0};
        size_t count = 0U;
        if (!strcmp(kind, "price"))
        {
            const char *x[] = {"1", "a", "1", "4.25", "123", "SMART", "0"};
            memcpy(extra, x, sizeof x);
            count = 7U;
        }
        else if (!strcmp(kind, "time"))
        {
            const char *x[] = {"3", "o", "1", "20261007 13:00:00 UTC"};
            memcpy(extra, x, sizeof x);
            count = 4U;
        }
        else if (!strcmp(kind, "margin"))
        {
            const char *x[] = {"4", "a", "1", "30"};
            memcpy(extra, x, sizeof x);
            count = 4U;
        }
        else if (!strcmp(kind, "execution"))
        {
            const char *x[] = {"5", "a", "STK", "LSE", "WORKSHOP"};
            memcpy(extra, x, sizeof x);
            count = 5U;
        }
        else if (!strcmp(kind, "volume"))
        {
            const char *x[] = {"6", "a", "1", "1000", "123", "SMART"};
            memcpy(extra, x, sizeof x);
            count = 6U;
        }
        else if (!strcmp(kind, "percent"))
        {
            const char *x[] = {"7", "a", "1", "2.5", "123", "SMART"};
            memcpy(extra, x, sizeof x);
            count = 6U;
        }
        else if (!strcmp(kind, "unknown"))
        {
            const char *x[] = {"99", "a"};
            memcpy(extra, x, sizeof x);
            count = 2U;
            expected = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else if (!strcmp(kind, "connector"))
        {
            const char *x[] = {"3", "x", "1", "20261007 13:00:00 UTC"};
            memcpy(extra, x, sizeof x);
            count = 4U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else
            return 2;
        extra[count++] = "0";
        extra[count++] = "0";
        CHECK(CompletedInsert(&value, CF_STOP, extra, count) == 0);
        known = true;
    }
    else if (!strcmp(mode, "valid"))
        known = true;
    CHECK(known);
    CHECK(UmiIbkrCompletedOrdersRequest(f->c, true, 10U) == UMI_STATUS_OK);
    CHECK(CompletedFeed(f, &value) == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == expected);
    if (expected == UMI_STATUS_OK)
    {
        UmiIbkrCompletedOrder row;
        CHECK(UmiIbkrCompletedOrderCopy(f->c, 0U, 11U, 15000U, &row) == UMI_STATUS_OK);
        CHECK(row.permanentId == 800U && row.allOrNone && !row.minimumQuantity.exact);
        CHECK(!strcmp(row.completedStatus, "Cancelled") &&
              !strcmp(row.completedTime, "20261007 13:25:10 UTC"));
        CHECK(row.order.wireFieldCount == value.count);
        CHECK(!strcmp(row.filledQuantity.reportedText, value.fields[CF_FILLED]) ||
              !strcmp(row.filledQuantity.reportedText, "10"));
        if (!strcmp(mode, "precision"))
            CHECK(!row.filledQuantity.exact);
        if (!strcmp(mode, "scientific"))
            CHECK(row.filledQuantity.exact);
        if (!strcmp(mode, "combo"))
            CHECK(row.comboLegCount == 1U);
        if (!strncmp(mode, "condition-", 10U))
            CHECK(row.conditionCount == 1U);
    }
    else
        CHECK(f->c->completed.snapshot.count == 0U && f->c->snapshot.stale);
    Delete(f);
    return 0;
}
