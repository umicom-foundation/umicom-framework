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
