/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/test_case_spec.h
 *
 * PURPOSE:
 *   Describe one expected test case and its pedagogical weight.
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
#ifndef UMICOM_TEACHER_TEST_CASE_SPEC_H
#define UMICOM_TEACHER_TEST_CASE_SPEC_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher test case spec data shared with callers of this public contract.
 */
typedef struct UmiTeacherTestCaseSpec {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherTestCaseSpec;

/**
 * Initialise teacher test case spec from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_test_case_spec_init(UmiTeacherTestCaseSpec *value);
/**
 * Provide the teacher test case spec configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_test_case_spec_configure(UmiTeacherTestCaseSpec *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher test case spec satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_teacher_test_case_spec_validate(const UmiTeacherTestCaseSpec *value);
/**
 * Provide the teacher test case spec priority operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_test_case_spec_priority(const UmiTeacherTestCaseSpec *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
