/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/curriculum_track.h
 *
 * PURPOSE:
 *   Describe a reusable curriculum track with language, level and mastery requirements.
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
#ifndef UMICOM_TEACHER_CURRICULUM_TRACK_H
#define UMICOM_TEACHER_CURRICULUM_TRACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherCurriculumTrack {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherCurriculumTrack;

void umi_teacher_curriculum_track_init(UmiTeacherCurriculumTrack *value);
UmiStatus umi_teacher_curriculum_track_configure(UmiTeacherCurriculumTrack *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_curriculum_track_validate(const UmiTeacherCurriculumTrack *value);
uint32_t umi_teacher_curriculum_track_priority(const UmiTeacherCurriculumTrack *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
