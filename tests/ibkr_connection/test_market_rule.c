/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_market_rule.c
 * PURPOSE: Review independent market-rule wire examples and reject stale or invalid captures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static int Run(Fixture *fixture, const char *mode)
{
    (void)PositionFeed;
    CHECK(Connect(fixture) == 0);
    if (!strcmp(mode, "invalid"))
    {
        CHECK(UmiIbkrMarketRuleRequest(fixture->c, 0U, 10U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(fixture->c->marketRuleCount == 0U);
        return 0;
    }
    if (!strcmp(mode, "queue-full"))
    {
        fixture->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrMarketRuleRequest(fixture->c, 26U, 10U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(fixture->c->marketRuleCount == 0U);
        return 0;
    }
    size_t before = fixture->outSize;
    CHECK(UmiIbkrMarketRuleRequest(fixture->c, 26U, 10U) == UMI_STATUS_OK);
    UmiIbkrMarketRuleSnapshot copy;
    if (!strcmp(mode, "wire"))
    {
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        const unsigned char expected[] = {0, 0, 0, 6, '9', '1', 0, '2', '6', 0};
        CHECK(fixture->outSize - before == sizeof expected);
        CHECK(memcmp(fixture->output + before, expected, sizeof expected) == 0);
        return 0;
    }
    if (!strcmp(mode, "busy"))
    {
        size_t pending = fixture->c->txSize;
        CHECK(UmiIbkrMarketRuleRequest(fixture->c, 26U, 11U) == UMI_STATUS_BUSY);
        CHECK(fixture->c->txSize == pending && fixture->c->marketRuleCount == 1U);
        return 0;
    }
    if (!strcmp(mode, "capacity"))
    {
        for (uint32_t rule = 27U; rule < 34U; ++rule)
            CHECK(UmiIbkrMarketRuleRequest(fixture->c, rule, 10U) == UMI_STATUS_OK);
        CHECK(UmiIbkrMarketRuleRequest(fixture->c, 34U, 10U) == UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    if (!strcmp(mode, "timeout"))
    {
        CHECK(UmiIbkrMarketRuleCopy(fixture->c, 26U, 10010U, &copy) == UMI_STATUS_OK);
        CHECK(copy.stale && !copy.complete);
        CHECK(UmiIbkrMarketRuleRequest(fixture->c, 26U, 10010U) == UMI_STATUS_TIMEOUT);
        FEED(fixture, "93", "26", "1", "0", "0.01");
        CHECK(UmiIbkrConnectionPump(fixture->c, 10010U) == UMI_STATUS_OK);
        CHECK(UmiIbkrMarketRuleCopy(fixture->c, 26U, 10010U, &copy) == UMI_STATUS_OK);
        CHECK(copy.stale && !copy.complete);
        return 0;
    }
    if (!strcmp(mode, "foreign"))
    {
        FEED(fixture, "93", "27", "1", "0", "0.01");
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrMarketRuleCopy(fixture->c, 26U, 11U, &copy) == UMI_STATUS_OK && !copy.complete);
        return 0;
    }
    if (!strcmp(mode, "empty"))
    {
        FEED(fixture, "93", "26", "0");
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrMarketRuleCopy(fixture->c, 26U, 11U, &copy) == UMI_STATUS_OK);
        CHECK(copy.complete && copy.count == 0U);
        return 0;
    }
    if (!strcmp(mode, "bad-count"))
        FEED(fixture, "93", "26", "2", "0", "0.01");
    else if (!strcmp(mode, "bad-order"))
        FEED(fixture, "93", "26", "2", "10", "0.01", "0", "0.05");
    else if (!strcmp(mode, "bad-increment"))
        FEED(fixture, "93", "26", "1", "0", "0");
    else if (!strcmp(mode, "precision"))
        FEED(fixture, "93", "26", "1", "0", "1e-10");
    else if (!strcmp(mode, "scientific"))
        FEED(fixture, "93", "26", "2", "0", "1e-4", "1e1", "5e-2");
    else if (!strcmp(mode, "valid") || !strcmp(mode, "duplicate") || !strcmp(mode, "disconnected"))
        FEED(fixture, "93", "26", "2", "0", "0.01", "10", "0.05");
    else
        return 2;
    UmiStatus status = UmiIbkrConnectionPump(fixture->c, 11U);
    bool bad = !strcmp(mode, "bad-count") || !strcmp(mode, "bad-order") || !strcmp(mode, "bad-increment") ||
               !strcmp(mode, "precision");
    if (bad)
    {
        CHECK(status != UMI_STATUS_OK);
        CHECK(UmiIbkrMarketRuleCopy(fixture->c, 26U, 11U, &copy) == UMI_STATUS_OK);
        CHECK(!copy.complete && copy.count == 0U && copy.stale);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiIbkrMarketRuleCopy(fixture->c, 26U, 11U, &copy) == UMI_STATUS_OK);
    CHECK(copy.complete && copy.count == 2U && !copy.stale && copy.receivedAtMilliseconds == 11U);
    CHECK(copy.bands[1].lowerBound.coefficient == 10 && copy.bands[1].lowerBound.scale == 0U);
    CHECK(copy.bands[1].increment.coefficient == 5 && copy.bands[1].increment.scale == 2U);
    size_t pending = fixture->c->txSize;
    CHECK(UmiIbkrMarketRuleRequest(fixture->c, 26U, 11U) == UMI_STATUS_OK);
    CHECK(fixture->c->txSize == pending);
    if (!strcmp(mode, "duplicate"))
    {
        FEED(fixture, "93", "26", "1", "0", "0.10");
        CHECK(UmiIbkrConnectionPump(fixture->c, 12U) == UMI_STATUS_OK);
        CHECK(UmiIbkrMarketRuleCopy(fixture->c, 26U, 12U, &copy) == UMI_STATUS_OK && copy.count == 2U);
    }
    else if (!strcmp(mode, "disconnected"))
    {
        UmiIbkrConnectionClose(fixture->c);
        CHECK(UmiIbkrMarketRuleCopy(fixture->c, 26U, 12U, &copy) == UMI_STATUS_OK && copy.stale &&
              copy.complete);
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    Fixture *fixture = New();
    if (fixture == NULL)
        return 1;
    int result = Run(fixture, argv[1]);
    Delete(fixture);
    return result;
}
