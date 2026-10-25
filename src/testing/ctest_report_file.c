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
  if (status == UMI_STATUS_OK)
    status = UmiCtestReportImport((const char *)bytes, size, out);
  UmiInputFileFree(bytes);
  return status;
}
