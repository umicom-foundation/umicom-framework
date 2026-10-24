/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_migration.c
 * PURPOSE: Refuse semantic data loss during explicitly requested schema upgrades.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
#include "umicom/application/experience/structured_form_migration.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  FormFixture *f = form_fixture_new(), *target = form_fixture_new();
  CHECK(f && target);
  const char *name = argv[1];
  UmiFormAnswer a = form_mass(9.0);
  CHECK(umi_structured_form_set(&f->schema, &f->before, 1U, "parcel.mass", &a) == UMI_STATUS_OK);
  target->schema.version = 2U;
  if (strcmp(name, "reorder") == 0) {
    UmiApplicationExperienceField field = target->fields[0];
    target->fields[0] = target->fields[2];
    target->fields[2] = field;
    UmiStructuredFormField rule = target->rules[0];
    target->rules[0] = target->rules[2];
    target->rules[2] = rule;
    CHECK(umi_structured_form_migrate(&f->schema, &f->before, &target->schema, &f->after) ==
          UMI_STATUS_OK);
    CHECK(f->after.schema_version == 2U && f->after.revision == 3U &&
          f->after.saved_revision == 0U);
    CHECK(f->after.answers[2].value.decimal_value == 9.0 &&
          strcmp(f->after.answers[2].unit_id, "mass.kg") == 0);
  } else if (strcmp(name, "alias") == 0) {
    CHECK(umi_structured_form_migrate(&f->schema, &f->before, &target->schema, &f->before) ==
          UMI_STATUS_OK);
    CHECK(f->before.schema_version == 2U && f->before.answers[0].value.decimal_value == 9.0);
  } else if (strcmp(name, "removed-empty") == 0) {
    target->base.field_count = target->schema.rule_count = 2U;
    CHECK(umi_structured_form_migrate(&f->schema, &f->before, &target->schema, &f->after) ==
              UMI_STATUS_OK &&
          f->after.count == 2U);
  } else {
    if (strcmp(name, "unit") == 0)
      target->rules[0].unit_id = "mass.lb";
    else if (strcmp(name, "type") == 0)
      target->fields[0].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    else if (strcmp(name, "format") == 0)
      target->rules[1].text_format = UMI_FORM_TEXT_DATE;
    else if (strcmp(name, "constraint") == 0)
      target->fields[0].maximum_number = 5.0;
    else if (strcmp(name, "removed-answer") == 0) {
      target->fields[0].field_id = "parcel.replacement";
      target->rules[0].field_id = "parcel.replacement";
    } else if (strcmp(name, "unknown-policy") == 0) {
      a = form_missing(UMI_FORM_UNKNOWN, "Not measured");
      CHECK(umi_structured_form_set(&f->schema, &f->before, 2U, "parcel.mass", &a) ==
            UMI_STATUS_OK);
      target->rules[0].allow_unknown = 0;
    } else if (strcmp(name, "version") == 0)
      target->schema.version = 1U;
    else
      CHECK(0);
    CHECK(umi_structured_form_migrate(&f->schema, &f->before, &target->schema, &f->after) !=
          UMI_STATUS_OK);
    CHECK(memcmp(&f->after, &f->saved, sizeof(f->saved)) == 0);
  }
  free(target);
  free(f);
  return 0;
}
