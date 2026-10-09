/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education_workspace/local_record.c
 * PURPOSE: Validate explicit learning storage inputs before creating or adopting a SQLite connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/education_workspace/local_record.h"
#include "private.h"
#include "umicom/platform/filesystem.h"
#include <string.h>
UmiStatus UmiEducationLocalRecordOpenAt(const char *path, const char *id, const char *name,
                                        UmiDataServer **out_server,
                                        UmiEducationWorkspace **out_workspace)
{
    if (out_server == NULL || out_workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_server = NULL;
    *out_workspace = NULL;
    if (!EwTextValid(path, UMI_PATH_CAPACITY, false) || !umi_path_is_absolute(path) ||
        !EwIdValid(id) || !EwTextValid(name, UMI_EDUCATION_NAME_CAPACITY, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; path[index] != '\0'; ++index)
        if ((unsigned char)path[index] < 32U || path[index] == '?' || path[index] == '#')
            return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    if (!((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) ||
        path[1] != ':' || (path[2] != '/' && path[2] != '\\'))
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    size_t length = strlen(path);
    if (path[length - 1U] == '/' || path[length - 1U] == '\\')
        return UMI_STATUS_INVALID_ARGUMENT;
    char normalised[UMI_PATH_CAPACITY], parent[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_normalise(path, normalised, sizeof normalised);
    if (status == UMI_STATUS_OK)
        status = umi_path_parent(normalised, parent, sizeof parent);
    if (status != UMI_STATUS_OK)
        return status;
    if (!umi_fs_is_directory(parent))
        return UMI_STATUS_NOT_FOUND;
    if (umi_fs_is_directory(normalised))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Do not replace a saved learner while preparing another connection. The
     * caller adopts both owners only after the existing record has been read.
     * SQLite availability is explicit: a memory fallback would lose progress. */
    UmiDataServer *server = NULL;
    UmiEducationWorkspace *workspace = NULL;
    status = umi_data_server_create_sqlite(normalised, &server);
    if (status == UMI_STATUS_OK)
        status = UmiEducationOpen(server, id, name, &workspace);
    if (status == UMI_STATUS_OK)
    {
        *out_server = server;
        *out_workspace = workspace;
    }
    else
    {
        UmiEducationClose(workspace);
        umi_data_server_destroy(server);
    }
    return status;
}
