/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_replay.c
 *
 * PURPOSE:
 *   Reject changed business operations while recognizing equivalent retry deliveries.
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
      "identical",          "new-delivery",     "changed-payload",
      "changed-recipient",  "changed-type",     "changed-schema",
      "changed-format",     "changed-revision", "changed-sender",
      "new-operation",      "message-conflict", "time-retry",
      "conversation-retry", "corrupt-record",   "invalid-candidate",
      "output-preserved",   "padding",          "domain-separation"};
  int c = Case(argc, argv, names, sizeof names / sizeof names[0]);
  UmiUfmpEnvelope e = Envelope();
  UmiUfmpReplayRecord saved;
  OK(UmiUfmpReplayCapture(&e, &saved));
  UmiUfmpReplayMatch expected = UMI_UFMP_REPLAY_SAME_OPERATION;
  if (c != 0 && c != 10 && c != 16)
    strcpy(e.message_id, "message.2");
  switch (c) {
  case 0:
    expected = UMI_UFMP_REPLAY_SAME_MESSAGE;
    break;
  case 2:
    Body(&e, "\x02", 1U);
    expected = UMI_UFMP_REPLAY_CONFLICT;
    break;
  case 3:
    strcpy(e.recipient, "Bank.C");
    expected = UMI_UFMP_REPLAY_CONFLICT;
    break;
  case 4:
    e.type = UMI_UFMP_PAYMENT_RETURN;
    expected = UMI_UFMP_REPLAY_CONFLICT;
    break;
  case 5:
    strcpy(e.schema, "synthetic.profile.2");
    expected = UMI_UFMP_REPLAY_CONFLICT;
    break;
  case 6:
    e.format = UMI_UFMP_PAYLOAD_ISO20022_XML;
    expected = UMI_UFMP_REPLAY_CONFLICT;
    break;
  case 7:
    ++e.revision;
    expected = UMI_UFMP_REPLAY_CONFLICT;
    break;
  case 8:
    strcpy(e.sender, "Bank.C");
    expected = UMI_UFMP_REPLAY_NO_MATCH;
    break;
  case 9:
    strcpy(e.idempotency_key, "operation.2");
    expected = UMI_UFMP_REPLAY_NO_MATCH;
    break;
  case 10:
    ++e.created_ms;
    expected = UMI_UFMP_REPLAY_CONFLICT;
    break;
  case 11:
    e.created_ms += 10U;
    e.expires_ms += 10U;
    break;
  case 12:
    strcpy(e.conversation_id, "conversation.2");
    strcpy(e.trace_id, "trace.2");
    break;
  case 13:
    memset(saved.sender, 'x', sizeof saved.sender);
    break;
  case 14:
    e.expires_ms = e.created_ms;
    break;
  case 15:
    e.type = (UmiUfmpMessageType)0;
    break;
  case 16:
    e.sender[sizeof e.sender - 1U] = 'x';
    expected = UMI_UFMP_REPLAY_SAME_MESSAGE;
    break;
  case 17:
    CHECK(memcmp(saved.operation_digest, saved.envelope_digest, sizeof saved.operation_digest) !=
          0);
    break;
  default:
    break;
  }
  UmiUfmpReplayMatch result = UMI_UFMP_REPLAY_NO_MATCH;
  UmiStatus status = UmiUfmpReplayCompare(&saved, &e, &result);
  if (c >= 13 && c <= 15) {
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result == UMI_UFMP_REPLAY_NO_MATCH);
    UmiUfmpReplayRecord untouched = saved;
    CHECK(UmiUfmpReplayCapture(&e, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&untouched, &saved, sizeof saved) == 0);
  } else {
    OK(status);
    CHECK(result == expected);
  }
  return 0;
}
