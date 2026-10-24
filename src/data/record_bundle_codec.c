/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Length framing and canonical metadata keep bundles portable across backends. */
#include "include/record_bundle_internal.h"
#include "umicom/base/sha256.h"
#include <stdlib.h>
#include <string.h>
#define HEADER_BYTES 16U
#define DIGEST_BYTES 64U
static const unsigned char magic[8] = {'U', 'M', 'I', 'R', 'B', 'D', '1', '\n'};
static void put_u32(unsigned char *p, uint32_t value) {
  for (size_t i = 0; i < 4; ++i)
    p[i] = (unsigned char)(value >> (8U * (3U - i)));
}
static uint32_t get_u32(const unsigned char *p) {
  uint32_t value = 0;
  for (size_t i = 0; i < 4; ++i)
    value = (value << 8U) | p[i];
  return value;
}
UmiStatus umi_record_bundle_validate(const UmiRecordBundle *bundle) {
  if (bundle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (bundle->count == 0 || bundle->count > UMI_RECORD_BUNDLE_MAX_REVISIONS)
    return UMI_STATUS_INVALID_STATE;
  uint64_t time = 0;
  for (size_t i = 0; i < bundle->count; ++i) {
    const UmiRecordDocument *d = &bundle->records[i];
    char metadata[UMI_RECORD_REVISION_TEXT_CAPACITY], digest[UMI_SHA256_HEX_CAPACITY];
    if (d->length > UMI_RECORD_DOCUMENT_MAX_BYTES)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_record_revision_format(&d->record, metadata, sizeof metadata);
    if (status != UMI_STATUS_OK)
      return status;
    if (d->record.revision != i + 1 || d->record.previous_revision != i ||
        strcmp(d->record.amendment.entity_id, bundle->records[0].record.amendment.entity_id) != 0 ||
        d->record.amendment.recorded_at_nanoseconds < time)
      return UMI_STATUS_INVALID_STATE;
    time = d->record.amendment.recorded_at_nanoseconds;
    status = UmiSha256Buffer(d->content, d->length, digest);
    if (status != UMI_STATUS_OK)
      return status;
    if (strcmp(digest, d->record.amendment.content_sha256) != 0)
      return UMI_STATUS_PARSE_ERROR;
  }
  return UMI_STATUS_OK;
}
void umi_record_bundle_destroy(UmiRecordBundle *bundle) { free(bundle); }
size_t umi_record_bundle_count(const UmiRecordBundle *bundle) {
  return bundle == NULL ? 0 : bundle->count;
}
UmiStatus umi_record_bundle_at(const UmiRecordBundle *bundle, size_t index,
                               UmiRecordDocument *out) {
  if (bundle == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= bundle->count)
    return UMI_STATUS_NOT_FOUND;
  *out = bundle->records[index];
  return UMI_STATUS_OK;
}
UmiStatus umi_record_bundle_encode(const UmiRecordBundle *bundle, unsigned char **out,
                                   size_t *out_length) {
  if (out == NULL || out_length == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_bundle_validate(bundle);
  if (status != UMI_STATUS_OK)
    return status;
  unsigned char *bytes = calloc(1, UMI_RECORD_BUNDLE_MAX_BYTES);
  if (bytes == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(bytes, magic, 8);
  put_u32(bytes + 8, (uint32_t)bundle->count);
  size_t at = HEADER_BYTES;
  for (size_t i = 0; i < bundle->count; ++i) {
    const UmiRecordDocument *d = &bundle->records[i];
    char text[UMI_RECORD_REVISION_TEXT_CAPACITY];
    status = umi_record_revision_format(&d->record, text, sizeof text);
    if (status != UMI_STATUS_OK)
      break;
    size_t length = strlen(text);
    put_u32(bytes + at, (uint32_t)length);
    put_u32(bytes + at + 4, (uint32_t)d->length);
    at += 8;
    memcpy(bytes + at, text, length);
    at += length;
    memcpy(bytes + at, d->content, d->length);
    at += d->length;
  }
  char digest[UMI_SHA256_HEX_CAPACITY];
  if (status == UMI_STATUS_OK)
    status = UmiSha256Buffer(bytes, at, digest);
  if (status == UMI_STATUS_OK) {
    memcpy(bytes + at, digest, DIGEST_BYTES);
    *out = bytes;
    *out_length = at + DIGEST_BYTES;
  } else
    free(bytes);
  return status;
}
UmiStatus umi_record_bundle_decode(const void *input, size_t length, UmiRecordBundle **out) {
  if (input == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length < HEADER_BYTES + DIGEST_BYTES || length > UMI_RECORD_BUNDLE_MAX_BYTES)
    return UMI_STATUS_PARSE_ERROR;
  const unsigned char *bytes = input;
  size_t end = length - DIGEST_BYTES;
  if (memcmp(bytes, magic, 8) != 0 || get_u32(bytes + 12) != 0)
    return UMI_STATUS_PARSE_ERROR;
  uint32_t count = get_u32(bytes + 8);
  if (count == 0 || count > UMI_RECORD_BUNDLE_MAX_REVISIONS)
    return UMI_STATUS_PARSE_ERROR;
  char digest[UMI_SHA256_HEX_CAPACITY];
  UmiStatus status = UmiSha256Buffer(bytes, end, digest);
  if (status != UMI_STATUS_OK)
    return status;
  if (memcmp(bytes + end, digest, DIGEST_BYTES) != 0)
    return UMI_STATUS_PARSE_ERROR;
  UmiRecordBundle *bundle = calloc(1, sizeof(*bundle));
  if (bundle == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  bundle->count = count;
  size_t at = HEADER_BYTES;
  for (size_t i = 0; i < count && status == UMI_STATUS_OK; ++i) {
    if (end - at < 8) {
      status = UMI_STATUS_PARSE_ERROR;
      break;
    }
    size_t text_length = get_u32(bytes + at), content_length = get_u32(bytes + at + 4);
    at += 8;
    if (text_length == 0 || text_length >= UMI_RECORD_REVISION_TEXT_CAPACITY ||
        content_length > UMI_RECORD_DOCUMENT_MAX_BYTES || text_length > end - at ||
        content_length > end - at - text_length) {
      status = UMI_STATUS_PARSE_ERROR;
      break;
    }
    UmiRecordDocument *d = &bundle->records[i];
    status = umi_record_revision_parse((const char *)bytes + at, text_length, &d->record);
    char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY];
    if (status == UMI_STATUS_OK)
      status = umi_record_revision_format(&d->record, canonical, sizeof canonical);
    if (status == UMI_STATUS_OK &&
        (strlen(canonical) != text_length || memcmp(canonical, bytes + at, text_length) != 0))
      status = UMI_STATUS_PARSE_ERROR;
    at += text_length;
    d->length = content_length;
    memcpy(d->content, bytes + at, content_length);
    at += content_length;
  }
  if (status == UMI_STATUS_OK && at != end)
    status = UMI_STATUS_PARSE_ERROR;
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_validate(bundle);
  if (status == UMI_STATUS_OK)
    *out = bundle;
  else
    free(bundle);
  return status;
}
void umi_record_bundle_bytes_free(void *bytes) { free(bytes); }
