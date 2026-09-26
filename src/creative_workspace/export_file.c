/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/export_file.c
 * PURPOSE: Write an explicitly selected new export without overwriting paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
/* Drive paths only: device aliases, alternate streams, UNC names, trailing
 * dots/spaces and parent segments are not export destinations. */
static bool OrdinaryComponent(const char *begin, size_t length)
{
    if (length == 0U || begin[length - 1U] == '.' || begin[length - 1U] == ' ')
        return false;

    /* Win32 recognises reserved names even before an extension. Trim the
     * stem's spaces as well, rather than relying on native path normalisation. */
    size_t stemLength = 0U;
    while (stemLength < length && begin[stemLength] != '.') ++stemLength;
    while (stemLength > 0U && begin[stemLength - 1U] == ' ') --stemLength;
    if (stemLength == 0U) return false;
    if (stemLength > 7U) return true;
    char stem[8] = {0};
    for (size_t index = 0U; index < stemLength; ++index) {
        unsigned char ch = (unsigned char)begin[index];
        stem[index] = (char)(ch >= 'a' && ch <= 'z' ? ch - ('a' - 'A') : ch);
    }
    if (strcmp(stem, "CON") == 0 || strcmp(stem, "PRN") == 0 ||
        strcmp(stem, "AUX") == 0 || strcmp(stem, "NUL") == 0 ||
        strcmp(stem, "CONIN$") == 0 || strcmp(stem, "CONOUT$") == 0)
        return false;
    if (strncmp(stem, "COM", 3U) == 0 || strncmp(stem, "LPT", 3U) == 0) {
        if (stemLength == 4U && stem[3] >= '0' && stem[3] <= '9') return false;
        /* Superscript one, two and three are also recognised device suffixes. */
        if (stemLength == 5U && (unsigned char)stem[3] == 0xC2U &&
            ((unsigned char)stem[4] == 0xB9U || (unsigned char)stem[4] == 0xB2U ||
             (unsigned char)stem[4] == 0xB3U)) return false;
    }
    return true;
}

#endif
static bool AbsolutePath(const char * path)
{
    if (!UmiCreativeTextValid(path, 4096U, false))return false;
#ifdef _WIN32
    unsigned char letter = (unsigned char)path[0];
    if (!((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z')) || path[1]!= ':' || (path[2]!= '\\' && path[2]!= '/'))return false;
    size_t start = 3U;
    for (size_t i = start;; ++ i){
        char c = path[i];
        if (c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|')return false;
        if (c == '\\' || c == '/' || c == '\0'){
            if (!OrdinaryComponent(path + start, i - start))return false;
            if (c == '\0')return true;
            start = i + 1U;
        }
    }
#else
    return path[0] == '/' && path[1]!= '\0';
#endif
}

UmiStatus UmiCreativeExportWriteNew(const UmiCreativeExport * exported, const char * path)
{
    if (exported == NULL || exported -> bytes == NULL || exported -> size == 0U ||
    exported -> size > UMI_CREATIVE_EXPORT_LIMIT || !AbsolutePath(path))return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (count <= 0)return UMI_STATUS_INVALID_ARGUMENT;
    wchar_t * wide = calloc((size_t)count, sizeof( * wide));
    if (wide == NULL)return UMI_STATUS_OUT_OF_MEMORY;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, count)!= count){free(wide);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    HANDLE file = CreateFileW(wide, GENERIC_WRITE, 0, NULL, CREATE_NEW,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    DWORD error = GetLastError();
    free(wide);
    if (file == INVALID_HANDLE_VALUE)return error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    UmiStatus status = GetFileType(file) == FILE_TYPE_DISK ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK){
        DWORD written = 0U;
        if (!WriteFile(file, exported -> bytes, (DWORD)exported -> size, & written, NULL) || written!= (DWORD)exported -> size)status = UMI_STATUS_IO_ERROR;
        if (!FlushFileBuffers(file))status = UMI_STATUS_IO_ERROR;
    }
    if (!CloseHandle(file))status = UMI_STATUS_IO_ERROR;
#else
    FILE * file = fopen(path, "wbx");
    if (file == NULL)return errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    UmiStatus status = UMI_STATUS_OK;
    if (fwrite(exported -> bytes, 1U, exported -> size, file)!= exported -> size)status = UMI_STATUS_IO_ERROR;
    if (fflush(file)!= 0)status = UMI_STATUS_IO_ERROR;
    if (fclose(file)!= 0)status = UMI_STATUS_IO_ERROR;
#endif
    return status;
}

