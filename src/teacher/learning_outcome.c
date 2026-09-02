/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_outcome.c
 *
 * PURPOSE:
 *   Track evidence that a learning objective has been achieved.
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
#include "umicom/teacher/learning_outcome.h"
#include <string.h>
/*
 * Initialise teacher learning outcome from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_learning_outcome_init(UmiTeacherLearningOutcome *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value!=NULL) { memset(value,0,sizeof(*value));
    value->state=UMI_TEACHER_STATE_READY;
    } }
/*
 * Provide the teacher learning outcome record operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_learning_outcome_record(UmiTeacherLearningOutcome *value,uint32_t score,int passed) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value==NULL || score>100U) return UMI_STATUS_INVALID_ARGUMENT;
    value->score=score;
    ++value->attempts;
    ++value->revision;
    value->state=passed?UMI_TEACHER_STATE_PASSED:UMI_TEACHER_STATE_FAILED;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher learning outcome set units operation used by this module and its
 * client applications.
 */
void umi_teacher_learning_outcome_set_units(UmiTeacherLearningOutcome *value,uint32_t completed_units,uint32_t total_units) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value==NULL) return;
    value->total_units=total_units;
    value->completed_units=completed_units>total_units?total_units:completed_units;
    ++value->revision;
    /* Apply this branch only when its contract condition is satisfied. */
    if (total_units>0U && value->completed_units==total_units) value->state=UMI_TEACHER_STATE_COMPLETED;
    }
/*
 * Provide the teacher learning outcome completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_learning_outcome_completion(const UmiTeacherLearningOutcome *value) { return value==NULL?0U:umi_teacher_percent(value->completed_units,value->total_units);
    }
/*
 * Provide the teacher learning outcome complete operation used by this module and its
 * client applications.
 */
int umi_teacher_learning_outcome_complete(const UmiTeacherLearningOutcome *value) { return value!=NULL && value->state==UMI_TEACHER_STATE_COMPLETED;
    }
