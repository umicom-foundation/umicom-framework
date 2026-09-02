/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_outcome.h
 *
 * PURPOSE:
 *   Track evidence that a learning objective has been achieved.
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
#ifndef UMICOM_TEACHER_LEARNING_OUTCOME_H
#define UMICOM_TEACHER_LEARNING_OUTCOME_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher learning outcome data shared with callers of this public contract.
 */
typedef struct UmiTeacherLearningOutcome {
    UmiTeacherState state;
    uint32_t score;
    uint32_t attempts;
    uint32_t completed_units;
    uint32_t total_units;
    uint64_t revision;
} UmiTeacherLearningOutcome;

/**
 * Initialise teacher learning outcome from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_learning_outcome_init(UmiTeacherLearningOutcome *value);
/**
 * Provide the teacher learning outcome record operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_learning_outcome_record(UmiTeacherLearningOutcome *value, uint32_t score, int passed);
/**
 * Provide the teacher learning outcome set units operation used by this module and its
 * client applications.
 */
void umi_teacher_learning_outcome_set_units(UmiTeacherLearningOutcome *value, uint32_t completed_units, uint32_t total_units);
/**
 * Provide the teacher learning outcome completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_learning_outcome_completion(const UmiTeacherLearningOutcome *value);
/**
 * Provide the teacher learning outcome complete operation used by this module and its
 * client applications.
 */
int umi_teacher_learning_outcome_complete(const UmiTeacherLearningOutcome *value);

#ifdef __cplusplus
}
#endif

#endif
