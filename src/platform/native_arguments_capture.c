/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/native_arguments_capture.c
 * PURPOSE: Capture the operating system argument vector through its native Unicode boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/native_arguments.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <shellapi.h>
#include <stdlib.h>
#include <windows.h>

/* Windows' narrow main arguments may already have lost characters. Read the original
 * wide command line and reject invalid UTF-16 instead of substituting another filename. */
UmiStatus UmiNativeArgumentsCapture(int argc, char *const *argv, UmiNativeArguments **out_arguments)
{
    int count = 0;
    wchar_t **wide;
    char **converted = NULL;
    size_t total = 0U;
    UmiStatus status = UMI_STATUS_OK;
    (void)argc;
    (void)argv;
    if (out_arguments == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    wide = CommandLineToArgvW(GetCommandLineW(), &count);
    if (wide == NULL)
        return UMI_STATUS_IO_ERROR;
    if (count < 0 || (unsigned)count > UMI_NATIVE_ARGUMENT_COUNT_LIMIT)
    {
        LocalFree(wide);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    converted = calloc((size_t)count + 1U, sizeof(*converted));
    if (converted == NULL)
    {
        LocalFree(wide);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (int index = 0; index < count; ++index)
    {
        /* Query the required byte count first; wide character counts are not UTF-8 byte capacities.
         */
        int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[index], -1, NULL, 0,
                                        NULL, NULL);
        if (bytes <= 0)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        if ((size_t)bytes > UMI_NATIVE_ARGUMENT_BYTE_LIMIT - total)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        converted[index] = malloc((size_t)bytes);
        if (converted[index] == NULL)
        {
            status = UMI_STATUS_OUT_OF_MEMORY;
            break;
        }
        if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[index], -1, converted[index],
                                bytes, NULL, NULL) != bytes)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        total += (size_t)bytes;
    }
    if (status == UMI_STATUS_OK)
        status = UmiNativeArgumentsCopy(count, (const char *const *)converted, out_arguments);
    for (int index = 0; index < count; ++index)
        free(converted[index]);
    free(converted);
    LocalFree(wide);
    return status;
}
#else
/* POSIX already supplies separate argument byte strings. Do not reinterpret shell quotes or locale.
 */
UmiStatus UmiNativeArgumentsCapture(int argc, char *const *argv, UmiNativeArguments **out_arguments)
{
    return UmiNativeArgumentsCopy(argc, (const char *const *)argv, out_arguments);
}
#endif
