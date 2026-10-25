/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_simulation.c
 *
 * PURPOSE:
 *   Check the synthetic scenario and bounded presentation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  static const char *const names[] = {"scenario", "format", "short", "arguments"};
  int c = Case(argc, argv, names, sizeof names / sizeof names[0]);
  if (c == 0) {
    UmiUfmpSimulationReport r;
    OK(UmiUfmpSimulationRun(&r));
    CHECK(r.delivery_attempts == 5U && r.accepted_operations == 1U && r.replay_matches == 4U);
    CHECK(r.changed_payload_refused && r.receipt_left_settlement_pending &&
          r.credit_left_settlement_pending && r.final_state == UMI_UFMP_PROGRESS_COMPLETED);
  } else if (c == 1) {
    size_t required = 0U;
    OK(UmiUfmpSimulationFormat(NULL, 0U, &required));
    CHECK(required > 0U && required < 1024U);
    char text[1024];
    OK(UmiUfmpSimulationFormat(text, sizeof text, &required));
    CHECK(strlen(text) == required && strstr(text, "TEST ONLY") != NULL);
    CHECK(strstr(text, "No network, signatures, durable commits or real funds.") != NULL);
  } else if (c == 2) {
    char out[4] = {'k', 'e', 'e', 'p'};
    size_t required = 0U;
    CHECK(UmiUfmpSimulationFormat(out, sizeof out, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(out, "keep", sizeof out) == 0 && required > sizeof out);
  } else {
    size_t required = 123U;
    CHECK(UmiUfmpSimulationRun(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUfmpSimulationFormat(NULL, 1U, &required) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(required == 123U);
    CHECK(UmiUfmpSimulationFormat(NULL, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  }
  return 0;
}
