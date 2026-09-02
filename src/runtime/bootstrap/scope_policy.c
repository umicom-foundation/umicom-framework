/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/scope_policy.c
 *
 * PURPOSE:
 *   Implement the scope policy behavior for
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
 * File: src/runtime/bootstrap/scope_policy.c
 *
 * PURPOSE:
 *   Enforce legal lifetime relationships between owners and requested dependencies.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/scope_policy.h"
#include "umicom/runtime/bootstrap/scope_kind.h"


/*
 * Provide the bootstrap scope policy can depend operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_scope_policy_can_depend(
    UmiBootstrapScopeKind owner_scope,
    UmiBootstrapScopeKind dependency_scope) {
    int owner = umi_bootstrap_scope_kind_rank(owner_scope);
    int dependency = umi_bootstrap_scope_kind_rank(dependency_scope);
    /* Apply this branch only when its contract condition is satisfied. */
    if (owner == 0 || dependency == 0) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (dependency_scope == UMI_BOOTSTRAP_SCOPE_TRANSIENT) return true;
    return dependency >= owner;
}
