/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/lifetime_policy.c
 *
 * PURPOSE:
 *   Implement the lifetime policy behavior for
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
 * File: src/runtime/bootstrap/lifetime_policy.c
 *
 * PURPOSE:
 *   Centralise eager, lazy and externally-owned service creation policy.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/lifetime_policy.h"


/*
 * Provide the bootstrap lifetime should create eagerly operation used by this module and
 * its client applications.
 */
bool umi_bootstrap_lifetime_should_create_eagerly(
    UmiBootstrapLifetimeKind lifetime,
    bool explicitly_required) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (lifetime == UMI_BOOTSTRAP_LIFETIME_EAGER) return true;
    /* Apply this branch only when its contract condition is satisfied. */
    if (lifetime == UMI_BOOTSTRAP_LIFETIME_LAZY) return explicitly_required;
    return false;
}
/*
 * Provide the bootstrap lifetime framework owns operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_lifetime_framework_owns(
    UmiBootstrapLifetimeKind lifetime) {
    return lifetime == UMI_BOOTSTRAP_LIFETIME_EAGER ||
           lifetime == UMI_BOOTSTRAP_LIFETIME_LAZY;
}
