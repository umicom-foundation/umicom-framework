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

typedef struct UmiTeacherLearningPlan {
    char ids[UMI_TEACHER_MEDIUM_CAPACITY][UMI_TEACHER_ID_CAPACITY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiTeacherLearningPlan;

void umi_teacher_learning_plan_init(UmiTeacherLearningPlan *sequence);
UmiStatus umi_teacher_learning_plan_append(UmiTeacherLearningPlan *sequence, const char *id);
const char *umi_teacher_learning_plan_current(const UmiTeacherLearningPlan *sequence);
UmiStatus umi_teacher_learning_plan_next(UmiTeacherLearningPlan *sequence);
UmiStatus umi_teacher_learning_plan_previous(UmiTeacherLearningPlan *sequence);
size_t umi_teacher_learning_plan_count(const UmiTeacherLearningPlan *sequence);

#ifdef __cplusplus
}
#endif

#endif
