/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/fixture.h
 *
 * PURPOSE:
 *   Share explicit synthetic envelope and evidence fixtures for native regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UFMP_TEST_FIXTURE_H
#define UMICOM_UFMP_TEST_FIXTURE_H
#include "umicom/base/sha256.h"
#include "umicom/financial_messaging/progress.h"
#include "umicom/financial_messaging/replay.h"
#include "umicom/financial_messaging/simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                   \
  do {                                                                                             \
    if (!(c)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                      \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)
static inline int Case(int argc, char **argv, const char *const *names, size_t count) {
  CHECK(argc == 2);
  for (size_t i = 0U; i < count; ++i)
    if (strcmp(argv[1], names[i]) == 0)
      return (int)i;
  fprintf(stderr, "Unknown case: %s\n", argv[1]);
  exit(2);
}
static inline void Body(UmiUfmpEnvelope *e, const void *p, size_t n) {
  UmiSha256 sha;
  UmiSha256Init(&sha);
  OK(UmiSha256Update(&sha, p, n));
  OK(UmiSha256Final(&sha, e->content_digest));
  e->payload_bytes = (uint64_t)n;
}
static inline UmiUfmpEnvelope Envelope(void) {
  UmiUfmpEnvelope e = {0};
  e.revision = UMI_UFMP_LOGICAL_REVISION;
  e.type = UMI_UFMP_PAYMENT_INITIATE;
  e.format = UMI_UFMP_PAYLOAD_CONTROL_CBOR;
  strcpy(e.message_id, "message.1");
  strcpy(e.conversation_id, "conversation.1");
  strcpy(e.idempotency_key, "operation.1");
  strcpy(e.sender, "Bank.B");
  strcpy(e.recipient, "Bank.A");
  strcpy(e.schema, "synthetic.profile.1");
  e.created_ms = 1000U;
  e.expires_ms = 2000U;
  Body(&e, "\x01", 1U);
  return e;
}
static inline UmiUfmpEnvelopePolicy Policy(void) {
  UmiUfmpEnvelopePolicy p = {1100U, 50U, 1000U, 4096U, UMI_UFMP_LOGICAL_REVISION};
  return p;
}
static inline UmiUfmpPaymentProgress Progress(void) {
  UmiUfmpPaymentProgress p;
  OK(UmiUfmpPaymentProgressInit(&p, "payment.1", "relay.1", "Bank.A", "settlement.1", "Wallet.C"));
  return p;
}
static inline UmiUfmpPaymentEvidence Evidence(UmiUfmpEvidenceKind kind) {
  UmiUfmpPaymentEvidence e = {0};
  e.kind = kind;
  strcpy(e.payment_id, "payment.1");
  const char *issuer = kind == UMI_UFMP_EVIDENCE_DELIVERY     ? "relay.1"
                       : kind == UMI_UFMP_EVIDENCE_SETTLEMENT ? "settlement.1"
                       : kind == UMI_UFMP_EVIDENCE_CREDIT     ? "Wallet.C"
                                                              : "Bank.A";
  strcpy(e.issuer, issuer);
  strcpy(e.reference, "evidence.1");
  e.occurred_ms = 1100U;
  return e;
}
#endif
