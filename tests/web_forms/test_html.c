/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_forms/test_html.c
 * PURPOSE: Check escaped rendering, identity controls and output refusal boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include <float.h>
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  CHECK(umi_structured_form_init(&f->schema, &f->before) == UMI_STATUS_OK);
  char *out = malloc(UMI_WEB_FORM_HTML_MAX_BYTES);
  CHECK(out != NULL);
  strcpy(out, "unchanged");
  size_t length = 19U, capacity = UMI_WEB_FORM_HTML_MAX_BYTES;
  const char *action = "/forms/review", *token = "test_token";
  UmiStatus expected = UMI_STATUS_OK;
  if (strcmp(name, "render") == 0 || strcmp(name, "empty") == 0) {
  } else if (strcmp(name, "escaping") == 0) {
    f->fields[2].label = "Note <script> & \"quote\"";
    f->before.answers[2] = form_text("</textarea><script>alert(1)</script>");
  } else if (strcmp(name, "readonly") == 0)
    f->fields[2].read_only = 1;
  else if (strcmp(name, "secret") == 0) {
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_SECRET;
    f->before.answers[2] = form_text("not-for-html");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (strcmp(name, "action") == 0) {
    action = "//another.example/submit";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (strcmp(name, "token") == 0) {
    token = "bad token";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (strcmp(name, "capacity") == 0) {
    capacity = 20U;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (strcmp(name, "utf8") == 0) {
    f->fields[2].label = "\xC0\xAF";
    expected = UMI_STATUS_PARSE_ERROR;
  } else if (strcmp(name, "schema") == 0) {
    f->schema.version = 2U;
    expected = UMI_STATUS_INVALID_STATE;
  } else if (strcmp(name, "numbers") == 0)
    f->before.answers[0] = form_mass(DBL_TRUE_MIN);
  else if (strcmp(name, "leading-newline") == 0)
    f->before.answers[2] = form_text("\nsecond line");
  else if (strcmp(name, "carriage-return") == 0) {
    f->before.answers[2] = form_text("first\rsecond");
    expected = UMI_STATUS_INVALID_STATE;
  } else
    CHECK(0);
  CHECK(umi_web_structured_form_render(&f->schema, &f->before, action, token, out, capacity,
                                       &length) == expected);
  if (expected != UMI_STATUS_OK)
    CHECK(strcmp(out, "unchanged") == 0 && length == 19U);
  else {
    CHECK(length == strlen(out) && strstr(out, "method=\"post\"") != NULL);
    CHECK(strstr(out, "name=\"version\" value=\"1\"") != NULL);
    CHECK(strstr(out, "name=\"revision\" value=\"1\"") != NULL);
    CHECK(strstr(out, "name=\"csrf\" value=\"test_token\"") != NULL);
    CHECK(strstr(out, "name=\"intent\" value=\"submit\"") != NULL);
    if (strcmp(name, "escaping") == 0) {
      CHECK(strstr(out, "<script>") == NULL && strstr(out, "&lt;script&gt;") != NULL);
      CHECK(strstr(out, "&lt;/textarea&gt;") != NULL && strstr(out, "&quot;quote&quot;") != NULL);
    }
    if (strcmp(name, "readonly") == 0)
      CHECK(strstr(out, "<fieldset disabled>") != NULL);
    if (strcmp(name, "numbers") == 0)
      CHECK(strstr(out, "e-324") != NULL);
    if (strcmp(name, "leading-newline") == 0)
      CHECK(strstr(out, "rows=\"2\">\n&#10;second line</textarea>") != NULL);
  }
  free(out);
  free(f);
  return 0;
}
