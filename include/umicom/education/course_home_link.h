/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/course_home_link.h
 * PURPOSE: Add discoverable, accessible navigation to existing educational
 *          HTML while retaining the original page byte-for-byte elsewhere.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_COURSE_HOME_LINK_H
#define UMICOM_EDUCATION_COURSE_HOME_LINK_H

#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_HOME_MAX_SOURCE_BYTES (4U * 1024U * 1024U)
#define UMI_EDUCATION_HOME_MAX_HREF_BYTES 128U
#define UMI_EDUCATION_HOME_MAX_LABEL_BYTES 160U

/* Compose a COMPLETE document by inserting one accessible navigation link at
 * the start of its existing <main> element. All original bytes are preserved
 * in their original order; there is no HTML parser or Markdown renderer.
 * If the exact destination is already linked anywhere, a second call returns
 * the original input unchanged (idempotence). The source must contain one
 * complete <main> opening tag and no embedded NUL; URLs must be relative,
 * plain ASCII .html paths without traversal, queries or encoded separators.
 * Link labels are HTML-escaped and may contain valid UTF-8, but control bytes
 * are refused. No input aliases output. No filesystem operation occurs.
 * Rejected calls leave output bytes and *outWritten completely unchanged.
 * This is a source-preserving course-navigation composer, NOT an arbitrary
 * untrusted HTML sanitiser. Inputs are reviewed, existing local HTML files. */
UmiStatus UmiEducationCourseHomeLinkCompose(
    const char *original, size_t originalLength,
    const char *relativeHref, const char *linkLabel,
    char *output, size_t capacity, size_t *outWritten);

#ifdef __cplusplus
}
#endif
#endif
