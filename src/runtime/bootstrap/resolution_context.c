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
void umi_bootstrap_resolution_context_init(UmiBootstrapResolutionContext *context) {
    if (context != NULL) memset(context, 0, sizeof(*context));
}
bool umi_bootstrap_resolution_context_contains(
    const UmiBootstrapResolutionContext *context,
    const UmiBootstrapServiceKey *key) {
    size_t i;
    if (context == NULL || key == NULL) return false;
    for (i = 0U; i < context->depth; ++i)
        if (umi_bootstrap_service_key_equal(&context->stack[i], key)) return true;
    return false;
}
UmiStatus umi_bootstrap_resolution_context_push(
    UmiBootstrapResolutionContext *context,
    const UmiBootstrapServiceKey *key) {
    if (context == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_bootstrap_resolution_context_contains(context, key))
        return UMI_STATUS_INVALID_STATE;
    if (context->depth >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    context->stack[context->depth++] = *key;
    return UMI_STATUS_OK;
}
UmiStatus umi_bootstrap_resolution_context_pop(UmiBootstrapResolutionContext *context) {
    if (context == NULL || context->depth == 0U) return UMI_STATUS_INVALID_STATE;
    --context->depth;
    return UMI_STATUS_OK;
}
