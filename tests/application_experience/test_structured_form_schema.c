/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_schema.c
 * PURPOSE: Reject malformed semantic schemas and unsafe numeric descriptors.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
#include <math.h>
int main(int argc, char **argv) {
  CHECK(argc == 2);
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  const char *name = argv[1];
  CHECK(umi_structured_form_schema_validate(&f->schema) == UMI_STATUS_OK);
  if (strcmp(name, "version") == 0)
    f->schema.version = 0U;
  else if (strcmp(name, "rules") == 0)
    f->schema.rules = NULL;
  else if (strcmp(name, "count") == 0)
    f->schema.rule_count = 4U;
  else if (strcmp(name, "identity") == 0)
    f->rules[1].field_id = "parcel.other";
  else if (strcmp(name, "unit") == 0)
    f->rules[1].unit_id = "mass.kg";
  else if (strcmp(name, "unit-token") == 0)
    f->rules[0].unit_id = "mass kg";
  else if (strcmp(name, "format") == 0)
    f->rules[0].text_format = UMI_FORM_TEXT_DATE;
  else if (strcmp(name, "policy") == 0)
    f->rules[0].allow_unknown = 2;
  else if (strcmp(name, "nan-range") == 0)
    f->fields[0].minimum_number = NAN;
  else if (strcmp(name, "infinite-range") == 0)
    f->fields[0].maximum_number = INFINITY;
  else if (strcmp(name, "default") == 0) {
    f->fields[0].default_value = form_mass(101.0).value;
    CHECK(umi_structured_form_init(&f->schema, &f->after) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&f->after, &f->saved, sizeof(f->after)) == 0);
    free(f);
    return 0;
  } else
    CHECK(0);
  CHECK(umi_structured_form_schema_validate(&f->schema) != UMI_STATUS_OK);
  CHECK(umi_structured_form_init(&f->schema, &f->after) != UMI_STATUS_OK);
  CHECK(memcmp(&f->after, &f->saved, sizeof(f->after)) == 0);
  free(f);
  return 0;
}
