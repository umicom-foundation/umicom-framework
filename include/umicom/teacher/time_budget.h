/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/time_budget.h
 *
 * PURPOSE:
 *   Allocate bounded study time across planned activities.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable learning capability. Umicom Studio, Desk and
 *   future applications are thin consumers and do not reimplement pedagogy,
 *   progression, assessment or AI Teacher orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_TIME_BUDGET_H
#define UMICOM_TEACHER_TIME_BUDGET_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher time budget data shared with callers of this public contract.
 */
typedef struct UmiTeacherTimeBudget { uint32_t total_minutes; uint32_t reserved_minutes; uint32_t consumed_minutes; } UmiTeacherTimeBudget;
/**
 * Initialise teacher time budget from caller-provided values so later operations receive a
 * known state.
 */
void umi_teacher_time_budget_init(UmiTeacherTimeBudget *budget, uint32_t total_minutes);
/**
 * Provide the teacher time budget reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_time_budget_reserve(UmiTeacherTimeBudget *budget, uint32_t minutes);
/**
 * Provide the teacher time budget consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_time_budget_consume(UmiTeacherTimeBudget *budget, uint32_t minutes);
/**
 * Provide the teacher time budget remaining operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_time_budget_remaining(const UmiTeacherTimeBudget *budget);
/**
 * Provide the teacher time budget utilisation operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_time_budget_utilisation(const UmiTeacherTimeBudget *budget);

#ifdef __cplusplus
}
#endif

#endif
