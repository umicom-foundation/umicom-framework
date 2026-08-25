/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_objective.h
 *
 * PURPOSE:
 *   Describe one measurable learning objective.
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
#ifndef UMICOM_TEACHER_LEARNING_OBJECTIVE_H
#define UMICOM_TEACHER_LEARNING_OBJECTIVE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherLearningObjective {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherLearningObjective;

void umi_teacher_learning_objective_init(UmiTeacherLearningObjective *value);
UmiStatus umi_teacher_learning_objective_configure(UmiTeacherLearningObjective *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_learning_objective_validate(const UmiTeacherLearningObjective *value);
uint32_t umi_teacher_learning_objective_priority(const UmiTeacherLearningObjective *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
