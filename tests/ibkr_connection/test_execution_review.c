/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_execution_review.c
 * PURPOSE: Check exact quantity aggregation and refuse incomplete, corrected or ambiguous observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/execution_observation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static UmiIbkrExecutionObservation Row(const char *id, int64_t quantity)
{
    UmiIbkrExecutionObservation row = {0};
    strcpy(row.executionId, id);
    strcpy(row.account, "DU123");
    strcpy(row.symbol, "EXAMPLE");
    strcpy(row.securityType, "STK");
    strcpy(row.currency, "GBP");
    strcpy(row.side, "BOT");
    strcpy(row.time, "20261007 12:00:00 UTC");
    strcpy(row.exchange, "LSE");
    row.contractId = 123U;
    row.permanentOrderId = 77U;
    row.orderId = 42;
    row.clientId = 9;
    row.quantity = (UmiDecimal){quantity, 0U};
    row.price = (UmiDecimal){123, 2U};
    row.cumulativeQuantity = (UmiDecimal){7000, 0U};
    row.averagePrice = row.price;
    return row;
}
static int Run(UmiIbkrExecutionSnapshot *s, const char *mode)
{
    s->requestId = 36000U;
    s->complete = true;
    s->count = 2U;
    strcpy(s->account, "DU123");
    s->rows[0] = Row("one.01", 10);
    s->rows[1] = Row("two.01", 6990);
    UmiDecimal intended = {7000, 0U};
    uint64_t permanent = 77U;
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(mode, "partial"))
    {
        s->complete = false;
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (!strcmp(mode, "stale"))
    {
        s->stale = true;
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (!strcmp(mode, "failed"))
    {
        s->failed = true;
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (!strcmp(mode, "foreign-account"))
    {
        strcpy(s->rows[1].account, "DU456");
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "correction"))
        strcpy(s->rows[1].executionId, "one.02");
    else if (!strcmp(mode, "combo-id"))
        strcpy(s->rows[1].executionId, "a.b.c.d.01");
    else if (!strcmp(mode, "asset"))
        strcpy(s->rows[1].securityType, "FUT");
    else if (!strcmp(mode, "negative-price"))
        s->rows[1].price.coefficient = -123;
    else if (!strcmp(mode, "below"))
        s->count = 1U;
    else if (!strcmp(mode, "above"))
        s->rows[1].quantity.coefficient = 7000;
    else if (!strcmp(mode, "scale"))
    {
        s->rows[0].quantity = (UmiDecimal){1000, 2U};
    }
    else if (!strcmp(mode, "overflow"))
    {
        s->rows[1].quantity.coefficient = INT64_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (!strcmp(mode, "duplicate"))
    {
        s->rows[1] = s->rows[0];
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "bad-count"))
    {
        s->count = UMI_IBKR_EXECUTION_OBSERVATION_LIMIT + 1U;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "zero-permanent"))
    {
        permanent = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(mode, "other-order"))
    {
        s->rows[1].permanentOrderId = 78U;
    }
    else if (!strcmp(mode, "missing"))
    {
        permanent = 99U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (!strcmp(mode, "unterminated"))
    {
        memset(s->rows[1].executionId, 'x', sizeof s->rows[1].executionId);
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (strcmp(mode, "valid"))
        return 2;
    UmiIbkrExecutionQuantityReview out = {0};
    out.executionCount = 999U;
    UmiStatus status = UmiIbkrExecutionsReviewQuantity(s, permanent, 123U, "BOT", intended, &out);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK)
    {
        CHECK(out.executionCount == 999U);
        return 0;
    }
    if (!strcmp(mode, "correction"))
    {
        CHECK(out.correctionNeedsReview && !out.equalsRequested && out.observedQuantity.coefficient == 0);
        return 0;
    }
    if (!strcmp(mode, "combo-id") || !strcmp(mode, "asset") || !strcmp(mode, "negative-price"))
    {
        CHECK(out.unsupportedExecutionShape && !out.equalsRequested);
        return 0;
    }
    int compared = 0;
    UmiDecimal want = {(!strcmp(mode, "below") || !strcmp(mode, "other-order")) ? 10
                       : !strcmp(mode, "above")                                 ? 7010
                                                                                : 7000,
                       0U};
    CHECK(UmiDecimalCompare(out.observedQuantity, want, &compared) == UMI_STATUS_OK && compared == 0);
    CHECK(!out.correctionNeedsReview && !out.unsupportedExecutionShape);
    CHECK(out.equalsRequested == (want.coefficient == 7000));
    CHECK(out.belowRequested == (want.coefficient < 7000));
    CHECK(out.exceedsRequested == (want.coefficient > 7000));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiIbkrExecutionSnapshot *s = calloc(1, sizeof *s);
    if (s == NULL)
        return 1;
    int result = Run(s, argv[1]);
    free(s);
    return result;
}
