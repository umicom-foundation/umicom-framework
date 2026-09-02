/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_plan.c
 *
 * PURPOSE:
 *   Maintain an ordered learner plan of lessons, exercises and assessments.
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
#include "umicom/teacher/learning_plan.h"
#include <string.h>
/*
 * Copy teacher learning plan into module-owned storage so callers keep ownership of their
 * input values.
 */
static void umi_teacher_learning_plan_copy(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') {
            destination[i] = source[i];
            ++i;
        }
    }
    destination[i] = '\0';
}

/*
 * Initialise teacher learning plan from caller-provided values so later operations receive
 * a known state.
 */
void umi_teacher_learning_plan_init(UmiTeacherLearningPlan *sequence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence!=NULL) memset(sequence,0,sizeof(*sequence));
    }
/*
 * Add teacher learning plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_teacher_learning_plan_append(UmiTeacherLearningPlan *sequence,const char *id) { size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(sequence==NULL||id==NULL||id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<sequence->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(sequence->ids[i],id)==0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(sequence->count>=UMI_TEACHER_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_teacher_learning_plan_copy(sequence->ids[sequence->count],UMI_TEACHER_ID_CAPACITY,id);
    ++sequence->count;
    ++sequence->revision;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher learning plan current operation used by this module and its client
 * applications.
 */
const char *umi_teacher_learning_plan_current(const UmiTeacherLearningPlan *sequence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence==NULL||sequence->count==0U||sequence->cursor>=sequence->count) return NULL;
    return sequence->ids[sequence->cursor];
    }
/*
 * Provide the teacher learning plan next operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_learning_plan_next(UmiTeacherLearningPlan *sequence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(sequence->cursor+1U>=sequence->count) return UMI_STATUS_NOT_FOUND;
    ++sequence->cursor;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher learning plan previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_learning_plan_previous(UmiTeacherLearningPlan *sequence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(sequence->cursor==0U) return UMI_STATUS_NOT_FOUND;
    --sequence->cursor;
    return UMI_STATUS_OK;
    }
/*
 * Return the number of records represented by teacher learning plan without changing their
 * state.
 */
size_t umi_teacher_learning_plan_count(const UmiTeacherLearningPlan *sequence) { return sequence==NULL?0U:sequence->count;
    }
