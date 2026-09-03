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

#include <stddef.h>
#include <stdint.h>

#include "umicom/teacher/learning_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named teacher foundations stage values accepted by this public contract.
 */
typedef enum UmiTeacherFoundationsStage {
    UMI_TEACHER_FOUNDATIONS_ORIENTATION = 0,
    UMI_TEACHER_FOUNDATIONS_TOOLS = 1,
    UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING = 2,
    UMI_TEACHER_FOUNDATIONS_FRAMEWORK = 3,
    UMI_TEACHER_FOUNDATIONS_CONTRIBUTION = 4
} UmiTeacherFoundationsStage;

/**
 * Represent the teacher foundations lesson data shared with callers of this public
 * contract.
 */
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

/**
 * Return the number of records represented by teacher foundations curriculum without
 * changing their state.
 */
size_t umi_teacher_foundations_curriculum_count(void);
/**
 * Find teacher foundations curriculum while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_at(
    size_t index);
/**
 * Find teacher foundations curriculum while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_find(
    const char *lesson_id);
/**
 * Provide the teacher foundations curriculum next operation used by this module and its
 * client applications.
 */
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_next(
    const char *lesson_id);
/**
 * Provide the teacher foundations curriculum plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_foundations_curriculum_plan(
    UmiTeacherLearningPlan *out_plan);
/**
 * Provide the teacher foundations curriculum minutes operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_foundations_curriculum_minutes(void);
/**
 * Provide the teacher foundations stage text operation used by this module and its client
 * applications.
 */
const char *umi_teacher_foundations_stage_text(
    UmiTeacherFoundationsStage stage);

#ifdef __cplusplus
}
#endif

#endif
