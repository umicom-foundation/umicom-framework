/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data_history/record_transfers.c
 *
 * PURPOSE:
 *   Original authorship and local import provenance remain separate records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Original authorship and local import provenance remain separate records. */
#include "umicom/data/record_transfer.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(x)                                                                                 \
  do {                                                                                             \
    UmiStatus status = (x);                                                                        \
    if (status != UMI_STATUS_OK) {                                                                 \
      fprintf(stderr, "%s: %d\n", #x, (int)status);                                                \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
int main(void) {
  int result = 1;
  UmiDataServer *source = NULL, *destination = NULL;
  UmiRecordBundle *bundle = NULL;
  unsigned char *bytes = NULL;
  size_t length = 0;
  REQUIRE(umi_data_server_create_memory(&source));
  REQUIRE(umi_data_server_create_memory(&destination));
  UmiRecordAmendment original = {.entity_id = "example.parcel",
                                 .source = "Example source",
                                 .actor = "Original author",
                                 .reason = "Fictional parcel note",
                                 .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  REQUIRE(umi_record_document_append(source, &original, "Parcel ready", 12, 0, &saved));
  REQUIRE(umi_record_bundle_capture(source, original.entity_id, &bundle));
  REQUIRE(umi_record_bundle_encode(bundle, &bytes, &length));
  char reviewed[65] = {0};
  memcpy(reviewed, bytes + length - 64, 64);
  /* Real hosts first display and authorize the captured history and destination.
   * This deterministic in-memory example contains only a fictional parcel. */
  UmiRecordTransferRequest request = {.receipt_id = "parcel-import",
                                      .attribution = {.source = "Example destination",
                                                      .actor = "Import operator",
                                                      .reason = "Reviewed example transfer",
                                                      .recorded_at_nanoseconds = 2}};
  UmiRecordTransferReceipt receipt;
  REQUIRE(umi_record_transfer_request_validate(&request));
  REQUIRE(umi_record_transfer_restore(destination, bundle, reviewed, &request, &receipt));
  REQUIRE(umi_record_transfer_receipt_verify(destination, "parcel-import", &receipt));
  if (strcmp(receipt.imported_head.amendment.actor, "Original author") != 0 ||
      strcmp(receipt.receipt.amendment.actor, "Import operator") != 0)
    goto cleanup;
  original.recorded_at_nanoseconds = 3;
  REQUIRE(umi_record_document_append(destination, &original, "Parcel dispatched", 17, 1, &saved));
  REQUIRE(umi_record_transfer_receipt_verify(destination, "parcel-import", &receipt));
  if (receipt.imported_head.revision != 1)
    goto cleanup;
  printf("Imported history and receipt committed together; later edits retain the original "
         "receipt.\n");
  result = 0;
cleanup:
  umi_record_bundle_bytes_free(bytes);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(destination);
  umi_data_server_destroy(source);
  return result;
}
