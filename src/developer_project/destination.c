/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/destination.c
 * PURPOSE: Resolve a project folder from an explicit parent selection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/new_project.h"
#include "umicom/platform/path.h"
#include "umicom/platform/rooted_files.h"
#include <string.h>

UmiStatus UmiDeveloperProjectChooseDestination(const char *parent, const char *folderName,
                                               char *out, size_t capacity)
{
    if (parent == NULL || folderName == NULL || out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A picker chooses one parent, so a folder name cannot silently select a
     * sibling or another nested location. Manual full-path entry stays separate. */
    if (folderName[0] == '\0' || strchr(folderName, '/') != NULL ||
        strchr(folderName, '\\') != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiRootedFileValidatePath(parent, folderName);
    if (status != UMI_STATUS_OK)
        return status;
    char candidate[UMI_PATH_CAPACITY];
    status = umi_path_join(parent, folderName, candidate, sizeof candidate);
    if (status != UMI_STATUS_OK)
        return status;
    size_t length = strlen(candidate);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, candidate, length + 1U);
    return UMI_STATUS_OK;
}
