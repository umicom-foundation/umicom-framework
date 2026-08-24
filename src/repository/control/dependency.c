/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/dependency.c
 *
 * PURPOSE:
 *   Define reusable repository dependency nodes.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/dependency.h"

#include <string.h>

#include "umicom/repository/path.h"

UmiStatus umi_repository_dependency_init(
    UmiRepositoryDependency *dependency,
    const char *id,
    const char *path,
    int required)
{
    size_t id_length;
    UmiStatus status;
    if (dependency == NULL || id == NULL || path == NULL || id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    id_length = strlen(id);
    if (id_length + 1U > sizeof(dependency->id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(dependency, 0, sizeof(*dependency));
    (void)memcpy(dependency->id, id, id_length + 1U);
    status = umi_repository_control_path_normalize(
        path, dependency->path, sizeof(dependency->path));
    if (status != UMI_STATUS_OK) return status;
    dependency->required = required != 0;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_dependency_validate(
    const UmiRepositoryDependency *dependency)
{
    if (dependency == NULL || dependency->id[0] == '\0' ||
        !umi_repository_control_path_is_safe_relative(dependency->path)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
