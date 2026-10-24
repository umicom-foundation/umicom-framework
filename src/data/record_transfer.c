/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Commit original history and local import provenance as one storage decision. */
#include "include/record_bundle_internal.h"
#include "include/record_document_internal.h"
#include "include/record_transfer_internal.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus digest_for(const UmiRecordBundle *bundle, char out[65]) {
  unsigned char *bytes = NULL;
  size_t length = 0;
  UmiStatus status = umi_record_bundle_encode(bundle, &bytes, &length);
  if (status == UMI_STATUS_OK) {
    memcpy(out, bytes + length - 64, 64);
    out[64] = '\0';
  }
  umi_record_bundle_bytes_free(bytes);
  return status;
}
static UmiStatus rollback(UmiDataServer *server, UmiStatus status) {
  return umi_data_server_rollback(server) == UMI_STATUS_OK ? status : UMI_STATUS_IO_ERROR;
}
UmiStatus umi_record_transfer_restore(UmiDataServer *server, const UmiRecordBundle *bundle,
                                      const char *reviewed_sha256,
                                      const UmiRecordTransferRequest *request,
                                      UmiRecordTransferReceipt *out) {
  if (!server || !bundle || !reviewed_sha256 || !request || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t n = 0;
  while (n < 65 && reviewed_sha256[n])
    ++n;
  if (n != 64)
    return UMI_STATUS_INVALID_ARGUMENT;
  char digest[65];
  UmiStatus status = digest_for(bundle, digest);
  if (status == UMI_STATUS_OK && memcmp(digest, reviewed_sha256, 65) != 0)
    status = UMI_STATUS_INVALID_STATE;
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordBundle *receipt = calloc(1, sizeof(*receipt));
  if (!receipt)
    return UMI_STATUS_OUT_OF_MEMORY;
  receipt->count = 1;
  UmiRecordTransferReceipt candidate;
  status = umi_record_transfer_receipt_document(&bundle->records[bundle->count - 1].record, digest,
                                                request, &receipt->records[0]);
  if (status == UMI_STATUS_OK)
    status = umi_record_transfer_receipt_decode(&receipt->records[0], &candidate);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_begin(server);
  if (status != UMI_STATUS_OK) {
    free(receipt);
    return status;
  }
  status = umi_record_bundle_stage_in_transaction(server, bundle);
  if (status == UMI_STATUS_OK)
    status = umi_record_bundle_stage_in_transaction(server, receipt);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_commit(server);
  if (status != UMI_STATUS_OK)
    status = rollback(server, status);
  if (status == UMI_STATUS_OK)
    *out = candidate;
  free(receipt);
  return status;
}
/* Loading a receipt and validating its referenced original history share one
 * read transaction. Verification deliberately excludes later legitimate edits. */
static UmiStatus read_receipt(UmiDataServer *server, const char *receipt_id, bool verify,
                              UmiRecordTransferReceipt *out) {
  if (!server || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  char identity[64];
  UmiStatus status = umi_record_transfer_receipt_identity(receipt_id, identity);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordDocument *document = malloc(sizeof(*document));
  UmiRecordBundle *original = verify ? calloc(1, sizeof(*original)) : NULL;
  if (!document || (verify && !original)) {
    free(document);
    free(original);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  status = umi_data_server_begin(server);
  if (status != UMI_STATUS_OK) {
    free(document);
    free(original);
    return status;
  }
  UmiRecordTransferReceipt candidate;
  UmiRecordRevision head;
  status = umi_record_history_latest(server, identity, &head);
  if (status == UMI_STATUS_OK && head.revision != 1)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_record_document_read_in_transaction(server, identity, 1, document);
  if (status == UMI_STATUS_OK)
    status = umi_record_transfer_receipt_decode(document, &candidate);
  if (status == UMI_STATUS_OK && verify) {
    status = umi_record_history_latest(server, candidate.imported_head.amendment.entity_id, &head);
    if (status == UMI_STATUS_OK && head.revision < candidate.imported_head.revision)
      status = UMI_STATUS_INVALID_STATE;
    original->count = (size_t)candidate.imported_head.revision;
    for (size_t i = 0; status == UMI_STATUS_OK && i < original->count; ++i)
      status = umi_record_document_read_in_transaction(
          server, candidate.imported_head.amendment.entity_id, i + 1, &original->records[i]);
    char digest[65];
    if (status == UMI_STATUS_OK)
      status = digest_for(original, digest);
    if (status == UMI_STATUS_OK && strcmp(digest, candidate.bundle_sha256) != 0)
      status = UMI_STATUS_INVALID_STATE;
  }
  if (status == UMI_STATUS_OK)
    status = umi_data_server_commit(server);
  if (status != UMI_STATUS_OK)
    status = rollback(server, status);
  if (status == UMI_STATUS_OK)
    *out = candidate;
  free(document);
  free(original);
  return status;
}
UmiStatus umi_record_transfer_receipt_load(UmiDataServer *server, const char *receipt_id,
                                           UmiRecordTransferReceipt *out) {
  return read_receipt(server, receipt_id, false, out);
}
UmiStatus umi_record_transfer_receipt_verify(UmiDataServer *server, const char *receipt_id,
                                             UmiRecordTransferReceipt *out) {
  return read_receipt(server, receipt_id, true, out);
}
