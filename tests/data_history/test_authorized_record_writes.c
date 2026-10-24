/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Mutation receipts derive actor/source/time and preserve optimistic revision checks. */
#include "../security/record_access_fixture.h"
static int exercise(AccessFixture *f, const char *name) {
  UmiAuthorizedRecordRevision out;
  memset(&out, 0xa5, sizeof out);
  UmiRecordRevision seed;
  UmiRecordDocument *document = malloc(sizeof *document);
  CHECK(document != NULL);
  unsigned char bytes[UMI_RECORD_DOCUMENT_MAX_BYTES];
  memset(bytes, 0x5a, sizeof bytes);
  const void *content = "new";
  size_t length = 3;
  uint64_t expected_revision = 0;
  const char *reason = "Review fictional record";
  char long_reason[256];
  UmiStatus expected = UMI_STATUS_OK;
  f->request.operation = UMI_RECORD_ACCESS_CREATE;
  if (!strcmp(name, "create")) {
  } else if (!strcmp(name, "amend")) {
    OK(access_seed(f, "example.record", "old", 0, &seed));
    expected_revision = 1;
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
  } else if (!strcmp(name, "stale")) {
    OK(access_seed(f, "example.record", "old", 0, &seed));
    expected_revision = 2;
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "occupied")) {
    OK(access_seed(f, "example.record", "old", 0, &seed));
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "wrong-create-mode")) {
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "wrong-amend-mode")) {
    expected_revision = 1;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "empty-content")) {
    content = NULL;
    length = 0;
  } else if (!strcmp(name, "null-content")) {
    content = NULL;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "maximum-content")) {
    content = bytes;
    length = sizeof bytes;
  } else if (!strcmp(name, "oversized-content")) {
    length = sizeof bytes + 1;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(name, "revision-overflow")) {
    expected_revision = UINT64_MAX;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(name, "empty-reason")) {
    reason = "";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "control-reason")) {
    reason = "line\nbreak";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "unclosed-reason")) {
    memset(long_reason, 'r', sizeof long_reason);
    reason = long_reason;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "maximum-reason")) {
    memset(long_reason, 'r', sizeof long_reason - 1);
    long_reason[255] = 0;
    reason = long_reason;
  } else if (!strcmp(name, "permission-denied")) {
    OK(umi_policy_engine_clear(f->policy));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "purpose-denied")) {
    strcpy(f->request.purpose, "export");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "create-denied")) {
    OK(access_rule(f, "example.editor", "record.create", "*", UMI_POLICY_DENY));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "amend-denied")) {
    OK(access_seed(f, "example.record", "old", 0, &seed));
    expected_revision = 1;
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
    OK(access_rule(f, "example.editor", "record.amend", "*", UMI_POLICY_DENY));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "disabled")) {
    OK(umi_identity_registry_set_enabled(f->identities, "example.editor", 0));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "revoked-after-read")) {
    OK(access_seed(f, "example.record", "old", 0, &seed));
    expected_revision = 1;
    UmiAuthorizedRecordDocument *read = malloc(sizeof *read);
    CHECK(read != NULL);
    f->request.operation = UMI_RECORD_ACCESS_READ_HEAD;
    OK(umi_authorized_record_latest(f->records, &f->request, read));
    free(read);
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
    OK(umi_session_registry_revoke(f->sessions, f->session.session_id));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "nested")) {
    OK(umi_data_server_begin(f->server));
    OK(umi_data_server_set(f->server, "pending", "retain"));
    expected = UMI_STATUS_BUSY;
  } else if (!strcmp(name, "null")) {
    CHECK(umi_authorized_record_append(NULL, &f->request, reason, 99, content, length, 0, &out) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_record_append(f->records, NULL, reason, 99, content, length, 0, &out) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_record_append(f->records, &f->request, NULL, 99, content, length, 0,
                                       &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_record_append(f->records, &f->request, reason, 99, content, length, 0,
                                       NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(access_filled(&out, sizeof out, 0xa5));
    free(document);
    return 0;
  } else {
    free(document);
    return 2;
  }
  CHECK(umi_authorized_record_append(f->records, &f->request, reason, 99, content, length,
                                     expected_revision, &out) == expected);
  if (expected == UMI_STATUS_OK) {
    CHECK(out.record.revision == expected_revision + 1);
    CHECK(!strcmp(out.record.amendment.actor, "example.editor"));
    CHECK(!strcmp(out.record.amendment.source, "Example record host"));
    CHECK(out.record.amendment.recorded_at_nanoseconds == 101);
    CHECK(out.record.amendment.observed_at_nanoseconds == 99);
    CHECK(!strcmp(out.record.amendment.reason, reason));
    CHECK(!strcmp(out.access.purpose, "review"));
    OK(umi_record_document_latest(f->server, "example.record", document));
    CHECK(document->length == length);
    if (length)
      CHECK(!memcmp(document->content, content, length));
  } else {
    CHECK(access_filled(&out, sizeof out, 0xa5));
    if (!strcmp(name, "nested")) {
      CHECK(umi_data_server_in_transaction(f->server));
      OK(umi_data_server_commit(f->server));
      char pending[16];
      OK(umi_data_server_get(f->server, "pending", pending, sizeof pending));
      CHECK(!strcmp(pending, "retain"));
    }
    bool seeded = !strcmp(name, "stale") || !strcmp(name, "occupied") ||
                  !strcmp(name, "amend-denied") || !strcmp(name, "revoked-after-read");
    UmiStatus state = umi_record_document_latest(f->server, "example.record", document);
    CHECK(state == (seeded ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND));
    if (seeded)
      CHECK(document->record.revision == 1 && document->length == 3 &&
            !memcmp(document->content, "old", 3));
  }
  free(document);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3 || (strcmp(argv[1], "memory") && strcmp(argv[1], "sqlite")))
    return 2;
  AccessFixture f;
  UmiStatus s = access_new(&f, argv[1]);
  if (s == UMI_STATUS_OK)
    s = access_allow(&f);
  int result = s == UMI_STATUS_OK ? exercise(&f, argv[2]) : 1;
  access_free(&f);
  return result;
}
