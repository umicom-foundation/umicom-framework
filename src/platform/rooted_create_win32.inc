/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/rooted_create_win32.inc
 *
 * PURPOSE:
 *   Create a new Unicode file while retaining its checked Windows parents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Parent handles refuse delete sharing, and CREATE_NEW never opens an existing
 * leaf. No shell or current-directory lookup participates in publication.
 * Keep a newly created partial file on failure and report its actual progress;
 * cleanup by pathname could otherwise delete a concurrently replaced file. */
static UmiStatus RootedCreateNew(RootedParent *parent, const void *bytes, size_t size,
    UmiRootedFileCreation *report)
{
    /* Metadata inspection has an explicit access right; data reading is not
     * needed to validate the newly created handle. */
    HANDLE file = CreateFileW(parent->path, GENERIC_WRITE | FILE_READ_ATTRIBUTES, 0U, NULL, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file == INVALID_HANDLE_VALUE) return RootedError(GetLastError());
    report->created = 1;
    BY_HANDLE_FILE_INFORMATION info;
    UmiStatus status = RootedInspectHandle(file, &info);
    while (status == UMI_STATUS_OK && report->bytes_written < size) {
        size_t remaining = size - report->bytes_written;
        if (remaining > 1048576U) remaining = 1048576U;
        DWORD count = 0U;
        BOOL written = WriteFile(file, (const unsigned char *)bytes + report->bytes_written,
            (DWORD)remaining, &count, NULL);
        if ((size_t)count > remaining) {
            status = UMI_STATUS_IO_ERROR;
            break;
        }
        report->bytes_written += (size_t)count;
        if (!written || count == 0U) status = UMI_STATUS_IO_ERROR;
    }
    if (status == UMI_STATUS_OK && !FlushFileBuffers(file)) status = UMI_STATUS_IO_ERROR;
    if (!CloseHandle(file) && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    report->complete = status == UMI_STATUS_OK;
    return status;
}
