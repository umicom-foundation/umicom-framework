/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/foundations_curriculum.h
 *
 * PURPOSE:
 *   Define the shared, ordered learning journey that takes a complete beginner
 *   from computer and C basics to an approved Umicom contribution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_FOUNDATIONS_CURRICULUM_H
#define UMICOM_TEACHER_FOUNDATIONS_CURRICULUM_H

#include "umicom/teacher/learning_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiTeacherFoundationsStage {
    UMI_TEACHER_FOUNDATIONS_ORIENTATION = 0,
    UMI_TEACHER_FOUNDATIONS_TOOLS = 1,
    UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING = 2,
    UMI_TEACHER_FOUNDATIONS_FRAMEWORK = 3,
    UMI_TEACHER_FOUNDATIONS_CONTRIBUTION = 4
} UmiTeacherFoundationsStage;

typedef struct UmiTeacherFoundationsLesson {
    const char *id;
    const char *title;
    const char *summary;
    const char *exercise;
    const char *resource_path;
    UmiTeacherFoundationsStage stage;
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t sequence;
    uint32_t estimated_minutes;
    uint32_t required_score;
    int requires_compiler;
    int requires_repository;
    int requires_github_account;
} UmiTeacherFoundationsLesson;

size_t umi_teacher_foundations_curriculum_count(void);
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_at(
    size_t index);
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_find(
    const char *lesson_id);
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_next(
    const char *lesson_id);
UmiStatus umi_teacher_foundations_curriculum_plan(
    UmiTeacherLearningPlan *out_plan);
uint32_t umi_teacher_foundations_curriculum_minutes(void);
const char *umi_teacher_foundations_stage_text(
    UmiTeacherFoundationsStage stage);

#ifdef __cplusplus
}
#endif

#endif
