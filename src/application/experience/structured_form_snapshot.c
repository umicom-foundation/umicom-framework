/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_snapshot.c
 * PURPOSE: Encode bounded semantic snapshots without native padding or locale.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/structured_form_snapshot.h"
#include "structured_form_internal.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>
typedef struct Writer {
  unsigned char *bytes;
  size_t at, capacity;
  UmiStatus status;
} Writer;
typedef struct Reader {
  const unsigned char *bytes;
  size_t at, length;
  UmiStatus status;
} Reader;
static void put(Writer *w, const void *bytes, size_t count) {
  if (w->status != UMI_STATUS_OK)
    return;
  if (count > w->capacity - w->at) {
    w->status = UMI_STATUS_CAPACITY_EXCEEDED;
    return;
  }
  memcpy(w->bytes + w->at, bytes, count);
  w->at += count;
}
static void number(Writer *w, uint64_t value, size_t width) {
  unsigned char bytes[8];
  for (size_t i = 0U; i < width; ++i)
    bytes[width - 1U - i] = (unsigned char)(value >> (8U * i));
  put(w, bytes, width);
}
static void string(Writer *w, const char *text) {
  size_t length = strlen(text);
  number(w, (uint64_t)length, 2U);
  put(w, text, length);
}
static uint64_t take_number(Reader *r, size_t width) {
  if (r->status != UMI_STATUS_OK)
    return 0U;
  if (width > r->length - r->at) {
    r->status = UMI_STATUS_PARSE_ERROR;
    return 0U;
  }
  uint64_t value = 0U;
  for (size_t i = 0U; i < width; ++i)
    value = (value << 8U) | r->bytes[r->at++];
  return value;
}
static void take_string(Reader *r, char *out, size_t capacity) {
  size_t length = (size_t)take_number(r, 2U);
  if (r->status != UMI_STATUS_OK)
    return;
  if (length >= capacity || length > r->length - r->at ||
      memchr(r->bytes + r->at, '\0', length) != NULL) {
    r->status = UMI_STATUS_PARSE_ERROR;
    return;
  }
  memcpy(out, r->bytes + r->at, length);
  out[length] = '\0';
  r->at += length;
}
static int decimal_supported(void) {
  if (sizeof(double) != sizeof(uint64_t) || DBL_MANT_DIG != 53 || DBL_MAX_EXP != 1024 ||
      FLT_RADIX != 2)
    return 0;
  double one = 1.0;
  uint64_t bits = 0U;
  memcpy(&bits, &one, sizeof(bits));
  return bits == UINT64_C(0x3ff0000000000000);
}
UmiStatus umi_structured_form_snapshot_encode(const UmiStructuredFormSchema *schema,
                                              const UmiStructuredForm *form, void *out,
                                              size_t capacity, size_t *out_length) {
  UmiStatus status = umi_structured_form_draft_check(schema, form);
  if (status != UMI_STATUS_OK)
    return status;
  if (out == NULL || out_length == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  unsigned char *staged = malloc(UMI_FORM_SNAPSHOT_MAX_BYTES);
  if (staged == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  Writer w = {staged, 0U, UMI_FORM_SNAPSHOT_MAX_BYTES, UMI_STATUS_OK};
  put(&w, "UMIFRM1\n", 8U);
  string(&w, form->form_id);
  number(&w, form->schema_version, 8U);
  number(&w, form->revision, 8U);
  number(&w, form->saved_revision, 8U);
  number(&w, (uint64_t)form->count, 2U);
  for (size_t i = 0U; i < form->count && w.status == UMI_STATUS_OK; ++i) {
    const UmiFormAnswer *a = &form->answers[i];
    string(&w, schema->base->fields[i].field_id);
    number(&w, (uint64_t)a->state, 1U);
    number(&w, (uint64_t)a->value.type, 1U);
    string(&w, a->unit_id);
    string(&w, a->reason);
    switch (a->value.type) {
    case UMI_APPLICATION_EXPERIENCE_VALUE_NONE:
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
      number(&w, (uint64_t)a->value.boolean_value, 1U);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER:
      number(&w, (uint64_t)a->value.integer_value, 8U);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL: {
      if (!decimal_supported()) {
        w.status = UMI_STATUS_NOT_IMPLEMENTED;
        break;
      }
      uint64_t bits;
      memcpy(&bits, &a->value.decimal_value, sizeof(bits));
      number(&w, bits, 8U);
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
      string(&w, a->value.text_value);
      break;
    default:
      w.status = UMI_STATUS_INVALID_STATE;
      break;
    }
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
UmiStatus umi_structured_form_snapshot_decode(const UmiStructuredFormSchema *schema,
                                              const void *bytes, size_t length,
                                              UmiStructuredForm *out) {
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (bytes == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length < 8U || length > UMI_FORM_SNAPSHOT_MAX_BYTES || memcmp(bytes, "UMIFRM1\n", 8U) != 0)
    return UMI_STATUS_PARSE_ERROR;
  UmiStructuredForm *candidate = calloc(1U, sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  Reader r = {bytes, 8U, length, UMI_STATUS_OK};
  take_string(&r, candidate->form_id, sizeof(candidate->form_id));
  candidate->schema_version = take_number(&r, 8U);
  candidate->revision = take_number(&r, 8U);
  candidate->saved_revision = take_number(&r, 8U);
  candidate->count = (size_t)take_number(&r, 2U);
  if (r.status == UMI_STATUS_OK)
    r.status = umi_structured_form_identity_check(schema, candidate);
  for (size_t i = 0U; i < candidate->count && r.status == UMI_STATUS_OK; ++i) {
    char field[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
    take_string(&r, field, sizeof(field));
    if (r.status != UMI_STATUS_OK)
      break;
    if (strcmp(field, schema->base->fields[i].field_id) != 0) {
      r.status = UMI_STATUS_INVALID_STATE;
      break;
    }
    UmiFormAnswer *a = &candidate->answers[i];
    uint64_t state = take_number(&r, 1U), type = take_number(&r, 1U);
    if (state > (uint64_t)UMI_FORM_NOT_APPLICABLE ||
        type > (uint64_t)UMI_APPLICATION_EXPERIENCE_VALUE_TEXT) {
      r.status = UMI_STATUS_PARSE_ERROR;
      break;
    }
    a->state = (UmiFormAnswerState)state;
    a->value.type = (UmiApplicationExperienceValueType)type;
    take_string(&r, a->unit_id, sizeof(a->unit_id));
    take_string(&r, a->reason, sizeof(a->reason));
    switch (a->value.type) {
    case UMI_APPLICATION_EXPERIENCE_VALUE_NONE:
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN: {
      uint64_t boolean = take_number(&r, 1U);
      if (boolean > 1U)
        r.status = UMI_STATUS_PARSE_ERROR;
      else
        a->value.boolean_value = (int)boolean;
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER: {
      uint64_t integer = take_number(&r, 8U);
      a->value.integer_value =
          integer <= (uint64_t)INT64_MAX ? (int64_t)integer : -1 - (int64_t)(UINT64_MAX - integer);
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL: {
      if (!decimal_supported()) {
        r.status = UMI_STATUS_NOT_IMPLEMENTED;
        break;
      }
      uint64_t bits = take_number(&r, 8U);
      memcpy(&a->value.decimal_value, &bits, sizeof(bits));
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
      take_string(&r, a->value.text_value, sizeof(a->value.text_value));
      break;
    default:
      r.status = UMI_STATUS_PARSE_ERROR;
      break;
    }
  }
  status = r.status;
  if (status == UMI_STATUS_OK && r.at != r.length)
    status = UMI_STATUS_PARSE_ERROR;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_draft_check(schema, candidate);
  if (status == UMI_STATUS_OK)
    *out = *candidate;
  free(candidate);
  return status;
}
