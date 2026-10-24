/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_forms/fixture.h
 * PURPOSE: Share complete fictional browser submissions and bounded mutation helpers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_FORM_FIXTURE_H
#define UMICOM_WEB_FORM_FIXTURE_H
#include "../application_experience/structured_form_fixture.h"
#include "umicom/web/structured_form.h"
#define WEB_BODY_CAPACITY 8192U
static inline void web_body(const FormFixture *f, char *out) {
  int n = snprintf(out, WEB_BODY_CAPACITY,
                   "form=%s&version=%llu&revision=%llu&csrf=test_token&intent=draft",
                   f->before.form_id, (unsigned long long)f->before.schema_version,
                   (unsigned long long)f->before.revision);
  if (n < 0 || (size_t)n >= WEB_BODY_CAPACITY)
    abort();
  for (size_t i = 0U; i < 3U; ++i) {
    if (f->fields[i].read_only)
      continue;
    size_t used = strlen(out);
    const char *value = i == 0U ? "3.5" : i == 1U ? "2024-02-29T12%3A00%3A00Z" : "A+note";
    n = snprintf(out + used, WEB_BODY_CAPACITY - used,
                 "&state.%s=1&value.%s=%s&unit.%s=%s&reason.%s=", f->rules[i].field_id,
                 f->rules[i].field_id, value, f->rules[i].field_id, f->rules[i].unit_id,
                 f->rules[i].field_id);
    if (n < 0 || (size_t)n >= WEB_BODY_CAPACITY - used)
      abort();
  }
}
static inline void web_replace(char *body, const char *from, const char *to) {
  char copy[WEB_BODY_CAPACITY];
  const char *at = strstr(body, from);
  if (at == NULL)
    abort();
  size_t prefix = (size_t)(at - body), tail = strlen(at + strlen(from)), value = strlen(to);
  if (prefix + value + tail >= sizeof(copy))
    abort();
  memcpy(copy, body, prefix);
  memcpy(copy + prefix, to, value);
  memcpy(copy + prefix + value, at + strlen(from), tail + 1U);
  strcpy(body, copy);
}
#endif
