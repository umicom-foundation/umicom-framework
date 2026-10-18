/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: tests/ctest_execution/report_file_helpers.h
 * PURPOSE: Keep injected report fixtures on the same UTF-8 filesystem boundary
 * as production. These helpers own no process execution or test verdict logic.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_REPORT_FILE_HELPERS_H
#define UMICOM_TEST_REPORT_FILE_HELPERS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#include "../../src/platform/native_path_internal.h"
#endif

static inline FILE *ReportFixtureOpen(const char *path, const char *mode)
{
#ifdef _WIN32
    const wchar_t *wideMode = strcmp(mode, "rb") == 0 ? L"rb" :
        strcmp(mode, "wb") == 0 ? L"wb" : NULL;
    if (wideMode == NULL) return NULL;
    wchar_t *wide = NULL;
    if (UmiNativePathWide(path, &wide) != UMI_STATUS_OK) return NULL;
    FILE *file = _wfopen(wide, wideMode);
    free(wide);
    return file;
#else
    return fopen(path, mode);
#endif
}

static inline int ReportFixtureRemove(const char *path)
{
#ifdef _WIN32
    wchar_t *wide = NULL;
    if (UmiNativePathWide(path, &wide) != UMI_STATUS_OK) return -1;
    int status = _wremove(wide);
    free(wide);
    return status;
#else
    return remove(path);
#endif
}

#ifdef _WIN32
static inline int ReportFixtureRemoveDirectory(const char *path)
{
    wchar_t *wide = NULL;
    if (UmiNativePathWide(path, &wide) != UMI_STATUS_OK) return -1;
    int status = _wrmdir(wide);
    free(wide);
    return status;
}
#endif
#endif
