/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_migration.c
 * PURPOSE: Preserve answer meaning across reviewed schema-version changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/structured_form_migration.h"
#include "structured_form_internal.h"
#include <stdlib.h>
#include <string.h>
UmiStatus umi_structured_form_migrate(const UmiStructuredFormSchema *source_schema,
                                      const UmiStructuredForm *source,
                                      const UmiStructuredFormSchema *target_schema,
                                      UmiStructuredForm *out) {
  UmiStatus status = umi_structured_form_draft_check(source_schema, source);
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_structured_form_schema_validate(target_schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (strcmp(source_schema->base->form_id, target_schema->base->form_id) != 0 ||
      target_schema->version <= source_schema->version)
    return UMI_STATUS_INVALID_STATE;
  if (source->revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStructuredForm *candidate = malloc(sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_structured_form_init(target_schema, candidate);
  if (status != UMI_STATUS_OK) {
    free(candidate);
    return status;
  }
  for (size_t i = 0U; i < source->count; ++i) {
    size_t target = umi_application_experience_form_schema_index(
        target_schema->base, source_schema->base->fields[i].field_id);
    if (target == SIZE_MAX) {
      if (source->answers[i].state != UMI_FORM_UNANSWERED) {
        status = UMI_STATUS_INVALID_STATE;
        break;
      }
      continue;
    }
    /* Even equal numeric payloads cannot establish equivalent units. The
     * caller must review a separate conversion workflow for such changes. */
    if (source_schema->base->fields[i].value_type !=
            target_schema->base->fields[target].value_type ||
        strcmp(source_schema->rules[i].unit_id, target_schema->rules[target].unit_id) != 0 ||
        source_schema->rules[i].text_format != target_schema->rules[target].text_format) {
      status = UMI_STATUS_INVALID_STATE;
      break;
    }
    status = umi_structured_form_answer_validate(target_schema, target, &source->answers[i], 0);
    if (status != UMI_STATUS_OK)
      break;
    candidate->answers[target] = source->answers[i];
  }
  if (status == UMI_STATUS_OK) {
    candidate->revision = source->revision + 1U;
    candidate->saved_revision = 0U;
    *out = *candidate;
  }
  free(candidate);
  return status;
}
