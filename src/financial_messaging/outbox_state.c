/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/outbox_state.c
 *
 * PURPOSE:
 *   Validate persisted delivery states and bound retry arithmetic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "outbox_internal.h"
#include <inttypes.h>
#include <stdio.h>
int UmiUfmpRetryValid(const UmiUfmpRetryPolicy *p) {
  return p != NULL && p->maximum_attempts > 0U && p->maximum_attempts <= 32U &&
    p->initial_delay_ms > 0U && p->maximum_delay_ms >= p->initial_delay_ms;
}
const char *UmiUfmpOutboxStateName(UmiUfmpOutboxState state) {
  switch (state) {
  case UMI_UFMP_OUTBOX_READY: return "ready";
  case UMI_UFMP_OUTBOX_IN_FLIGHT: return "in-flight";
  case UMI_UFMP_OUTBOX_RECEIVED: return "transport-received";
  case UMI_UFMP_OUTBOX_REVIEW: return "manual-review";
  case UMI_UFMP_OUTBOX_EXPIRED: return "expired";
  default: return "invalid";
  }
}
int UmiUfmpOutboxRecordValid(const UmiUfmpOutboxRecord *r) {
  if (!UmiUfmpRetryValid(&r->retry) || r->revision == 0U ||
      r->attempts > r->retry.maximum_attempts) return 0;
  if (r->state == UMI_UFMP_OUTBOX_READY)
    return r->attempts < r->retry.maximum_attempts && r->next_attempt_ms >= r->updated_ms &&
      r->next_attempt_ms < r->message.envelope.expires_ms;
  if (r->next_attempt_ms != 0U) return 0;
  if (r->state == UMI_UFMP_OUTBOX_EXPIRED)
    return r->updated_ms >= r->message.envelope.expires_ms || r->attempts > 0U;
  return r->attempts > 0U &&
    (r->state == UMI_UFMP_OUTBOX_IN_FLIGHT || r->state == UMI_UFMP_OUTBOX_RECEIVED ||
     r->state == UMI_UFMP_OUTBOX_REVIEW);
}
static int Number(const char *text, uint64_t *out) {
  if (!text[0] || (text[0] == '0' && text[1])) return 0;
  uint64_t n = 0U;
  for (size_t i = 0U; text[i]; ++i) {
    if (text[i] < '0' || text[i] > '9') return 0;
    uint64_t d = (uint64_t)(text[i] - '0');
    if (n > (UINT64_MAX - d) / 10U) return 0;
    n = n * 10U + d;
  }
  *out = n; return 1;
}
UmiStatus UmiUfmpOutboxStateEncode(const UmiUfmpOutboxRecord *r, char out[256]) {
  if (r == NULL || out == NULL || !UmiUfmpOutboxRecordValid(r)) return UMI_STATUS_INVALID_ARGUMENT;
  int n = snprintf(out, 256U, "UFMP-OUTBOX-1\n%" PRIu64 "\n%u\n%" PRIu32 "\n%" PRIu32
    "\n%" PRIu64 "\n%" PRIu64 "\n%" PRIu64 "\n%" PRIu64,
    r->revision, (unsigned)r->state, r->attempts, r->retry.maximum_attempts,
    r->retry.initial_delay_ms, r->retry.maximum_delay_ms, r->updated_ms, r->next_attempt_ms);
  return n < 0 || n >= 256 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
UmiStatus UmiUfmpOutboxStateDecode(char text[256], UmiUfmpOutboxRecord *r) {
  if (text == NULL || r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
  if (memchr(text, '\0', 256U) == NULL) return UMI_STATUS_PARSE_ERROR;
  char *fields[9] = {text}; size_t count = 1U;
  for (char *p = text; *p; ++p) if (*p == '\n') {
    if (count == 9U) return UMI_STATUS_PARSE_ERROR;
    *p = '\0'; fields[count++] = p + 1;
  }
  uint64_t n[8];
  if (count != 9U || strcmp(fields[0], "UFMP-OUTBOX-1") != 0) return UMI_STATUS_PARSE_ERROR;
  for (size_t i = 0U; i < 8U; ++i) if (!Number(fields[i+1U], &n[i])) return UMI_STATUS_PARSE_ERROR;
  if (n[1] < 1U || n[1] > 5U || n[2] > UINT32_MAX || n[3] > 32U) return UMI_STATUS_PARSE_ERROR;
  UmiUfmpOutboxRecord copy = *r;
  copy.revision = n[0]; copy.state = (UmiUfmpOutboxState)n[1];
  copy.attempts = (uint32_t)n[2]; copy.retry.maximum_attempts = (uint32_t)n[3];
  copy.retry.initial_delay_ms = n[4]; copy.retry.maximum_delay_ms = n[5];
  copy.updated_ms = n[6]; copy.next_attempt_ms = n[7];
  if (!UmiUfmpOutboxRecordValid(&copy)) return UMI_STATUS_PARSE_ERROR;
  *r = copy; return UMI_STATUS_OK;
}
UmiStatus UmiUfmpOutboxApply(UmiUfmpOutboxRecord *r, uint64_t now, UmiUfmpOutboxEvent event) {
  if (!UmiUfmpOutboxRecordValid(r)) return UMI_STATUS_PARSE_ERROR;
  if (event < UMI_UFMP_OUTBOX_BEGIN || event > UMI_UFMP_OUTBOX_RECOVER ||
      now < r->updated_ms) return UMI_STATUS_INVALID_ARGUMENT;
  if (r->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiUfmpOutboxRecord copy = *r;
  if (event == UMI_UFMP_OUTBOX_BEGIN) {
    if (r->state != UMI_UFMP_OUTBOX_READY) return UMI_STATUS_INVALID_STATE;
    if (now >= r->message.envelope.expires_ms) return UMI_STATUS_TIMEOUT;
    if (now < r->next_attempt_ms) return UMI_STATUS_BUSY;
    copy.state = UMI_UFMP_OUTBOX_IN_FLIGHT; copy.attempts++;
  } else if (event == UMI_UFMP_OUTBOX_RECOVER && r->state == UMI_UFMP_OUTBOX_READY &&
             now >= r->message.envelope.expires_ms) {
    copy.state = UMI_UFMP_OUTBOX_EXPIRED;
  } else {
    if (r->state != UMI_UFMP_OUTBOX_IN_FLIGHT) return UMI_STATUS_INVALID_STATE;
    copy.state = event == UMI_UFMP_OUTBOX_RECEIPT ? UMI_UFMP_OUTBOX_RECEIVED : UMI_UFMP_OUTBOX_REVIEW;
    if (event == UMI_UFMP_OUTBOX_NOT_SENT) {
      if (now >= r->message.envelope.expires_ms) copy.state = UMI_UFMP_OUTBOX_EXPIRED;
      else if (r->attempts < r->retry.maximum_attempts) {
        uint64_t delay = r->retry.initial_delay_ms;
        for (uint32_t i = 1U; i < r->attempts && delay < r->retry.maximum_delay_ms; ++i)
          delay = delay > r->retry.maximum_delay_ms / 2U ? r->retry.maximum_delay_ms : delay * 2U;
        /* Check before addition: wrapping must never authorize an immediate
         * retry. Expiry also bounds the result below UINT64_MAX. */
        if (delay >= r->message.envelope.expires_ms - now) copy.state = UMI_UFMP_OUTBOX_EXPIRED;
        else { copy.state = UMI_UFMP_OUTBOX_READY; copy.next_attempt_ms = now + delay; }
      }
    }
  }
  if (copy.state != UMI_UFMP_OUTBOX_READY) copy.next_attempt_ms = 0U;
  copy.updated_ms = now; copy.revision++;
  if (!UmiUfmpOutboxRecordValid(&copy)) return UMI_STATUS_INVALID_STATE;
  *r = copy; return UMI_STATUS_OK;
}
