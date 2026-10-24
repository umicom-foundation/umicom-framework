/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_structured_form_record.c
 * PURPOSE: Join typed answers and atomic record history without conflating revisions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "../application_experience/structured_form_fixture.h"
#include "umicom/data/structured_form_record.h"
int main(int argc, char **argv) {
  CHECK(argc == 2 || argc == 3);
  FormFixture *f = form_fixture_new();
  CHECK(f);
  const char *name = argv[1];
  UmiDataServer *server = NULL;
  if (argc == 3) {
    CHECK(strcmp(argv[2], "sqlite") == 0);
    CHECK(umi_data_server_create_sqlite(":memory:", &server) == UMI_STATUS_OK);
  } else {
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
  }
  UmiStructuredFormRecord *loaded = malloc(sizeof(*loaded));
  CHECK(loaded);
  UmiRecordAmendment author = {.entity_id = "forms.parcel",
                               .source = "Synthetic fixture",
                               .actor = "Example reviewer",
                               .reason = "Create draft",
                               .recorded_at_nanoseconds = 1U};
  UmiRecordRevision first, second;
  UmiFormAnswer a = form_mass(3.5);
  CHECK(umi_structured_form_set(&f->schema, &f->before, 1U, "parcel.mass", &a) == UMI_STATUS_OK);
  CHECK(umi_structured_form_record_save(server, &author, &f->schema, &f->before, 0U, &first) ==
        UMI_STATUS_OK);
  CHECK(first.revision == 1U && f->before.revision == 2U && f->before.saved_revision == 1U);
  if (strcmp(name, "roundtrip") == 0) {
    CHECK(umi_structured_form_record_load(server, author.entity_id, 0U, &f->schema, loaded) ==
          UMI_STATUS_OK);
    CHECK(loaded->record.revision == 1U && loaded->form.revision == 2U &&
          loaded->form.saved_revision == 2U && loaded->form.answers[0].value.decimal_value == 3.5);
  } else if (strcmp(name, "history") == 0 || strcmp(name, "stale-open") == 0 ||
             strcmp(name, "stale-save") == 0) {
    a = form_mass(8.0);
    CHECK(umi_structured_form_set(&f->schema, &f->before, 2U, "parcel.mass", &a) == UMI_STATUS_OK);
    author.recorded_at_nanoseconds = 2U;
    CHECK(umi_structured_form_record_save(server, &author, &f->schema, &f->before, 1U, &second) ==
          UMI_STATUS_OK);
    if (strcmp(name, "history") == 0) {
      CHECK(umi_structured_form_record_load(server, author.entity_id, 1U, &f->schema, loaded) ==
                UMI_STATUS_OK &&
            loaded->form.answers[0].value.decimal_value == 3.5);
      CHECK(umi_structured_form_record_open_reviewed(server, &second, &f->schema, loaded) ==
                UMI_STATUS_OK &&
            loaded->form.answers[0].value.decimal_value == 8.0);
    } else if (strcmp(name, "stale-open") == 0) {
      memset(loaded, 0x5a, sizeof(*loaded));
      CHECK(umi_structured_form_record_open_reviewed(server, &first, &f->schema, loaded) ==
            UMI_STATUS_INVALID_STATE);
      const unsigned char *bytes = (const unsigned char *)loaded;
      for (size_t i = 0U; i < sizeof(*loaded); ++i)
        CHECK(bytes[i] == 0x5aU);
    } else {
      UmiRecordRevision output;
      memset(&output, 0x5a, sizeof(output));
      CHECK(umi_structured_form_record_save(server, &author, &f->schema, &f->before, 1U, &output) ==
            UMI_STATUS_INVALID_STATE);
      const unsigned char *bytes = (const unsigned char *)&output;
      for (size_t i = 0U; i < sizeof(output); ++i)
        CHECK(bytes[i] == 0x5aU);
      CHECK(umi_structured_form_record_load(server, author.entity_id, 0U, &f->schema, loaded) ==
                UMI_STATUS_OK &&
            loaded->record.revision == 2U);
    }
  } else if (strcmp(name, "wrong-schema") == 0) {
    f->schema.version = 2U;
    memset(loaded, 0x5a, sizeof(*loaded));
    CHECK(umi_structured_form_record_load(server, author.entity_id, 0U, &f->schema, loaded) ==
          UMI_STATUS_INVALID_STATE);
    const unsigned char *bytes = (const unsigned char *)loaded;
    for (size_t i = 0U; i < sizeof(*loaded); ++i)
      CHECK(bytes[i] == 0x5aU);
  } else if (strcmp(name, "invalid-answer") == 0) {
    memcpy(f->before.answers[0].unit_id, "mass.lb", 8U);
    author.recorded_at_nanoseconds = 2U;
    CHECK(umi_structured_form_record_save(server, &author, &f->schema, &f->before, 1U, &second) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_record_load(server, author.entity_id, 0U, &f->schema, loaded) ==
              UMI_STATUS_OK &&
          loaded->record.revision == 1U);
  } else if (strcmp(name, "foreign-payload") == 0) {
    memcpy(author.entity_id, "forms.foreign", 14U);
    CHECK(umi_record_document_append(server, &author, "not a form", 10U, 0U, &second) ==
          UMI_STATUS_OK);
    CHECK(umi_structured_form_record_load(server, author.entity_id, 0U, &f->schema, loaded) ==
          UMI_STATUS_PARSE_ERROR);
  } else if (strcmp(name, "nested") == 0) {
    CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
    author.recorded_at_nanoseconds = 2U;
    CHECK(umi_structured_form_record_save(server, &author, &f->schema, &f->before, 1U, &second) ==
          UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(server));
    CHECK(umi_structured_form_record_load(server, author.entity_id, 0U, &f->schema, loaded) ==
          UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(server));
    CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
  } else if (strcmp(name, "oversize") == 0) {
    UmiApplicationExperienceField *fields = calloc(64U, sizeof(*fields));
    UmiStructuredFormField *rules = calloc(64U, sizeof(*rules));
    char (*ids)[32] = calloc(64U, sizeof(*ids));
    CHECK(fields && rules && ids);
    for (size_t i = 0U; i < 64U; ++i) {
      (void)snprintf(ids[i], sizeof(ids[i]), "large.%zu", i);
      fields[i] =
          (UmiApplicationExperienceField){.field_id = ids[i],
                                          .label = "Text",
                                          .help_text = "Synthetic long text",
                                          .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
                                          .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT};
      fields[i].default_value.type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT;
      memset(fields[i].default_value.text_value, 'x', 500U);
      fields[i].default_value.text_value[500] = '\0';
      rules[i] = (UmiStructuredFormField){ids[i], "", UMI_FORM_TEXT_PLAIN, 0, 0};
    }
    UmiApplicationExperienceFormSchema base = f->base;
    base.form_id = "example.large";
    base.fields = fields;
    base.field_count = 64U;
    UmiStructuredFormSchema schema = {&base, 1U, rules, 64U};
    CHECK(umi_structured_form_init(&schema, &f->after) == UMI_STATUS_OK);
    memcpy(author.entity_id, "forms.too-large", 16U);
    memset(&second, 0x5a, sizeof(second));
    CHECK(umi_structured_form_record_save(server, &author, &schema, &f->after, 0U, &second) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    const unsigned char *bytes = (const unsigned char *)&second;
    for (size_t i = 0U; i < sizeof(second); ++i)
      CHECK(bytes[i] == 0x5aU);
    UmiRecordRevision absent;
    CHECK(umi_record_history_latest(server, author.entity_id, &absent) == UMI_STATUS_NOT_FOUND);
    free(ids);
    free(rules);
    free(fields);
  } else
    CHECK(0);
  free(loaded);
  umi_data_server_destroy(server);
  free(f);
  return 0;
}
