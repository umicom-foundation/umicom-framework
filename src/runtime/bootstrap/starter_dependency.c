/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_dependency.c
 *
 * PURPOSE:
 *   Implement the starter dependency behavior for
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
 * File: src/runtime/bootstrap/starter_dependency.c
 *
 * PURPOSE:
 *   Declare explicit dependencies between reusable application starters.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/starter_dependency.h"
#include "../../base/value_construction_internal.h"


#include <string.h>
/*
 * Initialise bootstrap starter dependency from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_bootstrap_starter_dependency_init(
    UmiBootstrapStarterDependency *dependency,
    const char *starter_id,
    const char *requires_starter_id,
    bool required) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dependency == NULL || !umi_bootstrap_id_valid(starter_id) ||
        !umi_bootstrap_id_valid(requires_starter_id) ||
        strcmp(starter_id, requires_starter_id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(dependency, 0, sizeof(*dependency));
    status = umi_bootstrap_copy_text(dependency->starter_id,
        sizeof(dependency->starter_id), starter_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(dependency->requires_starter_id,
        sizeof(dependency->requires_starter_id), requires_starter_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    dependency->required = required;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_starter_dependency_init_build_value(
    UmiBootstrapStarterDependency *dependency,
    const char *starter_id,
    const char *requires_starter_id,
    bool required) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dependency == NULL || !umi_bootstrap_id_valid(starter_id) ||
        !umi_bootstrap_id_valid(requires_starter_id) ||
        strcmp(starter_id, requires_starter_id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(dependency, 0, sizeof(*dependency));
    status = umi_bootstrap_copy_text(dependency->starter_id,
        sizeof(dependency->starter_id), starter_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(dependency->requires_starter_id,
        sizeof(dependency->requires_starter_id), requires_starter_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    dependency->required = required;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_starter_dependency_init, UmiBootstrapStarterDependency, dependency,
    (UmiBootstrapStarterDependency *dependency, const char *starter_id, const char *requires_starter_id, bool required),
    umi_bootstrap_starter_dependency_init_build_value(candidate, starter_id, requires_starter_id, required))
