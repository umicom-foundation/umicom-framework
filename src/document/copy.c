/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/copy.c
 *
 * PURPOSE:
 *   Own immutable source-copy bytes and reuse exclusive native output for separate files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "copy_internal.h"
#include "umicom/document/text_encoding.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#include <string.h>

struct UmiDocumentCopy {
  UmiDocumentCopyInfo info;
  unsigned char *bytes;
};

UmiStatus DocumentCopyCreate(const UmiDocumentCopyInfo *info, const char *text, size_t bytes,
                             UmiDocumentCopy **outCopy) {
  if (outCopy == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *outCopy = NULL;
  if (info == NULL || text == NULL || info->document_id == 0U ||
      info->encoding < UMI_DOCUMENT_ENCODING_UTF8 ||
      info->encoding > UMI_DOCUMENT_ENCODING_UTF16_BE ||
      memchr(info->source_path, '\0', sizeof info->source_path) == NULL ||
      memchr(info->display_name, '\0', sizeof info->display_name) == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (memchr(text, '\0', bytes) != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDocumentCopy *copy = calloc(1U, sizeof *copy);
  if (copy == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  copy->info = *info;
  copy->info.text_bytes = bytes;
  /* Encoding owns an independent byte buffer. Later editor mutations cannot
   * change the file selected by a delayed chooser completion. */
  UmiStatus status = umi_document_encode_text(text, bytes, info->encoding,
                                              info->encoding != UMI_DOCUMENT_ENCODING_UTF8,
                                              &copy->bytes, &copy->info.file_bytes);
  if (status != UMI_STATUS_OK) {
    UmiDocumentCopyDestroy(copy);
    return status;
  }
  *outCopy = copy;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCopyInspect(const UmiDocumentCopy *copy, UmiDocumentCopyInfo *outInfo) {
  if (copy == NULL || outInfo == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *outInfo = copy->info;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCopyRead(const UmiDocumentCopy *copy, const unsigned char **outBytes,
                              size_t *outCount) {
  if (copy == NULL || outBytes == NULL || outCount == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *outBytes = copy->bytes;
  *outCount = copy->info.file_bytes;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCopyWriteNew(const UmiDocumentCopy *copy, const char *destination,
                                  UmiDocumentCopyResult *outResult) {
  if (copy == NULL || destination == NULL || outResult == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDocumentCopyResult result = {0};
  result.status = UmiOutputFileValidatePath(destination);
  if (result.status == UMI_STATUS_OK && copy->info.source_path[0] != '\0' &&
      umi_path_equal(copy->info.source_path, destination))
    result.status = UMI_STATUS_INVALID_ARGUMENT;
  UmiOutputFile *file = NULL;
  if (result.status == UMI_STATUS_OK)
    result.status = UmiOutputFileCreate(destination, &file);
  if (result.status == UMI_STATUS_OK) {
    result.created = true;
    result.status = UmiOutputFileWrite(file, copy->bytes, copy->info.file_bytes);
    /* Close even on failure; its first error and accepted-byte receipt are
     * preserved by the shared output owner, including short writes. */
    UmiStatus closed = UmiOutputFileClose(file);
    if (result.status == UMI_STATUS_OK)
      result.status = closed;
    UmiOutputFileSnapshot written;
    if (UmiOutputFileRead(file, &written) == UMI_STATUS_OK)
      result.bytes_written = (size_t)written.bytes_written;
  }
  UmiOutputFileDestroy(file);
  *outResult = result;
  return result.status;
}
void UmiDocumentCopyDestroy(UmiDocumentCopy *copy) {
  if (copy == NULL)
    return;
  umi_document_encoding_free(copy->bytes);
  free(copy);
}
