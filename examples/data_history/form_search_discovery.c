/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Discover a named inventory query and open exactly the row reviewed. */
#include "umicom/data/structured_form_search_catalogue.h"
#include "umicom/data/structured_form_search_preset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const UmiApplicationExperienceField fields[] = {
    {.field_id = "quantity",
     .label = "Quantity",
     .help_text = "Fictional count",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER}};
static const UmiStructuredFormField rules[] = {
    {"quantity", "count.items", UMI_FORM_TEXT_PLAIN, 0, 0}};
static const UmiApplicationExperienceFormSchema base = {sizeof base,
                                                        UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                        "example.inventory",
                                                        "Inventory",
                                                        "Fictional inventory",
                                                        "example.save",
                                                        fields,
                                                        1};
static const UmiStructuredFormSchema schema = {&base, 1, rules, 1};
int main(void) {
  UmiDataServer *server = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormSearchPage *page = calloc(1, sizeof *page);
  UmiStructuredFormHistoricalSearch *opened = calloc(1, sizeof *opened);
  UmiStatus status = preset && page && opened ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
  const UmiStructuredFormSchema *schemas[] = {&schema};
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_catalogue_create(schemas, 1, &catalogue);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_create_memory(&server);
  if (status == UMI_STATUS_OK) {
    strcpy(preset->title, "Inventory overview");
    strcpy(preset->entity_prefix, "inventory.record.");
    UmiRecordAmendment metadata = {.entity_id = "form.search.inventory",
                                   .source = "SDK example",
                                   .actor = "Example author",
                                   .reason = "Reusable view",
                                   .recorded_at_nanoseconds = 1};
    UmiRecordRevision revision;
    status =
        umi_structured_form_search_preset_save(server, &metadata, &schema, preset, 0, &revision);
  }
  UmiStructuredFormSearchQuery query = {.form_id = "example.inventory",
                                        .schema_version = 1,
                                        .entity_prefix = "inventory.record.",
                                        .text = "overview",
                                        .limit = 8};
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_catalogue_page(server, catalogue, &query, page);
  if (status == UMI_STATUS_OK && page->count != 1)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_reader_open_reviewed(server, catalogue,
                                                             &page->rows[0].record, opened);
  if (status == UMI_STATUS_OK)
    printf("%s: %s, revision %llu\n", opened->resolved.preset.title,
           opened->record.amendment.entity_id, (unsigned long long)opened->record.revision);
  else
    fprintf(stderr, "Search discovery failed: %d\n", (int)status);
  umi_data_server_destroy(server);
  umi_structured_form_catalogue_destroy(catalogue);
  free(preset);
  free(page);
  free(opened);
  return status == UMI_STATUS_OK ? 0 : 1;
}
