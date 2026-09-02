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
/*
 * Initialise teacher spaced repetition from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_spaced_repetition_init(UmiTeacherSpacedRepetition *schedule) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(schedule!=NULL) { schedule->interval_days=1U;
    schedule->repetitions=0U;
    schedule->ease_percent=250U;
    schedule->last_quality=0U;
    } }
/*
 * Provide the teacher spaced repetition record operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_spaced_repetition_record(UmiTeacherSpacedRepetition *schedule,uint32_t recall_quality) { uint64_t next;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(schedule==NULL||recall_quality>5U) return UMI_STATUS_INVALID_ARGUMENT;
    schedule->last_quality=recall_quality;
    /* Apply this branch only when its contract condition is satisfied. */
    if(recall_quality<3U) { schedule->repetitions=0U;
    schedule->interval_days=1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if(schedule->ease_percent>140U) schedule->ease_percent-=20U;
    return UMI_STATUS_OK;
    } ++schedule->repetitions;
    /* Apply this branch only when its contract condition is satisfied. */
    if(schedule->repetitions==1U) schedule->interval_days=1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if(schedule->repetitions==2U) schedule->interval_days=6U;
    /* Use this fallback path when the earlier condition does not apply. */
    else { next=(uint64_t)schedule->interval_days*schedule->ease_percent/100U;
    schedule->interval_days=next>3650U?3650U:(uint32_t)next;
    } /* Apply this branch only when its contract condition is satisfied. */ if(recall_quality==5U && schedule->ease_percent<300U) schedule->ease_percent+=10U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher spaced repetition next interval operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_spaced_repetition_next_interval(const UmiTeacherSpacedRepetition *schedule) { return schedule==NULL?0U:schedule->interval_days;
    }
