/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/constructor_binding.h
 *
 * PURPOSE:
 *   Capture constructor-style dependency lists for C factory functions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONSTRUCTOR_BINDING_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONSTRUCTOR_BINDING_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap constructor binding from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_constructor_binding_init(
    UmiBootstrapConstructorBinding *binding,
    const char *implementation_id);
/**
 * Provide the bootstrap constructor binding add dependency operation used by this module
 * and its client applications.
 */
UmiStatus umi_bootstrap_constructor_binding_add_dependency(
    UmiBootstrapConstructorBinding *binding,
    const UmiBootstrapDependencyDescriptor *dependency);

#ifdef __cplusplus
}
#endif

#endif
