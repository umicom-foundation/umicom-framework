/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data_history/form_search_presets.c
 *
 * PURPOSE:
 *   Save and reopen a reusable inventory search through installed SDK contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Save and reopen a reusable inventory search through installed SDK contracts. */
#include "umicom/data/structured_form_query.h"
#include "umicom/data/structured_form_record.h"
#include "umicom/data/structured_form_search_preset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const UmiApplicationExperienceField fields[] = {
    {.field_id = "quantity",
     .label = "Quantity",
     .help_text = "Fictional inventory count",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER}};
static const UmiStructuredFormField rules[] = {
    {"quantity", "count.items", UMI_FORM_TEXT_PLAIN, 1, 0}};
static const UmiApplicationExperienceFormSchema base = {sizeof base,
                                                        UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                        "example.inventory",
                                                        "Inventory example",
                                                        "Local fictional data",
                                                        "example.save",
                                                        fields,
                                                        1};
static const UmiStructuredFormSchema schema = {&base, 1, rules, 1};
int main(void) {
  UmiDataServer *server = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredForm *form = malloc(sizeof *form);
  UmiStructuredFormQuery *query = calloc(1, sizeof *query);
  UmiStructuredFormQueryPage *page = malloc(sizeof *page);
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormSearchPresetRecord *loaded = malloc(sizeof *loaded);
  UmiStatus status =
      form && query && page && preset && loaded ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
  const UmiStructuredFormSchema *schemas[] = {&schema};
  if (status == UMI_STATUS_OK)
    status = umi_data_server_create_memory(&server);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_catalogue_create(schemas, 1, &catalogue);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_init(&schema, form);
  UmiRecordAmendment metadata = {.source = "Fictional inventory",
                                 .actor = "Example user",
                                 .reason = "Demonstrate typed search",
                                 .recorded_at_nanoseconds = 1};
  for (int i = 1; status == UMI_STATUS_OK && i <= 3; ++i) {
    UmiFormAnswer answer = {.state = UMI_FORM_KNOWN, .unit_id = "count.items"};
    status = umi_application_experience_value_integer(&answer.value, i * 10);
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_set(&schema, form, form->revision, "quantity", &answer);
    (void)snprintf(metadata.entity_id, sizeof metadata.entity_id, "inventory.item.%d", i);
    UmiRecordRevision saved;
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_record_save(server, &metadata, &schema, form, 0, &saved);
  }
  if (status == UMI_STATUS_OK) {
    strcpy(query->entity_prefix, "inventory.");
    strcpy(query->form_id, base.form_id);
    query->schema_version = 1;
    query->limit = 1;
    query->filter.count = 1;
    UmiFormFilterTerm *term = &query->filter.terms[0];
    strcpy(term->field_id, "quantity");
    term->operation = UMI_FORM_FILTER_GREATER_EQUAL;
    term->operand.state = UMI_FORM_KNOWN;
    strcpy(term->operand.unit_id, "count.items");
    status = umi_application_experience_value_integer(&term->operand.value, 20);
    if (status == UMI_STATUS_OK) {
      strcpy(preset->title, "Inventory with at least twenty items");
      strcpy(preset->entity_prefix, query->entity_prefix);
      preset->filter = query->filter;
      UmiRecordAmendment definition = {.entity_id = "form.search.inventory",
                                       .source = "Fictional presets",
                                       .actor = "Example user",
                                       .reason = "Save reusable search",
                                       .recorded_at_nanoseconds = 1};
      UmiRecordRevision saved;
      status =
          umi_structured_form_search_preset_save(server, &definition, &schema, preset, 0, &saved);
      if (status == UMI_STATUS_OK)
        status = umi_structured_form_search_preset_open_reviewed(server, &saved, &schema, loaded);
      if (status == UMI_STATUS_OK)
        status = umi_structured_form_query_from_preset(&schema, &loaded->preset, 1, query);
    }
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_query_page(server, catalogue, query, page);
    if (status == UMI_STATUS_OK &&
        (page->count != 1 || page->matching_count != 2 || !page->next_after_entity[0]))
      status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
      printf("First match: %s\n", page->rows[0].amendment.entity_id);
      strcpy(query->after_entity, page->next_after_entity);
      strcpy(query->expected_snapshot, page->snapshot);
      status = umi_structured_form_query_page(server, catalogue, query, page);
      if (status == UMI_STATUS_OK && (page->count != 1 || page->next_after_entity[0]))
        status = UMI_STATUS_INVALID_STATE;
    }
  }
  if (status != UMI_STATUS_OK)
    fprintf(stderr, "Typed search: %s\n", umi_status_text(status));
  umi_structured_form_catalogue_destroy(catalogue);
  umi_data_server_destroy(server);
  free(loaded);
  free(preset);
  free(page);
  free(query);
  free(form);
  return status == UMI_STATUS_OK ? 0 : 1;
}
