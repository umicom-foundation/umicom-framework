/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File ownership stays in Platform; Data Server owns bundle integrity. */
#include "umicom/data/record_bundle_files.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/rooted_files.h"
#include <stdlib.h>
#include <string.h>
UmiStatus umi_record_bundle_write_new(const char *path, const UmiRecordBundle *bundle) {
  UmiStatus status = UmiOutputFileValidatePath(path);
  unsigned char *bytes = NULL;
  size_t length = 0;
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_encode(bundle, &bytes, &length);
  UmiOutputFile *file = NULL;
  if (status == UMI_STATUS_OK)
    status = UmiOutputFileCreate(path, &file);
  if (status == UMI_STATUS_OK)
    status = UmiOutputFileWrite(file, bytes, length);
  if (file != NULL) {
    UmiStatus closed = UmiOutputFileClose(file);
    if (status == UMI_STATUS_OK)
      status = closed;
  }
  UmiOutputFileDestroy(file);
  umi_record_bundle_bytes_free(bytes);
  return status;
}
UmiStatus umi_record_bundle_read(const char *directory, const char *relative_path,
                                 UmiRecordBundle **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  unsigned char *bytes = NULL;
  size_t length = 0;
  UmiStatus status =
      UmiRootedFileRead(directory, relative_path, UMI_RECORD_BUNDLE_MAX_BYTES, &bytes, &length);
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_decode(bytes, length, out);
  UmiRootedFileFree(bytes);
  return status;
}

/* Capture first, compare canonical values, then create output. This ordering
 * prevents a stale editor from exporting an unseen newer revision and avoids
 * platform-dependent comparisons of struct padding. */
UmiStatus umi_record_bundle_export_reviewed(UmiDataServer *server,
                                            const UmiRecordRevision *reviewed_head,
                                            const char *path) {
  if (server == NULL || reviewed_head == NULL || reviewed_head->revision == 0)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiOutputFileValidatePath(path);
  char expected[UMI_RECORD_REVISION_TEXT_CAPACITY], actual[UMI_RECORD_REVISION_TEXT_CAPACITY];
  if (status == UMI_STATUS_OK)
    status = umi_record_revision_format(reviewed_head, expected, sizeof(expected));
  UmiRecordBundle *bundle = NULL;
  UmiRecordDocument *latest = NULL;
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_capture(server, reviewed_head->amendment.entity_id, &bundle);
  if (status == UMI_STATUS_OK) {
    latest = malloc(sizeof(*latest));
    status = latest == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
  }
  if (status == UMI_STATUS_OK) {
    size_t count = umi_record_bundle_count(bundle);
    status =
        count == 0 ? UMI_STATUS_INVALID_STATE : umi_record_bundle_at(bundle, count - 1, latest);
  }
  if (status == UMI_STATUS_OK)
    status = umi_record_revision_format(&latest->record, actual, sizeof(actual));
  if (status == UMI_STATUS_OK && strcmp(expected, actual) != 0)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_write_new(path, bundle);
  free(latest);
  umi_record_bundle_destroy(bundle);
  return status;
}
