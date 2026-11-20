/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/language_cohort.h
 *
 * PURPOSE:
 *   Expose a bounded report of priority language metadata and local tool files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_COHORT_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_COHORT_H

#include "umicom/base/status.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_LANGUAGE_COHORT_JSON_CAPACITY 32768U

/* Emit one UTF-8, NUL-terminated schema 1 JSON object. The report contains pack
 * declarations and observations of regular tool files on this host. It never
 * launches a tool or includes resolved paths. NULL selects a single inherited
 * PATH snapshot. An explicit absolute directory must exist and has no fallback.
 * If output has positive capacity, any failure clears its first byte. Borrowed
 * directory storage must not overlap output. */
UmiStatus UmiProjectLanguageCohortFormatJson(const char *tool_directory,
                                            char *output, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
