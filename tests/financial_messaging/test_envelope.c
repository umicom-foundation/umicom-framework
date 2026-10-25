/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_envelope.c
 *
 * PURPOSE:
 *   Check bounded identities, expiry boundaries, payload integrity and unchanged input.
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
  static const char *const names[] = {
      "valid",         "revision",     "expired",       "expiry-boundary", "future",
      "skew-boundary", "lifetime",     "overflow-time", "payload-limit",   "missing-key",
      "sender",        "unterminated", "unknown-type",  "schema",          "body",
      "tampered",      "empty-body",   "null",          "policy",          "optional-fields"};
  int c = Case(argc, argv, names, sizeof names / sizeof names[0]);
  UmiUfmpEnvelope e = Envelope();
  UmiUfmpEnvelopePolicy p = Policy();
  UmiStatus expected = UMI_STATUS_OK;
  switch (c) {
  case 1:
    e.revision = 2U;
    expected = UMI_STATUS_NOT_IMPLEMENTED;
    break;
  case 2:
    p.now_ms = 2001U;
    expected = UMI_STATUS_TIMEOUT;
    break;
  case 3:
    p.now_ms = 2000U;
    expected = UMI_STATUS_TIMEOUT;
    break;
  case 4:
    e.created_ms = 1151U;
    e.expires_ms = 2151U;
    expected = UMI_STATUS_INVALID_STATE;
    break;
  case 5:
    e.created_ms = 1150U;
    e.expires_ms = 2150U;
    break;
  case 6:
    ++e.expires_ms;
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 7:
    e.created_ms = UINT64_MAX - 20U;
    e.expires_ms = UINT64_MAX;
    p.now_ms = UINT64_MAX - 25U;
    p.future_skew_ms = 5U;
    break;
  case 8:
    e.payload_bytes = 4097U;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
    break;
  case 9:
    e.idempotency_key[0] = '\0';
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 10:
    strcpy(e.sender, "Bank B");
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 11:
    memset(e.recipient, 'a', sizeof e.recipient);
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 12:
    e.type = (UmiUfmpMessageType)999;
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 13:
    memset(e.schema, 'a', sizeof e.schema);
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 14:
    OK(UmiUfmpEnvelopeCheckPayload(&e, "\x01", 1U));
    CHECK(UmiUfmpEnvelopeCheckPayload(&e, "\x01", 0U) == UMI_STATUS_INVALID_ARGUMENT);
    break;
  case 15:
    CHECK(UmiUfmpEnvelopeCheckPayload(&e, "\x02", 1U) == UMI_STATUS_PARSE_ERROR);
    break;
  case 16:
    Body(&e, NULL, 0U);
    OK(UmiUfmpEnvelopeCheckPayload(&e, NULL, 0U));
    break;
  case 17:
    CHECK(UmiUfmpEnvelopeValidate(NULL, &p) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUfmpEnvelopeValidate(&e, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUfmpEnvelopeCheckPayload(&e, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
    break;
  case 18:
    p.maximum_lifetime_ms = 0U;
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 19:
    e.type = UMI_UFMP_NETWORK_HEARTBEAT;
    e.idempotency_key[0] = '\0';
    break;
  default:
    break;
  }
  UmiUfmpEnvelope saved = e;
  CHECK(UmiUfmpEnvelopeValidate(&e, &p) == expected);
  CHECK(memcmp(&saved, &e, sizeof e) == 0);
  return 0;
}
