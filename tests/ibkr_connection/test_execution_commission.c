/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_execution_commission.c
 * PURPOSE: Check commission correlation, missing reports, conflicting updates and exact totals with injected provider frames.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static int Execution(Fixture *f, const char *id, const char *quantity)
{
    const char *fields[] = {"11",    "36000", "42",  "123",    "EXAMPLE", "STK", "",  "0",
                            "",      "",      "LSE", "GBP",    "EXM",     "EXM", id,  "20261007 12:00:00 UTC",
                            "DU123", "LSE",   "BOT", quantity, "1.23",    "77",  "9", "0",
                            "7000",  "1.23",  "",    "",       "0",       "",    "1"};
    return Feed(f, fields, sizeof fields / sizeof fields[0]);
}
static int Fee(Fixture *f, const char *id, const char *amount, const char *currency)
{
    const char *fields[] = {
        "59", "1", id, amount, currency, "1.7976931348623157e+308", "1.7976931348623157e+308", "0"};
    return Feed(f, fields, sizeof fields / sizeof fields[0]);
}
static int Run(Fixture *f, UmiIbkrExecutionSnapshot *copy, const char *mode)
{
    (void)PositionFeed;
    CHECK(Connect(f) == 0);
    uint32_t request = 0U;
    CHECK(UmiIbkrExecutionsRequest(f->c, "DU123", 10U, &request) == UMI_STATUS_OK);
    if (!strcmp(mode, "early"))
    {
        CHECK(Fee(f, "one.01", "3", "GBP") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    }
    CHECK(Execution(f, "one.01", "10") == 0);
    CHECK(Execution(f, "two.01", "6990") == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
    if (!strcmp(mode, "after-end"))
    {
        FEED(f, "55", "1", "36000");
        CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
    }
    if (!strcmp(mode, "early") || !strcmp(mode, "missing"))
    {
        CHECK(Fee(f, "two.01", "2", "GBP") == 0);
    }
    else
    {
        CHECK(Fee(f, "one.01",
                  !strcmp(mode, "rebate")            ? "-0.5"
                  : !strcmp(mode, "unrepresentable") ? "1.7976931348623157e+308"
                  : !strcmp(mode, "precision")       ? "0.0000000001"
                                                     : "3",
                  "GBP") == 0);
        CHECK(Fee(f, "two.01", !strcmp(mode, "overflow") ? "9223372036854775807" : "2",
                  !strcmp(mode, "mixed-currency") ? "USD" : "GBP") == 0);
    }
    if (!strcmp(mode, "duplicate"))
        CHECK(Fee(f, "one.01", "3.000", "GBP") == 0);
    if (!strcmp(mode, "conflicting"))
        CHECK(Fee(f, "one.01", "4", "GBP") == 0);
    if (!strcmp(mode, "changed-currency"))
        CHECK(Fee(f, "one.01", "3", "USD") == 0);
    if (!strcmp(mode, "unknown"))
        CHECK(Fee(f, "foreign.01", "99", "USD") == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 14U) == UMI_STATUS_OK);
    if (strcmp(mode, "after-end"))
    {
        FEED(f, "55", "1", "36000");
        CHECK(UmiIbkrConnectionPump(f->c, 15U) == UMI_STATUS_OK);
    }
    if (!strcmp(mode, "expired"))
    {
        CHECK(Fee(f, "one.01", "99", "GBP") == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 10010U) == UMI_STATUS_OK);
    }
    CHECK(UmiIbkrExecutionsCopy(f->c, request, !strcmp(mode, "expired") ? 10010U : 15U, copy) ==
          UMI_STATUS_OK);
    CHECK(copy->complete && !copy->failed);
    if (!strcmp(mode, "corrupt-value"))
        copy->fees[0].amount.coefficient = 999;
    if (!strcmp(mode, "corrupt-currency"))
        memset(copy->fees[0].firstCurrency, 'x', sizeof copy->fees[0].firstCurrency);
    UmiIbkrExecutionCommissionReview out = {0};
    out.executionCount = 999U;
    UmiStatus status = UmiIbkrExecutionsReviewCommission(copy, 77U, 123U, "BOT", &out);
    if (!strcmp(mode, "early") || !strcmp(mode, "missing") || !strcmp(mode, "precision") ||
        !strcmp(mode, "unrepresentable"))
    {
        CHECK(status == UMI_STATUS_UNAVAILABLE && out.executionCount == 999U);
        return 0;
    }
    if (!strcmp(mode, "mixed-currency"))
    {
        CHECK(status == UMI_STATUS_NOT_IMPLEMENTED && out.executionCount == 999U);
        return 0;
    }
    if (!strcmp(mode, "conflicting") || !strcmp(mode, "changed-currency") || !strcmp(mode, "corrupt-value") ||
        !strcmp(mode, "corrupt-currency"))
    {
        CHECK(status == UMI_STATUS_INVALID_STATE && out.executionCount == 999U);
        if (!strcmp(mode, "conflicting"))
        {
            CHECK(strcmp(copy->fees[0].firstReportedAmount, "3") == 0 &&
                  strcmp(copy->fees[0].latestReportedAmount, "4") == 0);
        }
        return 0;
    }
    if (!strcmp(mode, "overflow"))
    {
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && out.executionCount == 999U);
        return 0;
    }
    if (strcmp(mode, "valid") && strcmp(mode, "after-end") && strcmp(mode, "duplicate") &&
        strcmp(mode, "unknown") && strcmp(mode, "rebate") && strcmp(mode, "expired"))
        return 2;
    CHECK(status == UMI_STATUS_OK && out.executionCount == 2U && strcmp(out.currency, "GBP") == 0);
    UmiDecimal expected = !strcmp(mode, "rebate") ? (UmiDecimal){15, 1U} : (UmiDecimal){5, 0U};
    int comparison = 0;
    CHECK(UmiDecimalCompare(out.amount, expected, &comparison) == UMI_STATUS_OK && comparison == 0);
    CHECK(!copy->fees[0].conflicting);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    Fixture *f = New();
    UmiIbkrExecutionSnapshot *copy = calloc(1, sizeof *copy);
    if (f == NULL || copy == NULL)
    {
        Delete(f);
        free(copy);
        return 1;
    }
    int result = Run(f, copy, argv[1]);
    free(copy);
    Delete(f);
    return result;
}
