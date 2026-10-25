/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_inbox_store.c
 *
 * PURPOSE:
 *   Cover transaction ownership, persistent receipt identities and conflicting replay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/financial_messaging/inbox_store.h"
static UmiStatus Receive(UmiDataServer *s, UmiUfmpEnvelope *e, UmiUfmpInboxReceipt *r) {
  UmiUfmpEnvelopePolicy p = Policy();
  return UmiUfmpInboxReceive(s, e, &p, "\x01", 1U, r);
}
int main(int argc, char **argv) {
  static const char *const cases[] = {"record",           "retry",
                                      "new-delivery",     "alias-retry",
                                      "payload-conflict", "recipient-conflict",
                                      "message-conflict", "sender-scope",
                                      "binary",           "capacity",
                                      "expired",          "transaction-owner",
                                      "corrupt-record",   "missing-record",
                                      "corrupt-alias",    "corrupt-operation",
                                      "missing-receipt",  "receipt-syntax",
                                      "no-operation-key", "output-preserved",
                                      "maximum-fields"};
  int selected = Case(argc, argv, cases, sizeof cases / sizeof cases[0]);
  UmiDataServer *server = NULL;
  OK(umi_data_server_create_memory(&server));
  UmiUfmpEnvelope e = Envelope();
  UmiUfmpInboxReceipt first;
  OK(Receive(server, &e, &first));
  CHECK(first.match == UMI_UFMP_REPLAY_NO_MATCH && first.backend == UMI_DATA_BACKEND_MEMORY);
  CHECK(strlen(first.receipt_id) == 64U);
  size_t before = 0U;
  OK(UmiDataServerCountChecked(server, &before));
  CHECK(before == 3U);
  UmiUfmpInboxReceipt next;
  memset(&next, 0xA5, sizeof next);
  UmiUfmpInboxReceipt unchanged = next;
  UmiUfmpInboxRecord record;
  char key[192];
  switch (selected) {
  case 0:
    OK(UmiUfmpInboxLoad(server, first.receipt_id, &record));
    CHECK(record.payload_length == 1U && record.payload[0] == 1U);
    CHECK(strcmp(record.envelope.message_id, e.message_id) == 0);
    break;
  case 1:
    OK(Receive(server, &e, &next));
    CHECK(next.match == UMI_UFMP_REPLAY_SAME_MESSAGE);
    CHECK(strcmp(next.receipt_id, first.receipt_id) == 0);
    CHECK(umi_data_server_count(server) == before);
    break;
  case 2:
  case 3:
    strcpy(e.message_id, "message.2");
    e.created_ms++;
    OK(Receive(server, &e, &next));
    CHECK(next.match == UMI_UFMP_REPLAY_SAME_OPERATION);
    CHECK(strcmp(next.receipt_id, first.receipt_id) == 0);
    CHECK(umi_data_server_count(server) == before + 1U);
    if (selected == 3) {
      OK(Receive(server, &e, &next));
      CHECK(next.match == UMI_UFMP_REPLAY_SAME_MESSAGE);
      CHECK(strcmp(next.original_message_id, "message.1") == 0);
    }
    break;
  case 4: {
    strcpy(e.message_id, "message.2");
    Body(&e, "\x02", 1U);
    UmiUfmpEnvelopePolicy p = Policy();
    CHECK(UmiUfmpInboxReceive(server, &e, &p, "\x02", 1U, &next) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(memcmp(&next, &unchanged, sizeof next) == 0);
    CHECK(umi_data_server_count(server) == before);
    break;
  }
  case 5:
    strcpy(e.message_id, "message.2");
    strcpy(e.recipient, "Bank.C");
    CHECK(Receive(server, &e, &next) == UMI_STATUS_ALREADY_EXISTS);
    break;
  case 6:
    strcpy(e.trace_id, "new-trace");
    CHECK(Receive(server, &e, &next) == UMI_STATUS_ALREADY_EXISTS);
    break;
  case 7:
    strcpy(e.sender, "Bank.C");
    OK(Receive(server, &e, &next));
    CHECK(next.match == UMI_UFMP_REPLAY_NO_MATCH);
    CHECK(strcmp(next.receipt_id, first.receipt_id) != 0);
    break;
  case 8:
  case 9: {
    unsigned char bytes[UMI_UFMP_INBOX_PAYLOAD_CAPACITY + 1U];
    for (size_t i = 0U; i < sizeof bytes; ++i)
      bytes[i] = (unsigned char)(i % 256U);
    strcpy(e.message_id, "binary.1");
    strcpy(e.idempotency_key, "binary.operation");
    size_t length = selected == 8 ? sizeof bytes - 1U : sizeof bytes;
    Body(&e, bytes, length);
    UmiUfmpEnvelopePolicy p = Policy();
    UmiStatus status = UmiUfmpInboxReceive(server, &e, &p, bytes, length, &next);
    if (selected == 9) {
      CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
      CHECK(memcmp(&next, &unchanged, sizeof next) == 0);
    } else {
      OK(status);
      OK(UmiUfmpInboxLoad(server, next.receipt_id, &record));
      CHECK(record.payload_length == length && memcmp(record.payload, bytes, length) == 0);
    }
    break;
  }
  case 10: {
    UmiUfmpEnvelopePolicy p = Policy();
    p.now_ms = e.expires_ms;
    CHECK(UmiUfmpInboxReceive(server, &e, &p, "\x01", 1U, &next) == UMI_STATUS_TIMEOUT);
    /* Historical retrieval is independent of fresh-message admission. */
    OK(UmiUfmpInboxLoad(server, first.receipt_id, &record));
    break;
  }
  case 11:
    OK(umi_data_server_begin(server));
    CHECK(Receive(server, &e, &next) == UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(server));
    OK(umi_data_server_set(server, "owner.marker", "preserved"));
    OK(umi_data_server_commit(server));
    CHECK(umi_data_server_count(server) == before + 1U);
    break;
  case 12:
  case 13:
    (void)snprintf(key, sizeof key, "ufmp.inbox.v1/record/%s", first.receipt_id);
    if (selected == 12)
      OK(umi_data_server_set(server, key, "damaged"));
    else
      OK(umi_data_server_delete(server, key));
    CHECK(Receive(server, &e, &next) == UMI_STATUS_PARSE_ERROR);
    break;
  case 14:
    OK(umi_data_server_set(server, "ufmp.inbox.v1/message/Bank.B/message.1", "bad"));
    CHECK(Receive(server, &e, &next) == UMI_STATUS_PARSE_ERROR);
    break;
  case 15:
    strcpy(e.message_id, "message.2");
    OK(umi_data_server_set(server, "ufmp.inbox.v1/operation/Bank.B/operation.1", "bad"));
    CHECK(Receive(server, &e, &next) == UMI_STATUS_PARSE_ERROR);
    break;
  case 16:
    CHECK(UmiUfmpInboxLoad(server,
                           "0000000000000000000000000000000000000000000000000000000000000000",
                           &record) == UMI_STATUS_NOT_FOUND);
    break;
  case 17:
    CHECK(UmiUfmpInboxLoad(server, "../record", &record) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUfmpInboxLoad(server, NULL, &record) == UMI_STATUS_INVALID_ARGUMENT);
    break;
  case 18:
    e.type = UMI_UFMP_NETWORK_HEARTBEAT;
    e.idempotency_key[0] = '\0';
    strcpy(e.message_id, "heartbeat.1");
    OK(Receive(server, &e, &next));
    CHECK(umi_data_server_count(server) == before + 2U);
    OK(Receive(server, &e, &next));
    CHECK(next.match == UMI_UFMP_REPLAY_SAME_MESSAGE);
    break;
  case 19: {
    UmiUfmpEnvelopePolicy p = Policy();
    CHECK(UmiUfmpInboxReceive(server, &e, &p, "\x02", 1U, &next) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&next, &unchanged, sizeof next) == 0);
    CHECK(!umi_data_server_in_transaction(server));
    break;
  }
  case 20:
    memset(e.message_id, 'm', sizeof e.message_id - 1U);
    memset(e.conversation_id, 'c', sizeof e.conversation_id - 1U);
    memset(e.causation_id, 'x', sizeof e.causation_id - 1U);
    memset(e.idempotency_key, 'i', sizeof e.idempotency_key - 1U);
    memset(e.sender, 's', sizeof e.sender - 1U);
    memset(e.recipient, 'r', sizeof e.recipient - 1U);
    memset(e.trace_id, 't', sizeof e.trace_id - 1U);
    memset(e.schema, 'p', sizeof e.schema - 1U);
    OK(Receive(server, &e, &next));
    OK(UmiUfmpInboxLoad(server, next.receipt_id, &record));
    CHECK(strcmp(record.envelope.schema, e.schema) == 0);
    break;
  default:
    CHECK(0);
  }
  CHECK(!umi_data_server_in_transaction(server));
  umi_data_server_destroy(server);
  return 0;
}
