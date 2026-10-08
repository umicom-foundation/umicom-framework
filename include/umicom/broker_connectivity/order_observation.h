/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/order_observation.h
 * PURPOSE: Keep broker status observations distinct from local order-state decisions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_ORDER_OBSERVATION_H
#define UMICOM_BROKER_CONNECTIVITY_ORDER_OBSERVATION_H
#include <stdbool.h>
#include "umicom/finance/decimal.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiIbkrOrderPhase
    {
        UMI_IBKR_ORDER_UNRECOGNIZED,
        UMI_IBKR_ORDER_INACTIVE,
        UMI_IBKR_ORDER_PENDING_SUBMIT,
        UMI_IBKR_ORDER_PRE_SUBMITTED,
        UMI_IBKR_ORDER_WORKING,
        UMI_IBKR_ORDER_PENDING_CANCEL,
        UMI_IBKR_ORDER_CANCELLED,
        UMI_IBKR_ORDER_FILLED,
        UMI_IBKR_ORDER_WARNING
    } UmiIbkrOrderPhase;
    typedef struct UmiIbkrOrderObservation
    {
        char providerStatus[32];
        UmiIbkrOrderPhase phase;
        UmiDecimal filled, remaining;
        bool hasFills, hasRemaining, cancellationPending, terminal, conflictingQuantities;
        bool hasCanonicalStatus;
        UmiOrderStatus canonicalStatus;
    } UmiIbkrOrderObservation;
    /* Pure classification; no order is submitted, cancelled or added to a journal.
 * Keep the original provider status even when its meaning is not recognized.
 * Filled/remaining are cumulative observations, never quantities to add again.
 * A cancelled remainder does not mean that no shares filled. Inactive and
 * cancellation-pending messages are not evidence of rejection or cancellation.
 * Use canonicalStatus only when hasCanonicalStatus is true; sequence/identity
 * checks still belong to the journal owner. Failure leaves out unchanged. */
    UmiStatus UmiIbkrReviewOrderObservation(const char *providerStatus, UmiDecimal filled,
                                            UmiDecimal remaining, UmiIbkrOrderObservation *out);
    /* Recognize only a bounded, nonempty, printable ASCII status (at most 31 bytes).
 * A future status remains UNRECOGNIZED, rather than acquiring trading meaning. */
    UmiStatus UmiIbkrClassifyOrderPhase(const char *providerStatus, UmiIbkrOrderPhase *out);
#ifdef __cplusplus
}
#endif
#endif
