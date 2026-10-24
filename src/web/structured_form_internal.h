/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/structured_form_internal.h
 * PURPOSE: Share private browser-form bounds and control naming.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_STRUCTURED_FORM_INTERNAL_H
#define UMICOM_WEB_STRUCTURED_FORM_INTERNAL_H
#include "umicom/application/experience/structured_form_text.h"
#include "umicom/base/ascii_number.h"
#include "umicom/document/text_encoding.h"
#include "umicom/web/structured_form.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static inline int umi_form_web_text(const char *text, size_t maximum) {
  if (text == NULL)
    return 0;
  const char *end = memchr(text, '\0', maximum);
  return end != NULL &&
         umi_document_utf8_validate((const unsigned char *)text, (size_t)(end - text), NULL);
}
static inline int umi_form_web_token(const char *token) {
  if (token == NULL)
    return 0;
  for (size_t i = 0U; i < UMI_WEB_FORM_TOKEN_CAPACITY; ++i) {
    unsigned char c = (unsigned char)token[i];
    if (c == 0U)
      return i != 0U;
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' ||
          c == '_'))
      return 0;
  }
  return 0;
}
static inline UmiStatus umi_form_web_control_name(char *out, size_t capacity, const char *prefix,
                                                  const char *id) {
  int n = snprintf(out, capacity, "%s.%s", prefix, id);
  return n < 0 || (size_t)n >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
#endif
