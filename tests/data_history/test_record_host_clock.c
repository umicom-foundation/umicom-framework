/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A queued timestamp cannot extend a session after current-time admission. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "record_host_fixture.h"
#include "umicom/data/record_host_clock.h"
#include <stdatomic.h>
typedef struct ClockState {
  atomic_uint_fast64_t now;
  atomic_uint reads;
} ClockState;
typedef struct ClockFixture {
  HostFixture access;
  ClockState time;
  UmiClock clock;
  FormFixture *form;
  UmiAuthorizedRecordRevision document_saved, form_saved;
} ClockFixture;
static uint64_t trusted_now(const UmiClock *clock) {
  ClockState *state = clock->state;
  (void)atomic_fetch_add_explicit(&state->reads, 1, memory_order_relaxed);
  return atomic_load_explicit(&state->now, memory_order_relaxed);
}
static UmiStatus clock_fixture(ClockFixture *f, const char *backend) {
  memset(f, 0, sizeof *f);
  atomic_init(&f->time.now, 100);
  atomic_init(&f->time.reads, 0);
  f->clock = (UmiClock){.monotonic_nanoseconds = trusted_now, .state = &f->time};
  UmiRecordHostOptions options = {.backend = !strcmp(backend, "sqlite") ? UMI_RECORD_HOST_SQLITE
                                                                        : UMI_RECORD_HOST_MEMORY,
                                  .database_path = !strcmp(backend, "sqlite") ? ":memory:" : NULL,
                                  .source = "Clock test host",
                                  .entity_prefix = "example."};
  UmiStatus s = umi_record_host_create_with_clock(&options, &f->clock, &f->access.host);
  UmiIdentityRecord identity = {
      .identity_id = "example.editor", .display_name = "Example", .enabled = 1};
  if (s == UMI_STATUS_OK)
    s = umi_record_host_register_identity(f->access.host, &identity);
  if (s == UMI_STATUS_OK)
    s = umi_record_host_issue_current_session(f->access.host, "example.editor", 1000,
                                              &f->access.session);
  uint64_t revision;
  if (s == UMI_STATUS_OK)
    s = host_policy(f->access.host, UMI_POLICY_ALLOW, &revision);
  strcpy(f->access.request.session_id, f->access.session.session_id);
  strcpy(f->access.request.entity_id, "example.record");
  strcpy(f->access.request.purpose, "review");
  f->access.request.operation = UMI_RECORD_ACCESS_CREATE;
  f->access.request.now_nanoseconds = UINT64_MAX;
  atomic_store_explicit(&f->time.now, 101, memory_order_relaxed);
  if (s == UMI_STATUS_OK)
    s = umi_record_host_append_current(f->access.host, &f->access.request,
                                       "Fictional initial record", 0, "first", 5, 0,
                                       &f->document_saved);
  f->form = form_fixture_new();
  if (!f->form)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiFormAnswer mass = form_mass(5);
  if (s == UMI_STATUS_OK)
    s = umi_structured_form_set(&f->form->schema, &f->form->before, 1, "parcel.mass", &mass);
  strcpy(f->access.request.entity_id, "example.form");
  if (s == UMI_STATUS_OK)
    s = umi_record_host_form_save_current(f->access.host, &f->access.request, &f->form->schema,
                                          &f->form->before, "Fictional form", 0, 0, &f->form_saved);
  strcpy(f->access.request.entity_id, "example.record");
  f->access.request.operation = UMI_RECORD_ACCESS_READ_HEAD;
  atomic_store_explicit(&f->time.now, 102, memory_order_relaxed);
  return s;
}
static int exercise(ClockFixture *f, const char *name) {
  UmiSessionRecord session;
  memset(&session, 0xa5, sizeof session);
  UmiAuthorizedRecordDocument *document = malloc(sizeof *document);
  UmiAuthorizedFormRecord *form = malloc(sizeof *form);
  CHECK(document && form);
  memset(document, 0xa5, sizeof *document);
  memset(form, 0xa5, sizeof *form);
  UmiAuthorizedRecordRevision saved;
  memset(&saved, 0xa5, sizeof saved);
  UmiStatus expected = UMI_STATUS_OK, s;
  unsigned kind = 0;
  const char *principal = "example.editor";
  uint64_t lifetime = 1000;
  unsigned reads = atomic_load_explicit(&f->time.reads, memory_order_relaxed);
  if (!strcmp(name, "issue"))
    kind = 1;
  else if (!strcmp(name, "issue-zero")) {
    kind = 1;
    atomic_store(&f->time.now, 0);
    expected = UMI_STATUS_UNAVAILABLE;
  } else if (!strcmp(name, "issue-overflow")) {
    kind = 1;
    lifetime = UINT64_MAX;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(name, "issue-disabled")) {
    kind = 1;
    OK(umi_record_host_set_identity_enabled(f->access.host, principal, 0));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "issue-missing")) {
    kind = 1;
    principal = "missing";
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "issue-retired")) {
    kind = 1;
    OK(umi_record_host_retire(f->access.host));
    expected = UMI_STATUS_CANCELLED;
  } else if (!strcmp(name, "latest-time") || !strcmp(name, "unchanged-request")) {
  } else if (!strcmp(name, "append-time")) {
    kind = 2;
    f->access.request.operation = UMI_RECORD_ACCESS_AMEND;
  } else if (!strcmp(name, "history-time")) {
    kind = 3;
    f->access.request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
  } else if (!strcmp(name, "reviewed-time"))
    kind = 4;
  else if (!strcmp(name, "form-save-time")) {
    kind = 5;
    strcpy(f->access.request.entity_id, "example.form");
    f->access.request.operation = UMI_RECORD_ACCESS_AMEND;
    UmiFormAnswer answer = form_mass(8);
    OK(umi_structured_form_set(&f->form->schema, &f->form->before, 2, "parcel.mass", &answer));
  } else if (!strcmp(name, "form-load-time")) {
    kind = 6;
    strcpy(f->access.request.entity_id, "example.form");
  } else if (!strcmp(name, "form-reviewed-time")) {
    kind = 7;
    strcpy(f->access.request.entity_id, "example.form");
  } else if (!strcmp(name, "stale-request-time"))
    f->access.request.now_nanoseconds = 0;
  else if (!strcmp(name, "expired-current")) {
    f->access.request.now_nanoseconds = 101;
    atomic_store(&f->time.now, 1100);
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "rollback")) {
    atomic_store(&f->time.now, 100);
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "zero-current")) {
    atomic_store(&f->time.now, 0);
    expected = UMI_STATUS_UNAVAILABLE;
  } else if (!strcmp(name, "retired-no-clock")) {
    OK(umi_record_host_retire(f->access.host));
    expected = UMI_STATUS_CANCELLED;
  } else if (!strcmp(name, "clock-copy")) {
    f->clock.monotonic_nanoseconds = NULL;
  } else if (!strcmp(name, "policy-denied")) {
    uint64_t revision;
    OK(host_policy(f->access.host, UMI_POLICY_DENY, &revision));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "null-request")) {
    CHECK(umi_record_host_latest_current(f->access.host, NULL, document) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(host_filled(document, sizeof *document, 0xa5));
    free(form);
    free(document);
    return 0;
  } else if (!strcmp(name, "null-clock") || !strcmp(name, "default-system")) {
    UmiRecordHostOptions options = {
        .backend = UMI_RECORD_HOST_MEMORY, .source = "System host", .entity_prefix = "example."};
    UmiRecordHost *other = NULL;
    if (!strcmp(name, "null-clock")) {
      UmiClock empty = {0};
      other = f->access.host;
      CHECK(umi_record_host_create_with_clock(&options, NULL, &other) ==
            UMI_STATUS_INVALID_ARGUMENT);
      CHECK(umi_record_host_create_with_clock(&options, &empty, &other) ==
            UMI_STATUS_INVALID_ARGUMENT);
      CHECK(other == f->access.host);
    } else {
      OK(umi_record_host_create(&options, &other));
      UmiIdentityRecord identity = {
          .identity_id = "example.editor", .display_name = "Example", .enabled = 1};
      OK(umi_record_host_register_identity(other, &identity));
      OK(umi_record_host_issue_current_session(other, identity.identity_id, UINT64_C(1000000000),
                                               &session));
      CHECK(session.issued_at_ns > 0 &&
            session.expires_at_ns - session.issued_at_ns == UINT64_C(1000000000));
      umi_record_host_release(other);
    }
    free(form);
    free(document);
    return 0;
  } else {
    free(form);
    free(document);
    return 2;
  }
  UmiRecordAccessRequest before;
  memcpy(&before, &f->access.request, sizeof before);
  switch (kind) {
  case 1:
    s = umi_record_host_issue_current_session(f->access.host, principal, lifetime, &session);
    break;
  case 2:
    s = umi_record_host_append_current(f->access.host, &f->access.request, "Fictional amendment", 0,
                                       "second", 6, 1, &saved);
    break;
  case 3:
    s = umi_record_host_load_current(f->access.host, &f->access.request, 1, document);
    break;
  case 4:
    s = umi_record_host_open_reviewed_current(f->access.host, &f->access.request,
                                              &f->document_saved.record, document);
    break;
  case 5:
    s = umi_record_host_form_save_current(f->access.host, &f->access.request, &f->form->schema,
                                          &f->form->before, "Amend fictional form", 0, 1, &saved);
    break;
  case 6:
    s = umi_record_host_form_load_current(f->access.host, &f->access.request, 0, &f->form->schema,
                                          form);
    break;
  case 7:
    s = umi_record_host_form_open_reviewed_current(f->access.host, &f->access.request,
                                                   &f->form_saved.record, &f->form->schema, form);
    break;
  default:
    s = umi_record_host_latest_current(f->access.host, &f->access.request, document);
    break;
  }
  CHECK(s == expected && !memcmp(&before, &f->access.request, sizeof before));
  if (expected == UMI_STATUS_CANCELLED)
    CHECK(atomic_load_explicit(&f->time.reads, memory_order_relaxed) == reads);
  if (expected == UMI_STATUS_OK) {
    if (kind == 1)
      CHECK(session.issued_at_ns == 102 && session.expires_at_ns == 1102);
    else if (kind == 2 || kind == 5)
      CHECK(saved.access.checked_at_nanoseconds == 102 &&
            saved.record.amendment.recorded_at_nanoseconds == 102 && saved.record.revision == 2);
    else if (kind == 6 || kind == 7)
      CHECK(form->access.checked_at_nanoseconds == 102);
    else
      CHECK(document->access.checked_at_nanoseconds == 102 &&
            document->document.record.amendment.recorded_at_nanoseconds == 101);
  } else {
    CHECK(host_filled(&session, sizeof session, 0xa5));
    CHECK(host_filled(document, sizeof *document, 0xa5));
    CHECK(host_filled(form, sizeof *form, 0xa5));
    CHECK(host_filled(&saved, sizeof saved, 0xa5));
  }
  free(form);
  free(document);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  ClockFixture f;
  UmiStatus s = clock_fixture(&f, argv[1]);
  int result = s == UMI_STATUS_OK ? exercise(&f, argv[2]) : 1;
  umi_record_host_release(f.access.host);
  free(f.form);
  return result;
}
