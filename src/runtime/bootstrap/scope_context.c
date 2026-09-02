/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/scope_context.c
 *
 * PURPOSE:
 *   Implement the scope context behavior for
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
 * File: src/runtime/bootstrap/scope_context.c
 *
 * PURPOSE:
 *   Track nested runtime scopes without allocating application-owned containers.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/scope_context.h"
#include "umicom/runtime/bootstrap/scope_kind.h"


#include <string.h>
/*
 * Initialise bootstrap scope context from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_scope_context_init(UmiBootstrapScopeContext *context) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context != NULL) memset(context, 0, sizeof(*context));
}
/*
 * Provide the bootstrap scope context push operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_scope_context_push(UmiBootstrapScopeContext *context,
                                           UmiBootstrapScopeKind scope) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || umi_bootstrap_scope_kind_rank(scope) == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (context->depth >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    context->stack[context->depth++] = scope;
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap scope context pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_scope_context_pop(UmiBootstrapScopeContext *context,
                                          UmiBootstrapScopeKind *out_scope) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->depth == 0U) return UMI_STATUS_INVALID_STATE;
    --context->depth;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_scope != NULL) *out_scope = context->stack[context->depth];
    return UMI_STATUS_OK;
}
