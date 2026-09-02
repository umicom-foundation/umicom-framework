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
/*
 * Initialise teacher time budget from caller-provided values so later operations receive a
 * known state.
 */
void umi_teacher_time_budget_init(UmiTeacherTimeBudget *budget,uint32_t total_minutes) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(budget!=NULL) { budget->total_minutes=total_minutes;
    budget->reserved_minutes=0U;
    budget->consumed_minutes=0U;
    } }
/*
 * Provide the teacher time budget remaining operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_time_budget_remaining(const UmiTeacherTimeBudget *budget) { uint64_t used;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(budget==NULL) return 0U;
    used=(uint64_t)budget->reserved_minutes+budget->consumed_minutes;
    return used>=budget->total_minutes?0U:(uint32_t)((uint64_t)budget->total_minutes-used);
    }
/*
 * Provide the teacher time budget reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_time_budget_reserve(UmiTeacherTimeBudget *budget,uint32_t minutes) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(budget==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(minutes>umi_teacher_time_budget_remaining(budget)) return UMI_STATUS_CAPACITY_EXCEEDED;
    budget->reserved_minutes+=minutes;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher time budget consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_time_budget_consume(UmiTeacherTimeBudget *budget,uint32_t minutes) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(budget==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(minutes>budget->reserved_minutes+umi_teacher_time_budget_remaining(budget)) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(minutes<=budget->reserved_minutes) budget->reserved_minutes-=minutes;
    /* Use this fallback path when the earlier condition does not apply. */
    else budget->reserved_minutes=0U;
    budget->consumed_minutes+=minutes;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher time budget utilisation operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_time_budget_utilisation(const UmiTeacherTimeBudget *budget) { return budget==NULL?0U:umi_teacher_percent((uint64_t)budget->reserved_minutes+budget->consumed_minutes,budget->total_minutes);
    }
