/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_transfer_receipt.c
 *
 * PURPOSE:
 *   Receipt decoding validates framing and meaning before publishing any output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Receipt decoding validates framing and meaning before publishing any output. */
#include "../../src/data/include/record_transfer_internal.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  const char *cases[] = {"id-contract", "validate-digest", "validate-body",  "validate-receipt",
                         "roundtrip",   "digest",          "checksum",       "magic",
                         "separator",   "truncated",       "noncanonical",   "foreign-namespace",
                         "amended",     "nested-receipt",  "invalid-source", "unterminated-id",
                         "boundary-id", "zero-time",       "null",           "long-history",
                         "metadata",    "identity"};
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  bool known = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  int result = 0;
  UmiDataServer *source = NULL;
  UmiRecordDocument *document = calloc(1, sizeof(*document));
  CHECK(document);
  if (strcmp(name, "id-contract") == 0) {
    char bounded[UMI_RECORD_TRANSFER_ID_CAPACITY];
    memset(bounded, 'a', sizeof(bounded));
    CHECK(umi_record_transfer_id_validate(bounded) == UMI_STATUS_INVALID_ARGUMENT);
    bounded[sizeof(bounded) - 1] = '\0';
    OK(umi_record_transfer_id_validate(bounded));
    OK(umi_record_transfer_id_validate("Abc_123-X"));
    CHECK(umi_record_transfer_id_validate("") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_transfer_id_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_transfer_id_validate("bad.id") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_transfer_id_validate("bad id") == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  OK(umi_data_server_create_memory(&source));
  UmiRecordAmendment author = {.entity_id = "receipt.example",
                               .source = "Original source",
                               .actor = "Original author",
                               .reason = "Original edit",
                               .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  OK(umi_record_document_append(source, &author, "first", 5, 0, &saved));
  author.recorded_at_nanoseconds = 2;
  OK(umi_record_document_append(source, &author, "second", 6, 1, &saved));
  char digest[65];
  memset(digest, 'a', 64);
  digest[64] = '\0';
  UmiRecordTransferRequest request = {.receipt_id = "example",
                                      .attribution = {.source = "Import source",
                                                      .actor = "Import operator",
                                                      .reason = "Explicit review",
                                                      .recorded_at_nanoseconds = 3}};
  UmiRecordTransferReceipt output, sentinel;
  memset(&sentinel, 0x6b, sizeof(sentinel));
  output = sentinel;
  if (strcmp(name, "invalid-source") == 0)
    memcpy(request.attribution.source, "bad\nsource", 11);
  if (strcmp(name, "zero-time") == 0)
    request.attribution.recorded_at_nanoseconds = 0;
  if (strcmp(name, "unterminated-id") == 0)
    memset(request.receipt_id, 'a', sizeof(request.receipt_id));
  if (strcmp(name, "boundary-id") == 0) {
    memset(request.receipt_id, 'z', 40);
    request.receipt_id[40] = '\0';
  }
  if (strcmp(name, "identity") == 0)
    memcpy(request.receipt_id, "bad.id", 7);
  if (strcmp(name, "nested-receipt") == 0)
    memcpy(saved.amendment.entity_id, "transfer.receipt.inner", sizeof("transfer.receipt.inner"));
  if (strcmp(name, "long-history") == 0) {
    saved.revision = 17;
    saved.previous_revision = 16;
  }
  bool invalid_request = strcmp(name, "invalid-source") == 0 || strcmp(name, "zero-time") == 0 ||
                         strcmp(name, "unterminated-id") == 0 || strcmp(name, "identity") == 0 ||
                         strcmp(name, "nested-receipt") == 0 || strcmp(name, "long-history") == 0;
  bool invalid_attribution = strcmp(name, "invalid-source") == 0 ||
                             strcmp(name, "zero-time") == 0 ||
                             strcmp(name, "unterminated-id") == 0 || strcmp(name, "identity") == 0;
  UmiStatus validation = umi_record_transfer_request_validate(&request);
  CHECK(invalid_attribution ? validation != UMI_STATUS_OK : validation == UMI_STATUS_OK);
  if (invalid_request) {
    CHECK(umi_record_transfer_receipt_document(&saved, digest, &request, document) !=
          UMI_STATUS_OK);
    CHECK(document->length == 0);
    goto cleanup;
  }
  OK(umi_record_transfer_receipt_document(&saved, digest, &request, document));
  if (strncmp(name, "validate-", 9) == 0) {
    OK(umi_record_transfer_receipt_decode(document, &output));
    OK(umi_record_transfer_receipt_validate(&output));
    if (strcmp(name, "validate-digest") == 0)
      output.bundle_sha256[0] = 'b';
    if (strcmp(name, "validate-body") == 0)
      memcpy(output.imported_head.amendment.actor, "Changed author", sizeof("Changed author"));
    if (strcmp(name, "validate-receipt") == 0)
      output.receipt.amendment.content_sha256[0] =
          output.receipt.amendment.content_sha256[0] == 'a' ? 'b' : 'a';
    CHECK(umi_record_transfer_receipt_validate(&output) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "null") == 0) {
    CHECK(umi_record_transfer_receipt_decode(NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_transfer_receipt_decode(document, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "digest") == 0)
    document->content[7] = 'g';
  if (strcmp(name, "magic") == 0)
    document->content[0] = 'X';
  if (strcmp(name, "separator") == 0)
    document->content[71] = ' ';
  if (strcmp(name, "truncated") == 0)
    document->length = 72;
  if (strcmp(name, "checksum") == 0)
    document->content[8] = 'b';
  if (strcmp(name, "foreign-namespace") == 0)
    memcpy(document->record.amendment.entity_id, "other.receipt", sizeof("other.receipt"));
  if (strcmp(name, "amended") == 0) {
    document->record.revision = 2;
    document->record.previous_revision = 1;
  }
  if (strcmp(name, "metadata") == 0)
    memset(document->record.amendment.actor, 'x', sizeof(document->record.amendment.actor));
  if (strcmp(name, "noncanonical") == 0) {
    char *field = strstr((char *)document->content + 72, "\t2\t1\t");
    CHECK(field);
    size_t at = (size_t)(field - (char *)document->content) + 1;
    memmove(document->content + at + 1, document->content + at, document->length - at);
    document->content[at] = '0';
    ++document->length;
  }
  if (strcmp(name, "checksum") != 0)
    OK(UmiSha256Buffer(document->content, document->length,
                       document->record.amendment.content_sha256));
  UmiStatus status = umi_record_transfer_receipt_decode(document, &output);
  if (strcmp(name, "roundtrip") == 0 || strcmp(name, "boundary-id") == 0) {
    OK(status);
    CHECK(output.imported_head.revision == 2 && output.receipt.revision == 1);
    CHECK(strcmp(output.imported_head.amendment.actor, "Original author") == 0);
    CHECK(strcmp(output.receipt.amendment.actor, "Import operator") == 0);
    CHECK(strcmp(output.bundle_sha256, digest) == 0);
    if (strcmp(name, "boundary-id") == 0)
    /* The fixed namespace has 17 bytes; retain the earlier miscount for
     * review of this boundary regression. Forty ID bytes produce 57 total. */
#if 0
      CHECK(strlen(output.receipt.amendment.entity_id) == 56);
#endif
      CHECK(strlen(output.receipt.amendment.entity_id) == 57);
  } else {
    CHECK(status != UMI_STATUS_OK);
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
  }
cleanup:
  umi_data_server_destroy(source);
  free(document);
  return result;
}
