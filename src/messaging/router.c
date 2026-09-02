/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/router.c
 *
 * PURPOSE:
 *   Implement subscription-based fan-out routing into destination channels with delivery and unrouted counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/router.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiRoute {
    UmiSubscription subscription;
    UmiChannel *destination;
} UmiRoute;

struct UmiMessageRouter {
    UmiRoute *routes;
    size_t capacity;
    size_t count;
    UmiRouterStats stats;
};

/*
 * Initialise message router from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_message_router_create(size_t capacity,
                                    UmiMessageRouter **out_router)
{
    UmiMessageRouter *router;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_router == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_router = NULL;
    router = (UmiMessageRouter *)calloc(1U, sizeof(*router));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    router->routes = (UmiRoute *)calloc(capacity, sizeof(*router->routes));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router->routes == NULL) {
        free(router);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    router->capacity = capacity;
    *out_router = router;
    return UMI_STATUS_OK;
}

/* Release or reset state held by message router so the same storage can be reused safely. */
void umi_message_router_destroy(UmiMessageRouter *router)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL) return;
    free(router->routes);
    free(router);
}

/* Add message router only after its inputs and available capacity have been checked. */
UmiStatus umi_message_router_add(UmiMessageRouter *router,
                                 const UmiSubscription *subscription,
                                 UmiChannel *destination)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || subscription == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (router->count >= router->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    router->routes[router->count].subscription = *subscription;
    router->routes[router->count].destination = destination;
    router->count++;
    router->stats.routes = router->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the message router route operation used by this module and its client
 * applications.
 */
UmiStatus umi_message_router_route(UmiMessageRouter *router,
                                   const UmiMessageEnvelope *message,
                                   size_t *out_deliveries)
{
    size_t index;
    size_t deliveries = 0U;
    UmiStatus first_failure = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    router->stats.messages++;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < router->count; ++index) {
        UmiStatus status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!umi_subscription_matches(&router->routes[index].subscription,
                                      message)) {
            continue;
        }
        status = umi_channel_push(router->routes[index].destination, message);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            deliveries++;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (first_failure == UMI_STATUS_OK) {
            first_failure = status;
        }
    }
    router->stats.deliveries += (uint64_t)deliveries;
    /* Apply this branch only when its contract condition is satisfied. */
    if (deliveries == 0U) router->stats.unrouted++;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_deliveries != NULL) *out_deliveries = deliveries;
    return first_failure;
}

/*
 * Provide the message router stats operation used by this module and its client
 * applications.
 */
UmiRouterStats umi_message_router_stats(const UmiMessageRouter *router)
{
    UmiRouterStats stats;
    (void)memset(&stats, 0, sizeof(stats));
    return router != NULL ? router->stats : stats;
}
