/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/lesson_checkpoint.h
 *
 * PURPOSE:
 *   Capture durable learner checkpoint state between lesson activities.
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
#ifndef UMICOM_TEACHER_LESSON_CHECKPOINT_H
#define UMICOM_TEACHER_LESSON_CHECKPOINT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher lesson checkpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherLessonCheckpoint {
    UmiTeacherState state;
    uint32_t score;
    uint32_t attempts;
    uint32_t completed_units;
    uint32_t total_units;
    uint64_t revision;
} UmiTeacherLessonCheckpoint;

/**
 * Initialise teacher lesson checkpoint from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_lesson_checkpoint_init(UmiTeacherLessonCheckpoint *value);
/**
 * Provide the teacher lesson checkpoint record operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_lesson_checkpoint_record(UmiTeacherLessonCheckpoint *value, uint32_t score, int passed);
/**
 * Provide the teacher lesson checkpoint set units operation used by this module and its
 * client applications.
 */
void umi_teacher_lesson_checkpoint_set_units(UmiTeacherLessonCheckpoint *value, uint32_t completed_units, uint32_t total_units);
/**
 * Provide the teacher lesson checkpoint completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_lesson_checkpoint_completion(const UmiTeacherLessonCheckpoint *value);
/**
 * Provide the teacher lesson checkpoint complete operation used by this module and its
 * client applications.
 */
int umi_teacher_lesson_checkpoint_complete(const UmiTeacherLessonCheckpoint *value);

#ifdef __cplusplus
}
#endif

#endif
