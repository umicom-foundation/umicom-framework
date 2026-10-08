/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_completed_review.c
 * PURPOSE: Compare final filled quantities without confusing cancellations, corrections or missing fees.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "completed_fixture.h"
static int Fill(Fixture *f, const char *request, const char *id, const char *quantity, const char *contract,
                const char *side, const char *currency)
{
    const char *fields[] = {
        "11",    request, "42",           contract, "WORKSHOP", "STK",      "",   "0",
        "",      "",      "LSE",          currency, "WORKSHOP", "WORKSHOP", id,   "20261007 13:24:00 UTC",
        "DU123", "LSE",   side,           quantity, "4.25",     "800",      "35", "0",
        "10",    "4.25",  "review order", "",       "0",        "",         "1"};
    return Feed(f, fields, sizeof fields / sizeof fields[0]);
}
static int Fee(Fixture *f, const char *id, const char *amount, const char *currency)
{
    const char *fields[] = {
        "59", "1", id, amount, currency, "1.7976931348623157e+308", "1.7976931348623157e+308", "0"};
    return Feed(f, fields, sizeof fields / sizeof fields[0]);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    /* Reject misspelled case names so a registration cannot silently run a default. */
    static const char *const cases[] = {
        "cancelled-partial", "below",         "above",   "missing-fee", "conflicting-fee",
        "mixed-fees",        "correction",    "account", "zero",        "precision",
        "unsupported",       "contract",      "side",    "currency",    "completed-pending",
        "execution-pending", "wrong-request", "age",     "disconnect"};
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
    uint32_t request = 0U;
    CHECK(UmiIbkrExecutionsRequest(f->c, "DU123", 10U, &request) == UMI_STATUS_OK);
    char requestText[24];
    snprintf(requestText, sizeof requestText, "%u", (unsigned)request);
    CompletedFields fields = CompletedExample();
    if (!strcmp(mode, "account"))
        fields.fields[CF_ACCOUNT] = "DU456";
    if (!strcmp(mode, "zero"))
        fields.fields[CF_FILLED] = "0";
    if (!strcmp(mode, "precision"))
        fields.fields[CF_FILLED] = "0.0000000001";
    if (!strcmp(mode, "unsupported"))
        fields.fields[CF_SECURITY] = "FUT";
    CHECK(CompletedFeed(f, &fields) == 0);
    if (strcmp(mode, "completed-pending"))
        FEED(f, "102");
    bool two = !strcmp(mode, "correction") || !strcmp(mode, "mixed-fees");
    CHECK(Fill(f, requestText, "one.01",
               two                      ? "5"
               : !strcmp(mode, "below") ? "9"
               : !strcmp(mode, "above") ? "11"
                                        : "10",
               !strcmp(mode, "contract") ? "999" : "123", !strcmp(mode, "side") ? "SLD" : "BOT",
               !strcmp(mode, "currency") ? "USD" : "GBP") == 0);
    if (two)
        CHECK(Fill(f, requestText, !strcmp(mode, "correction") ? "one.02" : "two.01", "5", "123", "BOT",
                   "GBP") == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    if (strcmp(mode, "missing-fee"))
        CHECK(Fee(f, "one.01", "3", "GBP") == 0);
    if (!strcmp(mode, "mixed-fees"))
        CHECK(Fee(f, "two.01", "2", "USD") == 0);
    if (!strcmp(mode, "conflicting-fee"))
        CHECK(Fee(f, "one.01", "4", "GBP") == 0);
    if (strcmp(mode, "execution-pending"))
        FEED(f, "55", "1", requestText);
    CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
    if (!strcmp(mode, "disconnect"))
        UmiIbkrConnectionClose(f->c);
    UmiIbkrCompletedExecutionReview review = {0};
    review.quantity.executionCount = 999U;
    UmiStatus status = UmiIbkrCompletedOrderReviewExecutions(
        f->c, 0U, !strcmp(mode, "wrong-request") ? request + 1U : request,
        !strcmp(mode, "age") ? 15013U : 12U, 15000U, &review);
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(mode, "account"))
        expected = UMI_STATUS_PERMISSION_DENIED;
    else if (!strcmp(mode, "zero") || !strcmp(mode, "precision") || !strcmp(mode, "completed-pending") ||
             !strcmp(mode, "execution-pending") || !strcmp(mode, "age") || !strcmp(mode, "disconnect"))
        expected = UMI_STATUS_UNAVAILABLE;
    else if (!strcmp(mode, "unsupported"))
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    else if (!strcmp(mode, "contract") || !strcmp(mode, "side") || !strcmp(mode, "currency"))
        expected = UMI_STATUS_INVALID_STATE;
    else if (!strcmp(mode, "wrong-request"))
        expected = UMI_STATUS_NOT_FOUND;
    CHECK(status == expected);
    if (status != UMI_STATUS_OK)
        CHECK(review.quantity.executionCount == 999U);
    else if (!strcmp(mode, "correction"))
    {
        CHECK(review.quantity.correctionNeedsReview && !review.quantity.equalsRequested);
        CHECK(!review.feeAvailable);
    }
    else
    {
        CHECK(review.quantity.executionCount == (two ? 2U : 1U));
        CHECK(review.quantity.belowRequested == (!strcmp(mode, "below")));
        CHECK(review.quantity.exceedsRequested == (!strcmp(mode, "above")));
        CHECK(review.quantity.equalsRequested == (strcmp(mode, "below") && strcmp(mode, "above")));
        if (!strcmp(mode, "missing-fee"))
            CHECK(!review.feeAvailable && review.commissionStatus == UMI_STATUS_UNAVAILABLE);
        else if (!strcmp(mode, "mixed-fees"))
            CHECK(!review.feeAvailable && review.commissionStatus == UMI_STATUS_NOT_IMPLEMENTED);
        else if (!strcmp(mode, "conflicting-fee"))
            CHECK(!review.feeAvailable && review.commissionStatus == UMI_STATUS_INVALID_STATE);
        else
            CHECK(review.feeAvailable && review.commission.amount.coefficient == 3 &&
                  review.commission.amount.scale == 0U && !strcmp(review.commission.currency, "GBP"));
    }
    /* Original quantity is 7,000 and the order is cancelled. An equality above
     * means only that the ten reported fills match ten captured executions. */
    CHECK(!strcmp(f->c->completed.rows[0].order.totalQuantity.reportedText, "7000"));
    Delete(f);
    return 0;
}
