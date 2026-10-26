/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_report_file.c
 *
 * PURPOSE:
 *   Centralise saved-report file bounds and parser admission for native and command-line hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_report_file.h"
#include "umicom/platform/input_file.h"
/* The earlier reader remains for review of admission and ownership. The
 * replacement adds optional content identity within the single owned read,
 * avoiding a second read that could identify different bytes after replacement. */
#if 0
UmiStatus UmiCtestReportReadFile(const char *path, UmiCtestReport **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  unsigned char *bytes = NULL;
  size_t size = 0U;
  UmiStatus status = UmiInputFileRead(path, UMI_CTEST_COLLECTION_MAX_BYTES, &bytes, &size);
  /* An empty regular file is admitted input with malformed report content.
   * Keep the lower-level import API's argument contract intact while the file
   * service distinguishes content errors from invalid paths and I/O failures. */
#if 0
  /* Direct forwarding is retained for review; it classified an empty saved
   * file as an API argument error instead of a report parsing failure. */
  if (status == UMI_STATUS_OK)
    status = UmiCtestReportImport((const char *)bytes, size, out);
#endif
  if (status == UMI_STATUS_OK)
    status = size == 0U ? UMI_STATUS_PARSE_ERROR
                        : UmiCtestReportImport((const char *)bytes, size, out);
  UmiInputFileFree(bytes);
  return status;
}
#endif

UmiStatus UmiCtestReportReadFileWithIdentity(const char *path, UmiCtestReport **out,
    UmiCtestReportFileIdentity *identity)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiCtestReportFileIdentity captured = {0};
    UmiStatus status = UmiInputFileRead(path, UMI_CTEST_COLLECTION_MAX_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK && size == 0U) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && identity != NULL) {
        captured.bytes = size;
        status = UmiSha256Buffer(bytes, size, captured.sha256);
    }
    if (status == UMI_STATUS_OK)
        status = UmiCtestReportImport((const char *)bytes, size, out);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK && identity != NULL) *identity = captured;
    return status;
}

UmiStatus UmiCtestReportReadFile(const char *path, UmiCtestReport **out)
{
    return UmiCtestReportReadFileWithIdentity(path, out, NULL);
}
