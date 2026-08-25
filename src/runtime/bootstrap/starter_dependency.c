/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_dependency.c
 *
 * PURPOSE:
 *   Declare explicit dependencies between reusable application starters.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/starter_dependency.h"


#include <string.h>
UmiStatus umi_bootstrap_starter_dependency_init(
    UmiBootstrapStarterDependency *dependency,
    const char *starter_id,
    const char *requires_starter_id,
    bool required) {
    UmiStatus status;
    if (dependency == NULL || !umi_bootstrap_id_valid(starter_id) ||
        !umi_bootstrap_id_valid(requires_starter_id) ||
        strcmp(starter_id, requires_starter_id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(dependency, 0, sizeof(*dependency));
    status = umi_bootstrap_copy_text(dependency->starter_id,
        sizeof(dependency->starter_id), starter_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(dependency->requires_starter_id,
        sizeof(dependency->requires_starter_id), requires_starter_id);
    if (status != UMI_STATUS_OK) return status;
    dependency->required = required;
    return UMI_STATUS_OK;
}
