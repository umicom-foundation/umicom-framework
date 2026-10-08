/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/execution_identity.c
 * PURPOSE: Validate execution observations and compare replayed business fields explicitly.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <string.h>
bool UmiIbkrExecutionObservationValid(const UmiIbkrExecutionObservation *v)
{
    if (v == NULL || v->contractId == 0U || !UmiIbkrText(v->executionId, sizeof v->executionId, false) ||
        !UmiIbkrText(v->account, sizeof v->account, false) ||
        !UmiIbkrText(v->symbol, sizeof v->symbol, false) ||
        !UmiIbkrText(v->securityType, sizeof v->securityType, false) ||
        !UmiIbkrText(v->currency, sizeof v->currency, false) ||
        !UmiIbkrText(v->side, sizeof v->side, false) || !UmiIbkrText(v->exchange, sizeof v->exchange, true) ||
        !UmiIbkrText(v->time, sizeof v->time, false) ||
        !UmiIbkrText(v->orderReference, sizeof v->orderReference, true) ||
        (strcmp(v->side, "BOT") != 0 && strcmp(v->side, "SLD") != 0))
        return false;
    return v->quantity.scale <= 9 && v->quantity.coefficient > 0 && v->price.scale <= 9 &&
           v->cumulativeQuantity.scale <= 9 && v->cumulativeQuantity.coefficient >= 0 &&
           v->averagePrice.scale <= 9;
}
static bool ExecutionDecimalEqual(UmiDecimal a, UmiDecimal b)
{
    int comparison = 0;
    return UmiDecimalCompare(a, b, &comparison) == UMI_STATUS_OK && comparison == 0;
}
bool UmiIbkrExecutionObservationEqual(const UmiIbkrExecutionObservation *a,
                                      const UmiIbkrExecutionObservation *b)
{
    if (!UmiIbkrExecutionObservationValid(a) || !UmiIbkrExecutionObservationValid(b))
        return false;
    return strcmp(a->executionId, b->executionId) == 0 && strcmp(a->account, b->account) == 0 &&
           strcmp(a->symbol, b->symbol) == 0 && strcmp(a->securityType, b->securityType) == 0 &&
           strcmp(a->currency, b->currency) == 0 && strcmp(a->side, b->side) == 0 &&
           strcmp(a->exchange, b->exchange) == 0 && strcmp(a->time, b->time) == 0 &&
           strcmp(a->orderReference, b->orderReference) == 0 && a->contractId == b->contractId &&
           a->orderId == b->orderId && a->clientId == b->clientId &&
           a->permanentOrderId == b->permanentOrderId && ExecutionDecimalEqual(a->quantity, b->quantity) &&
           ExecutionDecimalEqual(a->price, b->price) &&
           ExecutionDecimalEqual(a->cumulativeQuantity, b->cumulativeQuantity) &&
           ExecutionDecimalEqual(a->averagePrice, b->averagePrice);
}
