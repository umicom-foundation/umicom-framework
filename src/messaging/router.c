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

UmiStatus umi_message_router_create(size_t capacity,
                                    UmiMessageRouter **out_router)
{
    UmiMessageRouter *router;
    if (capacity == 0U || out_router == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_router = NULL;
    router = (UmiMessageRouter *)calloc(1U, sizeof(*router));
    if (router == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    router->routes = (UmiRoute *)calloc(capacity, sizeof(*router->routes));
    if (router->routes == NULL) {
        free(router);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    router->capacity = capacity;
    *out_router = router;
    return UMI_STATUS_OK;
}

void umi_message_router_destroy(UmiMessageRouter *router)
{
    if (router == NULL) return;
    free(router->routes);
    free(router);
}

UmiStatus umi_message_router_add(UmiMessageRouter *router,
                                 const UmiSubscription *subscription,
                                 UmiChannel *destination)
{
    if (router == NULL || subscription == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (router->count >= router->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    router->routes[router->count].subscription = *subscription;
    router->routes[router->count].destination = destination;
    router->count++;
    router->stats.routes = router->count;
    return UMI_STATUS_OK;
}

UmiStatus umi_message_router_route(UmiMessageRouter *router,
                                   const UmiMessageEnvelope *message,
                                   size_t *out_deliveries)
{
    size_t index;
    size_t deliveries = 0U;
    UmiStatus first_failure = UMI_STATUS_OK;
    if (router == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    router->stats.messages++;
    for (index = 0U; index < router->count; ++index) {
        UmiStatus status;
        if (!umi_subscription_matches(&router->routes[index].subscription,
                                      message)) {
            continue;
        }
        status = umi_channel_push(router->routes[index].destination, message);
        if (status == UMI_STATUS_OK) {
            deliveries++;
        } else if (first_failure == UMI_STATUS_OK) {
            first_failure = status;
        }
    }
    router->stats.deliveries += (uint64_t)deliveries;
    if (deliveries == 0U) router->stats.unrouted++;
    if (out_deliveries != NULL) *out_deliveries = deliveries;
    return first_failure;
}

UmiRouterStats umi_message_router_stats(const UmiMessageRouter *router)
{
    UmiRouterStats stats;
    (void)memset(&stats, 0, sizeof(stats));
    return router != NULL ? router->stats : stats;
}
