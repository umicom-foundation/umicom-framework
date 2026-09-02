/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/teacher_assessment_bridge.c
 *
 * PURPOSE:
 *   Bind Teacher assessments to existing Framework compiler/testing services.
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
#include "umicom/teacher/teacher_assessment_bridge.h"

#include <string.h>

/*
 * Copy teacher teacher assessment bridge into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_teacher_teacher_assessment_bridge_copy(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') {
            destination[i] = source[i];
            ++i;
        }
    }
    destination[i] = '\0';
}

/*
 * Initialise teacher teacher assessment bridge from caller-provided values so later
 * operations receive a known state.
 */
void umi_teacher_teacher_assessment_bridge_init(UmiTeacherTeacherAssessmentBridge *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->language = UMI_TEACHER_LANGUAGE_GENERAL;
    value->level = UMI_TEACHER_LEVEL_FOUNDATION;
    value->enabled = 1;
}
/*
 * Provide the teacher teacher assessment bridge configure operation used by this module
 * and its client applications.
 */
UmiStatus umi_teacher_teacher_assessment_bridge_configure(UmiTeacherTeacherAssessmentBridge *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || id == NULL || id[0] == '\0' || required_score > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_teacher_teacher_assessment_bridge_init(value);
    umi_teacher_teacher_assessment_bridge_copy(value->id, sizeof(value->id), id);
    umi_teacher_teacher_assessment_bridge_copy(value->title, sizeof(value->title), title);
    value->language = language;
    value->level = level;
    value->weight = weight;
    value->required_score = required_score;
    value->revision = 1U;
    return UMI_STATUS_OK;
}
/*
 * Check that teacher teacher assessment bridge satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_teacher_teacher_assessment_bridge_validate(const UmiTeacherTeacherAssessmentBridge *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->id[0] == '\0' || value->required_score > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    return value->enabled ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}
/*
 * Provide the teacher teacher assessment bridge priority operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_teacher_assessment_bridge_priority(const UmiTeacherTeacherAssessmentBridge *value, uint32_t relevance) {
    uint32_t bonus;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !value->enabled) return 0U;
    relevance = umi_teacher_clamp_score(relevance);
    bonus = value->weight > 25U ? 25U : value->weight;
    return umi_teacher_clamp_score(relevance + bonus);
}
