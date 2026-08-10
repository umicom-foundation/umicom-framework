/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/deadline.h
 *
 * PURPOSE:
 *   Represent monotonic operation deadlines and remaining budgets without
 *   platform-specific timer types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_DEADLINE_H
#define UMICOM_RESILIENCE_DEADLINE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeadline { uint64_t expires_at_ns; } UmiDeadline;
UmiDeadline umi_deadline_after(uint64_t now_ns, uint64_t duration_ns);
int umi_deadline_expired(UmiDeadline deadline, uint64_t now_ns);
uint64_t umi_deadline_remaining(UmiDeadline deadline, uint64_t now_ns);
#ifdef __cplusplus
}
#endif

#endif
