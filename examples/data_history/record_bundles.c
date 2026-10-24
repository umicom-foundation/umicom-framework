/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Transfer fictional history to another store without replacing existing data. */
#include "umicom/data/record_bundle.h"
#include <stdio.h>
#include <stdlib.h>
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
  UmiRecordBundle *bundle = NULL, *decoded = NULL;
  unsigned char *bytes = NULL;
  size_t length = 0;
  UmiRecordDocument *read = malloc(sizeof(*read));
  if (read == NULL)
    return 1;
  REQUIRE(umi_data_server_create_memory(&source));
  REQUIRE(umi_data_server_create_memory(&destination));
  UmiRecordAmendment author = {.entity_id = "example.transfer",
                               .source = "Portable example",
                               .actor = "Example author",
                               .reason = "Initial note",
                               .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  REQUIRE(umi_record_document_append(source, &author, "First note", 10, 0, &saved));
  author.recorded_at_nanoseconds = 2;
  REQUIRE(umi_record_document_append(source, &author, "Revised note", 12, 1, &saved));
  REQUIRE(umi_record_bundle_capture(source, "example.transfer", &bundle));
  REQUIRE(umi_record_bundle_encode(bundle, &bytes, &length));
  REQUIRE(umi_record_bundle_decode(bytes, length, &decoded));
  REQUIRE(umi_record_bundle_restore_new(destination, decoded, &saved));
  REQUIRE(umi_record_document_load(destination, "example.transfer", 1, read));
  if (read->length != 10 || memcmp(read->content, "First note", 10) != 0)
    goto cleanup;
  if (umi_record_bundle_restore_new(destination, decoded, &saved) != UMI_STATUS_INVALID_STATE)
    goto cleanup;
  printf("Restored two revisions; original note retained; duplicate restore refused.\n");
  result = 0;
cleanup:
  umi_record_bundle_bytes_free(bytes);
  umi_record_bundle_destroy(decoded);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(destination);
  umi_data_server_destroy(source);
  free(read);
  return result;
}
