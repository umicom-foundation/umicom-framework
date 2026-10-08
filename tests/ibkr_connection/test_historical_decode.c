/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_historical_decode.c
 * PURPOSE: Reject malformed historical payloads atomically and preserve usable broker evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "historical_fixture.h"
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
                                        "nan",
                                        "bad-time",
                                        "time-overflow",
                                        "high-below-close",
                                        "low-above-open",
                                        "negative-volume",
                                        "bad-count",
                                        "missing-field",
                                        "extra-field",
                                        "bad-range",
                                        "duplicate-time",
                                        "reversed-time",
                                        "foreign",
                                        "unsolicited",
                                        "duplicate",
                                        "capacity"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    HistoricalReferences();
    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    UmiIbkrHistoricalQuery q = HistoricalQuery();
    uint32_t request = 1U;
    if (!strcmp(mode, "midpoint"))
        q.dataKind = UMI_IBKR_HISTORY_MIDPOINT;
    if (strcmp(mode, "unsolicited"))
        CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"17",
                            id,
                            "20261007 09:00:00 UTC",
                            "20261007 13:00:00 UTC",
                            "2",
                            "1791363600",
                            "10",
                            "12",
                            "9",
                            "11",
                            "100",
                            "10.5",
                            "8",
                            "1791363660",
                            "11",
                            "13",
                            "10",
                            "12",
                            "200",
                            "11.5",
                            "9",
                            "extra"};
    size_t count = 21U;
    bool invalid = false;
    if (!strcmp(mode, "negative-price"))
    {
        fields[6] = "-10";
        fields[7] = "-9";
        fields[8] = "-12";
        fields[9] = "-11";
    }
    else if (!strcmp(mode, "scientific"))
        fields[6] = "1e1";
    else if (!strcmp(mode, "precision"))
        fields[6] = "10.0000000001";
    else if (!strcmp(mode, "missing-volume") || !strcmp(mode, "midpoint"))
    {
        fields[10] = "-1";
        fields[11] = "-1";
        fields[12] = "-1";
    }
    else if (!strcmp(mode, "nan"))
    {
        fields[6] = "NaN";
        invalid = true;
    }
    else if (!strcmp(mode, "bad-time"))
    {
        fields[5] = "20261007 09:00:00 UTC";
        invalid = true;
    }
    else if (!strcmp(mode, "time-overflow"))
    {
        fields[5] = "9223372036854775807";
        invalid = true;
    }
    else if (!strcmp(mode, "high-below-close"))
    {
        fields[7] = "10";
        invalid = true;
    }
    else if (!strcmp(mode, "low-above-open"))
    {
        fields[8] = "11";
        invalid = true;
    }
    else if (!strcmp(mode, "negative-volume"))
    {
        fields[10] = "-2";
        invalid = true;
    }
    else if (!strcmp(mode, "bad-count"))
    {
        fields[4] = "3";
        invalid = true;
    }
    else if (!strcmp(mode, "missing-field"))
    {
        --count;
        invalid = true;
    }
    else if (!strcmp(mode, "extra-field"))
    {
        ++count;
        invalid = true;
    }
    else if (!strcmp(mode, "bad-range"))
    {
        fields[2] = "bad\nrange";
        invalid = true;
    }
    else if (!strcmp(mode, "duplicate-time"))
    {
        fields[13] = fields[5];
        invalid = true;
    }
    else if (!strcmp(mode, "reversed-time"))
    {
        fields[13] = "1791363500";
        invalid = true;
    }
    else if (!strcmp(mode, "foreign"))
        fields[1] = "999";
    if (!strcmp(mode, "capacity"))
    {
        /* More than 1,024 fields proves the historical decoder uses its own
         * explicit bound, while the response remains below the byte cap. */
        char (*times)[32] = calloc(512U, sizeof *times);
        const char **many = calloc(4101U, sizeof *many);
        CHECK(times && many);
        many[0] = "17";
        many[1] = id;
        many[2] = "start";
        many[3] = "end";
        many[4] = "512";
        for (size_t i = 0U; i < 512U; ++i)
        {
            (void)snprintf(times[i], sizeof times[i], "%llu", (unsigned long long)(1791363600U + i * 60U));
            size_t at = 5U + i * 8U;
            many[at] = times[i];
            for (size_t j = 1U; j < 8U; ++j)
                many[at + j] = fields[5U + j];
        }
        CHECK(Feed(f, many, 4101U) == 0);
        free(times);
        free(many);
    }
    else
        CHECK(Feed(f, fields, count) == 0);
    UmiStatus result = UmiIbkrConnectionPump(f->c, 11U);
    if (invalid)
    {
        CHECK(result == UMI_STATUS_PARSE_ERROR);
        CHECK(f->c->history->snapshot.count == 0U && !f->c->history->snapshot.complete);
    }
    else
    {
        CHECK(result == UMI_STATUS_OK);
        if (!strcmp(mode, "unsolicited"))
            CHECK(!f->c->history);
        else if (!strcmp(mode, "foreign"))
            CHECK(f->c->history->snapshot.pending && !f->c->history->snapshot.count);
        else
        {
            UmiIbkrHistoricalSnapshot snapshot;
            CHECK(UmiIbkrHistoricalCopy(f->c, request, 12U, 100U, &snapshot) == UMI_STATUS_OK);
            CHECK(snapshot.complete && snapshot.count == (!strcmp(mode, "capacity") ? 512U : 2U));
            UmiIbkrHistoricalBar bar;
            CHECK(UmiIbkrHistoricalBarCopy(f->c, request, 0U, &bar) == UMI_STATUS_OK);
            CHECK(bar.timeMilliseconds == INT64_C(1791363600000));
            CHECK(bar.open.exact == (strcmp(mode, "precision") != 0));
            CHECK(bar.volumeAvailable ==
                  (strcmp(mode, "missing-volume") != 0 && strcmp(mode, "midpoint") != 0));
            if (!strcmp(mode, "scientific"))
                CHECK(bar.open.value.coefficient == 10 && bar.open.value.scale == 0U);
            if (!strcmp(mode, "duplicate"))
            {
                CHECK(HistoricalFeed(f, request) == 0);
                CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
                CHECK(f->c->history->snapshot.completedAtMilliseconds == 11U &&
                      f->c->history->snapshot.count == 2U);
            }
        }
    }
    Delete(f);
    return 0;
}
