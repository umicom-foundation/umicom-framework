/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/deadline.h
 *
 * PURPOSE:
 *   Represent monotonic operation deadlines and remaining budgets without
 *   platform-specific timer types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_DEADLINE_H
#define UMICOM_RESILIENCE_DEADLINE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the deadline data shared with callers of this public contract.
 */
typedef struct UmiDeadline { uint64_t expires_at_ns; } UmiDeadline;
/**
 * Provide the deadline after operation used by this module and its client applications.
 */
UmiDeadline umi_deadline_after(uint64_t now_ns, uint64_t duration_ns);
/**
 * Provide the deadline expired operation used by this module and its client applications.
 */
int umi_deadline_expired(UmiDeadline deadline, uint64_t now_ns);
/**
 * Provide the deadline remaining operation used by this module and its client
 * applications.
 */
uint64_t umi_deadline_remaining(UmiDeadline deadline, uint64_t now_ns);
#ifdef __cplusplus
}
#endif

#endif
