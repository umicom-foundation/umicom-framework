/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_execution_capture.c
 * PURPOSE: Exercise recent execution framing, account isolation, replay and request lifetime with injected I/O.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>
static int ExecutionFeed(Fixture *f, const char *request, const char *id, const char *account,
                         const char *quantity, const char *price, const char *side, size_t count)
{
    /* Independent provider-shaped example: conId 123, permanent order 77 and
     * one reported execution. Cumulative quantity is intentionally not the
     * sum of this packet's quantity; callers must not add it a second time. */
    const char *fields[] = {
        "11",    request, "42",           "123",    "EXAMPLE", "STK", "",  "0",
        "",      "",      "LSE",          "GBP",    "EXM",     "EXM", id,  "20261007 12:00:00 Europe/London",
        account, "LSE",   side,           quantity, price,     "77",  "9", "0",
        "7000",  "1.23",  "local-review", "",       "0",       "",    "1"};
    return Feed(f, fields, count);
}
static int Run(Fixture *f, UmiIbkrExecutionSnapshot *copy, const char *mode)
{
    (void)PositionFeed;
    CHECK(Connect(f) == 0);
    uint32_t request = 999U;
    if (!strcmp(mode, "unlisted"))
    {
        CHECK(UmiIbkrExecutionsRequest(f->c, "DU999", 10U, &request) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(request == 999U && f->c->executions.requestId == 0U);
        return 0;
    }
    if (!strcmp(mode, "queue-full"))
    {
        f->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrExecutionsRequest(f->c, "DU123", 10U, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(request == 999U && f->c->executions.requestId == 0U);
        return 0;
    }
    if (!strcmp(mode, "unsupported"))
    {
        f->c->snapshot.protocolVersion = 178;
        CHECK(UmiIbkrExecutionsRequest(f->c, "DU123", 10U, &request) == UMI_STATUS_NOT_IMPLEMENTED);
        CHECK(request == 999U);
        return 0;
    }
    size_t sent = f->outSize;
    CHECK(UmiIbkrExecutionsRequest(f->c, "DU123", 10U, &request) == UMI_STATUS_OK);
    CHECK(request == 36000U);
    CHECK(UmiIbkrExecutionsCopy(f->c, request, 10U, copy) == UMI_STATUS_OK);
    CHECK(!copy->complete && !copy->failed && copy->count == 0U);
    if (!strcmp(mode, "wire"))
    {
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        const char expected[] = "7\0"
                                "3\0"
                                "36000\0"
                                "0\0"
                                "DU123\0"
                                "\0"
                                "\0"
                                "\0"
                                "\0"
                                "\0";
        size_t body = sizeof expected - 1U;
        CHECK(f->outSize - sent == body + 4U);
        CHECK(f->output[sent] == 0U && f->output[sent + 1U] == 0U && f->output[sent + 2U] == 0U &&
              f->output[sent + 3U] == body);
        CHECK(memcmp(f->output + sent + 4U, expected, body) == 0);
        return 0;
    }
    if (!strcmp(mode, "busy"))
    {
        size_t pending = f->c->txSize;
        uint32_t sentinel = 999U;
        CHECK(UmiIbkrExecutionsRequest(f->c, "DU456", 11U, &sentinel) == UMI_STATUS_BUSY);
        CHECK(f->c->txSize == pending && sentinel == 999U);
        return 0;
    }
    if (!strcmp(mode, "shared-identity"))
    {
        UmiIbkrQuoteContract contract = {0};
        contract.contractId = 123U;
        strcpy(contract.exchange, "SMART");
        uint32_t other = 0U;
        CHECK(UmiIbkrContractDetailsRequest(f->c, &contract, 10U, &other) == UMI_STATUS_OK);
        CHECK(other == request + 1U);
        return 0;
    }
    if (!strcmp(mode, "timeout"))
    {
        CHECK(UmiIbkrExecutionsCopy(f->c, request, 10010U, copy) == UMI_STATUS_OK);
        CHECK(copy->failed && copy->stale && !copy->complete);
        FEED(f, "55", "1", "36000");
        CHECK(UmiIbkrConnectionPump(f->c, 10010U) == UMI_STATUS_OK);
        CHECK(UmiIbkrExecutionsCopy(f->c, request, 10010U, copy) == UMI_STATUS_OK);
        CHECK(copy->failed && !copy->complete);
        return 0;
    }
    if (!strcmp(mode, "abandon"))
    {
        CHECK(UmiIbkrExecutionsAbandon(f->c, request) == UMI_STATUS_OK);
        FEED(f, "55", "1", "36000");
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrExecutionsCopy(f->c, request, 11U, copy) == UMI_STATUS_OK);
        CHECK(copy->failed && !copy->complete);
        return 0;
    }
    if (!strcmp(mode, "late-request"))
    {
        CHECK(UmiIbkrExecutionsAbandon(f->c, request) == UMI_STATUS_OK);
        CHECK(UmiIbkrExecutionsRequest(f->c, "DU456", 10U, &request) == UMI_STATUS_OK);
        CHECK(request == 36001U);
        CHECK(ExecutionFeed(f, "36000", "one.01", "DU123", "10", "1.23", "BOT", 31U) == 0);
        FEED(f, "55", "1", "36000");
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrExecutionsCopy(f->c, request, 11U, copy) == UMI_STATUS_OK);
        CHECK(!copy->complete && copy->count == 0U && strcmp(copy->account, "DU456") == 0);
        return 0;
    }
    if (!strcmp(mode, "refusal"))
    {
        FEED(f, "4", "2", "36000", "321", "Execution request refused.");
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrExecutionsCopy(f->c, request, 11U, copy) == UMI_STATUS_OK);
        CHECK(copy->failed && f->c->snapshot.state == UMI_IBKR_READY);
        return 0;
    }
    if (!strcmp(mode, "disconnected"))
    {
        f->eof = true;
        CHECK(UmiIbkrConnectionPump(f->c, 11U) != UMI_STATUS_OK);
        CHECK(UmiIbkrExecutionsCopy(f->c, request, 11U, copy) == UMI_STATUS_OK);
        CHECK(copy->stale);
        return 0;
    }
    if (!strcmp(mode, "empty"))
    {
        FEED(f, "55", "1", "36000");
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrExecutionsCopy(f->c, request, 11U, copy) == UMI_STATUS_OK);
        CHECK(copy->complete && !copy->failed && copy->count == 0U);
        return 0;
    }
    if (!strcmp(mode, "foreign") || !strcmp(mode, "unsolicited"))
    {
        CHECK(ExecutionFeed(f, !strcmp(mode, "unsolicited") ? "-1" : "36000", "one.01",
                            !strcmp(mode, "foreign") ? "DU456" : "DU123", "10", "1.23", "BOT", 31U) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        CHECK(f->c->executions.count == 0U);
        return 0;
    }
    if (!strcmp(mode, "invalid-utf8") || !strcmp(mode, "negative") || !strcmp(mode, "precision") ||
        !strcmp(mode, "bad-side") || !strcmp(mode, "bad-count"))
    {
        CHECK(ExecutionFeed(f, "36000", !strcmp(mode, "invalid-utf8") ? "bad\xc0\xaf" : "one.01", "DU123",
                            !strcmp(mode, "negative") ? "-1" : "10",
                            !strcmp(mode, "precision") ? "0.0000000001" : "1.23",
                            !strcmp(mode, "bad-side") ? "BUY" : "BOT",
                            !strcmp(mode, "bad-count") ? 30U : 31U) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 11U) != UMI_STATUS_OK);
        CHECK(f->c->executions.count == 0U);
        return 0;
    }
    if (!strcmp(mode, "capacity"))
    {
        f->c->executions.count = UMI_IBKR_EXECUTION_OBSERVATION_LIMIT;
        CHECK(ExecutionFeed(f, "36000", "new.01", "DU123", "10", "1.23", "BOT", 31U) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->c->executions.count == UMI_IBKR_EXECUTION_OBSERVATION_LIMIT);
        return 0;
    }
    CHECK(ExecutionFeed(f, "36000", "one.01", "DU123", "10", "1.23", "BOT", 31U) == 0);
    CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    if (!strcmp(mode, "duplicate-conflict"))
    {
        CHECK(ExecutionFeed(f, "36000", "one.01", "DU123", "11", "1.23", "BOT", 31U) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_PARSE_ERROR);
        CHECK(f->c->executions.rows[0].quantity.coefficient == 10);
        return 0;
    }
    if (!strcmp(mode, "valid") || !strcmp(mode, "duplicate"))
        CHECK(ExecutionFeed(f, "36000", "one.01", "DU123", "10.0", "1.2300", "BOT", 31U) == 0);
    if (!strcmp(mode, "valid"))
        CHECK(ExecutionFeed(f, "36000", "two.01", "DU123", "6990", "1.23", "BOT", 31U) == 0);
    if (!strcmp(mode, "correction"))
        CHECK(ExecutionFeed(f, "36000", "one.02", "DU123", "9", "1.24", "BOT", 31U) == 0);
    if (strcmp(mode, "valid") && strcmp(mode, "duplicate") && strcmp(mode, "correction"))
        return 2;
    FEED(f, "55", "1", "36000");
    CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
    CHECK(UmiIbkrExecutionsCopy(f->c, request, 12U, copy) == UMI_STATUS_OK);
    CHECK(copy->complete && !copy->stale && !copy->failed);
    CHECK(copy->count == (!strcmp(mode, "duplicate") ? 1U : 2U));
    CHECK(copy->duplicateCount == (!strcmp(mode, "correction") ? 0U : 1U));
    CHECK(copy->rows[0].permanentOrderId == 77U && copy->rows[0].contractId == 123U);
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
