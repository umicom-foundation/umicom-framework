/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_data_readonly_records.c
 *
 * PURPOSE:
 *   Portable history services operate through the same inspection connection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Portable history services operate through the same inspection connection. */
#include "readonly_fixture.h"
#include "umicom/data/record_catalogue.h"
#include "umicom/data/record_history_page.h"
#include "umicom/data/record_transfer.h"
int main(int argc, char **argv) {
  const char *cases[] = {"document", "history",       "bundle",         "catalogue", "receipt",
                         "verify",   "append-denied", "restore-denied", "nested",    "later"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  int result = 0;
  const char *name = argv[1];
  char file[1024];
  UmiDataServer *source = NULL, *writer = NULL, *reader = NULL;
  UmiRecordBundle *bundle = NULL, *captured = NULL;
  unsigned char *bytes = NULL;
  size_t length = 0;
  UmiRecordDocument *document = malloc(sizeof(*document));
  CHECK(document);
  CHECK(ro_path(argv[2], name, file, true));
  OK(umi_data_server_create_memory(&source));
  OK(umi_data_server_create_sqlite(file, &writer));
  UmiRecordAmendment author = {.entity_id = "inspection.parcel",
                               .source = "Fictional source",
                               .actor = "Original author",
                               .reason = "Parcel observation",
                               .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  OK(umi_record_document_append(source, &author, "first", 5, 0, &saved));
  author.recorded_at_nanoseconds = 2;
  OK(umi_record_document_append(source, &author, "second", 6, 1, &saved));
  OK(umi_record_bundle_capture(source, author.entity_id, &bundle));
  OK(umi_record_bundle_encode(bundle, &bytes, &length));
  char digest[65] = {0};
  memcpy(digest, bytes + length - 64, 64);
  UmiRecordTransferRequest request = {.receipt_id = "readonly-import",
                                      .attribution = {.source = "Inspection fixture",
                                                      .actor = "Import operator",
                                                      .reason = "Reviewed fictional parcel",
                                                      .recorded_at_nanoseconds = 3}};
  UmiRecordTransferReceipt receipt;
  OK(umi_record_transfer_restore(writer, bundle, digest, &request, &receipt));
  OK(umi_data_server_open_sqlite_readonly(file, &reader));
/* Preserve the initial test expression for review. The public record API
 * supplies latest(), length and an explicit restored-metadata output. */
#if 0
    if(strcmp(name,"document")==0){OK(umi_record_document_load(reader,author.entity_id,document));CHECK(document->record.revision==2&&document->content_length==6&&memcmp(document->content,"second",6)==0);}
#endif
  if (strcmp(name, "document") == 0) {
    OK(umi_record_document_latest(reader, author.entity_id, document));
    CHECK(document->record.revision == 2 && document->length == 6 &&
          memcmp(document->content, "second", 6) == 0);
  } else if (strcmp(name, "history") == 0) {
    UmiRecordHistoryPage page;
    OK(umi_record_history_page(reader, author.entity_id, 0, 0, 16, &page));
    CHECK(page.count == 2 && page.rows[1].revision == 1);
  } else if (strcmp(name, "bundle") == 0) {
    OK(umi_record_bundle_capture(reader, author.entity_id, &captured));
    unsigned char *copy = NULL;
    size_t count = 0;
    UmiStatus status = umi_record_bundle_encode(captured, &copy, &count);
    bool same = status == UMI_STATUS_OK && count == length && memcmp(copy, bytes, length) == 0;
    umi_record_bundle_bytes_free(copy);
    CHECK(same);
  } else if (strcmp(name, "catalogue") == 0) {
    UmiRecordCatalogueQuery query = {.entity_prefix = "transfer.receipt.", .limit = 16};
    UmiRecordCataloguePage page;
    OK(umi_record_catalogue_page(reader, &query, &page));
    CHECK(page.count == 1 &&
          strcmp(page.rows[0].amendment.entity_id, receipt.receipt.amendment.entity_id) == 0);
  } else if (strcmp(name, "receipt") == 0) {
    OK(umi_record_transfer_receipt_load(reader, request.receipt_id, &receipt));
    CHECK(strcmp(receipt.receipt.amendment.actor, "Import operator") == 0);
  } else if (strcmp(name, "verify") == 0) {
    OK(umi_record_transfer_receipt_verify(reader, request.receipt_id, &receipt));
    CHECK(strcmp(receipt.bundle_sha256, digest) == 0);
  } else if (strcmp(name, "append-denied") == 0) {
    author.recorded_at_nanoseconds = 4;
    UmiRecordRevision original = saved;
    CHECK(umi_record_document_append(reader, &author, "forbidden", 9, 2, &saved) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&saved, &original, sizeof(saved)) == 0);
    CHECK(!umi_data_server_in_transaction(reader));
  }
/* Preserve the initial test expression for review. The public record API
 * supplies latest(), length and an explicit restored-metadata output. */
#if 0
    else if(strcmp(name,"restore-denied")==0){UmiDataServer *empty=NULL;char target[1024];CHECK(ro_path(argv[2],"restore-target",target,true));OK(umi_data_server_create_sqlite(target,&empty));umi_data_server_destroy(empty);empty=NULL;umi_data_server_destroy(reader);reader=NULL;OK(umi_data_server_open_sqlite_readonly(target,&reader));CHECK(umi_record_bundle_restore_new(reader,bundle)==UMI_STATUS_PERMISSION_DENIED);CHECK(!umi_data_server_in_transaction(reader));CHECK(umi_data_server_count(reader)==0);}
#endif
  else if (strcmp(name, "restore-denied") == 0) {
    UmiDataServer *empty = NULL;
    char target[1024];
    CHECK(ro_path(argv[2], "restore-target", target, true));
    OK(umi_data_server_create_sqlite(target, &empty));
    umi_data_server_destroy(empty);
    empty = NULL;
    umi_data_server_destroy(reader);
    reader = NULL;
    OK(umi_data_server_open_sqlite_readonly(target, &reader));
    CHECK(umi_record_bundle_restore_new(reader, bundle, &saved) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(!umi_data_server_in_transaction(reader));
    CHECK(umi_data_server_count(reader) == 0);
  } else if (strcmp(name, "nested") == 0) {
    OK(umi_data_server_begin(reader));
    CHECK(umi_record_transfer_receipt_verify(reader, request.receipt_id, &receipt) ==
          UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(reader));
    OK(umi_data_server_rollback(reader));
  } else {
    author.recorded_at_nanoseconds = 4;
    OK(umi_record_document_append(writer, &author, "later", 5, 2, &saved));
    OK(umi_record_transfer_receipt_verify(reader, request.receipt_id, &receipt));
    CHECK(receipt.imported_head.revision == 2);
  }
cleanup:
  free(document);
  umi_record_bundle_bytes_free(bytes);
  umi_record_bundle_destroy(captured);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(reader);
  umi_data_server_destroy(writer);
  umi_data_server_destroy(source);
  return result;
}
