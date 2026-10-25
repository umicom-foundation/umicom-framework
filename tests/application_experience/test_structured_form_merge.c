/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_merge.c
 *
 * PURPOSE:
 *   Independent branches, explicit missing values and stale choices must remain
 *   distinguishable. These proposals never write to storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Independent branches, explicit missing values and stale choices must remain
 * distinguishable. These proposals never write to storage. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/application/experience/structured_form_merge.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static UmiStatus set_mass(FormFixture *f, UmiStructuredForm *form, double value) {
  UmiFormAnswer answer = form_mass(value);
  return umi_structured_form_set(&f->schema, form, form->revision, "parcel.mass", &answer);
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  int result = 0;
  const char *name = argv[1];
  const char *cases[] = {
      "unchanged",       "local",       "incoming", "same",           "conflict", "choose-local",
      "choose-incoming", "choose-base", "custom",   "custom-invalid", "reset",    "stale",
      "schema",          "readonly",    "missing",  "inputs",         "overflow", "required",
      "owned",           "field",       "noop",     "disjoint"};
  int found = 0;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(name, cases[i]) == 0)
      found = 1;
  if (!found)
    return 2;
  FormFixture *f = form_fixture_new();
  UmiStructuredFormMerge *review = NULL;
  UmiStructuredForm *output = calloc(1, sizeof(*output));
  UmiFormMergeField *field = calloc(1, sizeof(*field));
  UmiFormMergeState state = {0};
  CHECK(f && output && field);
  if (strcmp(name, "schema") == 0) {
    f->saved.schema_version++;
    CHECK(umi_structured_form_merge_create(&f->schema, &f->before, &f->after, &f->saved, &review) !=
              UMI_STATUS_OK &&
          review == NULL);
    goto cleanup;
  }
  if (strcmp(name, "readonly") == 0) {
    OK(set_mass(f, &f->after, 2));
    f->fields[0].read_only = 1;
    CHECK(umi_structured_form_merge_create(&f->schema, &f->before, &f->after, &f->saved, &review) ==
              UMI_STATUS_PERMISSION_DENIED &&
          review == NULL);
    f->after = f->before;
    OK(umi_structured_form_merge_create(&f->schema, &f->before, &f->after, &f->saved, &review));
    UmiFormAnswer answer = form_mass(2);
    CHECK(umi_structured_form_merge_custom(review, 1, "parcel.mass", &answer) ==
          UMI_STATUS_PERMISSION_DENIED);
    goto cleanup;
  }
  if (strcmp(name, "unchanged") != 0 && strcmp(name, "required") != 0 &&
      strcmp(name, "field") != 0) {
    if (strcmp(name, "incoming") != 0)
      OK(set_mass(f, &f->after, 2));
    if (strcmp(name, "local") != 0 && strcmp(name, "disjoint") != 0)
      OK(set_mass(f, &f->saved, strcmp(name, "same") == 0 ? 2 : 3));
  }
  if (strcmp(name, "disjoint") == 0) {
    UmiFormAnswer note = form_text("Incoming note");
    OK(umi_structured_form_set(&f->schema, &f->saved, f->saved.revision, "parcel.note", &note));
  }
  if (strcmp(name, "missing") == 0) {
    UmiFormAnswer local = form_missing(UMI_FORM_UNKNOWN, "Not measured"),
                  incoming = form_missing(UMI_FORM_UNKNOWN, "Unavailable at source");
    OK(umi_structured_form_set(&f->schema, &f->after, f->after.revision, "parcel.mass", &local));
    OK(umi_structured_form_set(&f->schema, &f->saved, f->saved.revision, "parcel.mass", &incoming));
  }
  if (strcmp(name, "overflow") == 0) {
    f->after.revision = UINT64_MAX;
    f->saved.revision = UINT64_MAX;
  }
  OK(umi_structured_form_merge_create(&f->schema, &f->before, &f->after, &f->saved, &review));
  OK(umi_structured_form_merge_state(review, &state));
  CHECK(state.revision == 1 && state.field_count == 3);
  if (strcmp(name, "unchanged") == 0 || strcmp(name, "local") == 0 ||
      strcmp(name, "incoming") == 0 || strcmp(name, "same") == 0 || strcmp(name, "disjoint") == 0) {
    CHECK(state.unresolved_count == 0 && state.conflict_count == 0);
    OK(umi_structured_form_merge_finish(review, 1, 0, output));
    if (strcmp(name, "local") == 0 || strcmp(name, "disjoint") == 0) {
      CHECK(output->answers[0].value.decimal_value == 2 && output->revision > f->after.revision &&
            output->saved_revision == f->saved.revision);
      if (strcmp(name, "disjoint") == 0)
        CHECK(strcmp(output->answers[2].value.text_value, "Incoming note") == 0);
    } else
      CHECK(output->revision == f->saved.revision && output->saved_revision == output->revision);
    goto cleanup;
  }
  if (strcmp(name, "field") == 0) {
    OK(umi_structured_form_merge_field(review, 0, field));
    CHECK(strcmp(field->field_id, "parcel.mass") == 0 &&
          field->difference == UMI_FORM_MERGE_UNCHANGED && field->resolved);
    CHECK(umi_structured_form_merge_field(review, 3, field) == UMI_STATUS_NOT_FOUND);
    goto cleanup;
  }
  if (strcmp(name, "required") == 0) {
    CHECK(umi_structured_form_merge_finish(review, 1, 1, output) == UMI_STATUS_INVALID_STATE);
    CHECK(output->revision == 0);
    goto cleanup;
  }
  CHECK(state.conflict_count == 1 && state.unresolved_count == 1);
  if (strcmp(name, "conflict") == 0 || strcmp(name, "missing") == 0) {
    CHECK(umi_structured_form_merge_finish(review, 1, 0, output) == UMI_STATUS_INVALID_STATE &&
          output->revision == 0);
    OK(umi_structured_form_merge_field(review, 0, field));
    CHECK(!field->resolved && field->difference == UMI_FORM_MERGE_CONFLICT);
    if (strcmp(name, "missing") == 0)
      CHECK(strcmp(field->local.reason, "Not measured") == 0 &&
            strcmp(field->incoming.reason, "Unavailable at source") == 0);
    goto cleanup;
  }
  if (strcmp(name, "inputs") == 0) {
    OK(umi_structured_form_merge_check_inputs(review, &f->before, &f->after, &f->saved));
    OK(set_mass(f, &f->after, 5));
    CHECK(umi_structured_form_merge_check_inputs(review, &f->before, &f->after, &f->saved) ==
          UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "custom") == 0 || strcmp(name, "custom-invalid") == 0) {
    UmiFormAnswer custom = form_mass(4);
    if (strcmp(name, "custom-invalid") == 0) {
      memcpy(custom.unit_id, "mass.lb", 8);
      CHECK(umi_structured_form_merge_custom(review, 1, "parcel.mass", &custom) != UMI_STATUS_OK);
      OK(umi_structured_form_merge_state(review, &state));
      CHECK(state.revision == 1 && state.unresolved_count == 1);
      goto cleanup;
    }
    OK(umi_structured_form_merge_custom(review, 1, "parcel.mass", &custom));
    OK(umi_structured_form_merge_custom(review, 2, "parcel.mass", &custom));
    OK(umi_structured_form_merge_state(review, &state));
    CHECK(state.revision == 2);
    OK(umi_structured_form_merge_finish(review, 2, 0, output));
    CHECK(output->answers[0].value.decimal_value == 4);
    goto cleanup;
  }
  UmiFormMergeChoice choice = strcmp(name, "choose-incoming") == 0 ? UMI_FORM_MERGE_TAKE_INCOMING
                              : strcmp(name, "choose-base") == 0   ? UMI_FORM_MERGE_KEEP_BASE
                                                                   : UMI_FORM_MERGE_KEEP_LOCAL;
  OK(umi_structured_form_merge_select(review, 1, "parcel.mass", choice));
  if (strcmp(name, "reset") == 0) {
    OK(umi_structured_form_merge_select(review, 2, "parcel.mass", UMI_FORM_MERGE_AUTOMATIC));
    CHECK(umi_structured_form_merge_finish(review, 3, 0, output) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "stale") == 0) {
    CHECK(umi_structured_form_merge_select(
              review, 1, "parcel.mass", UMI_FORM_MERGE_TAKE_INCOMING) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_merge_finish(review, 1, 0, output) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "noop") == 0) {
    OK(umi_structured_form_merge_select(review, 2, "parcel.mass", UMI_FORM_MERGE_KEEP_LOCAL));
    OK(umi_structured_form_merge_state(review, &state));
    CHECK(state.revision == 2);
    goto cleanup;
  }
  if (strcmp(name, "overflow") == 0) {
    CHECK(umi_structured_form_merge_finish(review, 2, 0, output) == UMI_STATUS_CAPACITY_EXCEEDED &&
          output->revision == 0);
    OK(umi_structured_form_merge_select(review, 2, "parcel.mass", UMI_FORM_MERGE_TAKE_INCOMING));
    OK(umi_structured_form_merge_finish(review, 3, 0, output));
    CHECK(output->revision == UINT64_MAX);
    goto cleanup;
  }
  if (strcmp(name, "owned") == 0) {
    OK(set_mass(f, &f->after, 99));
    OK(set_mass(f, &f->saved, 98));
  }
  OK(umi_structured_form_merge_finish(review, 2, 0, output));
  if (choice == UMI_FORM_MERGE_KEEP_BASE)
    CHECK(output->answers[0].state == UMI_FORM_UNANSWERED);
  else
    CHECK(output->answers[0].value.decimal_value ==
          (choice == UMI_FORM_MERGE_TAKE_INCOMING ? 3 : 2));
cleanup:
  umi_structured_form_merge_destroy(review);
  free(field);
  free(output);
  free(f);
  return result;
}
