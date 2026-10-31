/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_option_snapshot.c
 *
 * PURPOSE:
 *   Verify bounded-age option evidence and checked candidate requests without network activity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/broker_connectivity/option_contract.h"

static int Run(Fixture *f, const char *test)
{
    f->c->snapshot.state = UMI_IBKR_READY; f->c->snapshot.stale = false;
    f->c->snapshot.protocolVersion = 176; f->c->lastNow = 10U;
    f->c->optionChains = calloc(1U, sizeof(*f->c->optionChains));
    CHECK(f->c->optionChains != NULL);
    UmiIbkrOptionChainStore *store = f->c->optionChains;
    store->items[0] = calloc(1U, sizeof(*store->items[0])); CHECK(store->items[0] != NULL);
    UmiIbkrOptionChain *chain = store->items[0];
    store->snapshot.requestId = 70U; store->snapshot.requestedAtMilliseconds = 10U;
    store->snapshot.completedAtMilliseconds = 15U; store->snapshot.complete = true; store->snapshot.count = 1U;
    store->snapshot.query.underlyingContractId = chain->underlyingContractId = 123U;
    strcpy(store->snapshot.query.underlyingSymbol, "EXAMPLE"); strcpy(store->snapshot.query.underlyingSecurityType, "STK");
    strcpy(chain->exchange, "SMART"); strcpy(chain->tradingClass, "CLASS"); strcpy(chain->multiplier, "100");
    chain->expiryCount = chain->strikeCount = 1U; strcpy(chain->expirations[0], "20270115");
    strcpy(chain->strikes[0].reportedText, "100.000000001");
    chain->strikes[0].exact = true; chain->strikes[0].value = (UmiDecimal){100000000001LL, 9U};
    uint64_t now = 20U, age = 50U; uint32_t expected = 70U;
    UmiStatus result = UMI_STATUS_OK;
    if (strncmp(test, "item-", 5U) == 0) {
        UmiIbkrOptionChain *out = malloc(sizeof(*out)), *before = malloc(sizeof(*before));
        CHECK(out != NULL && before != NULL);
        memset(out, 0x5a, sizeof(*out)); *before = *out;
        size_t index = 0U;
        if (!strcmp(test, "item-invalid")) {
            CHECK(UmiIbkrOptionChainItemCopyChecked(NULL, 70U, 0U, out) == UMI_STATUS_INVALID_ARGUMENT);
            expected = 0U; result = UMI_STATUS_INVALID_ARGUMENT;
        } else if (!strcmp(test, "item-replaced")) { expected = 71U; result = UMI_STATUS_NOT_FOUND; }
        else if (!strcmp(test, "item-index")) { index = UMI_IBKR_OPTION_CHAIN_LIMIT; result = UMI_STATUS_NOT_FOUND; }
        else if (!strcmp(test, "item-null-storage")) { store->items[0] = NULL; result = UMI_STATUS_UNAVAILABLE; }
        else if (!strcmp(test, "item-count")) { store->snapshot.count = 33U; result = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(test, "item-expiries")) { chain->expiryCount = 129U; result = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(test, "item-strikes")) { chain->strikeCount = 513U; result = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(test, "item-identity")) { chain->underlyingContractId = 456U; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "item-text")) { memset(chain->exchange, 'X', sizeof(chain->exchange)); result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "item-expiry-text")) { memset(chain->expirations[0], '1', 9U); result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "item-strike-text")) {
            memset(chain->strikes[0].reportedText, '1', sizeof(chain->strikes[0].reportedText)); result = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(test, "item-owned")) return 2;
        CHECK(UmiIbkrOptionChainItemCopyChecked(f->c, expected, index, out) == result);
        if (result == UMI_STATUS_OK) {
            strcpy(chain->strikes[0].reportedText, "9"); CHECK(!strcmp(out->strikes[0].reportedText, "100.000000001"));
        } else CHECK(!memcmp(out, before, sizeof(*out)));
        store->items[0] = chain; free(out); free(before);
    } else if (strncmp(test, "resolve-", 8U) == 0) {
        UmiIbkrOptionSelection selection = {0}; selection.chainRequest = 70U; selection.right = 'C';
        strcpy(selection.currency, "USD"); strcpy(selection.exchange, "SMART");
        uint32_t request = 900U;
        if (!strcmp(test, "resolve-stale")) { now = 65U; result = UMI_STATUS_INVALID_STATE; }
        else if (!strcmp(test, "resolve-partial")) { store->snapshot.complete = false; result = UMI_STATUS_INVALID_STATE; }
        else if (!strcmp(test, "resolve-session-stale")) { f->c->snapshot.stale = true; result = UMI_STATUS_INVALID_STATE; }
        else if (!strcmp(test, "resolve-replaced")) { selection.chainRequest = 71U; result = UMI_STATUS_NOT_FOUND; }
        else if (!strcmp(test, "resolve-missing-item")) { store->items[0] = NULL; result = UMI_STATUS_UNAVAILABLE; }
        else if (!strcmp(test, "resolve-bounds")) { selection.expiryIndex = 1U; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "resolve-invalid-right")) { selection.right = 'X'; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "resolve-empty-route")) { selection.exchange[0] = '\0'; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "resolve-queue-full")) { f->c->txSize = UMI_IBKR_TX_LIMIT; result = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(test, "resolve-unsupported")) { f->c->snapshot.protocolVersion = 163; result = UMI_STATUS_NOT_IMPLEMENTED; }
        else if (!strcmp(test, "resolve-invalid")) {
            CHECK(UmiIbkrOptionContractRequestWithAge(NULL, &selection, now, age, &request) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrOptionContractRequestWithAge(f->c, NULL, now, age, &request) == UMI_STATUS_INVALID_ARGUMENT);
            age = 0U; result = UMI_STATUS_INVALID_ARGUMENT;
        } else if (!strcmp(test, "resolve-put")) selection.right = 'P';
        else if (strcmp(test, "resolve-call")) return 2;
        size_t queued = f->c->txSize;
        CHECK(UmiIbkrOptionContractRequestWithAge(f->c, &selection, now, age, &request) == result);
        if (result == UMI_STATUS_OK) {
            CHECK(f->c->txSize > queued && request != 900U);
            CHECK(f->c->contractDetails.optionLookup && !f->c->contractDetails.complete);
            CHECK(f->c->contractDetails.requestedOption.right[0] == selection.right);
            CHECK(!strcmp(f->c->contractDetails.requestedOption.strike, "100.000000001"));
        } else { CHECK(request == 900U && f->c->txSize == queued); CHECK(f->c->contractDetails.requestId == 0U); }
        store->items[0] = chain;
    } else {
        UmiIbkrOptionChainSnapshot out, before;
        memset(&out, 0x5a, sizeof(out)); before = out;
        if (!strcmp(test, "invalid")) { age = 0U; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "missing")) { expected = 71U; result = UMI_STATUS_NOT_FOUND; }
        else if (!strcmp(test, "partial")) store->snapshot.complete = false;
        else if (!strcmp(test, "age-boundary")) now = 65U;
        else if (!strcmp(test, "age-before-boundary")) now = 64U;
        else if (!strcmp(test, "disconnect")) f->c->snapshot.state = UMI_IBKR_DISCONNECTED;
        else if (!strcmp(test, "session-stale")) f->c->snapshot.stale = true;
        else if (!strcmp(test, "clock-backward")) { now = 9U; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "future-request")) { store->snapshot.requestedAtMilliseconds = 21U; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "future-completion")) { store->snapshot.completedAtMilliseconds = 21U; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "completion-before-request")) { store->snapshot.completedAtMilliseconds = 9U; result = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(test, "capacity")) { store->snapshot.count = 33U; result = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(test, "timeout")) { store->snapshot.complete = false; now = 60010U; }
        else if (!strcmp(test, "abandoned")) store->snapshot.abandoned = true;
        else if (!strcmp(test, "failed")) store->snapshot.failed = true;
        else if (strcmp(test, "complete") && strcmp(test, "latest")) return 2;
        UmiStatus status = !strcmp(test, "latest")
            ? UmiIbkrOptionChainCopyLatestWithAge(f->c, now, age, &out)
            : UmiIbkrOptionChainCopyWithAge(f->c, expected, now, age, &out);
        CHECK(status == result);
        if (result != UMI_STATUS_OK) CHECK(!memcmp(&out, &before, sizeof(out)));
        else {
            bool stale = !store->snapshot.complete || store->snapshot.failed || store->snapshot.abandoned ||
                f->c->snapshot.stale || f->c->snapshot.state != UMI_IBKR_READY || now >= 65U;
            CHECK(out.stale == stale && out.requestId == 70U);
            CHECK(out.requestedAtMilliseconds == 10U && !store->snapshot.stale);
            if (!strcmp(test, "timeout")) CHECK(out.failed && !store->snapshot.failed);
        }
        CHECK(f->c->txSize == 0U);
    }
    CHECK(f->opens == 0U && f->outSize == 0U);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *f = New(); if (f == NULL) return 1;
    int result = Run(f, argv[1]); Delete(f); return result;
}
