/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/starter_dependency.h
 *
 * PURPOSE:
 *   Declare explicit dependencies between reusable application starters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTER_DEPENDENCY_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTER_DEPENDENCY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap starter dependency from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_starter_dependency_init(
    UmiBootstrapStarterDependency *dependency,
    const char *starter_id,
    const char *requires_starter_id,
    bool required);

#ifdef __cplusplus
}
#endif

#endif
