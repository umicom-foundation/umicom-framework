/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/progress.c
 *
 * PURPOSE:
 *   Project verified financial observations without inferring funds movement from receipts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_messaging/progress.h"
#include <string.h>
static int Id(const char *text) { return UmiUfmpIdentifierValid(text, UMI_UFMP_ID_CAPACITY); }
static int EvidenceValid(const UmiUfmpPaymentEvidence *e) {
  return e != NULL && e->kind >= UMI_UFMP_EVIDENCE_DELIVERY &&
         e->kind <= UMI_UFMP_EVIDENCE_REJECTION && Id(e->payment_id) && Id(e->issuer) &&
         Id(e->reference) && e->occurred_ms != 0U;
}
static int SlotValid(const UmiUfmpPaymentProgress *p, const UmiUfmpPaymentEvidence *e,
                     UmiUfmpEvidenceKind kind, const char *agent) {
  if (e->kind == 0)
    return 1;
  return EvidenceValid(e) && e->kind == kind && strcmp(e->payment_id, p->payment_id) == 0 &&
         strcmp(e->issuer, agent) == 0;
}
static int Valid(const UmiUfmpPaymentProgress *p) {
  if (p == NULL || p->revision == 0U || !Id(p->payment_id) || !Id(p->delivery_agent) ||
      !Id(p->acceptance_agent) || !Id(p->settlement_agent) || !Id(p->credit_agent))
    return 0;
  if (!SlotValid(p, &p->delivery, UMI_UFMP_EVIDENCE_DELIVERY, p->delivery_agent) ||
      !SlotValid(p, &p->acceptance, UMI_UFMP_EVIDENCE_ACCEPTANCE, p->acceptance_agent) ||
      !SlotValid(p, &p->settlement, UMI_UFMP_EVIDENCE_SETTLEMENT, p->settlement_agent) ||
      !SlotValid(p, &p->credit, UMI_UFMP_EVIDENCE_CREDIT, p->credit_agent) ||
      !SlotValid(p, &p->rejection, UMI_UFMP_EVIDENCE_REJECTION, p->acceptance_agent))
    return 0;
  if ((p->settlement.kind != 0 || p->credit.kind != 0) && p->acceptance.kind == 0)
    return 0;
  if (p->rejection.kind != 0 && p->acceptance.kind != 0)
    return 0;
  return 1;
}
UmiStatus UmiUfmpPaymentProgressInit(UmiUfmpPaymentProgress *out, const char *payment,
                                     const char *delivery_agent, const char *acceptance_agent,
                                     const char *settlement_agent, const char *credit_agent) {
  if (out == NULL || !Id(payment) || !Id(delivery_agent) || !Id(acceptance_agent) ||
      !Id(settlement_agent) || !Id(credit_agent))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiUfmpPaymentProgress p = {0};
  p.revision = 1U;
  memcpy(p.payment_id, payment, strlen(payment) + 1U);
  memcpy(p.delivery_agent, delivery_agent, strlen(delivery_agent) + 1U);
  memcpy(p.acceptance_agent, acceptance_agent, strlen(acceptance_agent) + 1U);
  memcpy(p.settlement_agent, settlement_agent, strlen(settlement_agent) + 1U);
  memcpy(p.credit_agent, credit_agent, strlen(credit_agent) + 1U);
  *out = p;
  return UMI_STATUS_OK;
}
static int Same(const UmiUfmpPaymentEvidence *a, const UmiUfmpPaymentEvidence *b) {
  return a->kind == b->kind && a->occurred_ms == b->occurred_ms &&
         strcmp(a->payment_id, b->payment_id) == 0 && strcmp(a->issuer, b->issuer) == 0 &&
         strcmp(a->reference, b->reference) == 0;
}
UmiStatus UmiUfmpPaymentProgressApply(UmiUfmpPaymentProgress *p, uint64_t expected_revision,
                                      const UmiUfmpPaymentEvidence *e) {
  if (!Valid(p) || !EvidenceValid(e))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (p->revision != expected_revision)
    return UMI_STATUS_BUSY;
  if (strcmp(e->payment_id, p->payment_id) != 0)
    return UMI_STATUS_NOT_FOUND;
  UmiUfmpPaymentEvidence *slot = NULL;
  const char *agent = NULL;
  switch (e->kind) {
  case UMI_UFMP_EVIDENCE_DELIVERY:
    slot = &p->delivery;
    agent = p->delivery_agent;
    break;
  case UMI_UFMP_EVIDENCE_ACCEPTANCE:
    slot = &p->acceptance;
    agent = p->acceptance_agent;
    break;
  case UMI_UFMP_EVIDENCE_SETTLEMENT:
    slot = &p->settlement;
    agent = p->settlement_agent;
    break;
  case UMI_UFMP_EVIDENCE_CREDIT:
    slot = &p->credit;
    agent = p->credit_agent;
    break;
  case UMI_UFMP_EVIDENCE_REJECTION:
    slot = &p->rejection;
    agent = p->acceptance_agent;
    break;
  default:
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(e->issuer, agent) != 0)
    return UMI_STATUS_PERMISSION_DENIED;
  if (slot->kind != 0)
    return Same(slot, e) ? UMI_STATUS_OK : UMI_STATUS_ALREADY_EXISTS;
  if ((e->kind == UMI_UFMP_EVIDENCE_ACCEPTANCE && p->rejection.kind != 0) ||
      (e->kind == UMI_UFMP_EVIDENCE_REJECTION && p->acceptance.kind != 0) ||
      ((e->kind == UMI_UFMP_EVIDENCE_SETTLEMENT || e->kind == UMI_UFMP_EVIDENCE_CREDIT) &&
       p->acceptance.kind == 0))
    return UMI_STATUS_INVALID_STATE;
  if (p->revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  /* Copy first so callers may submit an existing slot as the input. No live
   * state changes until identity, authority binding and transition checks pass. */
  UmiUfmpPaymentEvidence accepted = *e;
  *slot = accepted;
  ++p->revision;
  return UMI_STATUS_OK;
}
UmiStatus UmiUfmpPaymentProgressRead(const UmiUfmpPaymentProgress *p, UmiUfmpProgressState *out) {
  if (out == NULL || !Valid(p))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiUfmpProgressState state = UMI_UFMP_PROGRESS_CREATED;
  if (p->rejection.kind != 0)
    state = UMI_UFMP_PROGRESS_REJECTED;
  else if (p->settlement.kind != 0 && p->credit.kind != 0)
    state = UMI_UFMP_PROGRESS_COMPLETED;
  else if (p->settlement.kind != 0)
    state = UMI_UFMP_PROGRESS_SETTLED_AWAITING_CREDIT;
  else if (p->credit.kind != 0)
    state = UMI_UFMP_PROGRESS_CREDITED_AWAITING_SETTLEMENT;
  else if (p->acceptance.kind != 0)
    state = UMI_UFMP_PROGRESS_ACCEPTED;
  else if (p->delivery.kind != 0)
    state = UMI_UFMP_PROGRESS_DELIVERED;
  *out = state;
  return UMI_STATUS_OK;
}
const char *UmiUfmpProgressStateText(UmiUfmpProgressState state) {
  switch (state) {
  case UMI_UFMP_PROGRESS_CREATED:
    return "Created";
  case UMI_UFMP_PROGRESS_DELIVERED:
    return "Message delivered; payment not accepted";
  case UMI_UFMP_PROGRESS_ACCEPTED:
    return "Payment accepted; settlement and credit unconfirmed";
  case UMI_UFMP_PROGRESS_SETTLED_AWAITING_CREDIT:
    return "Settlement confirmed; credit unconfirmed";
  case UMI_UFMP_PROGRESS_CREDITED_AWAITING_SETTLEMENT:
    return "Credit confirmed; settlement unconfirmed";
  case UMI_UFMP_PROGRESS_COMPLETED:
    return "Settlement and beneficiary credit confirmed";
  case UMI_UFMP_PROGRESS_REJECTED:
    return "Payment rejected";
  default:
    return "Unknown";
  }
}
