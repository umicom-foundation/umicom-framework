/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/route.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/route.h"

#include <string.h>

/* Provide the match text operation used by this module and its client applications. */
static bool match_text(const char *pattern, const char *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pattern == NULL || value == NULL) {
        return false;
    }
    return strcmp(pattern, "*") == 0 || strcmp(pattern, value) == 0;
}

/*
 * Provide the integration route matches operation used by this module and its client
 * applications.
 */
bool umi_integration_route_matches(
    const UmiIntegrationRoute *route,
    const char *source_application,
    const char *target_application,
    const char *topic,
    UmiIntegrationMessageKind kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (route == NULL) {
        return false;
    }
    return route->kind == kind &&
           match_text(route->source_application, source_application) &&
           match_text(route->target_application, target_application) &&
           match_text(route->topic, topic);
}
