/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_progress.c
 *
 * PURPOSE:
 *   Prevent transport receipts from becoming settlement or beneficiary-credit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  static const char *const names[] = {
      "created",       "delivery",         "acceptance",    "settlement",    "prefunded-credit",
      "complete",      "wrong-issuer",     "wrong-payment", "stale",         "no-acceptance",
      "rejected",      "late-rejection",   "duplicate",     "contradiction", "revision-overflow",
      "corrupt-state", "invalid-evidence", "null"};
  int c = Case(argc, argv, names, sizeof names / sizeof names[0]);
  UmiUfmpPaymentProgress p = Progress();
  UmiUfmpProgressState state;
  OK(UmiUfmpPaymentProgressRead(&p, &state));
  CHECK(state == UMI_UFMP_PROGRESS_CREATED);
  if (c == 0)
    return 0;
  UmiUfmpPaymentEvidence e = Evidence(UMI_UFMP_EVIDENCE_DELIVERY);
  if (c == 2 || c == 3 || c == 4 || c == 5 || c == 11)
    e = Evidence(UMI_UFMP_EVIDENCE_ACCEPTANCE);
  if (c == 10)
    e = Evidence(UMI_UFMP_EVIDENCE_REJECTION);
  UmiStatus expected = UMI_STATUS_OK;
  uint64_t revision = p.revision;
  if (c == 6) {
    strcpy(e.issuer, "untrusted.issuer");
    expected = UMI_STATUS_PERMISSION_DENIED;
  }
  if (c == 7) {
    strcpy(e.payment_id, "payment.2");
    expected = UMI_STATUS_NOT_FOUND;
  }
  if (c == 8) {
    ++revision;
    expected = UMI_STATUS_BUSY;
  }
  if (c == 9) {
    e = Evidence(UMI_UFMP_EVIDENCE_SETTLEMENT);
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (c == 14) {
    p.revision = UINT64_MAX;
    revision = p.revision;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  if (c == 15) {
    p.credit = Evidence(UMI_UFMP_EVIDENCE_CREDIT);
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (c == 16) {
    memset(e.reference, 'x', sizeof e.reference);
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (c == 17) {
    CHECK(UmiUfmpPaymentProgressApply(NULL, 1U, &e) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUfmpPaymentProgressApply(&p, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUfmpPaymentProgressRead(&p, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  UmiUfmpPaymentProgress before = p;
  CHECK(UmiUfmpPaymentProgressApply(&p, revision, &e) == expected);
  if (expected != UMI_STATUS_OK) {
    CHECK(memcmp(&p, &before, sizeof p) == 0);
    return 0;
  }
  OK(UmiUfmpPaymentProgressRead(&p, &state));
  if (c == 1)
    CHECK(state == UMI_UFMP_PROGRESS_DELIVERED);
  if (c == 2)
    CHECK(state == UMI_UFMP_PROGRESS_ACCEPTED);
  if (c == 3 || c == 4 || c == 5) {
    e = Evidence(c == 3 ? UMI_UFMP_EVIDENCE_SETTLEMENT : UMI_UFMP_EVIDENCE_CREDIT);
    OK(UmiUfmpPaymentProgressApply(&p, p.revision, &e));
    OK(UmiUfmpPaymentProgressRead(&p, &state));
    CHECK(state == (c == 3 ? UMI_UFMP_PROGRESS_SETTLED_AWAITING_CREDIT
                           : UMI_UFMP_PROGRESS_CREDITED_AWAITING_SETTLEMENT));
    if (c == 5) {
      e = Evidence(UMI_UFMP_EVIDENCE_SETTLEMENT);
      OK(UmiUfmpPaymentProgressApply(&p, p.revision, &e));
      OK(UmiUfmpPaymentProgressRead(&p, &state));
      CHECK(state == UMI_UFMP_PROGRESS_COMPLETED);
    }
  }
  if (c == 10 || c == 11) {
    e = Evidence(c == 10 ? UMI_UFMP_EVIDENCE_ACCEPTANCE : UMI_UFMP_EVIDENCE_REJECTION);
    before = p;
    CHECK(UmiUfmpPaymentProgressApply(&p, p.revision, &e) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&p, &before, sizeof p) == 0);
  }
  if (c == 12 || c == 13) {
    before = p;
    if (c == 13)
      strcpy(e.reference, "changed.reference");
    CHECK(UmiUfmpPaymentProgressApply(&p, p.revision, &e) ==
          (c == 12 ? UMI_STATUS_OK : UMI_STATUS_ALREADY_EXISTS));
    CHECK(memcmp(&p, &before, sizeof p) == 0);
  }
  return 0;
}
