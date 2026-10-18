/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/impact_expression.c
 * Purpose: Encode exact test selections without wildcard expansion or empty-filter ambiguity.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "impact_internal.h"
#include <string.h>
#define EXPRESSION_LIMIT (24U * 1024U)
static bool Special(unsigned char ch)
{
    return strchr(".[](){}*+?^$|\\", (int)ch) != NULL;
}
UmiStatus UmiReleaseImpactTestExpression(const UmiReleaseImpactPlan *plan,
    char *output, size_t capacity, size_t *outRequired)
{
    if (plan == NULL || outRequired == NULL || (output == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->summary.fallbackReasons != 0U || plan->summary.tests == 0U)
        return UMI_STATUS_UNAVAILABLE;
    size_t required = 5U; /* ^( ... )$ plus terminating NUL. */
    size_t names = 0U;
    for (size_t i = 0U; i < plan->count; ++i) {
        const UmiReleaseImpactItem *item = &plan->items[i];
        if (item->kind != UMI_RELEASE_INVENTORY_TEST) continue;
        if (names++ != 0U) ++required;
        for (const unsigned char *p = (const unsigned char *)item->name; *p != 0U; ++p) {
            if (*p < 32U || *p == 127U) return UMI_STATUS_UNAVAILABLE;
            required += Special(*p) ? 2U : 1U;
            if (required > EXPRESSION_LIMIT) return UMI_STATUS_UNAVAILABLE;
        }
    }
    if (required > EXPRESSION_LIMIT) return UMI_STATUS_UNAVAILABLE;
    if (output != NULL && capacity < required) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (output != NULL) {
        size_t used = 0U;
        output[used++] = '^'; output[used++] = '(';
        names = 0U;
        for (size_t i = 0U; i < plan->count; ++i) {
            const UmiReleaseImpactItem *item = &plan->items[i];
            if (item->kind != UMI_RELEASE_INVENTORY_TEST) continue;
            if (names++ != 0U) output[used++] = '|';
            for (const unsigned char *p = (const unsigned char *)item->name; *p != 0U; ++p) {
                if (Special(*p)) output[used++] = '\\';
                output[used++] = (char)*p;
            }
        }
        output[used++] = ')'; output[used++] = '$'; output[used] = '\0';
    }
    *outRequired = required;
    return UMI_STATUS_OK;
}
