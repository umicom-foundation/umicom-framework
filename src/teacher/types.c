/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/types.c
 *
 * PURPOSE:
 *   Define shared AI Teacher learning states, levels, languages, capacities and score helpers.
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
#include "umicom/teacher/types.h"

const char *umi_teacher_level_text(UmiTeacherLevel level) {
    switch (level) {
        case UMI_TEACHER_LEVEL_BEGINNER: return "beginner";
        case UMI_TEACHER_LEVEL_INTERMEDIATE: return "intermediate";
        case UMI_TEACHER_LEVEL_ADVANCED: return "advanced";
        case UMI_TEACHER_LEVEL_EXPERT: return "expert";
        case UMI_TEACHER_LEVEL_FOUNDATION:
        default: return "foundation";
    }
}
const char *umi_teacher_language_text(UmiTeacherLanguage language) {
    switch (language) {
        case UMI_TEACHER_LANGUAGE_C23: return "c23";
        case UMI_TEACHER_LANGUAGE_ASSEMBLY: return "assembly";
        case UMI_TEACHER_LANGUAGE_CPP: return "cpp";
        case UMI_TEACHER_LANGUAGE_CPYTHON: return "cpython";
        case UMI_TEACHER_LANGUAGE_GENERAL:
        default: return "general";
    }
}
const char *umi_teacher_state_text(UmiTeacherState state) {
    switch (state) {
        case UMI_TEACHER_STATE_LOCKED: return "locked";
        case UMI_TEACHER_STATE_READY: return "ready";
        case UMI_TEACHER_STATE_ACTIVE: return "active";
        case UMI_TEACHER_STATE_PASSED: return "passed";
        case UMI_TEACHER_STATE_FAILED: return "failed";
        case UMI_TEACHER_STATE_COMPLETED: return "completed";
        case UMI_TEACHER_STATE_UNKNOWN:
        default: return "unknown";
    }
}
uint32_t umi_teacher_percent(uint64_t value, uint64_t maximum) {
    if (maximum == 0U) return 0U;
    if (value >= maximum) return 100U;
    return (uint32_t)((value * UINT64_C(100)) / maximum);
}
uint32_t umi_teacher_clamp_score(uint32_t score) { return score > 100U ? 100U : score;
    }
