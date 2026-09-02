/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/types.h
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
#ifndef UMICOM_TEACHER_TYPES_H
#define UMICOM_TEACHER_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEACHER_ID_CAPACITY 96U
#define UMI_TEACHER_TEXT_CAPACITY 256U
#define UMI_TEACHER_PATH_CAPACITY 512U
#define UMI_TEACHER_SMALL_CAPACITY 32U
#define UMI_TEACHER_MEDIUM_CAPACITY 64U

/**
 * List the named teacher level values accepted by this public contract.
 */
typedef enum UmiTeacherLevel {
    UMI_TEACHER_LEVEL_FOUNDATION = 0,
    UMI_TEACHER_LEVEL_BEGINNER = 1,
    UMI_TEACHER_LEVEL_INTERMEDIATE = 2,
    UMI_TEACHER_LEVEL_ADVANCED = 3,
    UMI_TEACHER_LEVEL_EXPERT = 4
} UmiTeacherLevel;

/**
 * List the named teacher language values accepted by this public contract.
 */
typedef enum UmiTeacherLanguage {
    UMI_TEACHER_LANGUAGE_GENERAL = 0,
    UMI_TEACHER_LANGUAGE_C23 = 1,
    UMI_TEACHER_LANGUAGE_ASSEMBLY = 2,
    UMI_TEACHER_LANGUAGE_CPP = 3,
    UMI_TEACHER_LANGUAGE_CPYTHON = 4
} UmiTeacherLanguage;

/**
 * List the named teacher state values accepted by this public contract.
 */
typedef enum UmiTeacherState {
    UMI_TEACHER_STATE_UNKNOWN = 0,
    UMI_TEACHER_STATE_LOCKED = 1,
    UMI_TEACHER_STATE_READY = 2,
    UMI_TEACHER_STATE_ACTIVE = 3,
    UMI_TEACHER_STATE_PASSED = 4,
    UMI_TEACHER_STATE_FAILED = 5,
    UMI_TEACHER_STATE_COMPLETED = 6
} UmiTeacherState;

/**
 * List the named teacher progress decision values accepted by this public contract.
 */
typedef enum UmiTeacherProgressDecision {
    UMI_TEACHER_PROGRESS_HOLD = 0,
    UMI_TEACHER_PROGRESS_REMEDIATE = 1,
    UMI_TEACHER_PROGRESS_REPEAT = 2,
    UMI_TEACHER_PROGRESS_ADVANCE = 3
} UmiTeacherProgressDecision;

/**
 * Provide the teacher level text operation used by this module and its client
 * applications.
 */
const char *umi_teacher_level_text(UmiTeacherLevel level);
/**
 * Provide the teacher language text operation used by this module and its client
 * applications.
 */
const char *umi_teacher_language_text(UmiTeacherLanguage language);
/**
 * Provide the teacher state text operation used by this module and its client
 * applications.
 */
const char *umi_teacher_state_text(UmiTeacherState state);
/**
 * Provide the teacher percent operation used by this module and its client applications.
 */
uint32_t umi_teacher_percent(uint64_t value, uint64_t maximum);
/**
 * Provide the teacher clamp score operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_clamp_score(uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
