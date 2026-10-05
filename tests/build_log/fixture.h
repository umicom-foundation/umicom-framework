/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_log/fixture.h
 * PURPOSE: Isolate native log fixtures and read UTF-8 Windows paths without changing user files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_LOG_FIXTURE_H
#define UMICOM_BUILD_LOG_FIXTURE_H
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)

/* Reserve a fresh directory below this case's CTest working directory. Leave
 * evidence for diagnosis; never delete or reuse an existing entry. */
static inline void FixtureDirectory(char *out)
{
    char root[UMI_PATH_CAPACITY]; unsigned long process;
#ifdef _WIN32
    wchar_t wide[UMI_PATH_CAPACITY];
    DWORD count = GetCurrentDirectoryW(UMI_PATH_CAPACITY, wide);
    CHECK(count != 0U && count < UMI_PATH_CAPACITY);
    CHECK(WideCharToMultiByte(CP_UTF8, 0, wide, -1, root, (int)sizeof(root), NULL, NULL) != 0);
    process = (unsigned long)GetCurrentProcessId();
#else
    CHECK(getcwd(root, sizeof(root)) != NULL); process = (unsigned long)getpid();
#endif
    for (unsigned index = 0U; index < 1000U; ++index) {
        int length = snprintf(out, UMI_PATH_CAPACITY, "%s/log-fixture-%lu-%u", root, process, index);
        CHECK(length > 0 && (size_t)length < UMI_PATH_CAPACITY);
#ifdef _WIN32
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, out, -1, wide, (int)UMI_PATH_CAPACITY) != 0);
        if (CreateDirectoryW(wide, NULL)) return;
        CHECK(GetLastError() == ERROR_ALREADY_EXISTS);
#else
        if (mkdir(out, 0700) == 0) return;
        CHECK(errno == EEXIST);
#endif
    }
    CHECK(0);
}

static inline void FixturePath(char *out, const char *root, const char *leaf)
{
    int length = snprintf(out, UMI_PATH_CAPACITY, "%s/%s", root, leaf);
    CHECK(length > 0 && (size_t)length < UMI_PATH_CAPACITY);
}

/* Native path opening avoids confusing a correct Unicode writer with an
 * unrelated narrow-character test reader on Windows. */
static inline unsigned char *FixtureRead(const char *path, size_t *length)
{
#ifdef _WIN32
    wchar_t wide[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, (int)UMI_PATH_CAPACITY) != 0);
    FILE *file = _wfopen(wide, L"rb");
#else
    FILE *file = fopen(path, "rb");
#endif
    CHECK(file != NULL && fseek(file, 0L, SEEK_END) == 0);
    long size = ftell(file); CHECK(size >= 0L && size <= 1048576L && fseek(file, 0L, SEEK_SET) == 0);
    unsigned char *bytes = calloc((size_t)size + 1U, 1U); CHECK(bytes != NULL);
    CHECK(fread(bytes, 1U, (size_t)size, file) == (size_t)size && fclose(file) == 0);
    *length = (size_t)size; return bytes;
}
#endif
