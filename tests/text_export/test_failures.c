/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/text_export/test_failures.c
 * PURPOSE: Verify first-error reporting and closure after partial output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/output_file.h"
#include <stdio.h>
static UmiStatus FakeCreate(const char *path, UmiOutputFile **out);
static UmiStatus FakeWrite(UmiOutputFile *file, const void *bytes, size_t length);
static UmiStatus FakeClose(UmiOutputFile *file);
static UmiStatus FakeRead(const UmiOutputFile *file, UmiOutputFileSnapshot *out);
static void FakeDestroy(UmiOutputFile *file);
/* Interpose only the output owner. Production GTK code and write ordering remain
 * intact; no real file or application service is needed for these failure cases. */
#define UmiOutputFileCreate FakeCreate
#define UmiOutputFileWrite FakeWrite
#define UmiOutputFileClose FakeClose
#define UmiOutputFileRead FakeRead
#define UmiOutputFileDestroy FakeDestroy
#define UmiTextExportGtk4Create TestExportCreate
#define UmiTextExportGtk4CreateForBuffer TestExportBuffer
#define UmiTextExportGtk4CreateGuardedBuffer TestExportGuardedBuffer
#define UmiTextExportGtk4Retire TestExportRetire
#define UmiTextExportGtk4Busy TestExportBusy
#include "../../adapters/gtk4/text_export_gtk4.c"
#define CHECK(c)                                                                                   \
  do {                                                                                             \
    if (!(c)) {                                                                                    \
      fprintf(stderr, "Line %d: %s\n", __LINE__, #c);                                              \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static unsigned mode, writes, closes, releases;
static unsigned char token;
static UmiStatus FakeCreate(const char *path, UmiOutputFile **out) {
  (void)path;
  *out = mode == 1U ? NULL : (UmiOutputFile *)&token;
  return mode == 1U ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_OK;
}
static UmiStatus FakeWrite(UmiOutputFile *file, const void *bytes, size_t length) {
  (void)file;
  (void)bytes;
  (void)length;
  ++writes;
  return mode == 2U ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiStatus FakeClose(UmiOutputFile *file) {
  (void)file;
  ++closes;
  return mode >= 2U ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_OK;
}
static UmiStatus FakeRead(const UmiOutputFile *file, UmiOutputFileSnapshot *out) {
  (void)file;
  *out = (UmiOutputFileSnapshot){0};
  out->bytes_written = mode == 2U ? 3U : 6U;
  return UMI_STATUS_OK;
}
static void FakeDestroy(UmiOutputFile *file) {
  if (file != NULL)
    ++releases;
}
int main(void) {
  for (mode = 0U; mode < 4U; ++mode) {
    writes = closes = releases = 0U;
    TextExportWrite write = {0};
    write.path = "unused";
    write.text = "report";
    write.length = 6U;
    ExportWriteFile(&write);
    if (mode == 1U) {
      CHECK(write.status == UMI_STATUS_ALREADY_EXISTS && write.written == 0U);
      CHECK(writes == 0U && closes == 0U && releases == 0U);
    } else {
      CHECK(writes == 1U && closes == 1U && releases == 1U);
      CHECK(write.written == (mode == 2U ? 3U : 6U));
      CHECK(write.status == (mode == 2U   ? UMI_STATUS_IO_ERROR
                             : mode == 3U ? UMI_STATUS_PERMISSION_DENIED
                                          : UMI_STATUS_OK));
    }
  }
  return 0;
}
