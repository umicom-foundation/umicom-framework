/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/nav_audit.h
 *
 * PURPOSE:
 *   Audit the local Markdown targets in the Bits2Banking MkDocs navigation.
 *   This is a focused, dependency-free preflight; MkDocs remains responsible
 *   for complete YAML syntax and website rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_NAV_AUDIT_H
#define UMICOM_EDUCATION_NAV_AUDIT_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEducationNavReport {
    size_t pageCount;
    size_t missingCount;
    size_t duplicateCount;
    size_t unsafeCount;
    size_t structuralErrors;
} UmiEducationNavReport;

/* Inspect mkdocs.yml and corresponding Markdown paths in content/ without
 * executing scripts, running MkDocs, or modifying the checkout.
 *
 * Return 0 on success, 1 for reported content problems, 2 for invalid input
 * or unreadable configuration. Every local target must be a relative .md path
 * beneath <repositoryRoot>/content. Relative parent segments, backslashes,
 * drive prefixes and duplicate navigation destinations are refused.
 *
 * This is deliberately NOT a general YAML parser. The full MkDocs build must
 * still validate YAML syntax, extension settings, theme settings and anchors.
 * diagnostics may be NULL to suppress messages. Output is always initialised.
 */
int UmiEducationAuditNavigation(const char *repositoryRoot,
                                UmiEducationNavReport *outReport,
                                FILE *diagnostics);

#ifdef __cplusplus
}
#endif
#endif
