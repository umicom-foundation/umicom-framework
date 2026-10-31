/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_option_computation.c
 *
 * PURPOSE:
 *   Verify option computation wire shapes, atomic results, subscription ownership and age.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

/* The transport fixture drives the real framing and session decoder. Inputs
 * describe provider packets independently from the implementation under test. */
static int Ready(Fixture *f, bool legacy)
{
    CHECK(UmiIbkrConnectionOpen(f->c, 0U) == UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionPump(f->c, 0U) == UMI_STATUS_OK);
    FEED(f, legacy ? "151" : "176", "20261031 00:00:00 UTC");
    CHECK(UmiIbkrConnectionPump(f->c, 1U) == UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionPump(f->c, 2U) == UMI_STATUS_OK);
    FEED(f, "15", "1", "DU123,DU456");
    FEED(f, "9", "1", "42");
    CHECK(UmiIbkrConnectionPump(f->c, 3U) == UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionPump(f->c, 4U) == UMI_STATUS_OK);
    FEED(f, "49", "1", "1790586000");
    CHECK(UmiIbkrConnectionPump(f->c, 5U) == UMI_STATUS_OK);
    CHECK(f->c->snapshot.state == UMI_IBKR_READY);
    return 0;
}
static int Exercise(Fixture *f, const char *name)
{
    bool legacy = strncmp(name, "legacy-", 7U) == 0;
    CHECK(Ready(f, legacy) == 0);
    UmiIbkrQuoteContract contract = {123U, "SMART"};
    uint32_t request = 0U;
    CHECK(UmiIbkrQuoteSubscribe(f->c, &contract, 10U, &request) == UMI_STATUS_OK);
    CHECK(request == 36000U);
    if (strcmp(name, "type-unconfirmed") != 0)
        FEED(f, "58", "1", "36000", !strcmp(name, "delayed-frozen") ? "4" : "1");
    const char *fields[] = {"21", legacy ? "6" : "36000", legacy ? "36000" : "10",
        legacy ? "10" : "1", "0.250000001", "-0.450000001", "12.125000001",
        "0.025", "0.004", "0.015", "-0.021", "102.375000001"};
    size_t count = sizeof(fields) / sizeof(fields[0]);
    size_t basis = 0U;
    bool delayed = strncmp(name, "delayed-", 8U) == 0;
    if (strstr(name, "-ask") != NULL) basis = 1U;
    if (strstr(name, "-last") != NULL) basis = 2U;
    if (strstr(name, "-model") != NULL) basis = 3U;
    const char *ticks[] = {"10", "11", "12", "13"};
    const char *delayedTicks[] = {"80", "81", "82", "83"};
    fields[legacy ? 3U : 2U] = delayed ? delayedTicks[basis] : ticks[basis];
    if (!strcmp(name, "legacy-partial")) { fields[1] = "5"; count = 6U; }
    if (!strcmp(name, "legacy-model")) { fields[1] = "5"; count = 8U; }
    if (!strcmp(name, "sentinels") || !strcmp(name, "normalised-sentinels")) {
        const char *one = !strcmp(name, "sentinels") ? "-1" : "-10.000e-1";
        const char *two = !strcmp(name, "sentinels") ? "-2" : "-0.0200e+2";
        for (size_t i = 0U; i < 8U; ++i)
            fields[4U + i] = i == 1U || (i >= 4U && i <= 6U) ? two : one;
    }
    if (!strcmp(name, "negative-values")) {
        fields[4] = "-0.25"; fields[5] = "-1.0001"; fields[10] = "-1";
    }
    if (!strcmp(name, "zero-values"))
        for (size_t i = 4U; i < count; ++i) fields[i] = "0.000";
    bool bad = strncmp(name, "bad-", 4U) == 0 || !strcmp(name, "legacy-bad-revision");
    if (bad) {
        /* Seed a complete result; rejection must not partially replace it. */
        CHECK(Feed(f, fields, count) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        fields[4] = "99.5";
        if (!strcmp(name, "bad-decimal")) fields[11] = "NaN";
        else if (!strcmp(name, "bad-count")) --count;
        else if (!strcmp(name, "bad-attribute")) fields[3] = "-1";
        else if (!strcmp(name, "bad-request")) fields[1] = "2147483648";
        else fields[1] = "bad";
    }
    if (!strcmp(name, "unrelated")) fields[1] = "99999";
    if (!strcmp(name, "unknown-tick")) fields[2] = "999";
    if (!strcmp(name, "fragmented")) f->readStep = 1U;
    CHECK(Feed(f, fields, count) == 0);
    UmiStatus status = UmiIbkrConnectionPump(f->c, 12U);
    if (!strcmp(name, "fragmented")) {
        for (unsigned i = 0U; i < 512U && f->inAt != f->inSize; ++i) {
            CHECK(status == UMI_STATUS_OK);
            status = UmiIbkrConnectionPump(f->c, 12U);
        }
        CHECK(f->inAt == f->inSize);
    }
    CHECK(status == (bad ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK));
    UmiIbkrQuoteSnapshot copy;
    CHECK(UmiIbkrQuoteCopy(f->c, request, 12U, 15U, &copy) == UMI_STATUS_OK);
    UmiIbkrOptionComputation *risk = &copy.options[basis];
    if (bad) {
        CHECK(!strcmp(risk->values[0].text, "0.250000001"));
        CHECK(risk->values[0].receivedAtMilliseconds == 11U && risk->values[0].stale);
        return 0;
    }
    if (!strcmp(name, "unrelated") || !strcmp(name, "unknown-tick")) {
        CHECK(!risk->received && !copy.bid.received && f->c->snapshot.ignoredFrames == 1U);
        return 0;
    }
    CHECK(risk->received && risk->attributeReceived == !legacy);
    CHECK(risk->tickAttribute == (legacy ? 0U : 1U));
    CHECK(!copy.bid.received && !copy.ask.received && !copy.last.received);
    for (size_t i = 0U; i < 8U; ++i) {
        CHECK(risk->values[i].received == (i + 4U < count));
        if (i + 4U < count) CHECK(!strcmp(risk->values[i].text, fields[i + 4U]));
        else CHECK(risk->values[i].stale);
    }
    if (strstr(name, "sentinels") != NULL) {
        for (size_t i = 0U; i < 8U; ++i)
            CHECK(risk->values[i].unavailable && risk->values[i].stale);
        return 0;
    }
    if (!strcmp(name, "negative-values") || !strcmp(name, "zero-values")) {
        for (size_t i = 0U; i < 8U; ++i)
            CHECK(!risk->values[i].unavailable && !risk->values[i].stale);
    }
    if (delayed) CHECK(risk->values[0].dataType ==
        (!strcmp(name, "delayed-frozen") ? UMI_IBKR_DATA_DELAYED_FROZEN : UMI_IBKR_DATA_DELAYED));
    if (!strcmp(name, "type-unconfirmed"))
        CHECK(risk->values[0].stale && risk->values[0].dataType == UMI_IBKR_DATA_UNKNOWN);
    else CHECK(!risk->values[0].stale);
    if (!strcmp(name, "freshness")) {
        CHECK(UmiIbkrQuoteCopy(f->c, request, 28U, 15U, &copy) == UMI_STATUS_OK);
        CHECK(copy.options[basis].values[0].stale && copy.options[basis].values[0].receivedAtMilliseconds == 12U);
    } else if (!strcmp(name, "type-switch")) {
        FEED(f, "58", "1", "36000", "2");
        CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(f->c, request, 13U, 15U, &copy) == UMI_STATUS_OK);
        CHECK(copy.options[basis].values[0].stale && copy.options[basis].values[0].dataType == UMI_IBKR_DATA_REALTIME);
    } else if (!strcmp(name, "independent-bases")) {
        fields[2] = "13"; fields[4] = "0.33";
        CHECK(Feed(f, fields, count) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 28U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(f->c, request, 28U, 15U, &copy) == UMI_STATUS_OK);
        CHECK(copy.options[0].values[0].stale && !copy.options[3].values[0].stale);
        CHECK(!strcmp(copy.options[0].values[0].text, "0.250000001"));
    } else if (!strcmp(name, "cancel") || !strcmp(name, "reuse")) {
        CHECK(UmiIbkrQuoteCancel(f->c, request) == UMI_STATUS_OK);
        if (!strcmp(name, "reuse"))
            CHECK(UmiIbkrQuoteSubscribe(f->c, &contract, 12U, &request) == UMI_STATUS_OK && request != 36000U);
        fields[4] = "99";
        CHECK(Feed(f, fields, count) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(f->c, request, 13U, 15U, &copy) == UMI_STATUS_OK);
        CHECK(copy.options[0].values[0].stale);
        if (!strcmp(name, "reuse")) CHECK(!copy.options[0].received);
        else CHECK(!strcmp(copy.options[0].values[0].text, "0.250000001"));
    } else if (!strcmp(name, "provider-error")) {
        FEED(f, "4", "2", "36000", "354", "Synthetic permission failure", "");
        fields[4] = "99"; CHECK(Feed(f, fields, count) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
        CHECK(UmiIbkrQuoteCopy(f->c, request, 13U, 15U, &copy) == UMI_STATUS_OK);
        CHECK(copy.failed && copy.options[0].values[0].stale);
        CHECK(!strcmp(copy.options[0].values[0].text, "0.250000001"));
    } else if (!strcmp(name, "disconnect")) {
        UmiIbkrConnectionClose(f->c);
        CHECK(UmiIbkrQuoteCopy(f->c, request, 12U, 15U, &copy) == UMI_STATUS_OK);
        CHECK(copy.options[0].values[0].stale);
    } else if (!strcmp(name, "copy-isolation")) {
        strcpy(copy.options[0].values[0].text, "999");
        CHECK(UmiIbkrQuoteCopy(f->c, request, 12U, 15U, &copy) == UMI_STATUS_OK);
        CHECK(!strcmp(copy.options[0].values[0].text, "0.250000001"));
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"modern-bid", "modern-ask", "modern-last", "modern-model", "legacy-full", "legacy-partial", "legacy-model", "delayed-bid", "delayed-ask", "delayed-last", "delayed-model", "delayed-frozen", "type-unconfirmed", "type-switch", "sentinels", "normalised-sentinels", "negative-values", "zero-values", "freshness", "independent-bases", "cancel", "reuse", "provider-error", "disconnect", "bad-decimal", "bad-count", "bad-attribute", "bad-request", "legacy-bad-revision", "unrelated", "unknown-tick", "fragmented", "copy-isolation"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (!strcmp(argv[1], cases[i])) known = true;
    if (!known) return 2;
    Fixture *fixture = New();
    if (fixture == NULL) return 1;
    int result = Exercise(fixture, argv[1]);
    Delete(fixture);
    return result;
}
