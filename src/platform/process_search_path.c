/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_search_path.c
 * PURPOSE: Build bounded child PATH overrides from an explicit tool directory.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/process_search_path.h"
#include "umicom/platform/path.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#define SEARCH_PATH_CAPACITY 131072U
#ifdef _WIN32
#define SEARCH_PATH_SEPARATOR ';'
#else
#define SEARCH_PATH_SEPARATOR ':'
#endif

UmiStatus UmiProcessSearchDirectoryValidate(const char *directory)
{
    if (directory == NULL || !umi_path_is_absolute(directory))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PATH_CAPACITY && directory[length] != '\0')
    {
        unsigned char byte = (unsigned char)directory[length++];
#ifndef _WIN32
        /* The shared path joiner treats a backslash as a separator. Reject
         * literal POSIX backslashes here rather than select a different file. */
        if (byte == '\\')
            return UMI_STATUS_INVALID_ARGUMENT;
#endif
        if (byte < 0x20U || byte == 0x7fU || byte == (unsigned char)SEARCH_PATH_SEPARATOR)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return length == UMI_PATH_CAPACITY ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

UmiStatus UmiProcessSearchPathJoin(const char *directory, const char *inheritedPath, char **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = UmiProcessSearchDirectoryValidate(directory);
    if (status != UMI_STATUS_OK)
        return status;
    size_t prefix = strlen(directory), inherited = 0U;
    if (inheritedPath != NULL)
        while (inherited < SEARCH_PATH_CAPACITY && inheritedPath[inherited] != '\0')
            ++inherited;
    size_t separator = inherited != 0U ? 1U : 0U;
    if (inherited >= SEARCH_PATH_CAPACITY || prefix + separator >= SEARCH_PATH_CAPACITY - inherited)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *value = malloc(prefix + separator + inherited + 1U);
    if (value == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(value, directory, prefix);
    if (separator != 0U)
        value[prefix] = SEARCH_PATH_SEPARATOR;
    if (inherited != 0U)
        memcpy(value + prefix + separator, inheritedPath, inherited);
    value[prefix + separator + inherited] = '\0';
    *out = value;
    return UMI_STATUS_OK;
}

/* Native PATH capture is now reusable by tool discovery as well as child launch. One UTF-8 conversion path prevents a Windows ANSI environment copy from changing a selected installation. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiProcessSearchPathCapture(const char *directory, char **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = UmiProcessSearchDirectoryValidate(directory);
    if (status != UMI_STATUS_OK)
        return status;
#ifdef _WIN32
    /* Query the native environment rather than the ANSI C runtime copy. A
     * bounded retry tolerates a changed value between sizing and copying. */
    for (unsigned attempt = 0U; attempt < 3U; ++attempt)
    {
        SetLastError(ERROR_SUCCESS);
        DWORD capacity = GetEnvironmentVariableW(L"PATH", NULL, 0U);
        if (capacity == 0U)
        {
            DWORD error = GetLastError();
            return error == ERROR_ENVVAR_NOT_FOUND || error == ERROR_SUCCESS
                       ? UmiProcessSearchPathJoin(directory, NULL, out)
                       : UMI_STATUS_IO_ERROR;
        }
        if (capacity > 32768U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        wchar_t *wide = calloc((size_t)capacity, sizeof *wide);
        if (wide == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
        SetLastError(ERROR_SUCCESS);
        DWORD length = GetEnvironmentVariableW(L"PATH", wide, capacity);
        DWORD error = GetLastError();
        if (length >= capacity)
        {
            free(wide);
            continue;
        }
        if (length == 0U && error != ERROR_SUCCESS && error != ERROR_ENVVAR_NOT_FOUND)
        {
            free(wide);
            return UMI_STATUS_IO_ERROR;
        }
        int bytes =
            WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, NULL, 0, NULL, NULL);
        if (bytes <= 0)
        {
            free(wide);
            return UMI_STATUS_PARSE_ERROR;
        }
        char *inherited = malloc((size_t)bytes);
        if (inherited == NULL)
        {
            free(wide);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        int written = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, inherited, bytes,
                                          NULL, NULL);
        free(wide);
        status = written == bytes ? UmiProcessSearchPathJoin(directory, inherited, out)
                                  : UMI_STATUS_PARSE_ERROR;
        free(inherited);
        return status;
    }
    return UMI_STATUS_BUSY;
#else
    return UmiProcessSearchPathJoin(directory, getenv("PATH"), out);
#endif
}
#endif
/* Copying even an empty value gives the caller one clear ownership rule.
 * This helper is shared by native Windows absence and POSIX environment reads. */
static UmiStatus SearchPathCopy(const char *value, char **out)
{
    if (value == NULL) value = "";
    size_t length = 0U;
    while (length < SEARCH_PATH_CAPACITY && value[length] != '\0') ++length;
    if (length == SEARCH_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *owned = malloc(length + 1U);
    if (owned == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(owned, value, length + 1U);
    *out = owned;
    return UMI_STATUS_OK;
}

UmiStatus UmiProcessSearchPathRead(char **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
#ifdef _WIN32
    /* Query the native environment rather than the ANSI C runtime copy. A
     * bounded retry tolerates a changed value between sizing and copying. */
    for (unsigned attempt = 0U; attempt < 3U; ++attempt)
    {
        SetLastError(ERROR_SUCCESS);
        DWORD capacity = GetEnvironmentVariableW(L"PATH", NULL, 0U);
        if (capacity == 0U)
        {
            DWORD error = GetLastError();
            return error == ERROR_ENVVAR_NOT_FOUND || error == ERROR_SUCCESS
                       ? SearchPathCopy("", out)
                       : UMI_STATUS_IO_ERROR;
        }
        if (capacity > 32768U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        wchar_t *wide = calloc((size_t)capacity, sizeof *wide);
        if (wide == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
        SetLastError(ERROR_SUCCESS);
        DWORD length = GetEnvironmentVariableW(L"PATH", wide, capacity);
        DWORD error = GetLastError();
        if (length >= capacity)
        {
            free(wide);
            continue;
        }
        if (length == 0U && error != ERROR_SUCCESS && error != ERROR_ENVVAR_NOT_FOUND)
        {
            free(wide);
            return UMI_STATUS_IO_ERROR;
        }
        int bytes =
            WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, NULL, 0, NULL, NULL);
        if (bytes <= 0)
        {
            free(wide);
            return UMI_STATUS_PARSE_ERROR;
        }
        char *inherited = malloc((size_t)bytes);
        if (inherited == NULL)
        {
            free(wide);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        int written = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, inherited, bytes,
                                          NULL, NULL);
        free(wide);
        if (written != bytes)
        {
            free(inherited);
            return UMI_STATUS_PARSE_ERROR;
        }
        *out = inherited;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_BUSY;
#else
    return SearchPathCopy(getenv("PATH"), out);
#endif
}

UmiStatus UmiProcessSearchPathCapture(const char *directory, char **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = UmiProcessSearchDirectoryValidate(directory);
    if (status != UMI_STATUS_OK) return status;
    char *inherited = NULL;
    status = UmiProcessSearchPathRead(&inherited);
    if (status == UMI_STATUS_OK)
        status = UmiProcessSearchPathJoin(directory, inherited, out);
    UmiProcessSearchPathFree(inherited);
    return status;
}

void UmiProcessSearchPathFree(char *path) { free(path); }

UmiStatus UmiProcessToolProgram(const char *directory, const char *name, char *out, size_t capacity)
{
    if (name == NULL || name[0] == '\0' || out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = 0U;
    while (count < 127U && name[count] != '\0')
    {
        unsigned char ch = (unsigned char)name[count++];
        if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') ||
              ch == '_' || ch == '-'))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (count == 127U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char candidate[UMI_PATH_CAPACITY];
    UmiStatus status = UMI_STATUS_OK;
    if (directory == NULL || directory[0] == '\0')
    {
        memcpy(candidate, name, count + 1U);
    }
    else
    {
        status = UmiProcessSearchDirectoryValidate(directory);
        if (status != UMI_STATUS_OK)
            return status;
        char filename[132];
        memcpy(filename, name, count + 1U);
#ifdef _WIN32
        memcpy(filename + count, ".exe", 5U);
#endif
        status = umi_path_join(directory, filename, candidate, sizeof candidate);
        if (status != UMI_STATUS_OK)
            return status;
    }
    size_t length = strlen(candidate);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, candidate, length + 1U);
    return UMI_STATUS_OK;
}

/* Windows environment names are case-insensitive; POSIX names are not.
 * Keep this comparison local to PATH so unrelated overrides remain untouched. */
static int SearchPathName(const char *name)
{
    if (name == NULL)
        return 0;
#ifdef _WIN32
    const char *expected = "PATH";
    size_t index = 0U;
    while (name[index] != '\0' && expected[index] != '\0')
    {
        unsigned char ch = (unsigned char)name[index];
        if (ch >= 'a' && ch <= 'z')
            ch = (unsigned char)(ch - 'a' + 'A');
        if (ch != (unsigned char)expected[index])
            return 0;
        ++index;
    }
    return name[index] == '\0' && expected[index] == '\0';
#else
    return strcmp(name, "PATH") == 0;
#endif
}

UmiStatus UmiProcessExecuteTool(const UmiProcessRequest *request, const char *directory,
                                UmiProcessLifetime lifetime, UmiProcessOutputObserver observer,
                                void *context, UmiProcessResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    out->exit_code = -1;
    if (request == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (directory == NULL || directory[0] == '\0')
        return UmiProcessExecuteWithLifetime(request, lifetime, NULL, observer, context, out);
    if (request->environment_count > UMI_PROCESS_MAX_ENVIRONMENT ||
        (request->environment_count != 0U && request->environment == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    char program[UMI_PATH_CAPACITY];
    UmiStatus status = UmiProcessToolProgram(directory, request->program, program, sizeof program);
    if (status != UMI_STATUS_OK)
        return status;
    UmiEnvironmentVariable environment[UMI_PROCESS_MAX_ENVIRONMENT];
    size_t count = request->environment_count, pathIndex = count;
    for (size_t index = 0U; index < count; ++index)
    {
        environment[index] = request->environment[index];
        if (environment[index].name == NULL || environment[index].value == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (SearchPathName(environment[index].name))
        {
            if (pathIndex != count)
                return UMI_STATUS_INVALID_ARGUMENT;
            pathIndex = index;
        }
    }
    if (pathIndex == count && count == UMI_PROCESS_MAX_ENVIRONMENT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *path = NULL;
    status = pathIndex == count
                 ? UmiProcessSearchPathCapture(directory, &path)
                 : UmiProcessSearchPathJoin(directory, environment[pathIndex].value, &path);
    if (status != UMI_STATUS_OK)
        return status;
    if (pathIndex == count)
        ++count;
    environment[pathIndex].name = "PATH";
    environment[pathIndex].value = path;
    UmiProcessRequest selected = *request;
    selected.program = program;
    selected.environment = environment;
    selected.environment_count = count;
    /* The synchronous runner consumes borrowed program/environment values
     * before these scoped buffers are released. The host PATH is never set. */
    status = UmiProcessExecuteWithLifetime(&selected, lifetime, NULL, observer, context, out);
    UmiProcessSearchPathFree(path);
    return status;
}
