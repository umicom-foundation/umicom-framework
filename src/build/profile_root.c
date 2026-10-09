/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/profile_root.c
 * PURPOSE: Resolve project roots once before background work leaves the owner thread.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/profile.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <string.h>

UmiStatus UmiBuildProfileSourceDirectory(const UmiBuildProfile *profile, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    char root[UMI_BUILD_PATH_CAPACITY], current[UMI_BUILD_PATH_CAPACITY];
    /* Absolute roots do not depend on a process directory that another tool
     * may change or remove. Relative callers capture their current root once. */
    if (umi_path_is_absolute(profile->source_directory))
        status = umi_path_normalise(profile->source_directory, root, sizeof(root));
    else
    {
        status = umi_fs_current_directory(current, sizeof(current));
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(profile->source_directory, current, root, sizeof(root));
    }
    if (status != UMI_STATUS_OK)
        return status;
    size_t length = strlen(root);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memmove(out, root, length + 1U);
    return UMI_STATUS_OK;
}
