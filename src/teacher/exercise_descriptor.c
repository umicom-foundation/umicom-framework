/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/exercise_descriptor.c
 *
 * PURPOSE:
 *   Describe one coding, reasoning or knowledge exercise.
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
#include "umicom/teacher/exercise_descriptor.h"

#include <string.h>

static void umi_teacher_exercise_descriptor_copy(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) {
        return;
    }
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') {
            destination[i] = source[i];
            ++i;
        }
    }
    destination[i] = '\0';
}

void umi_teacher_exercise_descriptor_init(UmiTeacherExerciseDescriptor *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->language = UMI_TEACHER_LANGUAGE_GENERAL;
    value->level = UMI_TEACHER_LEVEL_FOUNDATION;
    value->enabled = 1;
}
UmiStatus umi_teacher_exercise_descriptor_configure(UmiTeacherExerciseDescriptor *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score) {
    if (value == NULL || id == NULL || id[0] == '\0' || required_score > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_teacher_exercise_descriptor_init(value);
    umi_teacher_exercise_descriptor_copy(value->id, sizeof(value->id), id);
    umi_teacher_exercise_descriptor_copy(value->title, sizeof(value->title), title);
    value->language = language;
    value->level = level;
    value->weight = weight;
    value->required_score = required_score;
    value->revision = 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_teacher_exercise_descriptor_validate(const UmiTeacherExerciseDescriptor *value) {
    if (value == NULL || value->id[0] == '\0' || value->required_score > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    return value->enabled ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}
uint32_t umi_teacher_exercise_descriptor_priority(const UmiTeacherExerciseDescriptor *value, uint32_t relevance) {
    uint32_t bonus;
    if (value == NULL || !value->enabled) return 0U;
    relevance = umi_teacher_clamp_score(relevance);
    bonus = value->weight > 25U ? 25U : value->weight;
    return umi_teacher_clamp_score(relevance + bonus);
}
