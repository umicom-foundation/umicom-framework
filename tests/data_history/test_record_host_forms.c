/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Typed workflows retain access and schema checks inside the host's shared lock. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "record_host_fixture.h"
static int exercise(HostFixture *f, FormFixture *values, const char *name) {
  UmiFormAnswer answer = form_mass(5);
  OK(umi_structured_form_set(&values->schema, &values->before, 1, "parcel.mass", &answer));
  UmiAuthorizedRecordRevision saved;
  OK(umi_record_host_form_save(f->host, &f->request, &values->schema, &values->before,
                               "Create fictional form", 100, 0, &saved));
  UmiAuthorizedFormRecord *out = malloc(sizeof *out);
  CHECK(out != NULL);
  memset(out, 0xa5, sizeof *out);
  f->request.operation = UMI_RECORD_ACCESS_READ_HEAD;
  UmiStatus expected = UMI_STATUS_OK;
  bool reviewed = false;
  uint64_t revision = 0;
  UmiStructuredFormSchema schema = values->schema;
  if (!strcmp(name, "latest")) {
  } else if (!strcmp(name, "history")) {
    revision = 1;
    f->request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
  } else if (!strcmp(name, "reviewed"))
    reviewed = true;
  else if (!strcmp(name, "schema-mismatch")) {
    schema.version++;
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "retired")) {
    OK(umi_record_host_retire(f->host));
    expected = UMI_STATUS_CANCELLED;
  } else if (!strcmp(name, "disabled")) {
    OK(umi_record_host_set_identity_enabled(f->host, "example.editor", 0));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "amend")) {
    answer = form_mass(8);
    OK(umi_structured_form_set(&schema, &values->before, 2, "parcel.mass", &answer));
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
    OK(umi_record_host_form_save(f->host, &f->request, &schema, &values->before,
                                 "Amend fictional form", 100, 1, &saved));
    CHECK(values->before.revision == 3 && values->before.saved_revision == 1);
    f->request.operation = UMI_RECORD_ACCESS_READ_HEAD;
  } else if (!strcmp(name, "retired-save")) {
    OK(umi_record_host_retire(f->host));
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
    memset(&saved, 0xa5, sizeof saved);
    CHECK(umi_record_host_form_save(f->host, &f->request, &schema, &values->before, "Refused", 100,
                                    1, &saved) == UMI_STATUS_CANCELLED);
    CHECK(host_filled(&saved, sizeof saved, 0xa5));
    free(out);
    return 0;
  } else {
    free(out);
    return 2;
  }
  UmiStatus s = reviewed ? umi_record_host_form_open_reviewed(f->host, &f->request, &saved.record,
                                                              &schema, out)
                         : umi_record_host_form_load(f->host, &f->request, revision, &schema, out);
  CHECK(s == expected);
  if (s != UMI_STATUS_OK)
    CHECK(host_filled(out, sizeof *out, 0xa5));
  else {
    CHECK(out->form.answers[0].value.decimal_value == (!strcmp(name, "amend") ? 8.0 : 5.0));
    CHECK(out->form.saved_revision == out->form.revision);
    CHECK(out->record.revision == (!strcmp(name, "amend") ? 2U : 1U));
  }
  free(out);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  HostFixture f;
  UmiStatus s = host_new(&f, argv[1], true);
  FormFixture *values = form_fixture_new();
  int result = s == UMI_STATUS_OK && values ? exercise(&f, values, argv[2]) : 1;
  free(values);
  umi_record_host_release(f.host);
  return result;
}
