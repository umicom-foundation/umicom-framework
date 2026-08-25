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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTeacherLessonCheckpoint {
    UmiTeacherState state;
    uint32_t score;
    uint32_t attempts;
    uint32_t completed_units;
    uint32_t total_units;
    uint64_t revision;
} UmiTeacherLessonCheckpoint;

void umi_teacher_lesson_checkpoint_init(UmiTeacherLessonCheckpoint *value);
UmiStatus umi_teacher_lesson_checkpoint_record(UmiTeacherLessonCheckpoint *value, uint32_t score, int passed);
void umi_teacher_lesson_checkpoint_set_units(UmiTeacherLessonCheckpoint *value, uint32_t completed_units, uint32_t total_units);
uint32_t umi_teacher_lesson_checkpoint_completion(const UmiTeacherLessonCheckpoint *value);
int umi_teacher_lesson_checkpoint_complete(const UmiTeacherLessonCheckpoint *value);

#ifdef __cplusplus
}
#endif

#endif
