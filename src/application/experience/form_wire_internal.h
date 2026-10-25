/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/form_wire_internal.h
 *
 * PURPOSE:
 *   Private bounded framing shared by filter and saved-search codecs.
 *   Callers validate models and stage output; helpers never allocate or publish.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private bounded framing shared by filter and saved-search codecs.
 * Callers validate models and stage output; helpers never allocate or publish. */
#ifndef UMICOM_FORM_WIRE_INTERNAL_H
#define UMICOM_FORM_WIRE_INTERNAL_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
typedef struct UmiFormWireWriter {
  unsigned char *bytes;
  size_t at, capacity;
  UmiStatus status;
} UmiFormWireWriter;
typedef struct UmiFormWireReader {
  const unsigned char *bytes;
  size_t at, length;
  UmiStatus status;
} UmiFormWireReader;
static inline void umi_form_wire_put(UmiFormWireWriter *w, const void *bytes, size_t count) {
  if (w->status != UMI_STATUS_OK)
    return;
  if (count > w->capacity - w->at) {
    w->status = UMI_STATUS_CAPACITY_EXCEEDED;
    return;
  }
  if (count)
    memcpy(w->bytes + w->at, bytes, count);
  w->at += count;
}
static inline void umi_form_wire_number(UmiFormWireWriter *w, uint64_t value, size_t width) {
  unsigned char bytes[8];
  if (!width || width > sizeof bytes) {
    w->status = UMI_STATUS_INVALID_ARGUMENT;
    return;
  }
  for (size_t i = 0; i < width; ++i)
    bytes[width - 1 - i] = (unsigned char)(value >> (8 * i));
  umi_form_wire_put(w, bytes, width);
}
static inline void umi_form_wire_text(UmiFormWireWriter *w, const char *text) {
  size_t length = strlen(text);
  if (length > UINT16_MAX) {
    w->status = UMI_STATUS_CAPACITY_EXCEEDED;
    return;
  }
  umi_form_wire_number(w, (uint64_t)length, 2);
  umi_form_wire_put(w, text, length);
}
static inline uint64_t umi_form_wire_take_number(UmiFormWireReader *r, size_t width) {
  if (r->status != UMI_STATUS_OK)
    return 0;
  if (!width || width > 8 || width > r->length - r->at) {
    r->status = UMI_STATUS_PARSE_ERROR;
    return 0;
  }
  uint64_t value = 0;
  for (size_t i = 0; i < width; ++i)
    value = (value << 8) | r->bytes[r->at++];
  return value;
}
static inline void umi_form_wire_take_text(UmiFormWireReader *r, char *out, size_t capacity) {
  size_t length = (size_t)umi_form_wire_take_number(r, 2);
  if (r->status != UMI_STATUS_OK)
    return;
  if (length >= capacity || length > r->length - r->at || memchr(r->bytes + r->at, 0, length)) {
    r->status = UMI_STATUS_PARSE_ERROR;
    return;
  }
  if (length)
    memcpy(out, r->bytes + r->at, length);
  out[length] = 0;
  r->at += length;
}
#endif
