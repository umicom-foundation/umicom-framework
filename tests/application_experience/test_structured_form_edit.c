/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_edit.c
 * PURPOSE: Verify atomic edit rejection and monotonic, reviewed undo.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
#include "umicom/application/experience/structured_form_edit.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  const char *name = argv[1];
  UmiStructuredFormEdit *edit = calloc(1U, sizeof(*edit)), *inverse = calloc(1U, sizeof(*inverse));
  CHECK(edit && inverse);
  UmiFormAnswer a = form_mass(12.0);
  CHECK(umi_structured_form_set(&f->schema, &f->after, 1U, "parcel.mass", &a) == UMI_STATUS_OK);
  a = form_text("2024-02-29T00:00:00Z");
  CHECK(umi_structured_form_set(&f->schema, &f->after, 2U, "parcel.observed", &a) == UMI_STATUS_OK);
  CHECK(umi_structured_form_edit_capture(&f->schema, &f->before, &f->after, edit) ==
            UMI_STATUS_OK &&
        edit->count == 2U);
  if (strcmp(name, "undo") == 0) {
    CHECK(umi_structured_form_edit_apply(&f->schema, &f->before, edit) == UMI_STATUS_OK &&
          f->before.revision == 2U);
    CHECK(umi_structured_form_edit_inverse(&f->schema, &f->before, edit, inverse) == UMI_STATUS_OK);
    CHECK(umi_structured_form_edit_apply(&f->schema, &f->before, inverse) == UMI_STATUS_OK &&
          f->before.revision == 3U);
    CHECK(f->before.saved_revision == 1U && f->before.answers[0].state == UMI_FORM_UNANSWERED);
    CHECK(umi_structured_form_edit_apply(&f->schema, &f->before, edit) == UMI_STATUS_INVALID_STATE);
  } else if (strcmp(name, "invalid-second") == 0)
    edit->changes[1].after = form_mass(5.0);
  else if (strcmp(name, "duplicate") == 0)
    edit->changes[1] = edit->changes[0];
  else if (strcmp(name, "stale-value") == 0)
    edit->changes[1].before = form_text("2024-01-01T00:00:00Z");
  else if (strcmp(name, "stale-revision") == 0)
    edit->base_revision = 2U;
  else if (strcmp(name, "readonly") == 0)
    f->fields[1].read_only = 1;
  else if (strcmp(name, "unterminated") == 0)
    memset(edit->changes[1].field_id, 'x', sizeof(edit->changes[1].field_id));
  else if (strcmp(name, "identity") == 0)
    edit->schema_version = 2U;
  else if (strcmp(name, "empty") == 0) {
    CHECK(umi_structured_form_edit_capture(&f->schema, &f->before, &f->before, edit) ==
              UMI_STATUS_OK &&
          edit->count == 0U);
    CHECK(umi_structured_form_edit_apply(&f->schema, &f->before, edit) == UMI_STATUS_OK &&
          memcmp(&f->before, &f->saved, sizeof(f->saved)) == 0);
  } else if (strcmp(name, "inverse-stale") == 0) {
    memset(inverse, 0x5a, sizeof(*inverse));
    CHECK(umi_structured_form_edit_inverse(&f->schema, &f->before, edit, inverse) ==
          UMI_STATUS_INVALID_STATE);
    const unsigned char *p = (const unsigned char *)inverse;
    for (size_t i = 0U; i < sizeof(*inverse); ++i)
      CHECK(p[i] == 0x5aU);
  } else
    CHECK(0);
  if (strcmp(name, "undo") && strcmp(name, "empty") && strcmp(name, "inverse-stale")) {
    CHECK(umi_structured_form_edit_apply(&f->schema, &f->before, edit) != UMI_STATUS_OK);
    CHECK(memcmp(&f->before, &f->saved, sizeof(f->saved)) == 0);
  }
  free(edit);
  free(inverse);
  free(f);
  return 0;
}
