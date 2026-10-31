/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_contract_snapshot.c
 *
 * PURPOSE:
 *   Verify bounded-age instrument copies preserve exact data and outputs on failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

/* These cases exercise the public owned-copy boundary with explicit session
 * timestamps. Decoder byte framing is covered by test_contract_details.c. */
static int Run(Fixture *fixture, const char *test, UmiIbkrContractDetailsSnapshot *out,
    UmiIbkrContractDetailsSnapshot *before)
{
    UmiIbkrContractDetailsSnapshot *stored = &fixture->c->contractDetails;
    fixture->c->snapshot.state = UMI_IBKR_READY;
    fixture->c->snapshot.stale = false;
    fixture->c->lastNow = 10U;
    stored->requestId = 7U; stored->requestedAtMilliseconds = 10U;
    stored->requestedContract.contractId = 123U; strcpy(stored->requestedContract.exchange, "SMART");
    stored->complete = true; stored->completedAtMilliseconds = 15U; stored->count = 1U;
    stored->items[0].contractId = 123U; strcpy(stored->items[0].minimumTick, "0.000000001");
    strcpy(stored->items[0].minimumSize, "9007199254740993.123456789");
    memset(out, 0x5a, sizeof(*out)); memcpy(before, out, sizeof(*out));
    uint64_t now = 20U, age = 50U; uint32_t request = 7U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strncmp(test, "latest-", 7U) == 0) {
        if (strcmp(test, "latest-option") == 0) { stored->optionLookup = true; stored->requestedContract.contractId = 0U; }
        else if (strcmp(test, "latest-missing") == 0) stored->requestId = 0U;
        else if (strcmp(test, "latest-exact") != 0) return 2;
        CHECK(UmiIbkrContractDetailsCopyLatestWithAge(NULL, now, age, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrContractDetailsCopyLatestWithAge(fixture->c, now, 0U, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrContractDetailsCopyLatestWithAge(fixture->c, now, age, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(out, before, sizeof(*out)) == 0);
        CHECK(UmiIbkrContractDetailsCopyLatestWithAge(fixture->c, now, age, out) ==
            (stored->requestId == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
        if (stored->requestId == 0U) CHECK(memcmp(out, before, sizeof(*out)) == 0);
        else { CHECK(out->requestId == 7U && out->optionLookup == stored->optionLookup);
            CHECK(strcmp(out->items[0].minimumTick, "0.000000001") == 0); }
        CHECK(fixture->opens == 0U && fixture->outSize == 0U); return 0;
    }
    if (strcmp(test, "invalid") == 0) {
        CHECK(UmiIbkrContractDetailsCopyWithAge(NULL, 7U, now, age, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrContractDetailsCopyWithAge(fixture->c, 0U, now, age, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrContractDetailsCopyWithAge(fixture->c, 7U, now, age, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        age = 0U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(test, "missing") == 0) { request = 8U; expected = UMI_STATUS_NOT_FOUND; }
    else if (strcmp(test, "partial") == 0) { stored->complete = false; now = 60U; }
    else if (strcmp(test, "age-boundary") == 0) now = 65U;
    else if (strcmp(test, "age-before-boundary") == 0) now = 64U;
    else if (strcmp(test, "disconnect") == 0) fixture->c->snapshot.state = UMI_IBKR_DISCONNECTED;
    else if (strcmp(test, "session-stale") == 0) fixture->c->snapshot.stale = true;
    else if (strcmp(test, "clock-backward") == 0) { now = 9U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(test, "future-completion") == 0) {
        stored->completedAtMilliseconds = 21U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(test, "completion-before-request") == 0) {
        stored->completedAtMilliseconds = 9U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(test, "capacity") == 0) {
        stored->count = UMI_IBKR_CONTRACT_DETAILS_LIMIT + 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(test, "timeout") == 0) {
        stored->complete = false; now = 10010U;
    } else if (strcmp(test, "complete") != 0 && strcmp(test, "owned-copy") != 0) return 2;
    CHECK(UmiIbkrContractDetailsCopyWithAge(fixture->c, request, now, age, out) == expected);
    CHECK(fixture->opens == 0U && fixture->outSize == 0U);
    if (expected != UMI_STATUS_OK) {
        CHECK(memcmp(out, before, sizeof(*out)) == 0); return 0;
    }
    CHECK(out->requestId == 7U && out->count == 1U);
    CHECK(strcmp(out->items[0].minimumSize, "9007199254740993.123456789") == 0);
    CHECK(strcmp(out->items[0].minimumTick, "0.000000001") == 0);
    bool stale = strcmp(test, "partial") == 0 || strcmp(test, "age-boundary") == 0 ||
        strcmp(test, "disconnect") == 0 || strcmp(test, "session-stale") == 0 || strcmp(test, "timeout") == 0;
    CHECK(out->stale == stale);
    CHECK(!stored->stale); /* Reading cannot renew or mutate provider evidence. */
    CHECK(out->requestedAtMilliseconds == 10U && stored->completedAtMilliseconds == 15U);
    if (strcmp(test, "timeout") == 0) CHECK(out->failed && !out->complete && !stored->failed);
    if (strcmp(test, "owned-copy") == 0) {
        strcpy(stored->items[0].minimumTick, "9");
        CHECK(strcmp(out->items[0].minimumTick, "0.000000001") == 0);
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *fixture = New();
    UmiIbkrContractDetailsSnapshot *out = calloc(1U, sizeof(*out)), *before = calloc(1U, sizeof(*before));
    if (fixture == NULL || out == NULL || before == NULL) {
        Delete(fixture); free(out); free(before); return 1;
    }
    int result = Run(fixture, argv[1], out, before);
    Delete(fixture); free(out); free(before); return result;
}
