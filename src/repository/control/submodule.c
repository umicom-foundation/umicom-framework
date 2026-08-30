/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/submodule.c
 *
 * PURPOSE:
 *   Model one Framework-owned Git submodule dependency.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/submodule.h"

#include <string.h>

#include "umicom/repository/path.h"
#include "umicom/repository/ref.h"

static UmiStatus copy_field(
    char *out, size_t capacity, const char *text, int allow_empty)
{
    size_t length;
    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    if ((!allow_empty && length == 0U) || length + 1U > capacity) {
        return length + 1U > capacity
            ? UMI_STATUS_CAPACITY_EXCEEDED
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_submodule_init(
    UmiRepositorySubmodule *submodule,
    const char *name,
    const char *path,
    const char *url,
    const char *branch,
    int required)
{
    UmiStatus status;
    if (submodule == NULL || name == NULL || path == NULL || url == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(submodule, 0, sizeof(*submodule));
    status = copy_field(submodule->name, sizeof(submodule->name), name, 0);
    if (status != UMI_STATUS_OK) return status;
    status = umi_repository_control_path_normalize(
        path, submodule->path, sizeof(submodule->path));
    if (status != UMI_STATUS_OK) return status;
    status = copy_field(submodule->url, sizeof(submodule->url), url, 1);
    if (status != UMI_STATUS_OK) return status;
    if (branch != NULL && branch[0] != '\0') {
        status = umi_repository_ref_copy(
            branch, submodule->branch, sizeof(submodule->branch));
        if (status != UMI_STATUS_OK) return status;
    }
    submodule->required = required != 0;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_submodule_validate(
    const UmiRepositorySubmodule *submodule)
{
    if (submodule == NULL || submodule->name[0] == '\0' ||
        !umi_repository_control_path_is_safe_relative(submodule->path)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (submodule->branch[0] != '\0' &&
        !umi_repository_ref_is_valid(submodule->branch)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
