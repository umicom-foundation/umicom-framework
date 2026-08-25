/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/study_streak.h
 *
 * PURPOSE:
 *   Track consecutive study periods without imposing UI behaviour.
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
#ifndef UMICOM_TEACHER_STUDY_STREAK_H
#define UMICOM_TEACHER_STUDY_STREAK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherStudyStreak {
    UmiTeacherState state;
    uint32_t score;
    uint32_t attempts;
    uint32_t completed_units;
    uint32_t total_units;
    uint64_t revision;
} UmiTeacherStudyStreak;

void umi_teacher_study_streak_init(UmiTeacherStudyStreak *value);
UmiStatus umi_teacher_study_streak_record(UmiTeacherStudyStreak *value, uint32_t score, int passed);
void umi_teacher_study_streak_set_units(UmiTeacherStudyStreak *value, uint32_t completed_units, uint32_t total_units);
uint32_t umi_teacher_study_streak_completion(const UmiTeacherStudyStreak *value);
int umi_teacher_study_streak_complete(const UmiTeacherStudyStreak *value);

#ifdef __cplusplus
}
#endif

#endif
