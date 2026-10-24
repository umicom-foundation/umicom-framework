/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reject wrong history, retain drafts, and restore only the immutable review. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_bundle.h"
#include "umicom/data/structured_form_record.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct KeyPolicy {
  int calls;
  int accept;
} KeyPolicy;
static int key_check(const char *key, void *context) {
  KeyPolicy *policy = context;
  ++policy->calls;
  return policy->accept && strcmp(key, "transfer.parcel") == 0;
}
int main(int argc, char **argv) {
  const char *cases[] = {"receipt",
                         "receipt-digest",
                         "receipt-collision",
                         "summary",
                         "history",
                         "restore",
                         "collision",
                         "nested",
                         "wrong-digest",
                         "short-digest",
                         "namespace",
                         "namespace-invalid",
                         "empty-suffix",
                         "key-policy",
                         "key-rejected",
                         "foreign-history",
                         "foreign-current",
                         "unknown-history",
                         "unknown-current",
                         "invalid-history",
                         "invalid-current",
                         "owned-copy",
                         "draft",
                         "bad-index",
                         "null",
                         "provenance",
                         "clone"};
  if (argc != 2 && argc != 3)
    return 2;
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected || (argc == 3 && strcmp(argv[2], "sqlite") != 0))
    return 2;
  const char *name = argv[1];
  int result = 0;
  FormFixture *f = form_fixture_new(), *g = form_fixture_new();
  UmiDataServer *source = NULL, *target = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiRecordBundle *bundle = NULL;
  UmiStructuredFormBundleReview *review = NULL;
  UmiStructuredFormHistorical *out = malloc(sizeof(*out));
  unsigned char *before = malloc(sizeof(*out));
  UmiStructuredFormBundleSummary summary;
  KeyPolicy policy = {0, strcmp(name, "key-rejected") != 0};
  CHECK(f && g && out && before);
  if (strcmp(name, "foreign-history") == 0)
    f->base.form_id = "foreign.parcel";
  if (strcmp(name, "foreign-current") == 0)
    g->base.form_id = "foreign.parcel";
  g->schema.version = 2;
  OK(umi_structured_form_init(&f->schema, &f->before));
  OK(umi_structured_form_init(&g->schema, &g->before));
  const UmiStructuredFormSchema *schemas[] = {&f->schema, &g->schema};
  if (strcmp(name, "unknown-history") == 0)
    OK(umi_structured_form_catalogue_create(schemas + 1, 1, &catalogue));
  else
    OK(umi_structured_form_catalogue_create(schemas, strcmp(name, "unknown-current") == 0 ? 1 : 2,
                                            &catalogue));
  if (argc == 3) {
    OK(umi_data_server_create_sqlite(":memory:", &source));
    OK(umi_data_server_create_sqlite(":memory:", &target));
  } else {
    OK(umi_data_server_create_memory(&source));
    OK(umi_data_server_create_memory(&target));
  }
  UmiRecordAmendment metadata = {.entity_id = "transfer.parcel",
                                 .source = "Synthetic source",
                                 .actor = "Original author",
                                 .reason = "Original draft",
                                 .observed_at_nanoseconds = 0,
                                 .recorded_at_nanoseconds = 100};
  if (strcmp(name, "empty-suffix") == 0)
    memcpy(metadata.entity_id, "transfer.", 10);
  UmiRecordRevision saved;
  UmiFormAnswer answer = form_mass(9);
  if (strcmp(name, "draft") != 0)
    OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
  if (strcmp(name, "invalid-history") == 0)
    OK(umi_record_document_append(source, &metadata, "not a form", 10, 0, &saved));
  else
    OK(umi_structured_form_record_save(source, &metadata, &f->schema, &f->before, 0, &saved));
  metadata.recorded_at_nanoseconds = 200;
  memcpy(metadata.actor, "Second author", 14);
  memcpy(metadata.reason, "Revised draft", 14);
  answer = form_mass(15);
  if (strcmp(name, "draft") != 0)
    OK(umi_structured_form_set(&g->schema, &g->before, g->before.revision, "parcel.mass", &answer));
  if (strcmp(name, "invalid-current") == 0)
    OK(umi_record_document_append(source, &metadata, "not a form", 10, 1, &saved));
  else
    OK(umi_structured_form_record_save(source, &metadata, &g->schema, &g->before, 1, &saved));
  OK(umi_record_bundle_capture(source, metadata.entity_id, &bundle));
  if (strcmp(name, "namespace-invalid") == 0) {
    const char *invalid[] = {NULL, "", ".", "transfer", "transfer/", "transfer ."};
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
      CHECK(umi_structured_form_bundle_review(bundle, catalogue, "example.parcel", invalid[i], NULL,
                                              NULL, &review) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(review == NULL);
    }
    goto cleanup;
  }
  if (strcmp(name, "null") == 0) {
    CHECK(umi_structured_form_bundle_review(NULL, catalogue, "example.parcel", "transfer.", NULL,
                                            NULL, &review) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_structured_form_bundle_review(bundle, NULL, "example.parcel", "transfer.", NULL, NULL,
                                            &review) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_structured_form_bundle_review(bundle, catalogue, NULL, "transfer.", NULL, NULL,
                                            &review) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_structured_form_bundle_review(bundle, catalogue, "", "transfer.", NULL, NULL,
                                            &review) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(review == NULL);
    goto cleanup;
  }
  UmiStatus status = umi_structured_form_bundle_review(
      bundle, catalogue, "example.parcel", strcmp(name, "namespace") == 0 ? "other." : "transfer.",
      key_check, &policy, &review);
  if (strncmp(name, "unknown-", 8) == 0 || strncmp(name, "invalid-", 8) == 0 ||
      strncmp(name, "foreign-", 8) == 0 || strcmp(name, "namespace") == 0 ||
      strcmp(name, "empty-suffix") == 0 || strcmp(name, "key-rejected") == 0) {
    CHECK(status != UMI_STATUS_OK && review == NULL);
    CHECK(umi_data_server_count(target) == 0);
    CHECK(policy.calls == (strcmp(name, "key-rejected") == 0 ? 1 : 0));
    goto cleanup;
  }
  OK(status);
  CHECK(policy.calls == 1 && umi_data_server_count(target) == 0);
  if (strcmp(name, "clone") == 0) {
    UmiStructuredFormBundleReview *copy = NULL;
    OK(umi_structured_form_bundle_review_clone(review, &copy));
    umi_structured_form_bundle_review_destroy(review);
    review = copy;
  }
  OK(umi_structured_form_bundle_summary(review, &summary));
  CHECK(summary.revision_count == 2 && summary.first.revision == 1 && summary.latest.revision == 2);
  CHECK(summary.first_schema_version == 1 && summary.latest_schema_version == 2 &&
        summary.content_bytes > 0);
  CHECK(strlen(summary.bundle_sha256) == 64 && strcmp(summary.form_id, "example.parcel") == 0);
  if (strcmp(name, "owned-copy") == 0) {
    umi_record_bundle_destroy(bundle);
    bundle = NULL;
    umi_data_server_destroy(source);
    source = NULL;
    policy.accept = 0;
  }
  if (strcmp(name, "bad-index") == 0) {
    memset(out, 0x5a, sizeof(*out));
    memcpy(before, out, sizeof(*out));
    CHECK(umi_structured_form_bundle_inspect(review, 2, out) != UMI_STATUS_OK);
    CHECK(memcmp(before, out, sizeof(*out)) == 0);
    goto cleanup;
  }
  OK(umi_structured_form_bundle_inspect(review, 0, out));
  CHECK(out->record.revision == 1 && out->resolved.schema == &f->schema);
  CHECK(out->resolved.form.answers[0].state ==
        (strcmp(name, "draft") == 0 ? UMI_FORM_UNANSWERED : UMI_FORM_KNOWN));
  if (strcmp(name, "draft") != 0)
    CHECK(out->resolved.form.answers[0].value.decimal_value == 9);
  OK(umi_structured_form_bundle_inspect(review, 1, out));
  CHECK(out->record.revision == 2 && out->resolved.schema == &g->schema);
  if (strcmp(name, "draft") != 0)
    CHECK(out->resolved.form.answers[0].value.decimal_value == 15);
  if (strcmp(name, "summary") == 0 || strcmp(name, "history") == 0 ||
      strcmp(name, "key-policy") == 0)
    goto cleanup;
  if (strncmp(name, "receipt", 7) == 0) {
    UmiRecordTransferRequest request = {.receipt_id = "typed-1",
                                        .attribution = {.source = "Synthetic typed import",
                                                        .actor = "Import operator",
                                                        .reason = "Explicit review",
                                                        .recorded_at_nanoseconds = 300}};
    UmiRecordTransferReceipt receipt, sentinel;
    memset(&sentinel, 0x5a, sizeof(sentinel));
    receipt = sentinel;
    char digest[65];
    memcpy(digest, summary.bundle_sha256, sizeof(digest));
    if (strcmp(name, "receipt-digest") == 0)
      digest[0] = digest[0] == '0' ? '1' : '0';
    if (strcmp(name, "receipt-collision") == 0) {
      UmiRecordAmendment occupied = metadata;
      memcpy(occupied.entity_id, "transfer.receipt.typed-1", sizeof("transfer.receipt.typed-1"));
      OK(umi_record_document_append(target, &occupied, "occupied", 8, 0, &saved));
    }
    size_t count = umi_data_server_count(target);
    UmiStatus result_status =
        umi_structured_form_bundle_restore_receipted(target, review, digest, &request, &receipt);
    if (strcmp(name, "receipt") == 0) {
      OK(result_status);
      CHECK(receipt.imported_head.revision == 2);
      CHECK(strcmp(receipt.receipt.amendment.actor, "Import operator") == 0);
      CHECK(strcmp(receipt.imported_head.amendment.actor, "Second author") == 0);
      OK(umi_record_transfer_receipt_verify(target, "typed-1", &receipt));
    } else {
      CHECK(result_status != UMI_STATUS_OK && memcmp(&receipt, &sentinel, sizeof(receipt)) == 0);
      CHECK(umi_data_server_count(target) == count);
    }
    goto cleanup;
  }
  memset(&saved, 0xa5, sizeof(saved));
  UmiRecordRevision previous = saved;
  if (strcmp(name, "wrong-digest") == 0 || strcmp(name, "short-digest") == 0) {
    char digest[65];
    memcpy(digest, summary.bundle_sha256, sizeof(digest));
    if (strcmp(name, "wrong-digest") == 0)
      digest[0] = digest[0] == '0' ? '1' : '0';
    else
      digest[1] = '\0';
    CHECK(umi_structured_form_bundle_restore(target, review, digest, &saved) != UMI_STATUS_OK);
    CHECK(memcmp(&saved, &previous, sizeof(saved)) == 0 && umi_data_server_count(target) == 0);
    goto cleanup;
  }
  if (strcmp(name, "nested") == 0) {
    OK(umi_data_server_begin(target));
    CHECK(umi_structured_form_bundle_restore(target, review, summary.bundle_sha256, &saved) ==
          UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(target) && memcmp(&saved, &previous, sizeof(saved)) == 0);
    OK(umi_data_server_rollback(target));
    CHECK(umi_data_server_count(target) == 0);
    goto cleanup;
  }
  if (strcmp(name, "collision") == 0) {
    UmiRecordRevision original;
    OK(umi_record_document_append(target, &metadata, "existing record", 15, 0, &original));
    size_t count = umi_data_server_count(target);
    CHECK(umi_structured_form_bundle_restore(target, review, summary.bundle_sha256, &saved) !=
          UMI_STATUS_OK);
    CHECK(memcmp(&saved, &previous, sizeof(saved)) == 0 && umi_data_server_count(target) == count);
    UmiRecordDocument *doc = malloc(sizeof(*doc));
    CHECK(doc);
    status = umi_record_document_latest(target, metadata.entity_id, doc);
    int retained = status == UMI_STATUS_OK && doc->length == 15 &&
                   memcmp(doc->content, "existing record", 15) == 0;
    free(doc);
    CHECK(retained);
    goto cleanup;
  }
  OK(umi_structured_form_bundle_restore(target, review, summary.bundle_sha256, &saved));
  CHECK(saved.revision == 2 && saved.amendment.recorded_at_nanoseconds == 200 &&
        strcmp(saved.amendment.actor, "Second author") == 0);
  OK(umi_structured_form_reader_load(target, catalogue, metadata.entity_id, 1, out));
  CHECK(out->record.amendment.recorded_at_nanoseconds == 100 &&
        strcmp(out->record.amendment.actor, "Original author") == 0);
  CHECK(out->resolved.form.schema_version == 1 &&
        out->resolved.form.saved_revision == out->resolved.form.revision);
  CHECK(policy.calls == 1);
cleanup:
  umi_structured_form_bundle_review_destroy(review);
  umi_record_bundle_destroy(bundle);
  umi_structured_form_catalogue_destroy(catalogue);
  umi_data_server_destroy(target);
  umi_data_server_destroy(source);
  free(before);
  free(out);
  free(g);
  free(f);
  return result;
}
