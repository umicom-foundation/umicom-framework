/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_position_review.c
 * PURPOSE: Reject stale quote choices while retaining exact, independently owned position observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/position_review.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            failed = 1;                                                                                      \
            goto done;                                                                                       \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    int failed = 0;
    UmiIbkrConnectionSnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    UmiIbkrPositionReview *review = NULL;
    CHECK(snapshot != NULL);
    snapshot->state = UMI_IBKR_READY;
    snapshot->readOnly = true;
    snapshot->requestIssued = true;
    snapshot->requestedEnvironment = UMI_TRADING_PAPER;
    snapshot->positionsComplete = true;
    snapshot->requestedAtMilliseconds = 100U;
    snapshot->positionsAtMilliseconds = 200U;
    snapshot->accountCount = 1U;
    strcpy(snapshot->accounts[0], "PAPER-EXAMPLE");
    strcpy(snapshot->selectedAccount, "PAPER-EXAMPLE");
    snapshot->positionCount = 1U;
    UmiIbkrPositionObservation *position = &snapshot->positions[0];
    strcpy(position->contractId, "123");
    strcpy(position->symbol, "EXAMPLE");
    strcpy(position->securityType, "STK");
    strcpy(position->exchange, "SMART");
    strcpy(position->currency, "USD");
    strcpy(position->quantity, "0.00000000000001");
    strcpy(position->averageCost, "100.00000000000009");
    if (!strcmp(name, "no-request"))
        snapshot->requestIssued = false;
    else if (!strcmp(name, "unlisted"))
        strcpy(snapshot->selectedAccount, "OTHER");
    else if (!strcmp(name, "duplicate-account"))
    {
        snapshot->accountCount = 2U;
        strcpy(snapshot->accounts[1], snapshot->accounts[0]);
    }
    else if (!strcmp(name, "unterminated"))
        memset(position->quantity, 'x', sizeof(position->quantity));
    else if (!strcmp(name, "too-many"))
        snapshot->positionCount = UMI_IBKR_POSITION_LIMIT + 1U;
    else if (!strcmp(name, "partial"))
        snapshot->positionsComplete = false;
    else if (!strcmp(name, "disconnected"))
        snapshot->state = UMI_IBKR_DISCONNECTED;
    else if (!strcmp(name, "attested"))
        snapshot->environmentAttested = true;
    else if (!strcmp(name, "writable"))
        snapshot->readOnly = false;
    else if (!strcmp(name, "empty"))
        snapshot->positionCount = 0U;
    else if (!strcmp(name, "bad-id"))
        strcpy(position->contractId, "123x");
    else if (!strcmp(name, "id-overflow"))
        strcpy(position->contractId, "9999999999999999");
    else if (!strcmp(name, "no-exchange"))
        position->exchange[0] = '\0';
    else if (!strcmp(name, "combination"))
        strcpy(position->securityType, "BAG");
    else if (!strcmp(name, "exchange-control"))
        strcpy(position->exchange, "SMA\nRT");
    UmiStatus created = UmiIbkrPositionReviewCreate(snapshot, &review);
    if (!strcmp(name, "no-request") || !strcmp(name, "unlisted") || !strcmp(name, "duplicate-account") ||
        !strcmp(name, "attested") || !strcmp(name, "writable"))
    {
        CHECK(created == UMI_STATUS_INVALID_STATE && review == NULL);
        goto done;
    }
    if (!strcmp(name, "unterminated") || !strcmp(name, "too-many"))
    {
        CHECK(created == UMI_STATUS_INVALID_ARGUMENT && review == NULL);
        goto done;
    }
    CHECK(created == UMI_STATUS_OK);
    UmiIbkrPositionReviewSummary summary;
    CHECK(UmiIbkrPositionReviewRead(review, &summary) == UMI_STATUS_OK);
    CHECK(summary.count == snapshot->positionCount);
    if (!strcmp(name, "empty"))
    {
        UmiIbkrPositionObservation output;
        CHECK(UmiIbkrPositionReviewAt(review, 0U, &output) == UMI_STATUS_NOT_FOUND);
        goto done;
    }
    UmiIbkrPositionObservation captured;
    CHECK(UmiIbkrPositionReviewAt(review, 0U, &captured) == UMI_STATUS_OK);
    CHECK(!strcmp(captured.quantity, "0.00000000000001") &&
          !strcmp(captured.averageCost, "100.00000000000009"));
    if (!strcmp(name, "changed-row") || !strcmp(name, "owned"))
        strcpy(position->quantity, "42");
    else if (!strcmp(name, "changed-request"))
        ++snapshot->requestedAtMilliseconds;
    else if (!strcmp(name, "changed-completion"))
        ++snapshot->positionsAtMilliseconds;
    else if (!strcmp(name, "changed-account"))
    {
        strcpy(snapshot->accounts[0], "OTHER");
        strcpy(snapshot->selectedAccount, "OTHER");
    }
    else if (!strcmp(name, "changed-environment"))
        snapshot->requestedEnvironment = UMI_TRADING_LIVE;
    else if (!strcmp(name, "stale"))
        snapshot->stale = true;
    UmiIbkrQuoteContract output = {0}, before = {0};
    output.contractId = 777U;
    strcpy(output.exchange, "UNCHANGED");
    before = output;
    UmiStatus status = UmiIbkrPositionReviewQuote(review, 0U, snapshot, &output);
    if (!strcmp(name, "valid"))
        CHECK(status == UMI_STATUS_OK && output.contractId == 123U && !strcmp(output.exchange, "SMART"));
    else
    {
        CHECK(status != UMI_STATUS_OK && output.contractId == before.contractId &&
              !strcmp(output.exchange, before.exchange));
        if (!strcmp(name, "partial"))
            CHECK(!summary.complete && summary.completedAtMilliseconds == 0U);
        if (!strcmp(name, "disconnected"))
            CHECK(summary.stale);
        if (!strcmp(name, "owned"))
        {
            CHECK(UmiIbkrPositionReviewAt(review, 0U, &captured) == UMI_STATUS_OK);
            CHECK(!strcmp(captured.quantity, "0.00000000000001"));
        }
    }
done:
    UmiIbkrPositionReviewDestroy(review);
    free(snapshot);
    return failed;
}
