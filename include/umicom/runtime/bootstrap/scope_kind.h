/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/scope_kind.h
 *
 * PURPOSE:
 *   Provide stable names and breadth ordering for dependency scopes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_SCOPE_KIND_H
#define UMICOM_RUNTIME_BOOTSTRAP_SCOPE_KIND_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap scope kind text operation used by this module and its client
 * applications.
 */
const char *umi_bootstrap_scope_kind_text(UmiBootstrapScopeKind scope);
/**
 * Provide the bootstrap scope kind rank operation used by this module and its client
 * applications.
 */
int umi_bootstrap_scope_kind_rank(UmiBootstrapScopeKind scope);

#ifdef __cplusplus
}
#endif

#endif
