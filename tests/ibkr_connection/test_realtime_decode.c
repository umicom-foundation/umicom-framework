/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_realtime_decode.c
 * PURPOSE: Check exact numeric fields, malformed frames and atomic bar publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "realtime_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {"valid",
                                        "negative-price",
                                        "scientific",
                                        "precision",
                                        "missing-volume",
                                        "midpoint",
                                        "bid",
                                        "ask",
                                        "nan",
                                        "bad-time",
                                        "time-overflow",
                                        "high-below-close",
                                        "low-above-open",
                                        "bad-volume",
                                        "bad-count",
                                        "missing-field",
                                        "extra-field",
                                        "foreign",
                                        "bad-version",
                                        "fragmented-read"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    StreamReferences();

    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    UmiIbkrRealtimeQuery q = StreamQuery();
    if (!strcmp(mode, "midpoint"))
        q.dataKind = UMI_IBKR_HISTORY_MIDPOINT;
    if (!strcmp(mode, "bid"))
        q.dataKind = UMI_IBKR_HISTORY_BID;
    if (!strcmp(mode, "ask"))
        q.dataKind = UMI_IBKR_HISTORY_ASK;
    uint32_t request;
    CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"50", "3", id, "1791363600", "10", "12", "9", "11", "100", "10.5", "8", "extra"};
    size_t count = 11U;
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(mode, "negative-price"))
    {
        fields[4] = "-10";
        fields[5] = "-9";
        fields[6] = "-12";
        fields[7] = "-11";
    }
    if (!strcmp(mode, "scientific"))
        fields[4] = "1e1";
    if (!strcmp(mode, "precision"))
        fields[4] = "10.0000000001";
    if (!strcmp(mode, "missing-volume"))
    {
        fields[8] = "-1";
        fields[9] = "";
        fields[10] = "-1";
    }
    if (!strcmp(mode, "nan"))
        fields[4] = "NaN";
    if (!strcmp(mode, "bad-time"))
        fields[3] = "20261007";
    if (!strcmp(mode, "time-overflow"))
        fields[3] = "253402300800";
    if (!strcmp(mode, "high-below-close"))
        fields[5] = "10";
    if (!strcmp(mode, "low-above-open"))
        fields[6] = "11";
    if (!strcmp(mode, "bad-volume"))
        fields[8] = "-2";
    if (!strcmp(mode, "bad-count"))
        fields[10] = "3.5";
    if (!strcmp(mode, "missing-field"))
        count = 10U;
    if (!strcmp(mode, "extra-field"))
    {
        count = 12U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!strcmp(mode, "foreign"))
        fields[2] = "999";
    if (!strcmp(mode, "bad-version"))
        fields[1] = "9";
    if (!strcmp(mode, "bad-time"))
        fields[3] = "zero";
    const char *invalid[] = {"nan",        "bad-time",  "time-overflow", "high-below-close", "low-above-open",
                             "bad-volume", "bad-count", "missing-field", "bad-version"};
    if (HistoricalKnownCase(mode, invalid, sizeof invalid / sizeof invalid[0]))
        expected = UMI_STATUS_PARSE_ERROR;
    CHECK(Feed(f, fields, count) == 0);
    if (!strcmp(mode, "fragmented-read"))
        f->readStep = 1U;
    uint64_t now = 11U;
    UmiStatus result;
    do
    {
        result = UmiIbkrConnectionPump(f->c, now++);
    } while (result == UMI_STATUS_OK && f->inAt < f->inSize && now < 1000U);
    CHECK(result == expected);
    UmiIbkrRealtimeSnapshot s;
    CHECK(UmiIbkrRealtimeCopy(f->c, request, now, 1000U, &s) == UMI_STATUS_OK);
    if (expected != UMI_STATUS_OK)
        CHECK(s.count == 0U && s.failed && s.stale);
    else if (!strcmp(mode, "foreign"))
        CHECK(s.count == 0U && s.active);
    else
    {
        UmiIbkrRealtimeBar bar;
        CHECK(UmiIbkrRealtimeBarCopy(f->c, request, 0U, &bar) == UMI_STATUS_OK);
        CHECK(s.count == 1U && s.receivedBars == 1U && !s.stale);
        CHECK(bar.timeMilliseconds == INT64_C(1791363600000));
        CHECK(!strcmp(bar.open.reportedText, fields[4]));
        CHECK(bar.open.exact == (strcmp(mode, "precision") != 0));
        bool trade = q.dataKind == UMI_IBKR_HISTORY_TRADES && strcmp(mode, "missing-volume");
        CHECK(bar.volumeAvailable == trade && bar.weightedAverageAvailable == trade &&
              bar.tradeCountAvailable == trade);
    }
    Delete(f);
    return 0;
}
