/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_outbox_codec.c
 *
 * PURPOSE:
 *   Reject malformed state and impossible delivery combinations without changing outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/financial_messaging/outbox_internal.h"
int main(int argc, char **argv) {
  const char *cases[] = {"roundtrip", "version", "truncated", "extra", "leading-zero",
    "overflow", "unknown-state", "invalid-retry", "impossible-attempts", "terminal-deadline",
    "missing-terminator", "revision-overflow"};
  (void)Case(argc, argv, cases, sizeof cases / sizeof cases[0]);
  UmiUfmpOutboxRecord r = {0};
  r.message.envelope = Envelope(); r.message.payload[0] = 1U; r.message.payload_length = 1U;
  r.retry = (UmiUfmpRetryPolicy){3U, 10U, 40U}; r.revision = 1U;
  r.state = UMI_UFMP_OUTBOX_READY; r.updated_ms = 1100U; r.next_attempt_ms = 1100U;
  UmiUfmpOutboxRecord before = r;
  char text[256] = {0};
  OK(UmiUfmpOutboxStateEncode(&r, text));
  const char *name = argv[1];
  if (strcmp(name, "roundtrip") == 0) {
    r.revision = 99U; OK(UmiUfmpOutboxStateDecode(text, &r));
    CHECK(r.revision == 1U && r.next_attempt_ms == 1100U && r.retry.maximum_attempts == 3U);
  } else if (strcmp(name, "revision-overflow") == 0) {
    r.revision = UINT64_MAX; before = r;
    CHECK(UmiUfmpOutboxApply(&r, 1100U, UMI_UFMP_OUTBOX_BEGIN) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&r, &before, sizeof r) == 0);
  } else {
    if (strcmp(name, "version") == 0) text[12] = '2';
    else if (strcmp(name, "truncated") == 0) text[20] = '\0';
    else if (strcmp(name, "extra") == 0) strcat(text, "\n0");
    else if (strcmp(name, "missing-terminator") == 0) memset(text, '1', sizeof text);
    else {
      const char *revision = strcmp(name, "leading-zero") == 0 ? "01" :
        strcmp(name, "overflow") == 0 ? "18446744073709551616" : "1";
      unsigned state = strcmp(name, "unknown-state") == 0 ? 9U :
        strcmp(name, "terminal-deadline") == 0 ? 3U : 1U;
      unsigned attempts = strcmp(name, "impossible-attempts") == 0 ? 4U :
        strcmp(name, "terminal-deadline") == 0 ? 1U : 0U;
      unsigned limit = strcmp(name, "invalid-retry") == 0 ? 0U : 3U;
      snprintf(text, sizeof text, "UFMP-OUTBOX-1\n%s\n%u\n%u\n%u\n10\n40\n1100\n1100",
        revision, state, attempts, limit);
    }
    CHECK(UmiUfmpOutboxStateDecode(text, &r) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&r, &before, sizeof r) == 0);
  }
  return 0;
}
