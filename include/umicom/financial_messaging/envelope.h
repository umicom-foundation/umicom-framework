/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/envelope.h
 *
 * PURPOSE:
 *   Validate logical financial envelopes without treating a digest as authentication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_ENVELOPE_H
#define UMICOM_FINANCIAL_MESSAGING_ENVELOPE_H
#include "umicom/financial_messaging/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUfmpEnvelope {
  uint32_t revision;
  UmiUfmpMessageType type;
  UmiUfmpPayloadFormat format;
  char message_id[UMI_UFMP_ID_CAPACITY];
  char conversation_id[UMI_UFMP_ID_CAPACITY];
  char causation_id[UMI_UFMP_ID_CAPACITY];
  char idempotency_key[UMI_UFMP_ID_CAPACITY];
  char sender[UMI_UFMP_ID_CAPACITY];
  char recipient[UMI_UFMP_ID_CAPACITY];
  char trace_id[UMI_UFMP_ID_CAPACITY];
  char schema[UMI_UFMP_SCHEMA_CAPACITY];
  uint64_t created_ms;
  uint64_t expires_ms;
  uint64_t payload_bytes;
  unsigned char content_digest[UMI_UFMP_DIGEST_BYTES];
} UmiUfmpEnvelope;
typedef struct UmiUfmpEnvelopePolicy {
  uint64_t now_ms;
  uint64_t future_skew_ms;
  uint64_t maximum_lifetime_ms;
  uint64_t maximum_payload_bytes;
  uint32_t revision;
} UmiUfmpEnvelopePolicy;
/* Pure validation of complete fixed-size fields and caller-selected limits.
 * No certificate, schema, signature, directory, permission or funds check is
 * implied. The payload digest is verified separately against exact body bytes.
 * Expiry is exclusive: now == expires_ms is already expired. */
UmiStatus UmiUfmpEnvelopeValidate(const UmiUfmpEnvelope *value,
                                  const UmiUfmpEnvelopePolicy *policy);
UmiStatus UmiUfmpEnvelopeCheckPayload(const UmiUfmpEnvelope *value, const void *payload,
                                      size_t length);
/* Internal evidence digests, NOT UFMP wire encoding or COSE signing input.
 * Both use a domain tag, fixed big-endian integers and length-prefixed text.
 * Envelope digest binds every logical field. Operation digest deliberately
 * excludes delivery IDs, conversation/trace IDs and timestamps so a retried
 * operation can use a fresh message ID. It still binds revision, sender,
 * recipient, type, format, exact schema, key, payload size and payload digest.
 * Inputs must have valid bounded syntax; no allocation or I/O occurs.
 * Output is written only on success; source/output storage must not overlap. */
UmiStatus UmiUfmpEnvelopeDigest(const UmiUfmpEnvelope *value,
                                unsigned char out[UMI_UFMP_DIGEST_BYTES]);
UmiStatus UmiUfmpOperationDigest(const UmiUfmpEnvelope *value,
                                 unsigned char out[UMI_UFMP_DIGEST_BYTES]);
#ifdef __cplusplus
}
#endif
#endif
