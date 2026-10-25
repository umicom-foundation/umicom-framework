/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/simulation.c
 *
 * PURPOSE:
 *   Exercise duplicate requests and independent payment evidence using synthetic institutions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_messaging/simulation.h"
#include "umicom/base/sha256.h"
#include "umicom/financial_messaging/replay.h"
#include <stdio.h>
#include <string.h>
static UmiStatus Body(UmiUfmpEnvelope *e, const void *body, size_t length) {
  UmiSha256 hash;
  UmiSha256Init(&hash);
  e->payload_bytes = (uint64_t)length;
  UmiStatus status = UmiSha256Update(&hash, body, length);
  return status == UMI_STATUS_OK ? UmiSha256Final(&hash, e->content_digest) : status;
}
static UmiStatus Observe(UmiUfmpPaymentProgress *p, UmiUfmpEvidenceKind kind, const char *issuer,
                         const char *reference, uint64_t when) {
  UmiUfmpPaymentEvidence e = {0};
  e.kind = kind;
  e.occurred_ms = when;
  memcpy(e.payment_id, p->payment_id, strlen(p->payment_id) + 1U);
  memcpy(e.issuer, issuer, strlen(issuer) + 1U);
  memcpy(e.reference, reference, strlen(reference) + 1U);
  return UmiUfmpPaymentProgressApply(p, p->revision, &e);
}
UmiStatus UmiUfmpSimulationRun(UmiUfmpSimulationReport *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiUfmpSimulationReport report = {0};
  UmiUfmpEnvelope e = {0};
  e.revision = UMI_UFMP_LOGICAL_REVISION;
  e.type = UMI_UFMP_PAYMENT_INITIATE;
  e.format = UMI_UFMP_PAYLOAD_CONTROL_CBOR;
  strcpy(e.message_id, "synthetic.message.1");
  strcpy(e.conversation_id, "synthetic.conversation.1");
  strcpy(e.idempotency_key, "synthetic.operation.1");
  strcpy(e.sender, "Bank.B");
  strcpy(e.recipient, "Bank.A");
  strcpy(e.schema, "synthetic.cbor-amount");
  e.created_ms = 1000U;
  e.expires_ms = 2000U;
  /* Small deterministic CBOR fixtures: {"amount":10000} and {"amount":20000}.
   * These are synthetic values, not complete payment or signed wire messages. */
  const unsigned char body[] = {0xA1U, 0x66U, 'a', 'm', 'o', 'u', 'n', 't', 0x19U, 0x27U, 0x10U};
  const unsigned char changed_body[] = {0xA1U, 0x66U, 'a',   'm',   'o',  'u',
                                        'n',   't',   0x19U, 0x4EU, 0x20U};
  UmiStatus status = Body(&e, body, sizeof body);
  UmiUfmpEnvelopePolicy policy = {1100U, 0U, 1000U, 4096U, UMI_UFMP_LOGICAL_REVISION};
  if (status == UMI_STATUS_OK)
    status = UmiUfmpEnvelopeValidate(&e, &policy);
  if (status == UMI_STATUS_OK)
    status = UmiUfmpEnvelopeCheckPayload(&e, body, sizeof body);
  UmiUfmpReplayRecord record;
  if (status == UMI_STATUS_OK)
    status = UmiUfmpReplayCapture(&e, &record);
  if (status != UMI_STATUS_OK)
    return status;
  report.accepted_operations = 1U;
  report.delivery_attempts = 1U;
  for (unsigned i = 0U; i < 4U; ++i) {
    UmiUfmpReplayMatch match;
    status = UmiUfmpReplayCompare(&record, &e, &match);
    if (status != UMI_STATUS_OK)
      return status;
    if (match != UMI_UFMP_REPLAY_SAME_MESSAGE)
      return UMI_STATUS_INTERNAL_ERROR;
    ++report.delivery_attempts;
    ++report.replay_matches;
  }
  UmiUfmpEnvelope changed = e;
  strcpy(changed.message_id, "synthetic.message.2");
  status = Body(&changed, changed_body, sizeof changed_body);
  UmiUfmpReplayMatch match;
  if (status == UMI_STATUS_OK)
    status = UmiUfmpReplayCompare(&record, &changed, &match);
  if (status != UMI_STATUS_OK)
    return status;
  report.changed_payload_refused = match == UMI_UFMP_REPLAY_CONFLICT;
  UmiUfmpPaymentProgress progress;
  status = UmiUfmpPaymentProgressInit(&progress, "synthetic.payment.1", "Bank.A", "Bank.A",
                                      "Bank.A", "Wallet.C");
  if (status == UMI_STATUS_OK)
    status = Observe(&progress, UMI_UFMP_EVIDENCE_DELIVERY, "Bank.A", "receipt.1", 1100U);
  UmiUfmpProgressState state;
  if (status == UMI_STATUS_OK)
    status = UmiUfmpPaymentProgressRead(&progress, &state);
  if (status != UMI_STATUS_OK)
    return status;
  report.receipt_left_settlement_pending = state == UMI_UFMP_PROGRESS_DELIVERED;
  status = Observe(&progress, UMI_UFMP_EVIDENCE_ACCEPTANCE, "Bank.A", "acceptance.1", 1200U);
  if (status == UMI_STATUS_OK)
    status = Observe(&progress, UMI_UFMP_EVIDENCE_CREDIT, "Wallet.C", "credit.1", 1300U);
  if (status == UMI_STATUS_OK)
    status = UmiUfmpPaymentProgressRead(&progress, &state);
  if (status != UMI_STATUS_OK)
    return status;
  report.credit_left_settlement_pending = state == UMI_UFMP_PROGRESS_CREDITED_AWAITING_SETTLEMENT;
  status = Observe(&progress, UMI_UFMP_EVIDENCE_SETTLEMENT, "Bank.A", "settlement.1", 1400U);
  if (status == UMI_STATUS_OK)
    status = UmiUfmpPaymentProgressRead(&progress, &report.final_state);
  if (status == UMI_STATUS_OK)
    *out = report;
  return status;
}
UmiStatus UmiUfmpSimulationFormat(char *out, size_t capacity, size_t *required) {
  if (required == NULL || (out == NULL && capacity != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiUfmpSimulationReport report;
  UmiStatus status = UmiUfmpSimulationRun(&report);
  if (status != UMI_STATUS_OK)
    return status;
  char text[1024];
  int written = snprintf(text, sizeof text,
                         "UFMP logical simulation - TEST ONLY\n"
                         "Participants: Bank.B -> Bank.A -> Wallet.C\n"
                         "No network, signatures, durable commits or real funds.\n"
                         "Delivery attempts: %u; accepted operations: %u; replay matches: %u\n"
                         "Changed payload refused: %s\n"
                         "Delivery receipt left settlement pending: %s\n"
                         "Beneficiary credit left settlement pending: %s\n"
                         "Final simulated status: %s\n",
                         report.delivery_attempts, report.accepted_operations,
                         report.replay_matches, report.changed_payload_refused ? "yes" : "no",
                         report.receipt_left_settlement_pending ? "yes" : "no",
                         report.credit_left_settlement_pending ? "yes" : "no",
                         UmiUfmpProgressStateText(report.final_state));
  if (written < 0 || (size_t)written >= sizeof text)
    return UMI_STATUS_INTERNAL_ERROR;
  *required = (size_t)written;
  if (out == NULL && capacity == 0U)
    return UMI_STATUS_OK;
  if (capacity <= (size_t)written)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out, text, (size_t)written + 1U);
  return UMI_STATUS_OK;
}
