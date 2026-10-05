/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/position_review.c
 * PURPOSE: Keep a position review immutable while its source connection continues receiving observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/position_review.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct UmiIbkrPositionReview
{
    UmiIbkrPositionReviewSummary summary;
    UmiIbkrPositionObservation positions[UMI_IBKR_POSITION_LIMIT];
};

/* Fixed provider fields must terminate before any display, parse or comparison.
 * Keep this list beside the matching comparator when adding a contract field. */
static bool PositionTextValid(const UmiIbkrPositionObservation *position)
{
#define POSITION_FIELD(field)                                                                                \
    if (memchr(position->field, '\0', sizeof(position->field)) == NULL)                                      \
    return false
    POSITION_FIELD(contractId);
    POSITION_FIELD(symbol);
    POSITION_FIELD(securityType);
    POSITION_FIELD(expiry);
    POSITION_FIELD(strike);
    POSITION_FIELD(right);
    POSITION_FIELD(multiplier);
    POSITION_FIELD(exchange);
    POSITION_FIELD(currency);
    POSITION_FIELD(localSymbol);
    POSITION_FIELD(tradingClass);
    POSITION_FIELD(quantity);
    POSITION_FIELD(averageCost);
#undef POSITION_FIELD
    return true;
}
static bool PositionEqual(const UmiIbkrPositionObservation *left, const UmiIbkrPositionObservation *right)
{
#define SAME_FIELD(field)                                                                                    \
    if (strcmp(left->field, right->field) != 0)                                                              \
    return false
    SAME_FIELD(contractId);
    SAME_FIELD(symbol);
    SAME_FIELD(securityType);
    SAME_FIELD(expiry);
    SAME_FIELD(strike);
    SAME_FIELD(right);
    SAME_FIELD(multiplier);
    SAME_FIELD(exchange);
    SAME_FIELD(currency);
    SAME_FIELD(localSymbol);
    SAME_FIELD(tradingClass);
    SAME_FIELD(quantity);
    SAME_FIELD(averageCost);
#undef SAME_FIELD
    return true;
}
static UmiStatus PositionSnapshotValid(const UmiIbkrConnectionSnapshot *snapshot)
{
    if (snapshot == NULL || snapshot->positionCount > UMI_IBKR_POSITION_LIMIT ||
        snapshot->accountCount > UMI_IBKR_ACCOUNT_LIMIT ||
        memchr(snapshot->selectedAccount, '\0', sizeof(snapshot->selectedAccount)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!snapshot->requestIssued || !snapshot->readOnly || snapshot->environmentAttested ||
        snapshot->selectedAccount[0] == '\0' ||
        (snapshot->requestedEnvironment != UMI_TRADING_PAPER &&
         snapshot->requestedEnvironment != UMI_TRADING_LIVE) ||
        (snapshot->state != UMI_IBKR_READY && snapshot->state != UMI_IBKR_DISCONNECTED &&
         snapshot->state != UMI_IBKR_FAILED) ||
        (snapshot->positionsComplete &&
         snapshot->positionsAtMilliseconds < snapshot->requestedAtMilliseconds))
        return UMI_STATUS_INVALID_STATE;
    size_t matches = 0U;
    for (size_t i = 0U; i < snapshot->accountCount; ++i)
    {
        if (memchr(snapshot->accounts[i], '\0', sizeof(snapshot->accounts[i])) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(snapshot->accounts[i], snapshot->selectedAccount) == 0)
            ++matches;
    }
    if (matches != 1U)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < snapshot->positionCount; ++i)
        if (!PositionTextValid(&snapshot->positions[i]))
            return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrPositionReviewCreate(const UmiIbkrConnectionSnapshot *snapshot,
                                      UmiIbkrPositionReview **out_review)
{
    if (out_review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_review = NULL;
    UmiStatus status = PositionSnapshotValid(snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    UmiIbkrPositionReview *review = calloc(1U, sizeof(*review));
    if (review == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(review->summary.account, snapshot->selectedAccount, sizeof(review->summary.account));
    review->summary.count = snapshot->positionCount;
    review->summary.complete = snapshot->positionsComplete;
    review->summary.stale = snapshot->stale || snapshot->state != UMI_IBKR_READY;
    review->summary.requestedEnvironment = snapshot->requestedEnvironment;
    review->summary.requestedAtMilliseconds = snapshot->requestedAtMilliseconds;
    review->summary.completedAtMilliseconds =
        snapshot->positionsComplete ? snapshot->positionsAtMilliseconds : 0U;
    memcpy(review->positions, snapshot->positions, snapshot->positionCount * sizeof(*review->positions));
    *out_review = review;
    return UMI_STATUS_OK;
}
void UmiIbkrPositionReviewDestroy(UmiIbkrPositionReview *review) { free(review); }
UmiStatus UmiIbkrPositionReviewRead(const UmiIbkrPositionReview *review,
                                    UmiIbkrPositionReviewSummary *out_summary)
{
    if (review == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = review->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrPositionReviewAt(const UmiIbkrPositionReview *review, size_t index,
                                  UmiIbkrPositionObservation *out_position)
{
    if (review == NULL || out_position == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= review->summary.count)
        return UMI_STATUS_NOT_FOUND;
    *out_position = review->positions[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrPositionReviewQuote(const UmiIbkrPositionReview *review, size_t index,
                                     const UmiIbkrConnectionSnapshot *current,
                                     UmiIbkrQuoteContract *out_contract)
{
    if (review == NULL || out_contract == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = PositionSnapshotValid(current);
    if (status != UMI_STATUS_OK)
        return status;
    if (index >= review->summary.count)
        return UMI_STATUS_NOT_FOUND;
    if (!review->summary.complete || review->summary.stale || !current->positionsComplete || current->stale ||
        current->state != UMI_IBKR_READY || current->positionCount != review->summary.count ||
        current->requestedEnvironment != review->summary.requestedEnvironment ||
        strcmp(current->selectedAccount, review->summary.account) != 0 ||
        current->requestedAtMilliseconds != review->summary.requestedAtMilliseconds ||
        current->positionsAtMilliseconds != review->summary.completedAtMilliseconds ||
        !PositionEqual(&current->positions[index], &review->positions[index]))
        return UMI_STATUS_INVALID_STATE;
    const UmiIbkrPositionObservation *position = &review->positions[index];
    if (strcmp(position->securityType, "BAG") == 0 || position->contractId[0] == '\0' ||
        position->exchange[0] == '\0')
        return UMI_STATUS_UNAVAILABLE;
    uint32_t number = 0U;
    for (const char *cursor = position->contractId; *cursor != '\0'; ++cursor)
    {
        if (*cursor < '0' || *cursor > '9')
            return UMI_STATUS_PARSE_ERROR;
        uint32_t digit = (uint32_t)(*cursor - '0');
        if (number > ((uint32_t)INT_MAX - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        number = number * 10U + digit;
    }
    if (number == 0U)
        return UMI_STATUS_PARSE_ERROR;
    for (const unsigned char *cursor = (const unsigned char *)position->exchange; *cursor; ++cursor)
        if (*cursor < 33U || *cursor > 126U)
            return UMI_STATUS_PARSE_ERROR;
    UmiIbkrQuoteContract candidate = {0};
    candidate.contractId = number;
    memcpy(candidate.exchange, position->exchange, sizeof(candidate.exchange));
    *out_contract = candidate;
    return UMI_STATUS_OK;
}
