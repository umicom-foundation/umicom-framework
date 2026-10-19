/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/directory_guard.c
 * Purpose: Resolve one persistent guard path without changing the process working directory.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/directory_guard.h"
#include "umicom/platform/output_file.h"
#include <string.h>

UmiStatus UmiBuildDirectoryGuardPath(const char *directory, char *output, size_t capacity)
{
    if (directory == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PATH_CAPACITY && directory[length] != '\0') ++length;
    if (length == UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t minimum = 1U;
#ifdef _WIN32
    minimum = 3U;
#endif
    while (length > minimum && (directory[length - 1U] == '/'
#ifdef _WIN32
           || directory[length - 1U] == '\\'
#endif
    )) --length;
    const char leaf[] = ".umicom-build.lock";
    bool separator = directory[length - 1U] != '/';
#ifdef _WIN32
    separator = separator && directory[length - 1U] != '\\';
#endif
    size_t needed = length + (separator ? 1U : 0U) + sizeof(leaf);
    if (needed > UMI_PATH_CAPACITY || capacity < needed) return UMI_STATUS_CAPACITY_EXCEEDED;
    char path[UMI_PATH_CAPACITY];
    memcpy(path, directory, length);
    if (separator) path[length++] = '/';
    memcpy(path + length, leaf, sizeof(leaf));
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    memcpy(output, path, needed);
    return UMI_STATUS_OK;
}

UmiStatus UmiBuildDirectoryGuardAcquire(const char *directory, UmiFileLease **outLease)
{
    if (outLease == NULL || *outLease != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char path[UMI_PATH_CAPACITY];
    UmiStatus status = UmiBuildDirectoryGuardPath(directory, path, sizeof(path));
    if (status != UMI_STATUS_OK) return status;
    return UmiFileLeaseTryAcquire(path, outLease);
}
