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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTeacherTimeBudget { uint32_t total_minutes; uint32_t reserved_minutes; uint32_t consumed_minutes; } UmiTeacherTimeBudget;
void umi_teacher_time_budget_init(UmiTeacherTimeBudget *budget, uint32_t total_minutes);
UmiStatus umi_teacher_time_budget_reserve(UmiTeacherTimeBudget *budget, uint32_t minutes);
UmiStatus umi_teacher_time_budget_consume(UmiTeacherTimeBudget *budget, uint32_t minutes);
uint32_t umi_teacher_time_budget_remaining(const UmiTeacherTimeBudget *budget);
uint32_t umi_teacher_time_budget_utilisation(const UmiTeacherTimeBudget *budget);

#ifdef __cplusplus
}
#endif

#endif
