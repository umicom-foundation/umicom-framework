/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/scope_context.h
 *
 * PURPOSE:
 *   Track nested runtime scopes without allocating application-owned containers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_SCOPE_CONTEXT_H
#define UMICOM_RUNTIME_BOOTSTRAP_SCOPE_CONTEXT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_scope_context_init(UmiBootstrapScopeContext *context);
UmiStatus umi_bootstrap_scope_context_push(UmiBootstrapScopeContext *context,
                                           UmiBootstrapScopeKind scope);
UmiStatus umi_bootstrap_scope_context_pop(UmiBootstrapScopeContext *context,
                                          UmiBootstrapScopeKind *out_scope);

#ifdef __cplusplus
}
#endif

#endif
