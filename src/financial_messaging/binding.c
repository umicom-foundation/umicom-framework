/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/binding.c
 *
 * PURPOSE:
 *   Bind complete envelopes and stable business requests to separate evidence digests.
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
static UmiStatus Number(UmiSha256 *context, uint64_t value) {
  unsigned char bytes[8];
  for (size_t i = 0U; i < sizeof bytes; ++i) {
    bytes[sizeof bytes - 1U - i] = (unsigned char)(value & UINT64_C(255));
    value >>= 8U;
  }
  return UmiSha256Update(context, bytes, sizeof bytes);
}
static UmiStatus Text(UmiSha256 *context, const char *value) {
  size_t length = strlen(value);
  UmiStatus status = Number(context, (uint64_t)length);
  return status == UMI_STATUS_OK ? UmiSha256Update(context, value, length) : status;
}
static UmiStatus Digest(const UmiUfmpEnvelope *value, int operation, unsigned char *out) {
  UmiStatus status = UmiUfmpEnvelopeSyntax(value);
  if (status != UMI_STATUS_OK)
    return status;
  if (out == NULL || (operation && value->idempotency_key[0] == '\0'))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiSha256 context;
  unsigned char result[UMI_UFMP_DIGEST_BYTES];
  UmiSha256Init(&context);
  /* Domain separation prevents a full-envelope fingerprint being mistaken
   * for a business-operation fingerprint in a persisted replay record. */
  status = Text(&context,
                operation ? "umicom.ufmp.logical-operation.1" : "umicom.ufmp.logical-envelope.1");
#define UMI_UFMP_HASH(call)                                                                        \
  do {                                                                                             \
    if (status == UMI_STATUS_OK)                                                                   \
      status = (call);                                                                             \
  } while (0)
  UMI_UFMP_HASH(Number(&context, value->revision));
  UMI_UFMP_HASH(Number(&context, (uint64_t)value->type));
  UMI_UFMP_HASH(Number(&context, (uint64_t)value->format));
  UMI_UFMP_HASH(Text(&context, value->sender));
  UMI_UFMP_HASH(Text(&context, value->recipient));
  UMI_UFMP_HASH(Text(&context, value->schema));
  UMI_UFMP_HASH(Text(&context, value->idempotency_key));
  UMI_UFMP_HASH(Number(&context, value->payload_bytes));
  UMI_UFMP_HASH(UmiSha256Update(&context, value->content_digest, sizeof value->content_digest));
  if (!operation) {
    UMI_UFMP_HASH(Text(&context, value->message_id));
    UMI_UFMP_HASH(Text(&context, value->conversation_id));
    UMI_UFMP_HASH(Text(&context, value->causation_id));
    UMI_UFMP_HASH(Text(&context, value->trace_id));
    UMI_UFMP_HASH(Number(&context, value->created_ms));
    UMI_UFMP_HASH(Number(&context, value->expires_ms));
  }
  UMI_UFMP_HASH(UmiSha256Final(&context, result));
#undef UMI_UFMP_HASH
  if (status == UMI_STATUS_OK)
    memcpy(out, result, sizeof result);
  return status;
}
UmiStatus UmiUfmpEnvelopeDigest(const UmiUfmpEnvelope *value, unsigned char out[32]) {
  return Digest(value, 0, out);
}
UmiStatus UmiUfmpOperationDigest(const UmiUfmpEnvelope *value, unsigned char out[32]) {
  return Digest(value, 1, out);
}
