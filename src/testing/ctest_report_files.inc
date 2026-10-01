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

typedef struct UmiCtestReportFiles {
    int ownsDirectory;
    char path[4096];
#ifdef _WIN32
    wchar_t directoryWide[1024];
    wchar_t pathWide[1100];
#else
    char directory[4096];
#endif
} UmiCtestReportFiles;

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
