/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/course_companions.h
 * PURPOSE: Audit corresponding editable Markdown and published HTML lessons
 *          in the existing Bits2Banking content/courses and content/lessons.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_COURSE_COMPANIONS_H
#define UMICOM_EDUCATION_COURSE_COMPANIONS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_COMPANION_MAX_SPAN 512U

typedef enum UmiEducationCourseKind {
    UMI_EDUCATION_COURSE_NATIVE_AI = 1,
    UMI_EDUCATION_COURSE_MATHEMATICS = 2
} UmiEducationCourseKind;

typedef struct UmiEducationCourseCompanionReport {
    uint32_t firstNumber;
    uint32_t lastNumber;
    size_t completePairs;
    size_t markdownMissing;
    size_t htmlMissing;
    size_t bothMissing;
    size_t differentSlugs;
    size_t duplicateNumberedPages;
} UmiEducationCourseCompanionReport;

/* Read the two canonical directories for the selected course. Only numeric
 * <number>-<slug>.md / .html companions are considered; supplementary files,
 * illustrated diagrams and historic 15a-style supplements are untouched.
 * Compare exact slugs rather than generating duplicate manuscripts.
 * The status is OK when the inspection itself succeeds; an incomplete course
 * is shown by nonzero report fields, not hidden behind a generic I/O error.
 * The caller decides whether incomplete publication is acceptable.
 * Range must be nonzero, ascending and at most 512 numbered positions.
 * The repository root is a trusted local path, not a filesystem sandbox.
 * Never writes source or destination files. Failed requests preserve report. */
UmiStatus UmiEducationCourseCompanionsAudit(
    const char *repositoryRoot, UmiEducationCourseKind kind,
    uint32_t firstNumber, uint32_t lastNumber,
    UmiEducationCourseCompanionReport *outReport);

#ifdef __cplusplus
}
#endif
#endif
