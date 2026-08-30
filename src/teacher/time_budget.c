/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/time_budget.c
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
#include "umicom/teacher/time_budget.h"
void umi_teacher_time_budget_init(UmiTeacherTimeBudget *budget,uint32_t total_minutes) { if(budget!=NULL) { budget->total_minutes=total_minutes;
    budget->reserved_minutes=0U;
    budget->consumed_minutes=0U;
    } }
uint32_t umi_teacher_time_budget_remaining(const UmiTeacherTimeBudget *budget) { uint64_t used;
    if(budget==NULL) return 0U;
    used=(uint64_t)budget->reserved_minutes+budget->consumed_minutes;
    return used>=budget->total_minutes?0U:(uint32_t)((uint64_t)budget->total_minutes-used);
    }
UmiStatus umi_teacher_time_budget_reserve(UmiTeacherTimeBudget *budget,uint32_t minutes) { if(budget==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(minutes>umi_teacher_time_budget_remaining(budget)) return UMI_STATUS_CAPACITY_EXCEEDED;
    budget->reserved_minutes+=minutes;
    return UMI_STATUS_OK;
    }
UmiStatus umi_teacher_time_budget_consume(UmiTeacherTimeBudget *budget,uint32_t minutes) { if(budget==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(minutes>budget->reserved_minutes+umi_teacher_time_budget_remaining(budget)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(minutes<=budget->reserved_minutes) budget->reserved_minutes-=minutes;
    else budget->reserved_minutes=0U;
    budget->consumed_minutes+=minutes;
    return UMI_STATUS_OK;
    }
uint32_t umi_teacher_time_budget_utilisation(const UmiTeacherTimeBudget *budget) { return budget==NULL?0U:umi_teacher_percent((uint64_t)budget->reserved_minutes+budget->consumed_minutes,budget->total_minutes);
    }
