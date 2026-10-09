/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/location.c
 * PURPOSE: Validate terminal folder choices without changing the application working directory.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/location.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <string.h>

UmiStatus UmiTerminalDirectorySelect(const char *path, char *out_directory, size_t capacity)
{
    if (out_directory == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    char input[UMI_TERMINAL_PATH_CAPACITY];
    size_t length = 0U;
    if (path != NULL)
    {
        while (length < sizeof input && path[length] != '\0')
            ++length;
        if (length < sizeof input)
            memcpy(input, path, length + 1U);
    }
    out_directory[0] = '\0';
    if (path == NULL || length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length == sizeof input)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    path = input;
    if (!umi_path_is_absolute(path))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < length; ++index)
        if ((unsigned char)path[index] < 32U || (unsigned char)path[index] == 127U)
            return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    /* Keep interactive selection on an ordinary local drive. Device names and
     * network shares have different execution and availability rules. */
    if (length < 3U ||
        !((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) ||
        path[1] != ':' || (path[2] != '/' && path[2] != '\\'))
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    char selected[UMI_TERMINAL_PATH_CAPACITY];
    UmiStatus status = umi_path_normalise(path, selected, sizeof selected);
    if (status != UMI_STATUS_OK)
        return status;
    if (!umi_fs_is_directory(selected))
        return UMI_STATUS_NOT_FOUND;
    size_t selected_length = strlen(selected);
    if (selected_length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_directory, selected, selected_length + 1U);
    return UMI_STATUS_OK;
}
UmiStatus UmiTerminalSessionChooseDirectory(UmiTerminalSession *session, const char *path)
{
    UmiTerminalSessionSnapshot snapshot;
    if (session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_terminal_session_snapshot(session, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    if (snapshot.state == UMI_TERMINAL_CLOSED)
        return UMI_STATUS_INVALID_STATE;
    if (snapshot.state == UMI_TERMINAL_RUNNING)
        return UMI_STATUS_BUSY;
    char directory[UMI_TERMINAL_PATH_CAPACITY];
    status = UmiTerminalDirectorySelect(path, directory, sizeof directory);
    return status == UMI_STATUS_OK ? umi_terminal_session_set_working_directory(session, directory)
                                   : status;
}
