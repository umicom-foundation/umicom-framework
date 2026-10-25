/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/inbox_codec.c
 *
 * PURPOSE:
 *   Encode local journal fields explicitly instead of persisting C structure padding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "inbox_internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
void UmiUfmpInboxHex(const unsigned char *bytes, size_t length, char *out) {
  static const char digits[] = "0123456789abcdef";
  for (size_t i = 0U; i < length; ++i) {
    out[i * 2U] = digits[bytes[i] >> 4U];
    out[i * 2U + 1U] = digits[bytes[i] & 15U];
  }
  out[length * 2U] = '\0';
}
static int Digit(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1;
}
int UmiUfmpInboxReceiptValid(const char *text) {
  if (text == NULL)
    return 0;
  for (size_t i = 0U; i < 64U; ++i)
    if (text[i] == '\0' || Digit(text[i]) < 0)
      return 0;
  return text[64] == '\0';
}
static int Unhex(const char *text, size_t length, unsigned char *out) {
  if (strlen(text) != length * 2U)
    return 0;
  for (size_t i = 0U; i < length; ++i) {
    int high = Digit(text[i * 2U]), low = Digit(text[i * 2U + 1U]);
    if (high < 0 || low < 0)
      return 0;
    out[i] = (unsigned char)((unsigned int)high * 16U + (unsigned int)low);
  }
  return 1;
}
static int Number(const char *text, uint64_t *out) {
  if (text[0] == '\0' || (text[0] == '0' && text[1] != '\0'))
    return 0;
  uint64_t value = 0U;
  for (size_t i = 0U; text[i] != '\0'; ++i) {
    if (text[i] < '0' || text[i] > '9')
      return 0;
    uint64_t digit = (uint64_t)(text[i] - '0');
    if (value > (UINT64_MAX - digit) / 10U)
      return 0;
    value = value * 10U + digit;
  }
  *out = value;
  return 1;
}
static int Text(char *out, size_t capacity, const char *text) {
  size_t length = strlen(text);
  if (length >= capacity)
    return 0;
  memcpy(out, text, length + 1U);
  return 1;
}
UmiStatus UmiUfmpInboxEncode(const UmiUfmpInboxRecord *record,
                             char out[UMI_UFMP_INBOX_TEXT_CAPACITY]) {
  if (record == NULL || out == NULL || record->payload_length > UMI_UFMP_INBOX_PAYLOAD_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status =
      UmiUfmpEnvelopeCheckPayload(&record->envelope, record->payload, record->payload_length);
  if (status != UMI_STATUS_OK)
    return status;
  char digest[65], body[UMI_UFMP_INBOX_PAYLOAD_CAPACITY * 2U + 1U];
  UmiUfmpInboxHex(record->envelope.content_digest, 32U, digest);
  UmiUfmpInboxHex(record->payload, record->payload_length, body);
  const UmiUfmpEnvelope *e = &record->envelope;
  /* Fields have validated ASCII token syntax. Newlines therefore delimit
   * local records unambiguously, including empty optional fields. This is
   * a storage representation, never the signed UFMP/CBOR wire encoding. */
  int length = snprintf(out, UMI_UFMP_INBOX_TEXT_CAPACITY,
                        "UFMP-INBOX-1\n%" PRIu32 "\n%s\n%u\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n"
                        "%" PRIu64 "\n%" PRIu64 "\n%" PRIu64 "\n%s\n%s",
                        e->revision, UmiUfmpMessageTypeName(e->type), (unsigned int)e->format,
                        e->message_id, e->conversation_id, e->causation_id, e->idempotency_key,
                        e->sender, e->recipient, e->trace_id, e->schema, e->created_ms,
                        e->expires_ms, e->payload_bytes, digest, body);
  return length < 0 || (size_t)length >= UMI_UFMP_INBOX_TEXT_CAPACITY ? UMI_STATUS_CAPACITY_EXCEEDED
                                                                      : UMI_STATUS_OK;
}
UmiStatus UmiUfmpInboxDecode(char text[UMI_UFMP_INBOX_TEXT_CAPACITY],
                             UmiUfmpInboxRecord *out_record) {
  if (text == NULL || out_record == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (memchr(text, '\0', UMI_UFMP_INBOX_TEXT_CAPACITY) == NULL)
    return UMI_STATUS_PARSE_ERROR;
  char *fields[17] = {text};
  size_t count = 1U;
  for (char *cursor = text; *cursor != '\0'; ++cursor) {
    if (*cursor == '\n') {
      if (count >= 17U)
        return UMI_STATUS_PARSE_ERROR;
      *cursor = '\0';
      fields[count++] = cursor + 1;
    }
  }
  if (count != 17U || strcmp(fields[0], "UFMP-INBOX-1") != 0)
    return UMI_STATUS_PARSE_ERROR;
  UmiUfmpInboxRecord r = {0};
  UmiUfmpEnvelope *e = &r.envelope;
  uint64_t revision, format, size;
  if (!Number(fields[1], &revision) || revision == 0U || revision > UINT32_MAX ||
      UmiUfmpMessageTypeParse(fields[2], strlen(fields[2]), &e->type) != UMI_STATUS_OK ||
      !Number(fields[3], &format) || (format != 1U && format != 2U) ||
      !Text(e->message_id, sizeof e->message_id, fields[4]) ||
      !Text(e->conversation_id, sizeof e->conversation_id, fields[5]) ||
      !Text(e->causation_id, sizeof e->causation_id, fields[6]) ||
      !Text(e->idempotency_key, sizeof e->idempotency_key, fields[7]) ||
      !Text(e->sender, sizeof e->sender, fields[8]) ||
      !Text(e->recipient, sizeof e->recipient, fields[9]) ||
      !Text(e->trace_id, sizeof e->trace_id, fields[10]) ||
      !Text(e->schema, sizeof e->schema, fields[11]) || !Number(fields[12], &e->created_ms) ||
      !Number(fields[13], &e->expires_ms) || !Number(fields[14], &size) ||
      size > UMI_UFMP_INBOX_PAYLOAD_CAPACITY || !Unhex(fields[15], 32U, e->content_digest) ||
      !Unhex(fields[16], (size_t)size, r.payload))
    return UMI_STATUS_PARSE_ERROR;
  e->revision = (uint32_t)revision;
  e->format = (UmiUfmpPayloadFormat)format;
  e->payload_bytes = size;
  r.payload_length = (size_t)size;
  if (UmiUfmpEnvelopeCheckPayload(e, r.payload, r.payload_length) != UMI_STATUS_OK)
    return UMI_STATUS_PARSE_ERROR;
  *out_record = r;
  return UMI_STATUS_OK;
}
