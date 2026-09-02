/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/deadline.c
 *
 * PURPOSE:
 *   Implement monotonic deadline construction, expiry and remaining-budget
 *   calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/deadline.h"
/* Provide the deadline after operation used by this module and its client applications. */
UmiDeadline umi_deadline_after(uint64_t now_ns, uint64_t duration_ns)
{ UmiDeadline d; d.expires_at_ns = UINT64_MAX - now_ns < duration_ns ? UINT64_MAX : now_ns + duration_ns; return d; }
/* Provide the deadline expired operation used by this module and its client applications. */
int umi_deadline_expired(UmiDeadline deadline, uint64_t now_ns) { return now_ns >= deadline.expires_at_ns; }
/*
 * Provide the deadline remaining operation used by this module and its client
 * applications.
 */
uint64_t umi_deadline_remaining(UmiDeadline deadline, uint64_t now_ns) { return now_ns >= deadline.expires_at_ns ? 0U : deadline.expires_at_ns - now_ns; }
