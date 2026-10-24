/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Canonical text operands avoid binary floating-point and locale assumptions. */
#include "umicom/application/experience/structured_form_filter_snapshot.h"
#include "form_wire_internal.h"
#include <stdlib.h>
UmiStatus umi_structured_form_filter_snapshot_encode(const UmiStructuredFormSchema *schema,
                                                     const UmiStructuredFormFilter *filter,
                                                     void *out, size_t capacity,
                                                     size_t *out_length) {
  if (!out || !out_length)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_filter_validate(schema, filter);
  if (status != UMI_STATUS_OK)
    return status;
  unsigned char *staged = malloc(UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES);
  if (!staged)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiFormWireWriter w = {staged, 0, UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES, UMI_STATUS_OK};
  umi_form_wire_put(&w, "UMIFLT1\n", 8);
  umi_form_wire_text(&w, schema->base->form_id);
  umi_form_wire_number(&w, schema->version, 8);
  umi_form_wire_number(&w, filter->count, 1);
  for (size_t i = 0; i < filter->count && w.status == UMI_STATUS_OK; ++i) {
    UmiFormFilterTextTerm text;
    w.status = umi_structured_form_filter_term_format(schema, &filter->terms[i], &text);
    if (w.status != UMI_STATUS_OK)
      break;
    size_t index = umi_application_experience_form_schema_index(schema->base, text.field_id);
    umi_form_wire_text(&w, text.field_id);
    umi_form_wire_number(&w, (uint64_t)text.operation, 1);
    umi_form_wire_number(&w, (uint64_t)text.state, 1);
    umi_form_wire_number(&w, (uint64_t)schema->base->fields[index].value_type, 1);
    umi_form_wire_number(&w, (uint64_t)schema->rules[index].text_format, 1);
    umi_form_wire_text(&w, schema->rules[index].unit_id);
    umi_form_wire_text(&w, text.value);
  }
  status = w.status;
  if (status == UMI_STATUS_OK && w.at > capacity)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK) {
    memcpy(out, staged, w.at);
    *out_length = w.at;
  }
  free(staged);
  return status;
}
UmiStatus umi_structured_form_filter_snapshot_decode(const UmiStructuredFormSchema *schema,
                                                     const void *bytes, size_t length,
                                                     UmiStructuredFormFilter *out) {
  if (!bytes || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (length < 8 || length > UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES || memcmp(bytes, "UMIFLT1\n", 8))
    return UMI_STATUS_PARSE_ERROR;
  UmiStructuredFormFilter *candidate = calloc(1, sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiFormWireReader r = {bytes, 8, length, UMI_STATUS_OK};
  char id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  umi_form_wire_take_text(&r, id, sizeof id);
  uint64_t version = umi_form_wire_take_number(&r, 8);
  candidate->count = (size_t)umi_form_wire_take_number(&r, 1);
  if (r.status == UMI_STATUS_OK &&
      (strcmp(id, schema->base->form_id) || version != schema->version))
    r.status = UMI_STATUS_INVALID_STATE;
  if (r.status == UMI_STATUS_OK && candidate->count > UMI_FORM_FILTER_CAPACITY)
    r.status = UMI_STATUS_CAPACITY_EXCEEDED;
  for (size_t i = 0; i < candidate->count && r.status == UMI_STATUS_OK; ++i) {
    UmiFormFilterTextTerm text = {0};
    char unit[UMI_FORM_UNIT_CAPACITY];
    umi_form_wire_take_text(&r, text.field_id, sizeof text.field_id);
    uint64_t operation = umi_form_wire_take_number(&r, 1), state = umi_form_wire_take_number(&r, 1);
    uint64_t type = umi_form_wire_take_number(&r, 1), format = umi_form_wire_take_number(&r, 1);
    umi_form_wire_take_text(&r, unit, sizeof unit);
    umi_form_wire_take_text(&r, text.value, sizeof text.value);
    if (r.status != UMI_STATUS_OK)
      break;
    size_t index = umi_application_experience_form_schema_index(schema->base, text.field_id);
    if (index == SIZE_MAX || type != (uint64_t)schema->base->fields[index].value_type ||
        format != (uint64_t)schema->rules[index].text_format ||
        strcmp(unit, schema->rules[index].unit_id)) {
      r.status = UMI_STATUS_INVALID_STATE;
      break;
    }
    if (operation > UMI_FORM_FILTER_GREATER_EQUAL || state > UMI_FORM_NOT_APPLICABLE) {
      r.status = UMI_STATUS_PARSE_ERROR;
      break;
    }
    text.operation = (UmiFormFilterOperator)operation;
    text.state = (UmiFormAnswerState)state;
    r.status = umi_structured_form_filter_term_parse(schema, &text, &candidate->terms[i]);
    /* Refuse alternate numeric spellings so decoded queries have one emitted
     * representation. Equality of raw bytes is not authentication. */
    UmiFormFilterTextTerm canonical;
    if (r.status == UMI_STATUS_OK)
      r.status = umi_structured_form_filter_term_format(schema, &candidate->terms[i], &canonical);
    if (r.status == UMI_STATUS_OK && strcmp(text.value, canonical.value))
      r.status = UMI_STATUS_PARSE_ERROR;
  }
  if (r.status == UMI_STATUS_OK && r.at != r.length)
    r.status = UMI_STATUS_PARSE_ERROR;
  if (r.status == UMI_STATUS_OK)
    r.status = umi_structured_form_filter_validate(schema, candidate);
  if (r.status == UMI_STATUS_OK)
    *out = *candidate;
  status = r.status;
  free(candidate);
  return status;
}
