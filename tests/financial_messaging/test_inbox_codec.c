/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_inbox_codec.c
 *
 * PURPOSE:
 *   Reject malformed or ambiguous local message records without exposing partial output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../src/financial_messaging/inbox_internal.h"
#include "fixture.h"
int main(int argc, char **argv) {
  static const char *const cases[] = {"roundtrip", "binary-zero",  "truncated",    "extra-field",
                                      "version",   "overflow",     "leading-zero", "unknown-type",
                                      "bad-hex",   "wrong-digest", "no-terminator"};
  int selected = Case(argc, argv, cases, sizeof cases / sizeof cases[0]);
  UmiUfmpInboxRecord r = {0}, parsed;
  r.envelope = Envelope();
  r.payload[0] = 1U;
  r.payload_length = 1U;
  char text[UMI_UFMP_INBOX_TEXT_CAPACITY] = {0};
  OK(UmiUfmpInboxEncode(&r, text));
  if (selected == 0 || selected == 1) {
    if (selected == 1) {
      r.payload[0] = 0U;
      Body(&r.envelope, r.payload, 1U);
      OK(UmiUfmpInboxEncode(&r, text));
    }
    OK(UmiUfmpInboxDecode(text, &parsed));
    CHECK(parsed.payload_length == 1U && parsed.payload[0] == r.payload[0]);
    CHECK(parsed.envelope.causation_id[0] == '\0');
    return 0;
  }
  if (selected == 2)
    *strrchr(text, '\n') = '\0';
  if (selected == 3)
    strcat(text, "\nextra");
  if (selected == 4)
    text[11] = '2';
  if (selected == 5 || selected == 6) {
    char *revision = strchr(text, '\n') + 1;
    char *next = strchr(revision, '\n');
    const char *bad = selected == 5 ? "18446744073709551616" : "01";
    size_t n = strlen(bad);
    memmove(revision + n, next, strlen(next) + 1U);
    memcpy(revision, bad, n);
  }
  if (selected == 7) {
    char *type = strstr(text, "payment.initiate");
    CHECK(type != NULL);
    type[0] = 'x';
  }
  if (selected == 8)
    text[strlen(text) - 1U] = 'G';
  if (selected == 9)
    text[strlen(text) - 1U] = '2';
  if (selected == 10)
    memset(text, 'x', sizeof text);
  memset(&parsed, 0xA5, sizeof parsed);
  UmiUfmpInboxRecord before = parsed;
  CHECK(UmiUfmpInboxDecode(text, &parsed) == UMI_STATUS_PARSE_ERROR);
  CHECK(memcmp(&before, &parsed, sizeof parsed) == 0);
  return 0;
}
