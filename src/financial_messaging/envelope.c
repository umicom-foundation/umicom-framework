/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/envelope.c
 *
 * PURPOSE:
 *   Validate bounded fields, payload integrity and overflow-safe message freshness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/base/sha256.h"
static int OptionalId(const char *value) {
  return value[0] == '\0' || UmiUfmpIdentifierValid(value, UMI_UFMP_ID_CAPACITY);
}
static int SchemaValid(const char *value) {
  /* Profiles use bounded ASCII tokens, never a URL to fetch on receipt. */
  for (size_t i = 0U; i < UMI_UFMP_SCHEMA_CAPACITY; ++i) {
    unsigned char c = (unsigned char)value[i];
    if (c == 0U)
      return i != 0U;
    if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' ||
          c == '_' || c == ':' || c == '-'))
      return 0;
  }
  return 0;
}
UmiStatus UmiUfmpEnvelopeSyntax(const UmiUfmpEnvelope *value) {
  if (value == NULL || value->revision == 0U || UmiUfmpMessageTypeName(value->type) == NULL ||
      (value->format != UMI_UFMP_PAYLOAD_ISO20022_XML &&
       value->format != UMI_UFMP_PAYLOAD_CONTROL_CBOR) ||
      !UmiUfmpIdentifierValid(value->message_id, sizeof value->message_id) ||
      !UmiUfmpIdentifierValid(value->conversation_id, sizeof value->conversation_id) ||
      !UmiUfmpIdentifierValid(value->sender, sizeof value->sender) ||
      !UmiUfmpIdentifierValid(value->recipient, sizeof value->recipient) ||
      !OptionalId(value->causation_id) || !OptionalId(value->trace_id) ||
      !OptionalId(value->idempotency_key) || !SchemaValid(value->schema) ||
      value->created_ms == 0U || value->expires_ms <= value->created_ms ||
      (UmiUfmpMessageRequiresIdempotency(value->type) && value->idempotency_key[0] == '\0'))
    return UMI_STATUS_INVALID_ARGUMENT;
  return UMI_STATUS_OK;
}
UmiStatus UmiUfmpEnvelopeValidate(const UmiUfmpEnvelope *value,
                                  const UmiUfmpEnvelopePolicy *policy) {
  UmiStatus status = UmiUfmpEnvelopeSyntax(value);
  if (status != UMI_STATUS_OK)
    return status;
  if (policy == NULL || policy->revision == 0U || policy->maximum_lifetime_ms == 0U ||
      policy->maximum_payload_bytes == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (value->revision != policy->revision)
    return UMI_STATUS_NOT_IMPLEMENTED;
  if (value->payload_bytes > policy->maximum_payload_bytes)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  /* Subtract ordered unsigned timestamps rather than adding a skew/lifetime
   * to the current time, which could wrap near the representation limit. */
  if (value->expires_ms - value->created_ms > policy->maximum_lifetime_ms)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (value->created_ms > policy->now_ms &&
      value->created_ms - policy->now_ms > policy->future_skew_ms)
    return UMI_STATUS_INVALID_STATE;
  if (policy->now_ms >= value->expires_ms)
    return UMI_STATUS_TIMEOUT;
  return UMI_STATUS_OK;
}
UmiStatus UmiUfmpEnvelopeCheckPayload(const UmiUfmpEnvelope *value, const void *payload,
                                      size_t length) {
  UmiStatus status = UmiUfmpEnvelopeSyntax(value);
  if (status != UMI_STATUS_OK)
    return status;
  if ((payload == NULL && length != 0U) || value->payload_bytes != (uint64_t)length)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiSha256 context;
  unsigned char digest[UMI_UFMP_DIGEST_BYTES];
  UmiSha256Init(&context);
  status = UmiSha256Update(&context, payload, length);
  if (status == UMI_STATUS_OK)
    status = UmiSha256Final(&context, digest);
  if (status != UMI_STATUS_OK)
    return status;
  return memcmp(digest, value->content_digest, sizeof digest) == 0 ? UMI_STATUS_OK
                                                                   : UMI_STATUS_PARSE_ERROR;
}
