/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/scope_context.c
 *
 * PURPOSE:
 *   Track nested runtime scopes without allocating application-owned containers.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/scope_context.h"
#include "umicom/runtime/bootstrap/scope_kind.h"


#include <string.h>
void umi_bootstrap_scope_context_init(UmiBootstrapScopeContext *context) {
    if (context != NULL) memset(context, 0, sizeof(*context));
}
UmiStatus umi_bootstrap_scope_context_push(UmiBootstrapScopeContext *context,
                                           UmiBootstrapScopeKind scope) {
    if (context == NULL || umi_bootstrap_scope_kind_rank(scope) == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (context->depth >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    context->stack[context->depth++] = scope;
    return UMI_STATUS_OK;
}
UmiStatus umi_bootstrap_scope_context_pop(UmiBootstrapScopeContext *context,
                                          UmiBootstrapScopeKind *out_scope) {
    if (context == NULL || context->depth == 0U) return UMI_STATUS_INVALID_STATE;
    --context->depth;
    if (out_scope != NULL) *out_scope = context->stack[context->depth];
    return UMI_STATUS_OK;
}
