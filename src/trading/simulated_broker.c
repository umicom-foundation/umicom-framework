/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/simulated_broker.c
 *
 * PURPOSE:
 *   Provide a deterministic paper/simulation broker implementing the public broker contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of simulated broker. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/simulated_broker.h"
#include "umicom/trading/order_request.h"
#include "umicom/trading/order_state.h"
#include "umicom/finance/identifier.h"

/* Validation belongs to the shared adapter: calling through the public broker
 * table must enforce the same contracts as calling the domain services directly.
 * The previous callbacks are retained below for engineering review. They accepted
 * mismatched modes and changed terminal orders without checking revision bounds. */
#if 0
/* Provide the sim connect operation used by this module and its client applications. */
static UmiStatus sim_connect(void *instance,
                             UmiTradingEnvironment environment)
{
    UmiSimulatedBrokerStorage *broker =
        (UmiSimulatedBrokerStorage *)instance;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || environment == UMI_TRADING_LIVE) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    broker->environment = environment;
    broker->connected = 1;
    return UMI_STATUS_OK;
}

/* Provide the sim submit operation used by this module and its client applications. */
static UmiStatus sim_submit(void *instance,
                            const UmiOrderRequest *request)
{
    UmiSimulatedBrokerStorage *broker =
        (UmiSimulatedBrokerStorage *)instance;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || request == NULL || !broker->connected) {
        return UMI_STATUS_INVALID_STATE;
    }

    UmiOrder order = {0};
    order.request = *request;
    order.status = UMI_ORDER_ACCEPTED;
    order.version = 1U;
    return umi_order_store_add(&broker->orders, &order);
}

/* Provide the sim cancel operation used by this module and its client applications. */
static UmiStatus sim_cancel(void *instance,
                            const UmiFinancialId *client_order_id)
{
    UmiSimulatedBrokerStorage *broker =
        (UmiSimulatedBrokerStorage *)instance;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || client_order_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    UmiOrder *order = NULL;
    const UmiStatus status =
        umi_order_store_find(&broker->orders,
                             client_order_id->value,
                             &order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    order->status = UMI_ORDER_CANCELLED;
    order->version++;
    return UMI_STATUS_OK;
}

#endif

/* Only local simulation modes are supported. A rejected connection leaves the
 * existing owner state intact; this provider never opens a network connection. */
static UmiStatus sim_connect(void *instance, UmiTradingEnvironment environment)
{
    UmiSimulatedBrokerStorage *broker = instance;
    if (broker == NULL || environment == UMI_TRADING_LIVE)
        return UMI_STATUS_PERMISSION_DENIED;
    if (environment != UMI_TRADING_SIMULATION && environment != UMI_TRADING_PAPER)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (broker->orders.count > UMI_TRADING_MAX_ORDERS)
        return UMI_STATUS_INVALID_STATE;
    /* Retained orders keep their environment identity until this local session
     * is destroyed. Reconnecting the same mode preserves that order history. */
    if (broker->orders.count != 0U && broker->environment != environment)
        return UMI_STATUS_INVALID_STATE;
    broker->environment = environment;
    broker->connected = 1;
    return UMI_STATUS_OK;
}

static UmiStatus sim_submit(void *instance, const UmiOrderRequest *request)
{
    UmiSimulatedBrokerStorage *broker = instance;
    if (broker == NULL || request == NULL || !broker->connected)
        return UMI_STATUS_INVALID_STATE;
    if (broker->environment != UMI_TRADING_SIMULATION &&
        broker->environment != UMI_TRADING_PAPER) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = umi_order_request_validate(request);
    if (status != UMI_STATUS_OK) return status;
    if (request->environment != broker->environment) return UMI_STATUS_PERMISSION_DENIED;
    UmiOrder order = {0};
    order.request = *request;
    order.status = UMI_ORDER_ACCEPTED;
    order.version = 1U;
    return umi_order_store_add(&broker->orders, &order);
}

static UmiStatus sim_cancel(void *instance, const UmiFinancialId *client_order_id)
{
    UmiSimulatedBrokerStorage *broker = instance;
    if (broker == NULL || client_order_id == NULL ||
        !umi_financial_id_valid(client_order_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!broker->connected || (broker->environment != UMI_TRADING_SIMULATION &&
        broker->environment != UMI_TRADING_PAPER)) return UMI_STATUS_INVALID_STATE;
    UmiOrder *order = NULL;
    UmiStatus status = umi_order_store_find(&broker->orders, client_order_id->value, &order);
    if (status != UMI_STATUS_OK) return status;
    if (order->request.environment != broker->environment ||
        !umi_order_transition_allowed(order->status, UMI_ORDER_CANCELLED))
        return UMI_STATUS_INVALID_STATE;
    /* Repeated cancellation is an idempotent acknowledgement, not a new revision. */
    if (order->status == UMI_ORDER_CANCELLED) return UMI_STATUS_OK;
    if (order->version == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    order->status = UMI_ORDER_CANCELLED;
    ++order->version;
    return UMI_STATUS_OK;
}

/* Release or reset state held by sim so the same storage can be reused safely. */
static void sim_destroy(void *instance)
{
    UmiSimulatedBrokerStorage *broker =
        (UmiSimulatedBrokerStorage *)instance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker != NULL) {
        broker->connected = 0;
        umi_order_store_init(&broker->orders);
    }
}

/*
 * Initialise simulated broker from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_simulated_broker_create(UmiSimulatedBrokerStorage *storage,
                                      UmiBroker *out_broker)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (storage == NULL || out_broker == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *storage = (UmiSimulatedBrokerStorage){0};
    umi_order_store_init(&storage->orders);

    out_broker->instance = storage;
    out_broker->provider_name = "umicom.simulated";
    out_broker->connect = sim_connect;
    out_broker->submit_order = sim_submit;
    out_broker->cancel_order = sim_cancel;
    out_broker->destroy = sim_destroy;
    return UMI_STATUS_OK;
}
