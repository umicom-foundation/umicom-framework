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


UmiStatus umi_bootstrap_alias_binding_configure(
    UmiBootstrapBindingDescriptor *binding,
    const char *target_service_id);
bool umi_bootstrap_alias_binding_valid(
    const UmiBootstrapBindingDescriptor *binding);

#ifdef __cplusplus
}
#endif

#endif
