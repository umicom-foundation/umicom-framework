/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/record_transfer_receipt.c
 *
 * PURPOSE:
 *   Canonical receipt payloads separate import attribution from original authors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Canonical receipt payloads separate import attribution from original authors. */
#include "include/record_transfer_internal.h"
#include "umicom/base/sha256.h"
#include <stdlib.h>
#include <string.h>
static const char prefix[] = "transfer.receipt.";
static const unsigned char magic[] = {'U', 'M', 'I', 'T', 'R', '1', '\n'};
static bool digest_valid(const char *digest) {
  if (!digest)
    return false;
  for (size_t i = 0; i < 64; ++i)
    if (!((digest[i] >= '0' && digest[i] <= '9') || (digest[i] >= 'a' && digest[i] <= 'f')))
      return false;
  return digest[64] == '\0';
}
UmiStatus umi_record_transfer_receipt_identity(const char *id, char out[64]) {
  if (!id || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t n = 0;
  while (n < UMI_RECORD_TRANSFER_ID_CAPACITY && id[n]) {
    char c = id[n];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' ||
          c == '-'))
      return UMI_STATUS_INVALID_ARGUMENT;
    ++n;
  }
  if (n == 0 || n >= UMI_RECORD_TRANSFER_ID_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  memcpy(out, prefix, sizeof(prefix) - 1);
  memcpy(out + sizeof(prefix) - 1, id, n + 1);
  return UMI_STATUS_OK;
}
/* Native and headless readers share the receipt namespace grammar. */
UmiStatus umi_record_transfer_id_validate(const char *receipt_id) {
    char identity[64];
    return umi_record_transfer_receipt_identity(receipt_id, identity);
}
/* Reuse the canonical revision validator at native admission boundaries. The
 * zero digest is a syntax placeholder only; no record is written by this call. */
UmiStatus umi_record_transfer_request_validate(const UmiRecordTransferRequest *request) {
  if (!request)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordRevision candidate = {.revision = 1};
  UmiStatus status =
      umi_record_transfer_receipt_identity(request->receipt_id, candidate.amendment.entity_id);
  if (status != UMI_STATUS_OK)
    return status;
  memcpy(candidate.amendment.source, request->attribution.source,
         sizeof(candidate.amendment.source));
  memcpy(candidate.amendment.actor, request->attribution.actor, sizeof(candidate.amendment.actor));
  memcpy(candidate.amendment.reason, request->attribution.reason,
         sizeof(candidate.amendment.reason));
  candidate.amendment.observed_at_nanoseconds = request->attribution.observed_at_nanoseconds;
  candidate.amendment.recorded_at_nanoseconds = request->attribution.recorded_at_nanoseconds;
  memset(candidate.amendment.content_sha256, '0', 64);
  char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY];
  return umi_record_revision_format(&candidate, canonical, sizeof(canonical));
}
UmiStatus umi_record_transfer_receipt_document(const UmiRecordRevision *imported,
                                               const char digest[65],
                                               const UmiRecordTransferRequest *request,
                                               UmiRecordDocument *out) {
  if (!imported || !request || !out || !digest_valid(digest))
    return UMI_STATUS_INVALID_ARGUMENT;
  char original[UMI_RECORD_REVISION_TEXT_CAPACITY], identity[64];
  UmiStatus status = umi_record_revision_format(imported, original, sizeof(original));
  if (status == UMI_STATUS_OK &&
      (imported->revision > UMI_RECORD_BUNDLE_MAX_REVISIONS ||
       strncmp(imported->amendment.entity_id, prefix, sizeof(prefix) - 1) == 0))
    status = UMI_STATUS_INVALID_ARGUMENT;
  if (status == UMI_STATUS_OK)
    status = umi_record_transfer_receipt_identity(request->receipt_id, identity);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordDocument *candidate = calloc(1, sizeof(*candidate));
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  candidate->record.revision = 1;
  memcpy(candidate->record.amendment.entity_id, identity, strlen(identity) + 1);
  memcpy(candidate->record.amendment.source, request->attribution.source,
         sizeof(request->attribution.source));
  memcpy(candidate->record.amendment.actor, request->attribution.actor,
         sizeof(request->attribution.actor));
  memcpy(candidate->record.amendment.reason, request->attribution.reason,
         sizeof(request->attribution.reason));
  candidate->record.amendment.observed_at_nanoseconds =
      request->attribution.observed_at_nanoseconds;
  candidate->record.amendment.recorded_at_nanoseconds =
      request->attribution.recorded_at_nanoseconds;
  size_t start = sizeof(magic) + 65;
  memcpy(candidate->content, magic, sizeof(magic));
  memcpy(candidate->content + sizeof(magic), digest, 64);
  candidate->content[start - 1] = '\n';
  memcpy(candidate->content + start, original, strlen(original));
  candidate->length = start + strlen(original);
  status = UmiSha256Buffer(candidate->content, candidate->length,
                           candidate->record.amendment.content_sha256);
  char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY];
  if (status == UMI_STATUS_OK)
    status = umi_record_revision_format(&candidate->record, canonical, sizeof(canonical));
  if (status == UMI_STATUS_OK)
    *out = *candidate;
  free(candidate);
  return status;
}
UmiStatus umi_record_transfer_receipt_decode(const UmiRecordDocument *document,
                                             UmiRecordTransferReceipt *out) {
  if (!document || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  const size_t start = sizeof(magic) + 65;
  if (document->length <= start || document->length >= start + UMI_RECORD_REVISION_TEXT_CAPACITY ||
      document->record.revision != 1 || document->record.previous_revision != 0)
    return UMI_STATUS_PARSE_ERROR;
  char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY], actual[65], identity[64];
  UmiStatus status = umi_record_revision_format(&document->record, canonical, sizeof(canonical));
  if (status != UMI_STATUS_OK)
    return status;
  if (strncmp(document->record.amendment.entity_id, prefix, sizeof(prefix) - 1) != 0)
    return UMI_STATUS_PARSE_ERROR;
  status = umi_record_transfer_receipt_identity(
      document->record.amendment.entity_id + sizeof(prefix) - 1, identity);
  if (status != UMI_STATUS_OK)
    return UMI_STATUS_PARSE_ERROR;
  status = UmiSha256Buffer(document->content, document->length, actual);
  if (status != UMI_STATUS_OK)
    return status;
  if (strcmp(actual, document->record.amendment.content_sha256) != 0 ||
      memcmp(document->content, magic, sizeof(magic)) != 0 || document->content[start - 1] != '\n')
    return UMI_STATUS_PARSE_ERROR;
  UmiRecordTransferReceipt candidate = {0};
  candidate.receipt = document->record;
  memcpy(candidate.bundle_sha256, document->content + sizeof(magic), 64);
  if (!digest_valid(candidate.bundle_sha256))
    return UMI_STATUS_PARSE_ERROR;
  size_t length = document->length - start;
  status = umi_record_revision_parse((const char *)document->content + start, length,
                                     &candidate.imported_head);
  if (status == UMI_STATUS_OK)
    status = umi_record_revision_format(&candidate.imported_head, canonical, sizeof(canonical));
  if (status != UMI_STATUS_OK)
    return status;
  if (strlen(canonical) != length || memcmp(canonical, document->content + start, length) != 0 ||
      candidate.imported_head.revision > UMI_RECORD_BUNDLE_MAX_REVISIONS ||
      strncmp(candidate.imported_head.amendment.entity_id, prefix, sizeof(prefix) - 1) == 0)
    return UMI_STATUS_PARSE_ERROR;
  *out = candidate;
  return UMI_STATUS_OK;
}

UmiStatus umi_record_transfer_receipt_validate(const UmiRecordTransferReceipt *receipt) {
  if (!receipt)
    return UMI_STATUS_INVALID_ARGUMENT;
  char expected[UMI_RECORD_REVISION_TEXT_CAPACITY], actual[UMI_RECORD_REVISION_TEXT_CAPACITY];
  UmiStatus status = umi_record_revision_format(&receipt->receipt, expected, sizeof(expected));
  if (status != UMI_STATUS_OK)
    return status;
  if (receipt->receipt.revision != 1 ||
      strncmp(receipt->receipt.amendment.entity_id, prefix, sizeof(prefix) - 1) != 0)
    return UMI_STATUS_INVALID_STATE;
  const char *id = receipt->receipt.amendment.entity_id + sizeof(prefix) - 1;
  char identity[64];
  status = umi_record_transfer_receipt_identity(id, identity);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordTransferRequest request = {0};
  memcpy(request.receipt_id, id, strlen(id) + 1);
  memcpy(request.attribution.source, receipt->receipt.amendment.source,
         sizeof(request.attribution.source));
  memcpy(request.attribution.actor, receipt->receipt.amendment.actor,
         sizeof(request.attribution.actor));
  memcpy(request.attribution.reason, receipt->receipt.amendment.reason,
         sizeof(request.attribution.reason));
  request.attribution.observed_at_nanoseconds = receipt->receipt.amendment.observed_at_nanoseconds;
  request.attribution.recorded_at_nanoseconds = receipt->receipt.amendment.recorded_at_nanoseconds;
  UmiRecordDocument *document = malloc(sizeof(*document));
  if (!document)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_record_transfer_receipt_document(&receipt->imported_head, receipt->bundle_sha256,
                                                &request, document);
  if (status == UMI_STATUS_OK)
    status = umi_record_revision_format(&document->record, actual, sizeof(actual));
  if (status == UMI_STATUS_OK && strcmp(expected, actual) != 0)
    status = UMI_STATUS_INVALID_STATE;
  free(document);
  return status;
}
