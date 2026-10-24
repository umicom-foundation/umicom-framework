/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_forms/test_body.c
 * PURPOSE: Cover strict complete-body decoding and unchanged outputs on refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  UmiWebFormBody *out = malloc(sizeof(*out)), *before = malloc(sizeof(*before));
  CHECK(out != NULL && before != NULL);
  memset(out, 0xA5, sizeof(*out));
  *before = *out;
  const char *input = "a=one&b=two";
  size_t length = strlen(input);
  UmiStatus expected = UMI_STATUS_OK;
  char *owned = NULL;
  if (strcmp(name, "empty") == 0) {
    input = NULL;
    length = 0U;
  } else if (strcmp(name, "valid") == 0) {
  } else if (strcmp(name, "escaped") == 0) {
    input = "a=one+two%2Bthree%26four";
    length = strlen(input);
  } else if (strcmp(name, "duplicate") == 0) {
    input = "a=one&a=two";
    length = strlen(input);
    expected = UMI_STATUS_ALREADY_EXISTS;
  } else if (strcmp(name, "encoded-duplicate") == 0) {
    input = "a=one&%61=two";
    length = strlen(input);
    expected = UMI_STATUS_ALREADY_EXISTS;
  } else if (strcmp(name, "malformed") == 0) {
    const char *bad[] = {"a=%", "a=%2", "a=%GG", "a"};
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
      CHECK(umi_web_form_body_parse(bad[i], strlen(bad[i]), out) == UMI_STATUS_PARSE_ERROR &&
            memcmp(out, before, sizeof(*out)) == 0);
  } else if (strcmp(name, "nul") == 0) {
    input = "a=one%00two";
    length = strlen(input);
    expected = UMI_STATUS_PARSE_ERROR;
  } else if (strcmp(name, "unicode") == 0) {
    CHECK(umi_web_form_body_parse("a=%F0%9F%8C%8D", 14U, out) == UMI_STATUS_OK);
    CHECK(strcmp(umi_web_form_body_get(out, "a"), "\xF0\x9F\x8C\x8D") == 0);
    *out = *before;
    input = "a=%ED%A0%80";
    length = strlen(input);
    expected = UMI_STATUS_PARSE_ERROR;
  } else if (strcmp(name, "capacity") == 0) {
    owned = malloc(UMI_WEB_FORM_VALUE_CAPACITY + 3U);
    CHECK(owned != NULL);
    strcpy(owned, "a=");
    memset(owned + 2, 'x', UMI_WEB_FORM_VALUE_CAPACITY);
    owned[UMI_WEB_FORM_VALUE_CAPACITY + 2U] = '\0';
    input = owned;
    length = strlen(input);
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (strcmp(name, "names") == 0) {
    input = "=value";
    length = strlen(input);
    expected = UMI_STATUS_PARSE_ERROR;
  } else if (strcmp(name, "segments") == 0) {
    const char *bad[] = {"&", "a=1&", "a=1&&b=2", "&a=1"};
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
      CHECK(umi_web_form_body_parse(bad[i], strlen(bad[i]), out) == UMI_STATUS_PARSE_ERROR &&
            memcmp(out, before, sizeof(*out)) == 0);
  } else if (strcmp(name, "equals") == 0) {
    input = "a=one=two";
    length = strlen(input);
  } else if (strcmp(name, "source-overlap") == 0) {
    strcpy(out->controls[0].name, "a=one&b=two");
    input = out->controls[0].name;
    length = strlen(input);
  } else if (strcmp(name, "bounds") == 0) {
    CHECK(umi_web_form_body_parse("a", UMI_WEB_FORM_BODY_MAX_BYTES + 1U, out) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          memcmp(out, before, sizeof(*out)) == 0);
    const char exact[] = {'a', '=', '1'};
    CHECK(umi_web_form_body_parse(exact, sizeof(exact), out) == UMI_STATUS_OK &&
          strcmp(umi_web_form_body_get(out, "a"), "1") == 0);
    *out = *before;
  } else
    CHECK(0);
  CHECK(umi_web_form_body_parse(input, length, out) == expected);
  if (expected != UMI_STATUS_OK)
    CHECK(memcmp(out, before, sizeof(*out)) == 0);
  else if (strcmp(name, "empty") == 0)
    CHECK(out->count == 0U);
  else if (strcmp(name, "escaped") == 0)
    CHECK(strcmp(umi_web_form_body_get(out, "a"), "one two+three&four") == 0);
  else if (strcmp(name, "equals") == 0)
    CHECK(strcmp(umi_web_form_body_get(out, "a"), "one=two") == 0);
  else
    CHECK(out->count == 2U);
  free(owned);
  free(before);
  free(out);
  return 0;
}
