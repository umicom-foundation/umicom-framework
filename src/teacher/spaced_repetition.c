/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/spaced_repetition.c
 *
 * PURPOSE:
 *   Schedule review intervals from recall quality and prior interval.
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
#include "umicom/teacher/spaced_repetition.h"
void umi_teacher_spaced_repetition_init(UmiTeacherSpacedRepetition *schedule) { if(schedule!=NULL) { schedule->interval_days=1U;
    schedule->repetitions=0U;
    schedule->ease_percent=250U;
    schedule->last_quality=0U;
    } }
UmiStatus umi_teacher_spaced_repetition_record(UmiTeacherSpacedRepetition *schedule,uint32_t recall_quality) { uint64_t next;
    if(schedule==NULL||recall_quality>5U) return UMI_STATUS_INVALID_ARGUMENT;
    schedule->last_quality=recall_quality;
    if(recall_quality<3U) { schedule->repetitions=0U;
    schedule->interval_days=1U;
    if(schedule->ease_percent>140U) schedule->ease_percent-=20U;
    return UMI_STATUS_OK;
    } ++schedule->repetitions;
    if(schedule->repetitions==1U) schedule->interval_days=1U;
    else if(schedule->repetitions==2U) schedule->interval_days=6U;
    else { next=(uint64_t)schedule->interval_days*schedule->ease_percent/100U;
    schedule->interval_days=next>3650U?3650U:(uint32_t)next;
    } if(recall_quality==5U && schedule->ease_percent<300U) schedule->ease_percent+=10U;
    return UMI_STATUS_OK;
    }
uint32_t umi_teacher_spaced_repetition_next_interval(const UmiTeacherSpacedRepetition *schedule) { return schedule==NULL?0U:schedule->interval_days;
    }
