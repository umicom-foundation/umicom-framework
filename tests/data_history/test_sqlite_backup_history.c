/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_sqlite_backup_history.c
 *
 * PURPOSE:
 *   Whole-store recovery retains original history and separate import provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Whole-store recovery retains original history and separate import provenance. */
#include "readonly_fixture.h"
#include "umicom/data/backup.h"
#include "umicom/data/record_catalogue.h"
#include "umicom/data/record_history_page.h"
#include "umicom/data/record_transfer.h"
int main(int argc, char **argv) {
  const char *cases[] = {"document",         "history",   "receipt", "verify",
                         "append-recovered", "catalogue", "binary",  "metadata"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  int result = 0;
  const char *name = argv[1];
  UmiDataServer *original = NULL, *source = NULL, *restored = NULL;
  UmiRecordBundle *bundle = NULL;
  unsigned char *bytes = NULL;
  size_t length = 0;
  UmiRecordDocument *document = malloc(sizeof(*document));
  char file[1024], backup[1024], recovered[1024];
  CHECK(document);
  CHECK(ro_path(argv[2], name, file, true));
  int n = snprintf(backup, sizeof(backup), "%s.backup", file);
  CHECK(n > 0 && (size_t)n < sizeof(backup));
  n = snprintf(recovered, sizeof(recovered), "%s.recovered", file);
  CHECK(n > 0 && (size_t)n < sizeof(recovered));
  OK(umi_data_server_create_memory(&original));
  OK(umi_data_server_create_sqlite(file, &source));
  UmiRecordAmendment author = {.entity_id = "backup.parcel",
                               .source = "Original source",
                               .actor = "Original author",
                               .reason = "Fictional parcel",
                               .observed_at_nanoseconds = 1,
                               .recorded_at_nanoseconds = 2};
  UmiRecordRevision saved;
  const unsigned char first[] = {0, 1, 0xff, 0, 0x7f};
  OK(umi_record_document_append(original, &author, first, sizeof(first), 0, &saved));
  author.recorded_at_nanoseconds = 3;
  OK(umi_record_document_append(original, &author, "second", 6, 1, &saved));
  OK(umi_record_bundle_capture(original, author.entity_id, &bundle));
  OK(umi_record_bundle_encode(bundle, &bytes, &length));
  char digest[65] = {0};
  memcpy(digest, bytes + length - 64, 64);
  UmiRecordTransferRequest request = {.receipt_id = "backup-import",
                                      .attribution = {.source = "Import workstation",
                                                      .actor = "Import operator",
                                                      .reason = "Reviewed fictional history",
                                                      .recorded_at_nanoseconds = 4}};
  UmiRecordTransferReceipt receipt;
  OK(umi_record_transfer_restore(source, bundle, digest, &request, &receipt));
  UmiDataBackupReport created, recovery;
  OK(umi_data_backup_create_checked(source, backup, &created));
  OK(umi_data_backup_restore_checked(backup, recovered, &recovery));
  CHECK(created.record_count == recovery.record_count);
  OK(umi_data_server_create_sqlite(recovered, &restored));
  if (strcmp(name, "document") == 0) {
    OK(umi_record_document_latest(restored, author.entity_id, document));
    CHECK(document->record.revision == 2 && document->length == 6 &&
          memcmp(document->content, "second", 6) == 0);
  } else if (strcmp(name, "history") == 0) {
    UmiRecordHistoryPage page;
    OK(umi_record_history_page(restored, author.entity_id, 0, 0, 16, &page));
    CHECK(page.count == 2 && page.rows[0].revision == 2 && page.rows[1].revision == 1);
  } else if (strcmp(name, "receipt") == 0) {
    OK(umi_record_transfer_receipt_load(restored, request.receipt_id, &receipt));
    CHECK(strcmp(receipt.receipt.amendment.actor, "Import operator") == 0 &&
          strcmp(receipt.imported_head.amendment.actor, "Original author") == 0);
  } else if (strcmp(name, "verify") == 0) {
    OK(umi_record_transfer_receipt_verify(restored, request.receipt_id, &receipt));
    CHECK(strcmp(receipt.bundle_sha256, digest) == 0);
    uint64_t count = 0;
    OK(umi_record_document_verify(restored, author.entity_id, &count));
    CHECK(count == 2);
  } else if (strcmp(name, "append-recovered") == 0) {
    author.recorded_at_nanoseconds = 5;
    OK(umi_record_document_append(restored, &author, "recovered edit", 14, 2, &saved));
    OK(umi_record_transfer_receipt_verify(restored, request.receipt_id, &receipt));
    CHECK(receipt.imported_head.revision == 2);
    OK(umi_record_document_latest(source, author.entity_id, document));
    CHECK(document->record.revision == 2);
  } else if (strcmp(name, "catalogue") == 0) {
    UmiRecordCatalogueQuery query = {.limit = 16};
    UmiRecordCataloguePage page;
    OK(umi_record_catalogue_page(restored, &query, &page));
    CHECK(page.count == 2 && page.namespace_count == 2);
  } else if (strcmp(name, "binary") == 0) {
    OK(umi_record_document_load(restored, author.entity_id, 1, document));
    CHECK(document->length == sizeof(first) &&
          memcmp(document->content, first, sizeof(first)) == 0);
  } else {
    OK(umi_record_document_load(restored, author.entity_id, 1, document));
    CHECK(document->record.amendment.observed_at_nanoseconds == 1 &&
          document->record.amendment.recorded_at_nanoseconds == 2 &&
          strcmp(document->record.amendment.reason, "Fictional parcel") == 0);
  }
cleanup:
  free(document);
  umi_record_bundle_bytes_free(bytes);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(restored);
  umi_data_server_destroy(source);
  umi_data_server_destroy(original);
  return result;
}
