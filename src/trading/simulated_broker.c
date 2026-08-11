/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/simulated_broker.c
 *
 * PURPOSE:
 *   Provide a deterministic paper/simulation broker implementing the public broker contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of simulated broker. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/simulated_broker.h"

static UmiStatus sim_connect(void *instance,
                             UmiTradingEnvironment environment)
{
    UmiSimulatedBrokerStorage *broker =
        (UmiSimulatedBrokerStorage *)instance;

    if (broker == NULL || environment == UMI_TRADING_LIVE) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    broker->environment = environment;
    broker->connected = 1;
    return UMI_STATUS_OK;
}

static UmiStatus sim_submit(void *instance,
                            const UmiOrderRequest *request)
{
    UmiSimulatedBrokerStorage *broker =
        (UmiSimulatedBrokerStorage *)instance;

    if (broker == NULL || request == NULL || !broker->connected) {
        return UMI_STATUS_INVALID_STATE;
    }

    UmiOrder order = {0};
    order.request = *request;
    order.status = UMI_ORDER_ACCEPTED;
    order.version = 1U;
    return umi_order_store_add(&broker->orders, &order);
}

static UmiStatus sim_cancel(void *instance,
                            const UmiFinancialId *client_order_id)
{
    UmiSimulatedBrokerStorage *broker =
        (UmiSimulatedBrokerStorage *)instance;

    if (broker == NULL || client_order_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    UmiOrder *order = NULL;
    const UmiStatus status =
        umi_order_store_find(&broker->orders,
                             client_order_id->value,
                             &order);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    order->status = UMI_ORDER_CANCELLED;
    order->version++;
    return UMI_STATUS_OK;
}

static void sim_destroy(void *instance)
{
    UmiSimulatedBrokerStorage *broker =
        (UmiSimulatedBrokerStorage *)instance;
    if (broker != NULL) {
        broker->connected = 0;
        umi_order_store_init(&broker->orders);
    }
}

UmiStatus umi_simulated_broker_create(UmiSimulatedBrokerStorage *storage,
                                      UmiBrokerV1 *out_broker)
{
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
