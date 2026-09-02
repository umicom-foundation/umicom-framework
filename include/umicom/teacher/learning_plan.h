/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_plan.h
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
#ifndef UMICOM_TEACHER_LEARNING_PLAN_H
#define UMICOM_TEACHER_LEARNING_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher learning plan data shared with callers of this public contract.
 */
typedef struct UmiTeacherLearningPlan {
    char ids[UMI_TEACHER_MEDIUM_CAPACITY][UMI_TEACHER_ID_CAPACITY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiTeacherLearningPlan;

/**
 * Initialise teacher learning plan from caller-provided values so later operations receive
 * a known state.
 */
void umi_teacher_learning_plan_init(UmiTeacherLearningPlan *sequence);
/**
 * Add teacher learning plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_teacher_learning_plan_append(UmiTeacherLearningPlan *sequence, const char *id);
/**
 * Provide the teacher learning plan current operation used by this module and its client
 * applications.
 */
const char *umi_teacher_learning_plan_current(const UmiTeacherLearningPlan *sequence);
/**
 * Provide the teacher learning plan next operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_learning_plan_next(UmiTeacherLearningPlan *sequence);
/**
 * Provide the teacher learning plan previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_learning_plan_previous(UmiTeacherLearningPlan *sequence);
/**
 * Return the number of records represented by teacher learning plan without changing their
 * state.
 */
size_t umi_teacher_learning_plan_count(const UmiTeacherLearningPlan *sequence);

#ifdef __cplusplus
}
#endif

#endif
