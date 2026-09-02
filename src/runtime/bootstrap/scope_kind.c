/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/scope_kind.c
 *
 * PURPOSE:
 *   Implement the scope kind behavior for
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
 * File: src/runtime/bootstrap/scope_kind.c
 *
 * PURPOSE:
 *   Provide stable names and breadth ordering for dependency scopes.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/scope_kind.h"


/*
 * Provide the bootstrap scope kind text operation used by this module and its client
 * applications.
 */
const char *umi_bootstrap_scope_kind_text(UmiBootstrapScopeKind scope) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (scope) {
        case UMI_BOOTSTRAP_SCOPE_SINGLETON: return "singleton";
        case UMI_BOOTSTRAP_SCOPE_APPLICATION: return "application";
        case UMI_BOOTSTRAP_SCOPE_SESSION: return "session";
        case UMI_BOOTSTRAP_SCOPE_REQUEST: return "request";
        case UMI_BOOTSTRAP_SCOPE_TRANSIENT: return "transient";
        default: return "unknown";
    }
}
/*
 * Provide the bootstrap scope kind rank operation used by this module and its client
 * applications.
 */
int umi_bootstrap_scope_kind_rank(UmiBootstrapScopeKind scope) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (scope) {
        case UMI_BOOTSTRAP_SCOPE_SINGLETON: return 5;
        case UMI_BOOTSTRAP_SCOPE_APPLICATION: return 4;
        case UMI_BOOTSTRAP_SCOPE_SESSION: return 3;
        case UMI_BOOTSTRAP_SCOPE_REQUEST: return 2;
        case UMI_BOOTSTRAP_SCOPE_TRANSIENT: return 1;
        default: return 0;
    }
}
