/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_report_files.inc
 *
 * PURPOSE:
 *   Own a fresh CTest result file without reusing a prior run or a shared filename.
 *   Use native Windows/POSIX directory creation and bounded report reads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Native temporary storage is private to the existing CTest adapter. Each call
 * creates its directory exclusively; an existing report is never reused or
 * overwritten. The project being tested still runs with the user's privileges:
 * this temporary directory is not a sandbox against the same user or process. */

#ifdef _WIN32
#include "../platform/native_path_internal.h"
#else
#include <sys/stat.h>
#endif

typedef struct UmiCtestReportFiles {
    int ownsDirectory;
    /* Snapshot the selected root once for both report I/O and process argv. */
    char buildPath[4096];
    char path[4096];
#ifdef _WIN32
    wchar_t directoryWide[1024];
    wchar_t pathWide[1100];
#else
    char directory[4096];
#endif
} UmiCtestReportFiles;

/* Selected-test reports now live under the caller-selected build directory, with UTF-8 process arguments and the same exclusive per-run ownership. The system-temp and ANSI-code-page version is superseded. The former implementation is retained for review. */
#if 0
static UmiStatus CtestReportCreate(UmiCtestReportFiles *files)
{
    memset(files, 0, sizeof *files);
#ifdef _WIN32
    wchar_t temporary[900];
    DWORD length = GetTempPathW((DWORD)(sizeof temporary / sizeof temporary[0]), temporary);
    if (length == 0U || length >= sizeof temporary / sizeof temporary[0])
        return UMI_STATUS_IO_ERROR;
    static volatile LONG sequence = 0;
    for (unsigned attempt = 0U; attempt < 128U; ++attempt) {
        unsigned long serial = (unsigned long)InterlockedIncrement(&sequence);
        int written = swprintf(files->directoryWide,
            sizeof files->directoryWide / sizeof files->directoryWide[0],
            L"%lsumicom-ctest-%lu-%llu-%lu", temporary,
            (unsigned long)GetCurrentProcessId(),
            (unsigned long long)GetTickCount64(), serial);
        if (written < 0 || (size_t)written >=
            sizeof files->directoryWide / sizeof files->directoryWide[0])
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (CreateDirectoryW(files->directoryWide, NULL)) {
            files->ownsDirectory = 1; break;
        }
        if (GetLastError() != ERROR_ALREADY_EXISTS) return UMI_STATUS_IO_ERROR;
    }
    if (!files->ownsDirectory) return UMI_STATUS_ALREADY_EXISTS;
    int written = swprintf(files->pathWide,
        sizeof files->pathWide / sizeof files->pathWide[0],
        L"%ls\\result.xml", files->directoryWide);
    if (written < 0 || (size_t)written >=
        sizeof files->pathWide / sizeof files->pathWide[0])
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* The existing Framework process adapter uses CreateProcessA. Keep this
     * private OS path in its active code page, rejecting best-fit substitution.
     * This does not claim general Unicode argument support for that adapter. */
    BOOL usedDefault = FALSE;
    UINT codePage = GetACP();
    DWORD flags = codePage == CP_UTF8 ? WC_ERR_INVALID_CHARS : WC_NO_BEST_FIT_CHARS;
    if (WideCharToMultiByte(codePage, flags, files->pathWide, -1,
            files->path, (int)sizeof files->path, NULL,
            codePage == CP_UTF8 ? NULL : &usedDefault) == 0 || usedDefault)
        return UMI_STATUS_UNAVAILABLE;
#else
    const char *temporary = getenv("TMPDIR");
    if (temporary == NULL || temporary[0] == '\0') temporary = "/tmp";
    /* A stable absolute report location is needed while CTest changes directory. */
    if (temporary[0] != '/') return UMI_STATUS_INVALID_ARGUMENT;
    int written = snprintf(files->directory, sizeof files->directory,
        "%s/umicom-ctest-XXXXXX", temporary);
    if (written < 0 || (size_t)written >= sizeof files->directory)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (mkdtemp(files->directory) == NULL) return UMI_STATUS_IO_ERROR;
    files->ownsDirectory = 1;
    written = snprintf(files->path, sizeof files->path,
        "%s/result.xml", files->directory);
    if (written < 0 || (size_t)written >= sizeof files->path)
        return UMI_STATUS_CAPACITY_EXCEEDED;
#endif
    return UMI_STATUS_OK;
}
#else
static UmiStatus CtestReportCreate(UmiCtestReportFiles *files, const char *buildDirectory)
{
    memset(files, 0, sizeof *files);
    if (buildDirectory == NULL || buildDirectory[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    wchar_t *buildWide = NULL;
    UmiStatus pathStatus = UmiNativePathWide(buildDirectory, &buildWide);
    if (pathStatus != UMI_STATUS_OK) return pathStatus;
    wchar_t temporary[900];
    DWORD length = GetFullPathNameW(buildWide, (DWORD)(sizeof temporary / sizeof temporary[0]), temporary, NULL);
    free(buildWide);
    if (length == 0U) return UMI_STATUS_IO_ERROR;
    if (length >= sizeof temporary / sizeof temporary[0]) return UMI_STATUS_CAPACITY_EXCEEDED;
    DWORD attributes = GetFileAttributesW(temporary);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0U)
        return UMI_STATUS_NOT_FOUND;
    pathStatus = UmiNativePathUtf8(temporary, files->buildPath, sizeof files->buildPath);
    if (pathStatus != UMI_STATUS_OK) return pathStatus;
    const wchar_t suffix[] = L"\\test-work";
    if ((size_t)length + sizeof suffix / sizeof suffix[0] > sizeof temporary / sizeof temporary[0])
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(temporary + length, suffix, sizeof suffix);
    if (!CreateDirectoryW(temporary, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
        return UMI_STATUS_IO_ERROR;
    attributes = GetFileAttributesW(temporary);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0U)
        return UMI_STATUS_INVALID_STATE;
    static volatile LONG sequence = 0;
    for (unsigned attempt = 0U; attempt < 128U; ++attempt) {
        unsigned long serial = (unsigned long)InterlockedIncrement(&sequence);
        int written = swprintf(files->directoryWide,
            sizeof files->directoryWide / sizeof files->directoryWide[0],
            L"%ls\\umicom-ctest-%lu-%llu-%lu", temporary,
            (unsigned long)GetCurrentProcessId(),
            (unsigned long long)GetTickCount64(), serial);
        if (written < 0 || (size_t)written >=
            sizeof files->directoryWide / sizeof files->directoryWide[0])
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (CreateDirectoryW(files->directoryWide, NULL)) {
            files->ownsDirectory = 1;
            break;
        }
        if (GetLastError() != ERROR_ALREADY_EXISTS) return UMI_STATUS_IO_ERROR;
    }
    if (!files->ownsDirectory) return UMI_STATUS_ALREADY_EXISTS;
    int written = swprintf(files->pathWide,
        sizeof files->pathWide / sizeof files->pathWide[0],
        L"%ls\\result.xml", files->directoryWide);
    if (written < 0 || (size_t)written >= sizeof files->pathWide / sizeof files->pathWide[0])
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Process arguments are UTF-8 throughout Framework. Using the ANSI code
     * page here would corrupt a valid Unicode build path before CreateProcessW. */
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, files->pathWide, -1,
            files->path, (int)sizeof files->path, NULL, NULL) == 0)
        return UMI_STATUS_CAPACITY_EXCEEDED;
#else
    char temporary[4096];
    char current[4096];
    int written;
    if (buildDirectory[0] == '/')
        written = snprintf(temporary, sizeof temporary, "%s", buildDirectory);
    else {
        if (getcwd(current, sizeof current) == NULL) return UMI_STATUS_IO_ERROR;
        written = snprintf(temporary, sizeof temporary, "%s/%s", current, buildDirectory);
    }
    if (written < 0 || (size_t)written >= sizeof temporary) return UMI_STATUS_CAPACITY_EXCEEDED;
    struct stat metadata;
    if (stat(temporary, &metadata) != 0 || !S_ISDIR(metadata.st_mode)) return UMI_STATUS_NOT_FOUND;
    size_t length = (size_t)written;
    memcpy(files->buildPath, temporary, length + 1U);
    const char suffix[] = "/test-work";
    if (length + sizeof suffix > sizeof temporary) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(temporary + length, suffix, sizeof suffix);
    if (mkdir(temporary, 0700) != 0 && errno != EEXIST) return UMI_STATUS_IO_ERROR;
    if (stat(temporary, &metadata) != 0 || !S_ISDIR(metadata.st_mode)) return UMI_STATUS_INVALID_STATE;
    written = snprintf(files->directory, sizeof files->directory,
        "%s/umicom-ctest-XXXXXX", temporary);
    if (written < 0 || (size_t)written >= sizeof files->directory) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (mkdtemp(files->directory) == NULL) return UMI_STATUS_IO_ERROR;
    files->ownsDirectory = 1;
    written = snprintf(files->path, sizeof files->path, "%s/result.xml", files->directory);
    if (written < 0 || (size_t)written >= sizeof files->path) return UMI_STATUS_CAPACITY_EXCEEDED;
#endif
    return UMI_STATUS_OK;
}
#endif
static UmiStatus CtestReportRead(const UmiCtestReportFiles *files,
    char **outText, size_t *outLength)
{
    *outText = NULL; *outLength = 0U;
#ifdef _WIN32
    FILE *input = _wfopen(files->pathWide, L"rb");
#else
    FILE *input = fopen(files->path, "rb");
#endif
    if (input == NULL) return UMI_STATUS_IO_ERROR;
    UmiStatus status = UMI_STATUS_OK;
    size_t capacity = 4096U, used = 0U;
    char *bytes = (char *)malloc(capacity + 1U);
    if (bytes == NULL) { fclose(input); return UMI_STATUS_OUT_OF_MEMORY; }
    for (;;) {
        if (used == capacity) {
            if (capacity == UMI_CTEST_REPORT_MAX_BYTES) {
                int extra = fgetc(input);
                if (extra != EOF) status = UMI_STATUS_CAPACITY_EXCEEDED;
                else if (ferror(input)) status = UMI_STATUS_IO_ERROR;
                break;
            }
            size_t next = capacity * 2U;
            if (next > UMI_CTEST_REPORT_MAX_BYTES) next = UMI_CTEST_REPORT_MAX_BYTES;
            char *grown = (char *)realloc(bytes, next + 1U);
            if (grown == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; break; }
            bytes = grown; capacity = next;
        }
        size_t count = fread(bytes + used, 1U, capacity - used, input);
        used += count;
        if (count == 0U) {
            if (ferror(input)) status = UMI_STATUS_IO_ERROR;
            break;
        }
    }
    if (fclose(input) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK) { free(bytes); return status; }
    bytes[used] = '\0'; *outText = bytes; *outLength = used;
    return UMI_STATUS_OK;
}

static UmiStatus CtestReportCleanup(UmiCtestReportFiles *files)
{
    if (!files->ownsDirectory) return UMI_STATUS_OK;
    UmiStatus status = UMI_STATUS_OK;
#ifdef _WIN32
    if (files->pathWide[0] != L'\0' && !DeleteFileW(files->pathWide) &&
        GetLastError() != ERROR_FILE_NOT_FOUND) status = UMI_STATUS_IO_ERROR;
    if (!RemoveDirectoryW(files->directoryWide)) status = UMI_STATUS_IO_ERROR;
#else
    if (files->path[0] != '\0' && remove(files->path) != 0 && errno != ENOENT)
        status = UMI_STATUS_IO_ERROR;
    if (rmdir(files->directory) != 0) status = UMI_STATUS_IO_ERROR;
#endif
    /* Never recurse: unexpected files belong to neither this call nor cleanup. */
    files->ownsDirectory = 0;
    return status;
}
