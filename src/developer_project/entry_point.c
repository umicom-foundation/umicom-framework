/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/entry_point.c
 * PURPOSE: Resolve a generated project entry without assuming every template uses src/main.c.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/new_project.h"
#include "umicom/platform/path.h"
#include <string.h>
UmiStatus UmiDeveloperProjectEntryPath(const UmiDeveloperProjectModel *project, char *out_path,
                                       size_t capacity)
{
    if (out_path == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_developer_project_model_validate(project, NULL, 0U);
    if (status != UMI_STATUS_OK || !umi_path_is_absolute(project->root))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *entry = project->entry_point[0] != '\0' ? project->entry_point : "src/main.c";
    /* Template metadata is a portable relative source path. Do not interpret
     * drive-relative strings, absolute paths or device namespaces as entries. */
    if (entry[0] == '/' || entry[0] == '\\' || strchr(entry, ':') != NULL ||
        umi_path_is_absolute(entry))
        return UMI_STATUS_INVALID_ARGUMENT;
    char candidate[UMI_PATH_CAPACITY];
    status = umi_path_join(project->root, entry, candidate, sizeof candidate);
    if (status == UMI_STATUS_OK &&
        (!umi_path_is_within(project->root, candidate) || umi_path_equal(project->root, candidate)))
        return UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK)
        status = umi_path_copy(out_path, capacity, candidate);
    return status;
}
