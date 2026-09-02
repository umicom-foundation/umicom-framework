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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the teacher curriculum track data shared with callers of this public contract.
 */
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

/**
 * Initialise teacher curriculum track from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_curriculum_track_init(UmiTeacherCurriculumTrack *value);
/**
 * Provide the teacher curriculum track configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_curriculum_track_configure(UmiTeacherCurriculumTrack *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher curriculum track satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_teacher_curriculum_track_validate(const UmiTeacherCurriculumTrack *value);
/**
 * Provide the teacher curriculum track priority operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_curriculum_track_priority(const UmiTeacherCurriculumTrack *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
