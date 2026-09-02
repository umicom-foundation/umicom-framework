/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_context.c
 *
 * PURPOSE:
 *   Implement the resolution context behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_context.c
 *
 * PURPOSE:
 *   Detect recursive resolution cycles using an explicit dependency stack.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/resolution_context.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>
/*
 * Initialise bootstrap resolution context from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_resolution_context_init(UmiBootstrapResolutionContext *context) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context != NULL) memset(context, 0, sizeof(*context));
}
/*
 * Provide the bootstrap resolution context contains operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_resolution_context_contains(
    const UmiBootstrapResolutionContext *context,
    const UmiBootstrapServiceKey *key) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || key == NULL) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < context->depth; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_bootstrap_service_key_equal(&context->stack[i], key)) return true;
    return false;
}
/*
 * Provide the bootstrap resolution context push operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_resolution_context_push(
    UmiBootstrapResolutionContext *context,
    const UmiBootstrapServiceKey *key) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_bootstrap_resolution_context_contains(context, key))
        return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (context->depth >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    context->stack[context->depth++] = *key;
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap resolution context pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_resolution_context_pop(UmiBootstrapResolutionContext *context) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->depth == 0U) return UMI_STATUS_INVALID_STATE;
    --context->depth;
    return UMI_STATUS_OK;
}
