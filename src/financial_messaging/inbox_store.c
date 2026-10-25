/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/inbox_store.c
 *
 * PURPOSE:
 *   Commit replay indexes and exact message bodies before returning a local receipt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "inbox_internal.h"
#include <stdio.h>
static UmiStatus Key(char out[UMI_UFMP_INBOX_KEY_CAPACITY], const char *kind, const char *sender,
                     const char *id) {
  int size =
      sender == NULL
          ? snprintf(out, UMI_UFMP_INBOX_KEY_CAPACITY, "ufmp.inbox.v1/%s/%s", kind, id)
          : snprintf(out, UMI_UFMP_INBOX_KEY_CAPACITY, "ufmp.inbox.v1/%s/%s/%s", kind, sender, id);
  return size < 0 || (size_t)size >= UMI_UFMP_INBOX_KEY_CAPACITY ? UMI_STATUS_CAPACITY_EXCEEDED
                                                                 : UMI_STATUS_OK;
}
static UmiStatus Identity(const UmiUfmpEnvelope *envelope, char out[65]) {
  unsigned char digest[32];
  UmiStatus status = UmiUfmpEnvelopeDigest(envelope, digest);
  if (status == UMI_STATUS_OK)
    UmiUfmpInboxHex(digest, sizeof digest, out);
  return status;
}
UmiStatus UmiUfmpInboxLoad(const UmiDataServer *server, const char *receipt_id,
                           UmiUfmpInboxRecord *out_record) {
  if (server == NULL || out_record == NULL || !UmiUfmpInboxReceiptValid(receipt_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  char key[UMI_UFMP_INBOX_KEY_CAPACITY], text[UMI_UFMP_INBOX_TEXT_CAPACITY] = {0};
  UmiStatus status = Key(key, "record", NULL, receipt_id);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_get(server, key, text, sizeof text);
  if (status != UMI_STATUS_OK)
    return status == UMI_STATUS_CAPACITY_EXCEEDED ? UMI_STATUS_PARSE_ERROR : status;
  UmiUfmpInboxRecord record;
  char identity[65];
  status = UmiUfmpInboxDecode(text, &record);
  if (status == UMI_STATUS_OK)
    status = Identity(&record.envelope, identity);
  if (status != UMI_STATUS_OK || strcmp(identity, receipt_id) != 0)
    return UMI_STATUS_PARSE_ERROR;
  *out_record = record;
  return UMI_STATUS_OK;
}
static UmiStatus AliasRead(const UmiDataServer *server, const char *key, char receipt[65],
                           char identity[65]) {
  char value[130] = {0};
  UmiStatus status = umi_data_server_get(server, key, value, sizeof value);
  if (status != UMI_STATUS_OK)
    return status == UMI_STATUS_CAPACITY_EXCEEDED ? UMI_STATUS_PARSE_ERROR : status;
  if (strlen(value) != 129U || value[64] != ':')
    return UMI_STATUS_PARSE_ERROR;
  value[64] = '\0';
  if (!UmiUfmpInboxReceiptValid(value) || !UmiUfmpInboxReceiptValid(value + 65))
    return UMI_STATUS_PARSE_ERROR;
  memcpy(receipt, value, 65U);
  memcpy(identity, value + 65, 65U);
  return UMI_STATUS_OK;
}
static UmiStatus StoredMatch(const UmiDataServer *server, const char *receipt,
                             const UmiUfmpEnvelope *candidate, UmiUfmpInboxRecord *record,
                             UmiUfmpReplayMatch *match) {
  UmiStatus status = UmiUfmpInboxLoad(server, receipt, record);
  if (status != UMI_STATUS_OK)
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
  UmiUfmpReplayRecord replay;
  status = UmiUfmpReplayCapture(&record->envelope, &replay);
  if (status == UMI_STATUS_OK)
    status = UmiUfmpReplayCompare(&replay, candidate, match);
  return status;
}
static UmiStatus RecordIncoming(UmiDataServer *server, const UmiUfmpEnvelope *envelope,
                                const void *payload, size_t length, UmiUfmpInboxReceipt *out) {
  char message_key[UMI_UFMP_INBOX_KEY_CAPACITY], operation_key[UMI_UFMP_INBOX_KEY_CAPACITY];
  char identity[65], original[65], alias_identity[65];
  UmiStatus status = Identity(envelope, identity);
  if (status == UMI_STATUS_OK)
    status = Key(message_key, "message", envelope->sender, envelope->message_id);
  if (status != UMI_STATUS_OK)
    return status;
  UmiUfmpInboxRecord record;
  UmiUfmpReplayMatch match;
  status = AliasRead(server, message_key, original, alias_identity);
  if (status == UMI_STATUS_OK) {
    if (strcmp(identity, alias_identity) != 0)
      return UMI_STATUS_ALREADY_EXISTS;
    status = StoredMatch(server, original, envelope, &record, &match);
    if (status != UMI_STATUS_OK)
      return status;
    if (match != UMI_UFMP_REPLAY_SAME_MESSAGE && match != UMI_UFMP_REPLAY_SAME_OPERATION)
      return UMI_STATUS_PARSE_ERROR;
    /* The alias's full digest proves this delivery itself was already
     * recorded, even if its canonical record came from an earlier retry. */
    match = UMI_UFMP_REPLAY_SAME_MESSAGE;
  } else if (status == UMI_STATUS_NOT_FOUND) {
    status = UMI_STATUS_NOT_FOUND;
    if (envelope->idempotency_key[0] != '\0') {
      status = Key(operation_key, "operation", envelope->sender, envelope->idempotency_key);
      if (status != UMI_STATUS_OK)
        return status;
      memset(original, 0, sizeof original);
      status = umi_data_server_get(server, operation_key, original, sizeof original);
      if (status == UMI_STATUS_CAPACITY_EXCEEDED)
        return UMI_STATUS_PARSE_ERROR;
      if (status == UMI_STATUS_OK) {
        if (!UmiUfmpInboxReceiptValid(original))
          return UMI_STATUS_PARSE_ERROR;
        status = StoredMatch(server, original, envelope, &record, &match);
        if (status != UMI_STATUS_OK)
          return status;
        if (match == UMI_UFMP_REPLAY_CONFLICT)
          return UMI_STATUS_ALREADY_EXISTS;
        if (match != UMI_UFMP_REPLAY_SAME_OPERATION)
          return UMI_STATUS_PARSE_ERROR;
      }
    }
    if (status == UMI_STATUS_NOT_FOUND) {
      char key[UMI_UFMP_INBOX_KEY_CAPACITY], text[UMI_UFMP_INBOX_TEXT_CAPACITY];
      record = (UmiUfmpInboxRecord){0};
      record.envelope = *envelope;
      record.payload_length = length;
      if (length != 0U)
        memcpy(record.payload, payload, length);
      memcpy(original, identity, sizeof original);
      status = UmiUfmpInboxEncode(&record, text);
      if (status == UMI_STATUS_OK)
        status = Key(key, "record", NULL, original);
      if (status != UMI_STATUS_OK)
        return status;
      /* An orphaned record indicates inconsistent indexes. Do not
       * overwrite evidence and conceal a damaged journal. */
      char previous[UMI_UFMP_INBOX_TEXT_CAPACITY];
      status = umi_data_server_get(server, key, previous, sizeof previous);
      if (status == UMI_STATUS_OK)
        return UMI_STATUS_PARSE_ERROR;
      if (status != UMI_STATUS_NOT_FOUND)
        return status;
      status = umi_data_server_set(server, key, text);
      if (status == UMI_STATUS_OK && envelope->idempotency_key[0] != '\0')
        status = umi_data_server_set(server, operation_key, original);
      if (status != UMI_STATUS_OK)
        return status;
      match = UMI_UFMP_REPLAY_NO_MATCH;
    } else if (status != UMI_STATUS_OK) {
      return status;
    }
    char alias[130];
    memcpy(alias, original, 64U);
    alias[64] = ':';
    memcpy(alias + 65, identity, 65U);
    status = umi_data_server_set(server, message_key, alias);
    if (status != UMI_STATUS_OK)
      return status;
  } else {
    return status;
  }
  UmiUfmpInboxReceipt receipt = {0};
  memcpy(receipt.receipt_id, original, sizeof receipt.receipt_id);
  memcpy(receipt.original_message_id, record.envelope.message_id,
         strlen(record.envelope.message_id) + 1U);
  receipt.match = match;
  receipt.backend = umi_data_server_backend(server);
  *out = receipt;
  return UMI_STATUS_OK;
}
UmiStatus UmiUfmpInboxReceive(UmiDataServer *server, const UmiUfmpEnvelope *envelope,
                              const UmiUfmpEnvelopePolicy *policy, const void *payload,
                              size_t length, UmiUfmpInboxReceipt *out_receipt) {
  if (server == NULL || out_receipt == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length > UMI_UFMP_INBOX_PAYLOAD_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStatus status = UmiUfmpEnvelopeValidate(envelope, policy);
  if (status == UMI_STATUS_OK)
    status = UmiUfmpEnvelopeCheckPayload(envelope, payload, length);
  if (status != UMI_STATUS_OK)
    return status;
  /* The service owns its transaction so no receipt escapes an uncommitted
   * outer unit of work. Begin may also refuse a competing owner with BUSY. */
  if (umi_data_server_in_transaction(server))
    return UMI_STATUS_BUSY;
  status = umi_data_server_begin(server);
  if (status != UMI_STATUS_OK)
    return status;
  UmiUfmpInboxReceipt receipt;
  status = RecordIncoming(server, envelope, payload, length, &receipt);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_commit(server);
  if (status != UMI_STATUS_OK) {
    UmiStatus rollback = umi_data_server_rollback(server);
    return rollback == UMI_STATUS_OK ? status : rollback;
  }
  *out_receipt = receipt;
  return UMI_STATUS_OK;
}
