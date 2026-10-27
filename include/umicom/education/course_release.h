/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/course_release.h
 * PURPOSE: Read-only C23 editorial/HTML link gate for a numbered course span.
 *          Reuses the existing canonical Markdown/HTML companion inspector.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_COURSE_RELEASE_H
#define UMICOM_EDUCATION_COURSE_RELEASE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/education/course_companions.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEducationCourseReleaseReport {
    UmiEducationCourseCompanionReport companions;
    size_t htmlPagesRead;
    size_t markdownPagesRead;
    size_t localLinksChecked;
    size_t missingLocalLinks;
    size_t unsafeLocalLinks;
    size_t internalEditorialMarkers;
    size_t unreadableOrOversizedFiles;
} UmiEducationCourseReleaseReport;

/* A course is RELEASE-READY when every numbered companion exists with matching
 * slug, its HTML local href targets exist and no recognised internal delivery
 * markers occur in numbered manuscripts. This is a bounded source preflight,
 * not a full HTML5 parser, privacy scanner, YAML parser or site renderer.
 * It deliberately leaves all legacy Python/PowerShell utilities unmodified.
 * The report is only published when the audit itself completes successfully.
 * Individual defects are represented by its counters. */
UmiStatus UmiEducationCourseReleaseAudit(
    const char *repositoryRoot, UmiEducationCourseKind kind,
    uint32_t firstNumber, uint32_t lastNumber,
    UmiEducationCourseReleaseReport *outReport);

int UmiEducationCourseReleaseReady(const UmiEducationCourseReleaseReport *report);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_EDUCATION_COURSE_RELEASE_H */
