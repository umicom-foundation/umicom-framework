/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/course_catalogue.h
 * PURPOSE:
 *   Render a bounded, accessible HTML catalogue from reviewed course entries.
 *   Bits2Banking owns the teaching manuscripts and HTML documents; the native
 *   Framework owns reusable validation, escaping and catalogue composition.
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_COURSE_CATALOGUE_H
#define UMICOM_EDUCATION_COURSE_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_COURSE_MAX_ENTRIES 512U
#define UMI_EDUCATION_COURSE_MAX_TITLE 240U
#define UMI_EDUCATION_COURSE_MAX_HREF 256U

typedef struct UmiEducationCourseEntry {
    /* Distinct, ascending curriculum ordinal. Not a software version. */
    uint32_t lessonNumber;
    const char *title;
    /* A portable relative .html file, optionally within a subdirectory.
     * No protocol, host, leading slash, parent component or query string. */
    const char *href;
} UmiEducationCourseEntry;

/* Produce a complete standalone HTML5 catalogue in British English.
 * Escapes all HTML-sensitive title characters and rejects unsafe hyperlinks,
 * duplicate/out-of-order ordinals, control characters and oversized fields.
 * The caller owns courseTitle, entries, links and output storage throughout.
 *
 * capacity is in BYTES including the terminating NUL. The output and every
 * source string must not overlap. First validate and measure everything;
 * on any failure outHtml and *outLength are byte-for-byte unchanged.
 * On success outLength excludes the terminal NUL. No files, network, browser
 * or scripts are executed. This API creates a catalogue, not lesson content.
 * A successful HTML render is not proof that target files exist on disk. */
UmiStatus UmiEducationCourseCatalogueRender(
    const char *courseTitle,
    const UmiEducationCourseEntry *entries, size_t entryCount,
    const char *courseHomeHref,
    char *outHtml, size_t capacity, size_t *outLength);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_EDUCATION_COURSE_CATALOGUE_H */
