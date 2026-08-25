/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/scope_policy.h
 *
 * PURPOSE:
 *   Enforce legal lifetime relationships between owners and requested dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_SCOPE_POLICY_H
#define UMICOM_RUNTIME_BOOTSTRAP_SCOPE_POLICY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


bool umi_bootstrap_scope_policy_can_depend(
    UmiBootstrapScopeKind owner_scope,
    UmiBootstrapScopeKind dependency_scope);

#ifdef __cplusplus
}
#endif

#endif
