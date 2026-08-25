/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/lifetime_policy.h
 *
 * PURPOSE:
 *   Centralise eager, lazy and externally-owned service creation policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_LIFETIME_POLICY_H
#define UMICOM_RUNTIME_BOOTSTRAP_LIFETIME_POLICY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


bool umi_bootstrap_lifetime_should_create_eagerly(
    UmiBootstrapLifetimeKind lifetime,
    bool explicitly_required);
bool umi_bootstrap_lifetime_framework_owns(
    UmiBootstrapLifetimeKind lifetime);

#ifdef __cplusplus
}
#endif

#endif
