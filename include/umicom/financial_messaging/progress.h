/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/progress.h
 *
 * PURPOSE:
 *   Keep message delivery, payment acceptance, settlement and credit evidence distinct.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_PROGRESS_H
#define UMICOM_FINANCIAL_MESSAGING_PROGRESS_H
#include "umicom/financial_messaging/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiUfmpEvidenceKind {
  UMI_UFMP_EVIDENCE_DELIVERY = 1,
  UMI_UFMP_EVIDENCE_ACCEPTANCE = 2,
  UMI_UFMP_EVIDENCE_SETTLEMENT = 3,
  UMI_UFMP_EVIDENCE_CREDIT = 4,
  UMI_UFMP_EVIDENCE_REJECTION = 5
} UmiUfmpEvidenceKind;
typedef struct UmiUfmpPaymentEvidence {
  UmiUfmpEvidenceKind kind;
  char payment_id[UMI_UFMP_ID_CAPACITY];
  char issuer[UMI_UFMP_ID_CAPACITY];
  char reference[UMI_UFMP_ID_CAPACITY];
  uint64_t occurred_ms;
} UmiUfmpPaymentEvidence;
typedef struct UmiUfmpPaymentProgress {
  uint64_t revision;
  char payment_id[UMI_UFMP_ID_CAPACITY];
  char delivery_agent[UMI_UFMP_ID_CAPACITY];
  char acceptance_agent[UMI_UFMP_ID_CAPACITY];
  char settlement_agent[UMI_UFMP_ID_CAPACITY];
  char credit_agent[UMI_UFMP_ID_CAPACITY];
  UmiUfmpPaymentEvidence delivery;
  UmiUfmpPaymentEvidence acceptance;
  UmiUfmpPaymentEvidence settlement;
  UmiUfmpPaymentEvidence credit;
  UmiUfmpPaymentEvidence rejection;
} UmiUfmpPaymentProgress;
typedef enum UmiUfmpProgressState {
  UMI_UFMP_PROGRESS_CREATED = 0,
  UMI_UFMP_PROGRESS_DELIVERED,
  UMI_UFMP_PROGRESS_ACCEPTED,
  UMI_UFMP_PROGRESS_SETTLED_AWAITING_CREDIT,
  UMI_UFMP_PROGRESS_CREDITED_AWAITING_SETTLEMENT,
  UMI_UFMP_PROGRESS_COMPLETED,
  UMI_UFMP_PROGRESS_REJECTED
} UmiUfmpProgressState;
/* All identities are caller-resolved trusted policy inputs, not self-asserted
 * participant permissions. This view never authenticates an issuer or moves
 * funds. It is suitable for simulations and projecting already verified,
 * durably stored evidence. Each input string must be a terminated identifier. */
UmiStatus UmiUfmpPaymentProgressInit(UmiUfmpPaymentProgress *out, const char *payment,
                                     const char *delivery_agent, const char *acceptance_agent,
                                     const char *settlement_agent, const char *credit_agent);
/* Apply one verified observation to a caller-owned projection. expected_revision
 * is mandatory and must equal the current revision. Stale/contradictory input
 * leaves progress unchanged. A duplicate exact observation is a no-op.
 * Settlement and credit require acceptance but remain independent: prefunded
 * credit can precede interbank settlement. Completion requires BOTH.
 * This is not the authoritative payment orchestrator or a persistence API.
 * Keep signature/authority checks and the durable append in the Data Server
 * transaction before publishing this projection. No I/O or allocation occurs. */
UmiStatus UmiUfmpPaymentProgressApply(UmiUfmpPaymentProgress *progress, uint64_t expected_revision,
                                      const UmiUfmpPaymentEvidence *verified_evidence);
UmiStatus UmiUfmpPaymentProgressRead(const UmiUfmpPaymentProgress *progress,
                                     UmiUfmpProgressState *out);
const char *UmiUfmpProgressStateText(UmiUfmpProgressState state);
#ifdef __cplusplus
}
#endif
#endif
