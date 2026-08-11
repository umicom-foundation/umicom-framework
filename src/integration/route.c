/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/route.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/route.h"

#include <string.h>

static bool match_text(const char *pattern, const char *value)
{
    if (pattern == NULL || value == NULL) {
        return false;
    }
    return strcmp(pattern, "*") == 0 || strcmp(pattern, value) == 0;
}

bool umi_integration_route_matches(
    const UmiIntegrationRoute *route,
    const char *source_application,
    const char *target_application,
    const char *topic,
    UmiIntegrationMessageKind kind)
{
    if (route == NULL) {
        return false;
    }
    return route->kind == kind &&
           match_text(route->source_application, source_application) &&
           match_text(route->target_application, target_application) &&
           match_text(route->topic, topic);
}
