/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/dependency_descriptor.c
 *
 * PURPOSE:
 *   Implement the dependency descriptor behavior for
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
 * File: src/runtime/bootstrap/dependency_descriptor.c
 *
 * PURPOSE:
 *   Declare required, optional and collection dependencies between Framework services.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/dependency_descriptor.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>

UmiStatus umi_bootstrap_dependency_descriptor_init(
    UmiBootstrapDependencyDescriptor *dependency,
    const char *service_id,
    const char *qualifier,
    bool required,
    bool collection) {
    UmiStatus status;
    if (dependency == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(dependency, 0, sizeof(*dependency));
    status = umi_bootstrap_service_key_init(&dependency->key, service_id, qualifier);
    if (status != UMI_STATUS_OK) return status;
    dependency->required = required;
    dependency->collection = collection;
    return UMI_STATUS_OK;
}
