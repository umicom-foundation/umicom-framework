/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/form_body.c
 * PURPOSE: Stage complete decoded submissions before publishing caller-owned controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/form_body.h"
#include "umicom/document/text_encoding.h"
#include <stdlib.h>
#include <string.h>
static int hex(unsigned char c) {
  if (c >= '0' && c <= '9')
    return (int)(c - '0');
  if (c >= 'a' && c <= 'f')
    return (int)(c - 'a') + 10;
  if (c >= 'A' && c <= 'F')
    return (int)(c - 'A') + 10;
  return -1;
}
static UmiStatus decode(const unsigned char *bytes, size_t length, char *out, size_t capacity) {
  size_t used = 0U;
  for (size_t i = 0U; i < length; ++i) {
    unsigned char value = bytes[i];
    if (value == '%') {
      if (length - i < 3U)
        return UMI_STATUS_PARSE_ERROR;
      int high = hex(bytes[i + 1U]), low = hex(bytes[i + 2U]);
      if (high < 0 || low < 0)
        return UMI_STATUS_PARSE_ERROR;
      value = (unsigned char)((unsigned)high * 16U + (unsigned)low);
      i += 2U;
    } else if (value == '+')
      value = ' ';
    if (value == 0U)
      return UMI_STATUS_PARSE_ERROR;
    if (used + 1U >= capacity)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    out[used++] = (char)value;
  }
  out[used] = '\0';
  return umi_document_utf8_validate((const unsigned char *)out, used, NULL)
             ? UMI_STATUS_OK
             : UMI_STATUS_PARSE_ERROR;
}
UmiStatus umi_web_form_body_parse(const void *bytes, size_t length, UmiWebFormBody *out) {
  if (out == NULL || (bytes == NULL && length != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length > UMI_WEB_FORM_BODY_MAX_BYTES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiWebFormBody *candidate = calloc(1U, sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  const unsigned char *input = bytes;
  UmiStatus status = UMI_STATUS_OK;
  size_t at = 0U;
  while (at < length) {
    if (candidate->count == UMI_WEB_FORM_CONTROL_CAPACITY) {
      status = UMI_STATUS_CAPACITY_EXCEEDED;
      break;
    }
    size_t end = at, equals = SIZE_MAX;
    while (end < length && input[end] != '&') {
      if (input[end] == '=' && equals == SIZE_MAX)
        equals = end;
      ++end;
    }
    if (equals == SIZE_MAX || equals == at || (end < length && end + 1U == length)) {
      status = UMI_STATUS_PARSE_ERROR;
      break;
    }
    UmiWebFormControl *control = &candidate->controls[candidate->count];
    status = decode(input + at, equals - at, control->name, sizeof(control->name));
    if (status == UMI_STATUS_OK)
      status =
          decode(input + equals + 1U, end - equals - 1U, control->value, sizeof(control->value));
    if (status != UMI_STATUS_OK)
      break;
    for (size_t i = 0U; i < candidate->count; ++i)
      if (strcmp(candidate->controls[i].name, control->name) == 0) {
        status = UMI_STATUS_ALREADY_EXISTS;
        break;
      }
    if (status != UMI_STATUS_OK)
      break;
    ++candidate->count;
    at = end == length ? end : end + 1U;
  }
  if (status == UMI_STATUS_OK)
    *out = *candidate;
  free(candidate);
  return status;
}
const char *umi_web_form_body_get(const UmiWebFormBody *body, const char *name) {
  if (body == NULL || name == NULL || body->count > UMI_WEB_FORM_CONTROL_CAPACITY)
    return NULL;
  const char *result = NULL;
  for (size_t i = 0U; i < body->count; ++i) {
    if (memchr(body->controls[i].name, '\0', sizeof(body->controls[i].name)) == NULL ||
        memchr(body->controls[i].value, '\0', sizeof(body->controls[i].value)) == NULL)
      return NULL;
    if (strcmp(body->controls[i].name, name) == 0) {
      if (result != NULL)
        return NULL;
      result = body->controls[i].value;
    }
  }
  return result;
}
