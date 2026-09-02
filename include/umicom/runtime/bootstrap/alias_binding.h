/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/alias_binding.h
 *
 * PURPOSE:
 *   Map alternate service identifiers without copying or owning instances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_ALIAS_BINDING_H
#define UMICOM_RUNTIME_BOOTSTRAP_ALIAS_BINDING_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap alias binding configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_alias_binding_configure(
    UmiBootstrapBindingDescriptor *binding,
    const char *target_service_id);
/**
 * Check that bootstrap alias binding satisfies its contract before another service relies
 * on it.
 */
bool umi_bootstrap_alias_binding_valid(
    const UmiBootstrapBindingDescriptor *binding);

#ifdef __cplusplus
}
#endif

#endif
